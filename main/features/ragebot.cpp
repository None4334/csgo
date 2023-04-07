#include "../../includes.h"
#include "../handlers/localengine.h"
#include "../handlers/entityhandler.h"
#include "../gvars.h"
#include "../sdk.h"
#include "../math.h"
#include "../handlers/convarmanager.h"
#include "../handlers/animationsmanager.h"
#include "../ui/vars.h"
#include "../renderer.h"
#include "../handlers/eventlogger.h"
#include "ragebot.h"
#include "prediction.h"
#include "../handlers/cmdmgr.h"
#include "../handlers/packethandler.h"
#include "autowall.h"

void TRageBot::Clear() {
	stkHitboxInfo.clear();
	stkLatestAdjustedInfo.clear();
	stkOldAdjustedInfo.clear();
}

void TRageBot::Run(CUserCmd* pCmd, CBaseEntity* pBaseEntity) {

	if (!TVariables::TRagebot::bEnabled)
		return;

	 pWeapon = TBaseEntity::GetWeapon(TGlobalVars::pLocalEntity);

	if (!pWeapon)
		return;

	 pWeaponData = TBaseEntity::GetWeaponData(pWeapon);

	if (!pWeaponData)
		return;

	if (!pWeaponData->IsGun())
		return;

	THitboxSafety::ComputeSeeds();

	if (!THitboxSafety::bSeedsComputed)
		return;

	stkTraceInfo.clear();

	TEngineFireBullet::CacheWeaponData();

	Clear();

	bEarlyTargetFound = false;
	bTargetFound = false;

	TCommandInfo* CmdObject = TCommandManager::GetCommandData(pCmd->iCommandNumber);

	if (!CmdObject)
		return;

	static TTempAnimationRecord TempRecord;

	for (auto& CurrentObject : TEntityListener::vecEntities)
	{
		if (!CurrentObject->AnimationInstance)
			continue;

		if (CurrentObject->nClass != (int)EClassIndex::CCSPlayer)
			continue;

		if (!CurrentObject->pEntity->IsAlive() || CurrentObject->pEntity == pBaseEntity || CurrentObject->pEntity->HasImmunity() || CurrentObject->pEntity->IsDormant())
			continue;

		if (!pBaseEntity->IsEnemy(CurrentObject->pEntity))
			continue;

		const Model_t* pModel = CurrentObject->pEntity->GetModel();

		if (!pModel)
			continue;

		studiohdr_t* pStudioHdr = TInterfaces::ModelInfo->GetStudioModel(pModel);

		if (!pStudioHdr)
			continue;

		mstudiohitboxset_t* pStudioSet = pStudioHdr->GetHitboxSet(CurrentObject->pEntity->GetHitboxSet());

		if (!pStudioSet)
			continue;

		Clear();

		TAnimationRecord* recLatest = TAnimationUpdate::GetLatestValidRecord(CurrentObject->AnimationInstance);

		if (!recLatest)
			continue;

		THitboxSafety::bShouldForceSafePoint = THitboxSafety::EvaluateForceSafepointCondition(CurrentObject->pEntity);

		TAnimationRecord* recOldest = TAnimationUpdate::GetOldestValidRecord(CurrentObject->AnimationInstance);

		if (!TVariables::TRagebot::AccuracyBoost.bEnabled)
			recOldest = recLatest;

		TMultipointSystem::CopyAndAdjustMultipoints(CurrentObject, pStudioSet,recLatest,recOldest,true);

		if (stkLatestAdjustedInfo.empty() && stkOldAdjustedInfo.empty())
			continue;

		TTraceInfo Info;
		TempRecord.Store(CurrentObject->pEntity);
		TMultipointSystem::TraceMultipoints(CurrentObject, pStudioSet, CmdObject->vecEyePosition, Info, recLatest,recOldest,&TempRecord,true);
		TempRecord.Restore();

		if (Info.flDamage < 1.0f)
			continue;

		Clear();

		TMultipointSystem::CopyAndAdjustMultipoints(CurrentObject, pStudioSet, recLatest, recOldest, false);

		if (stkLatestAdjustedInfo.empty() && stkOldAdjustedInfo.empty())
			continue;

		TTraceInfo InfoNext;
		TMultipointSystem::TraceMultipoints(CurrentObject, pStudioSet, CmdObject->vecEyePosition, InfoNext, recLatest, recOldest, &TempRecord, false, TVariables::TRagebot::MinimumDamage.Value);
		TempRecord.Restore();

		if (InfoNext.flDamage < 1.f)
			continue;

		bEarlyTargetFound = true;

		//Clear();

		//TMultipointSystem::CopyAndAdjustMultipoints(CurrentObject, pStudioSet, recLatest, recOldest, false);

		//if (stkLatestAdjustedInfo.empty() && stkOldAdjustedInfo.empty())
		//	continue;

		TTraceInfo InfoFinal;
		TMultipointSystem::TraceMultipoints(CurrentObject, pStudioSet, CmdObject->vecEyePosition, InfoFinal, recLatest, recOldest, &TempRecord, false , TVariables::TRagebot::MinimumDamage.Value);
		TempRecord.Restore();

		if (InfoFinal.flDamage < 1.f)
			continue;

		InfoFinal.pStudioSet = pStudioSet;
		InfoFinal.EntityObject = CurrentObject;

		bTargetFound = true;
		stkTraceInfo.add_back(InfoFinal);

	}

	TRageBot::EvaluateAutostopCondition();
	TRageBot::RunAutostop(pCmd);

	if (!stkTraceInfo.empty()) {

		TTraceInfo* ViableInfo = stkTraceInfo.first();

		if (stkTraceInfo.size() > 1)
		{
			for (int iCurrent = 0; iCurrent < stkTraceInfo.size(); iCurrent++)
			{
				TTraceInfo* TraceObject = stkTraceInfo[iCurrent];
				if (TraceObject->flDamage > ViableInfo->flDamage)
					ViableInfo = TraceObject;
				
			}
		}

		Vector3D vecAimAngle = TMath::CalculateAngle(CmdObject->vecEyePosition,ViableInfo->vecPoint);
		vecAimAngle.NormalizeAngles();
		vecAimAngle.ClampAngles();

		if (!vecAimAngle.IsZero()) {
			
			if (CanFire(TGlobalVars::pLocalEntity, pWeapon)) {
				float flHitchance = THitboxSafety::GetHitchance(ViableInfo->AnimationRecord, ViableInfo->pStudioSet, CmdObject->vecEyePosition, ViableInfo->vecPoint, vecAimAngle, ViableInfo->Index);
				if (flHitchance >= TVariables::TRagebot::Hitchance.Value)
				{

					pCmd->angViewPoint.x = vecAimAngle.x;
					pCmd->angViewPoint.y = vecAimAngle.y;

					if (!TVariables::TRagebot::SilentAim.bEnabled)
						TInterfaces::ClientState->angViewPoint = pCmd->angViewPoint;
					
					if (TVariables::TRagebot::AutoFire.bEnabled)
						pCmd->iButtons |= IN_ATTACK;

					if (pCmd->iButtons & IN_ATTACK) {
						pCmd->iTickCount = TIME_TO_TICKS(ViableInfo->AnimationRecord->flCorrectSimulationTime + TConvarManager::flLerp);
					
						CmdObject->RecordInfo.bHasShotRecord = true;
						CmdObject->RecordInfo.EntityObject = ViableInfo->EntityObject;
				
						RtlCopyMemory(&CmdObject->RecordInfo.AnimationRecord, ViableInfo->AnimationRecord,sizeof TAnimationRecord);
					}
				}
			}
		}
	}

}

void TRageBot::RunAutostop(CUserCmd* pCmd) {

	if (!bRequireAutoStop)
		return;

	const float flSpeed2D = TGlobalVars::pLocalEntity->GetVelocity().Length2D();
	float flMaxSpeed = TBaseEntity::GetMaxPlayerSpeed(TGlobalVars::pLocalEntity, pWeapon, pWeaponData);
	const float flSlowSpeed = flMaxSpeed * 0.03f;

	std::unordered_map<int, bool> mapValues = TVariables::TRagebot::Doubletap.KeyState.bState ? TVariables::TRagebot::DoubletapAutostop.mapValues : TVariables::TRagebot::Autostop.mapValues;

	if (TGlobalVars::pLocalEntity->GetVelocity().Length2D() <= (flMaxSpeed * 0.34f) && mapValues[ASC_SLOWMOTION] )
	{
		pCmd->flSideMove /= flSpeed2D / flSlowSpeed;
		pCmd->flForwardMove /= flSpeed2D / flSlowSpeed;
		pCmd->flSideMove = std::clamp(pCmd->flSideMove, -450.0f, 450.0f);
		pCmd->flForwardMove = std::clamp(pCmd->flForwardMove, -450.0f, 450.0f);
	}
	else
	{
		Vector3D angResistance;
		TMath::VectorAngles((TGlobalVars::pLocalEntity->GetVelocity() * -1.f), angResistance);

		angResistance.y = pCmd->angViewPoint.y - angResistance.y;

		Vector3D vecResistance = Vector3D(0, 0, 0);
		TMath::AngleVectors(angResistance, &vecResistance);

		pCmd->flForwardMove = std::clamp(vecResistance.x, -450.f, 450.0f);
		pCmd->flSideMove = std::clamp(vecResistance.y, -450.f, 450.0f);
	}
}

void TRageBot::EvaluateAutostopCondition() {

	std::unordered_map<int, bool>& mapValues = TVariables::TRagebot::Doubletap.KeyState.bState ? TVariables::TRagebot::DoubletapAutostop.mapValues : TVariables::TRagebot::Autostop.mapValues;

	bool bActive = TVariables::TRagebot::Doubletap.KeyState.bState || TVariables::TRagebot::Autostop.bEnabled;

	if (!bActive)
		return;

	if (!bTargetFound && !bEarlyTargetFound) {
		bRequireAutoStop = false;
		return;
	}

	if (!mapValues[ASC_INAIR] && (!(TGlobalVars::pLocalEntity->GetFlags() & FL_ONGROUND) || !(TPrediction::iOldFlags & FL_ONGROUND)))
	{
		bRequireAutoStop = false;
		return;
	}

	if (mapValues[ASC_BETWEENSHOTS] && bTargetFound)
	{
		bRequireAutoStop = true;
		return;
	}

	if (!CanFire(TGlobalVars::pLocalEntity, pWeapon)) {
		bRequireAutoStop = false;
		return;
	}

	if (mapValues[ASC_EARLY] && (bEarlyTargetFound || bTargetFound))
	{
		bRequireAutoStop = true;
		return;
	}

	if (bTargetFound)
	{
		bRequireAutoStop = true;
		return;
	}

	bRequireAutoStop = false;
}

bool TRageBot::CanFire(CBaseEntity* pLocalPlayer, CBaseCombatWeapon* pWeapon) {

	if (!pLocalPlayer->CanShoot())
		return false;

	if (pWeapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
	{
		CWeaponCSBase* pWeaponBase = static_cast<CWeaponCSBase*>(pWeapon);

		const float flServerTime = TICKS_TO_TIME(pLocalPlayer->GetTickBase());

		float flFireReadyTimeDifference = pWeaponBase->GetFireReadyTime() - flServerTime;

		if (flFireReadyTimeDifference >= 0.0f)
			return false;
	}

	return true;
}

void TRageBot::THitboxSafety::ComputeSeeds() {

	static int iDefinitionIndex = -1;

	if (iDefinitionIndex != pWeapon->GetItemDefinitionIndex())
		bSeedsComputed = false;

	if (bSeedsComputed)
		return;

	iDefinitionIndex = pWeapon->GetItemDefinitionIndex();

	for (auto i = 0; i < 64; i++)
	{
		TInterfaces::RandomSeed(i);

		float a = TInterfaces::RandomFloat(0.0f, 6.2831855f);
		float c = TInterfaces::RandomFloat(0.0f, 1.0f);
		float b = TInterfaces::RandomFloat(0.0f, 6.2831855f);

		arrPrecomputedSmallSeeds[i] = std::tuple<float, float, float, float, float>(c,
			sin(a), cos(b), sin(b), cos(a));
	}

	for (auto i = 0; i < 128; i++) {

		TInterfaces::RandomSeed(arrSpreadSeeds[i]);

		float a = TInterfaces::RandomFloat(0.0f, 6.2831855f);
		float c = TInterfaces::RandomFloat(0.0f, 1.0f);
		float b = TInterfaces::RandomFloat(0.0f, 6.2831855f);

		int id = pWeapon->GetItemDefinitionIndex();
		CWeaponCSBase* pBaseWeapon = (CWeaponCSBase*)pWeapon;
		auto recoil_index = pBaseWeapon->GetRecoilIndex();

		if (id == 28 && recoil_index < 3.0f)
		{
			for (int i = 3; i > recoil_index; i--)
				c *= c;

			c = 1.0f - c;
		}

		arrPrecomputedSeeds[i] = std::tuple<float, float, float, float, float>(c,
			sin(a), cos(b), sin(b), cos(a));
	}

	bSeedsComputed = true;
}

bool TRageBot::THitboxSafety::HasMaximumAccuracy(CBaseCombatWeapon* pWeapon, CCSWeaponData*  pWeaponData, float flWeaponInAccuracy)
{
	if (TConvarManager::varWeaponNoSpread.pBase->GetBool())
		return true;

	if (pWeapon->GetNextPrimaryAttack() == TInterfaces::Globals->flCurrentTime)
		return false;

	if (pWeapon->GetItemDefinitionIndex() == WEAPON_SSG08 || pWeapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
		if (!(TGlobalVars::pLocalEntity->GetFlags() & FL_ONGROUND))
			if (flWeaponInAccuracy <= 0.009f)
				return true;

	float flDefaultInaccuracy = 0.0f;
	if (TGlobalVars::pLocalEntity->IsDucked() || TGlobalVars::pLocalEntity->GetFlags() & FL_DUCKING)
	{
		if (pWeapon->GetWeaponMode() != 0)
			flDefaultInaccuracy = pWeaponData->flInaccuracyCrouch[1];
		else
			flDefaultInaccuracy = pWeaponData->flInaccuracyCrouch[0];
	}
	else
	{
		if (pWeapon->GetWeaponMode() != 0)
			flDefaultInaccuracy = pWeaponData->flInaccuracyStand[1];	
		else
			flDefaultInaccuracy = pWeaponData->flInaccuracyStand[0];
	}

	if (fabsf(flWeaponInAccuracy - flDefaultInaccuracy) <= 0.0001f)
		return true;

	return false;
}

bool CollidePoint(const Vector3D& vecStart, const Vector3D& vecEnd, mstudiobbox_t* pHitbox, matrix3x4_t* aMatrix)
{
	if (!pHitbox || !aMatrix)
		return false;

	Ray_t Ray(vecStart, vecEnd);

	CGameTrace Trace;
	Trace.flFraction = 1.0f;
	Trace.bStartSolid = false;

	typedef int(__fastcall* ClipRayToHitbox_t)(const Ray_t&, mstudiobbox_t*, matrix3x4_t&, Trace_t&);
	static auto oClipRayToHitbox = (ClipRayToHitbox_t)(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 E4 F8 F3 0F 10 42")));
	return oClipRayToHitbox(Ray, pHitbox, aMatrix[pHitbox->iBone], Trace) >= 0;
}

bool TRageBot::THitboxSafety::EvaluateForceSafepointCondition(CBaseEntity* pBaseEntity)
{

	if (TVariables::TRagebot::ForceSafePointConditions.mapValues[SPC_STANDING])
		if (pBaseEntity->GetVelocity().Length2D() < 1.5f)
			return true;
	

	if (TVariables::TRagebot::ForceSafePointConditions.mapValues[SPC_MOVING])
		if (pBaseEntity->GetVelocity().Length2D() > 1.5f)
			return true;
	

	if (TVariables::TRagebot::ForceSafePointConditions.mapValues[SPC_INAIR])
		if (!(pBaseEntity->GetFlags() & FL_ONGROUND))
			return true;
	

	if (TVariables::TRagebot::ForceSafePointConditions.mapValues[SPC_CROUCHING])
		if (pBaseEntity->GetFlags() & FL_DUCKING)
			return true;
	
	return false;
}

bool TRageBot::THitboxSafety::SafepointEnabledOnHitbox(int Index) 
{

	if (bShouldForceSafePoint)
		return true;

	switch (Index)
	{
	case HITBOX_HEAD:
	{
		return (TVariables::TRagebot::ForceSafePointList.mapValues[CFG_HITBOXHEAD]);
		break;
	}
	case HITBOX_STOMACH:
	case HITBOX_PELVIS:
	{
		return (TVariables::TRagebot::ForceSafePointList.mapValues[CFG_HITBOXSTOMACH]);
		break;
	}
	case HITBOX_UPPER_CHEST:
	case HITBOX_CHEST:
	{
		return (TVariables::TRagebot::ForceSafePointList.mapValues[CFG_HITBOXCHEST]);
		break;
	}
	case HITBOX_LEFT_UPPER_ARM:
	case HITBOX_RIGHT_UPPER_ARM:
	case HITBOX_LEFT_FOREARM:
	case HITBOX_RIGHT_FOREARM:
	{
		return (TVariables::TRagebot::ForceSafePointList.mapValues[CFG_HITBOXARMS]);
		break;
	}
	case HITBOX_LEFT_CALF:
	case HITBOX_RIGHT_CALF:
	{
		return (TVariables::TRagebot::ForceSafePointList.mapValues[CFG_HITBOXLEGS]);
		break;
	}
	case HITBOX_LEFT_FOOT:
	case HITBOX_RIGHT_FOOT:
	{
		return (TVariables::TRagebot::ForceSafePointList.mapValues[CFG_HITBOXFEET]);
		break;
	}
	default:
	{
		break;
	}
	}

	return false; 
}

float TRageBot::THitboxSafety::GetHitchance(TAnimationRecord* AnimationRecord ,mstudiohitboxset_t* pStudioSet, Vector3D vecEyePosition, Vector3D vecEndPosition , Vector3D vecAimAngle, int iHitbox)
{
	mstudiobbox_t* pHitbox = pStudioSet->GetHitbox(iHitbox);

	if (!pHitbox)
		return 0.0f;

	const auto bIsCapsule = pHitbox->flRadius != -1.f;

	if (!bIsCapsule)
		return 0.f;

	CBaseCombatWeapon* pWeapon = TBaseEntity::GetWeapon(TGlobalVars::pLocalEntity);
	
	if (!pWeapon)
		return 0.0f;

	CCSWeaponData* pWeaponData = TBaseEntity::GetWeaponData(pWeapon);

	if (!pWeaponData)
		return 0.0f;

	std::tuple<float, float, float, float, float>* tpSeed;
	float flSeedCurr, flSpreadCal, flInaccuracyVal;
	Vector3D vecSpread, vecDirection, end;

	Vector3D vecForward, vecRight, vecUp;

	TMath::AngleVectors(vecAimAngle, &vecForward, &vecRight, &vecUp);

	int iHitTraces = 0;

	for (int iCurrentSeed = 0; iCurrentSeed < 128; iCurrentSeed++)
	{
		tpSeed = &arrPrecomputedSeeds[iCurrentSeed];

		flSeedCurr = std::get<0>(*tpSeed);

		flSpreadCal = flSeedCurr * pWeapon->GetSpread();
		flInaccuracyVal = flSeedCurr * pWeapon->GetInaccuracy();

		vecSpread = Vector3D((std::get<2>(*tpSeed) * flSpreadCal) + (std::get<4>(*tpSeed) * flInaccuracyVal), (std::get<3>(*tpSeed) * flSpreadCal) + (std::get<1>(*tpSeed) * flInaccuracyVal), 0.0F);

		vecDirection.x = vecForward.x + (vecSpread.x * vecRight.x) + (vecSpread.y * vecUp.x);
		vecDirection.y = vecForward.y + (vecSpread.x * vecRight.y) + (vecSpread.y * vecUp.y);
		vecDirection.z = vecForward.z + (vecSpread.x * vecRight.z) + (vecSpread.y * vecUp.z);

		vecDirection.NormalizeInPlace();

		Vector3D vecEndPosition = vecEyePosition + (vecDirection * 8192.f);

		Trace_t pEngineTrace = {};
		Ray_t pEngineRay(vecEyePosition, vecEndPosition);

		if (DoesIntersectHitbox(vecEndPosition, pHitbox, AnimationRecord->matModel, vecEyePosition))
			iHitTraces++;

	}

	float flHitTraceFraction = static_cast<float>(iHitTraces) / 128.f;

	int iFinalHitchance = static_cast<int>(flHitTraceFraction * 100.f);

	return iFinalHitchance;
}

void TRageBot::TMultipointSystem::AddMultipoints(TEntityObject* EntityObject, mstudiohitboxset_t* pStudioSet, int iHitbox, bool bAdjustForSingleScan) {

	mstudiobbox_t* pStudioBox = pStudioSet->GetHitbox(iHitbox);
	
	if (!pStudioBox)
		return;

	THitboxInfo* HitboxObject = stkHitboxInfo.add_back(THitboxInfo());
	HitboxObject->Index = iHitbox;
	HitboxObject->pStudioBox = pStudioBox;

	static const float flRotation = 0.70710678f;

	Vector3D vecMaxs = pStudioBox->vecBBMax;
	Vector3D vecMins = pStudioBox->vecBBMin;
	Vector3D vecCenter = (vecMins + vecMaxs) / 2.f;
	float flRadiusModifier = 0.95f;

	float flAmountOfPitch = 3.6f;
	float flLowerAmountOfPitch = flAmountOfPitch + .5f;
	constexpr float flLowerAdditionAmount = .5f;
	float flAmountOfYaw = 3.4f;
	float flHighYawAmount = 3.2f;

	float flConfigBodyYaw = static_cast<float>(TVariables::TRagebot::Bodyscale.Value) / 100.f;

	float flBodyHitboxRadius = (pStudioBox->flRadius * flRadiusModifier) * flConfigBodyYaw;
	float flBodyLowHitboxRadius = (pStudioBox->flRadius * 0.85f) * flConfigBodyYaw;

	float flConfigHeadYaw = static_cast<float>(TVariables::TRagebot::Headscale.Value) / 100.f;

	switch (iHitbox)
	{
	case HITBOX_HEAD:
	{
		if (!bAdjustForSingleScan && TVariables::TRagebot::MultipointList.mapValues[CFG_HITBOXHEAD]) {

		flAmountOfPitch = -0.5f;
		flLowerAmountOfPitch = flAmountOfPitch + flLowerAdditionAmount;
		flAmountOfYaw = 0.f;

		HitboxObject->stkMultipoints.add_back(TPointInfo(Vector3D(vecCenter.x - (flRotation * flAmountOfPitch), vecCenter.y - (-flRotation * flAmountOfPitch), vecCenter.z - flAmountOfYaw), Vector3D(vecCenter.x - (flRotation * flLowerAmountOfPitch), vecCenter.y - (-flRotation * flLowerAmountOfPitch), vecCenter.z - flAmountOfYaw), MULTIPOINT_CENTER));
		
		flAmountOfYaw = (flHighYawAmount * 0.75) * flConfigHeadYaw;
		HitboxObject->stkMultipoints.add_back(TPointInfo(Vector3D(vecCenter.x - (flRotation * flAmountOfPitch), vecCenter.y - (-flRotation * flAmountOfPitch), vecCenter.z - flAmountOfYaw), Vector3D(vecCenter.x - (flRotation * flLowerAmountOfPitch), vecCenter.y - (-flRotation * flLowerAmountOfPitch), vecCenter.z - flAmountOfYaw), MULTIPOINT_RIGHT));
		
		flAmountOfYaw = -((flHighYawAmount * 0.75) * flConfigHeadYaw);
		HitboxObject->stkMultipoints.add_back(TPointInfo(Vector3D(vecCenter.x - (flRotation * flAmountOfPitch), vecCenter.y - (-flRotation * flAmountOfPitch), vecCenter.z - flAmountOfYaw), Vector3D(vecCenter.x - (flRotation * flLowerAmountOfPitch), vecCenter.y - (-flRotation * flLowerAmountOfPitch), vecCenter.z - flAmountOfYaw), MULTIPOINT_LEFT));
		

		flAmountOfPitch = -2.5f * flConfigHeadYaw;
		flAmountOfYaw = ((flHighYawAmount * 0.65f) * flConfigHeadYaw);
		HitboxObject->stkMultipoints.add_back(TPointInfo(Vector3D(vecCenter.x - (flRotation * flAmountOfPitch), vecCenter.y - (-flRotation * flAmountOfPitch), vecCenter.z - flAmountOfYaw), Vector3D(vecCenter.x - (flRotation * flLowerAmountOfPitch), vecCenter.y - (-flRotation * flLowerAmountOfPitch), vecCenter.z - flAmountOfYaw), MULTIPOINT_RIGHT));

		flAmountOfYaw = -((flHighYawAmount * 0.65f) * flConfigHeadYaw);
		HitboxObject->stkMultipoints.add_back(TPointInfo(Vector3D(vecCenter.x - (flRotation * flAmountOfPitch), vecCenter.y - (-flRotation * flAmountOfPitch), vecCenter.z - flAmountOfYaw), Vector3D(vecCenter.x - (flRotation * flLowerAmountOfPitch), vecCenter.y - (-flRotation * flLowerAmountOfPitch), vecCenter.z - flAmountOfYaw), MULTIPOINT_LEFT));

		}

		flAmountOfPitch = -4.5f * flConfigHeadYaw;
		flLowerAmountOfPitch = flAmountOfPitch + flLowerAdditionAmount;
		flAmountOfYaw = 0.f * flConfigHeadYaw;
		HitboxObject->stkMultipoints.add_back(TPointInfo(Vector3D(vecCenter.x - (flRotation * flAmountOfPitch), vecCenter.y - (-flRotation * flAmountOfPitch), vecCenter.z - flAmountOfYaw), Vector3D(vecCenter.x - (flRotation * flLowerAmountOfPitch), vecCenter.y - (-flRotation * flLowerAmountOfPitch), vecCenter.z - flAmountOfYaw), MULTIPOINT_TOP));
		break;
	}
	case HITBOX_STOMACH:
	{	
		flAmountOfPitch = 0.f;

		flAmountOfYaw = 0.f;
		HitboxObject->stkMultipoints.add_back(TPointInfo(Vector3D(vecCenter.x - (flRotation * flAmountOfPitch),vecCenter.y - (-flRotation * flAmountOfPitch),vecCenter.z - flAmountOfYaw) , Vector3D(), MULTIPOINT_CENTER ));
		
		if (!bAdjustForSingleScan && TVariables::TRagebot::MultipointList.mapValues[CFG_HITBOXSTOMACH]) {

		flAmountOfYaw = 6.f * flConfigBodyYaw;
		HitboxObject->stkMultipoints.add_back(TPointInfo(Vector3D(vecCenter.x - (flRotation * flAmountOfPitch),vecCenter.y - (-flRotation * flAmountOfPitch),vecCenter.z - flAmountOfYaw) , Vector3D() ,MULTIPOINT_RIGHT));

		flAmountOfYaw = -6.f * flConfigBodyYaw;
		HitboxObject->stkMultipoints.add_back(TPointInfo(Vector3D(vecCenter.x - (flRotation * flAmountOfPitch),vecCenter.y - (-flRotation * flAmountOfPitch),vecCenter.z - flAmountOfYaw), Vector3D() , MULTIPOINT_LEFT ));
		}

		flAmountOfYaw = 0.f;
		HitboxObject->stkMultipoints.add_back(TPointInfo(Vector3D(vecCenter.x, vecMaxs.y - flBodyHitboxRadius, vecCenter.z) ,Vector3D()  , MULTIPOINT_CENTER ));
		break;
	}
	case HITBOX_PELVIS:
	{
		HitboxObject->stkMultipoints.add_back(TPointInfo(vecCenter, MULTIPOINT_CENTER));

		flAmountOfYaw = 0.f;
		HitboxObject->stkMultipoints.add_back(TPointInfo(Vector3D(vecCenter.x, vecMaxs.y - flBodyHitboxRadius, vecCenter.z), Vector3D(), MULTIPOINT_CENTER));
		break;
	}
	case HITBOX_UPPER_CHEST:
	case HITBOX_CHEST:
	{

		flAmountOfPitch = 0.f;
		flAmountOfYaw = 0.f;
		HitboxObject->stkMultipoints.add_back(TPointInfo(Vector3D(vecCenter.x - (flRotation * flAmountOfPitch),vecCenter.y - (-flRotation * flAmountOfPitch),vecCenter.z - flAmountOfYaw) , Vector3D() , MULTIPOINT_CENTER ));

		if (!bAdjustForSingleScan && TVariables::TRagebot::MultipointList.mapValues[CFG_HITBOXCHEST]) {
			flAmountOfYaw = 6.f * flConfigBodyYaw;
			HitboxObject->stkMultipoints.add_back(TPointInfo(Vector3D(vecCenter.x - (flRotation * flAmountOfPitch), vecCenter.y - (-flRotation * flAmountOfPitch), vecCenter.z - flAmountOfYaw), Vector3D(), MULTIPOINT_RIGHT));

			flAmountOfYaw = -6.f * flConfigBodyYaw;
			HitboxObject->stkMultipoints.add_back(TPointInfo(Vector3D(vecCenter.x - (flRotation * flAmountOfPitch), vecCenter.y - (-flRotation * flAmountOfPitch), vecCenter.z - flAmountOfYaw), Vector3D(), MULTIPOINT_LEFT));
		}

		if (iHitbox != HITBOX_UPPER_CHEST && TVariables::TRagebot::MultipointList.mapValues[CFG_HITBOXCHEST]) {
			flAmountOfYaw = 0.f;
			HitboxObject->stkMultipoints.add_back(TPointInfo(Vector3D(vecCenter.x, vecMaxs.y - flBodyLowHitboxRadius, vecCenter.z) ,Vector3D(),MULTIPOINT_CENTER ));
		}

		break;
	}
	case HITBOX_LEFT_UPPER_ARM:
	case HITBOX_RIGHT_UPPER_ARM:
	case HITBOX_LEFT_FOREARM:
	case HITBOX_RIGHT_FOREARM:
	case HITBOX_LEFT_CALF:
	case HITBOX_RIGHT_CALF:
	case HITBOX_LEFT_FOOT:
	case HITBOX_RIGHT_FOOT:
	{
		HitboxObject->stkMultipoints.add_back(TPointInfo(vecCenter,MULTIPOINT_CENTER));
		break;
	}
	default:
	{
		stkHitboxInfo.pop_back();
		break;
	}
	}

}

void TRageBot::TMultipointSystem::TransformVector(TEntityObject* EntityObject, TAnimationRecord* recLatest, TAnimationRecord* recOldest) {

	bool bRecordChange = recOldest != recLatest && recOldest;

	if (stkHitboxInfo.size())
	{
		stkHitboxInfo.copy_to(stkLatestAdjustedInfo);

		if (bRecordChange)
			stkHitboxInfo.copy_to(stkOldAdjustedInfo);

		for (int Current = 0; Current < stkHitboxInfo.size(); Current++)
		{
			THitboxInfo* Object = stkHitboxInfo[Current];

			THitboxInfo* ResolvedObject = stkLatestAdjustedInfo[Current];

			THitboxInfo* OldResolvedObject = nullptr;

			if (bRecordChange)
				OldResolvedObject = stkOldAdjustedInfo[Current];

			if (Object->stkMultipoints.size())
			{
				for (int nCurrent = 0; nCurrent < Object->stkMultipoints.size(); nCurrent++)
				{
					TPointInfo* MultipointObject = Object->stkMultipoints[nCurrent];

					TPointInfo* ResolvedLatest = ResolvedObject->stkMultipoints[nCurrent];

					TMath::VectorTransform(MultipointObject->vecHigher, recLatest->matModel[Object->pStudioBox->iBone], ResolvedLatest->vecHigher);
					TMath::VectorTransform(MultipointObject->vecLower, recLatest->matModel[Object->pStudioBox->iBone], ResolvedLatest->vecLower);

					if (OldResolvedObject)
					{
						TPointInfo* ResolvedOldest = OldResolvedObject->stkMultipoints[nCurrent];
						TMath::VectorTransform(MultipointObject->vecHigher, recOldest->matModel[Object->pStudioBox->iBone], ResolvedOldest->vecHigher);
						TMath::VectorTransform(MultipointObject->vecLower, recOldest->matModel[Object->pStudioBox->iBone], ResolvedOldest->vecLower);
					}
				}
			}
		}
	}
}

void TRageBot::TMultipointSystem::CopyAndAdjustMultipoints(TEntityObject* EntityObject, mstudiohitboxset_t* pStudioSet, TAnimationRecord* recLatest, TAnimationRecord* recOldest, bool bAdjustForSingleScan) {
	
	if (bAdjustForSingleScan)
	{	
		AddMultipoints(EntityObject, pStudioSet, HITBOX_HEAD,true);
		AddMultipoints(EntityObject, pStudioSet, HITBOX_PELVIS, true);
		AddMultipoints(EntityObject, pStudioSet, HITBOX_UPPER_CHEST, true);
		AddMultipoints(EntityObject, pStudioSet, HITBOX_CHEST, true);
		AddMultipoints(EntityObject, pStudioSet, HITBOX_LEFT_FOREARM, true);
		AddMultipoints(EntityObject, pStudioSet, HITBOX_RIGHT_FOREARM, true);
		AddMultipoints(EntityObject, pStudioSet, HITBOX_LEFT_FOOT, true);
		AddMultipoints(EntityObject, pStudioSet, HITBOX_RIGHT_FOOT, true);
	}
	else {
		
		if (TVariables::TRagebot::HitboxList.mapValues[CFG_HITBOXHEAD])
		    AddMultipoints(EntityObject, pStudioSet, HITBOX_HEAD, false);

		if (TVariables::TRagebot::HitboxList.mapValues[CFG_HITBOXSTOMACH]) {
			AddMultipoints(EntityObject, pStudioSet, HITBOX_STOMACH, false);
			AddMultipoints(EntityObject, pStudioSet, HITBOX_PELVIS, false);
		}

		if (TVariables::TRagebot::HitboxList.mapValues[CFG_HITBOXCHEST]) {
			AddMultipoints(EntityObject, pStudioSet, HITBOX_UPPER_CHEST, false);
			AddMultipoints(EntityObject, pStudioSet, HITBOX_CHEST, false);
		}

		if (TVariables::TRagebot::HitboxList.mapValues[CFG_HITBOXARMS]) {
			AddMultipoints(EntityObject, pStudioSet, HITBOX_LEFT_UPPER_ARM, false);
			AddMultipoints(EntityObject, pStudioSet, HITBOX_RIGHT_UPPER_ARM, false);
			AddMultipoints(EntityObject, pStudioSet, HITBOX_LEFT_FOREARM, false);
			AddMultipoints(EntityObject, pStudioSet, HITBOX_RIGHT_FOREARM, false);
		}

		if (TVariables::TRagebot::HitboxList.mapValues[CFG_HITBOXLEGS]) {
			AddMultipoints(EntityObject, pStudioSet, HITBOX_LEFT_CALF, false);
			AddMultipoints(EntityObject, pStudioSet, HITBOX_RIGHT_CALF, false);
		}

		if (TVariables::TRagebot::HitboxList.mapValues[CFG_HITBOXFEET]) {
			AddMultipoints(EntityObject, pStudioSet, HITBOX_LEFT_FOOT, false);
			AddMultipoints(EntityObject, pStudioSet, HITBOX_RIGHT_FOOT, false);
		}
	}

	TransformVector( EntityObject, recLatest,recOldest);
}

TTracePoint TRageBot::TMultipointSystem::TracePoint(TPointInfo Point, int Index , TEntityObject* EntityObject, Vector3D vecEyePosition, TTempAnimationRecord* TempRecord)
{
	if (!TVariables::TRagebot::AutoPenetration.bEnabled) {

		Trace_t EngineTrace = {};
		CTraceFilter EngineFilter(TGlobalVars::pLocalEntity);
		TInterfaces::EngineTrace->TraceRay(Ray_t(vecEyePosition, Point.vecHigher), MASK_SHOT, &EngineFilter, &EngineTrace);

		if (EngineTrace.pHitEntity != EntityObject->pEntity)
			return TTracePoint{ -1,false,Point, Index };

		if (Index == HITBOX_HEAD && EngineTrace.iHitGroup != HITGROUP_HEAD)
				return TTracePoint{ -1,false,Point, Index };

	}

	PenetrationData_t PenetrationData;
	PenetrationData.vecShootPosition = vecEyePosition;
	PenetrationData.vecTargetPosition = Point.vecHigher;
	TEngineFireBullet::ScanPoint(&PenetrationData);

	return TTracePoint{ (float)PenetrationData.flCurrentDamage,false,Point, Index };
}

bool TRageBot::THitboxSafety::DoesIntersectHitbox(Vector3D vecPoint, mstudiobbox_t* pStudioBox, matrix3x4_t* matModex, Vector3D vecEyePosition) {
	
	Vector3D vecMin, vecMax;

	TMath::VectorTransform(pStudioBox->vecBBMin, matModex[pStudioBox->iBone], vecMin);
	TMath::VectorTransform(pStudioBox->vecBBMax, matModex[pStudioBox->iBone], vecMax);

	const auto flDistance = TMath::SegmentToSegment(vecEyePosition, vecPoint, vecMin, vecMax);

	return flDistance < pStudioBox->flRadius;
}

bool TRageBot::THitboxSafety::IsSafePoint(Vector3D vecPoint, mstudiohitboxset_t* pStudioSet, int Index, Vector3D vecEyePosition, TAnimationRecord* recAnimation, TEntityObject* EntityObject)
{
	mstudiobbox_t* pHitbox = pStudioSet->GetHitbox(Index);

	if (!pHitbox)
		return false;

	const auto bIsCapsule = pHitbox->flRadius != -1.f;

	if (!bIsCapsule)
		return false;

	if (!TRageBot::THitboxSafety::DoesIntersectHitbox(vecPoint, pHitbox, recAnimation->matModel, vecEyePosition))
		return false;

	if (!TRageBot::THitboxSafety::DoesIntersectHitbox(vecPoint, pHitbox, recAnimation->matCenter, vecEyePosition))
		return false;

	if (!TRageBot::THitboxSafety::DoesIntersectHitbox(vecPoint, pHitbox, recAnimation->matLeft, vecEyePosition))
		return false;

	if (!TRageBot::THitboxSafety::DoesIntersectHitbox(vecPoint, pHitbox, recAnimation->matRight, vecEyePosition))
		return false;

	return true;
}

TTraceInfo TRageBot::TMultipointSystem::TraceList(TAnimationRecord* recAnimation, mstudiohitboxset_t* pStudioSet, TDynamicStack<THitboxInfo, HITBOX_MAX>& stkList, TEntityObject* EntityObject, Vector3D vecEyePosition, TTempAnimationRecord* TempRecord, bool bAdjustForSingleScan, float flMinimumDamage)
{
	static TDynamicStack<TTracePoint, MAX_MULTIPOINTS> arrHeadSafePointList;
	static TDynamicStack<TTracePoint, HITBOX_MAX * MAX_MULTIPOINTS> arrUpperBodySafePointList;
	static TDynamicStack<TTracePoint, HITBOX_MAX * MAX_MULTIPOINTS> arrLowerBodySafePointList;

	static TDynamicStack<TTracePoint, MAX_MULTIPOINTS> arrHeadPointList;
	static TDynamicStack<TTracePoint, HITBOX_MAX* MAX_MULTIPOINTS> arrUpperBodyPointList;
	static TDynamicStack<TTracePoint, HITBOX_MAX* MAX_MULTIPOINTS> arrLowerBodyPointList;

	static TDynamicStack<TTracePoint, HITBOX_MAX* MAX_MULTIPOINTS> arrAdjustedList;

	arrHeadSafePointList.clear();
	arrUpperBodyPointList.clear();
	arrLowerBodyPointList.clear();

	arrHeadPointList.clear();
	arrUpperBodySafePointList.clear();
	arrLowerBodySafePointList.clear();

	arrAdjustedList.clear();

	for (int Current = 0; Current < stkList.size(); Current++)
	{
		THitboxInfo* BoxObject = stkList[Current];

		if (BoxObject->stkMultipoints.empty())
			continue;

		for (int nCurrent = 0; nCurrent < BoxObject->stkMultipoints.size(); nCurrent++)
		{
			TPointInfo* PointObject = BoxObject->stkMultipoints[nCurrent];

			if (BoxObject->Index == HITBOX_HEAD && !bAdjustForSingleScan) {

				if (!TLocalEngine::TTraceSystem::DoesHitHeadClipTrace(vecEyePosition, PointObject->vecLower, EntityObject->pEntity)) 
					continue;

				if (!TLocalEngine::TTraceSystem::DoesHitHeadClipTrace(vecEyePosition, PointObject->vecHigher, EntityObject->pEntity))
					continue;
			}

			bool bIsSafepoint = false;
		
			if (!bAdjustForSingleScan) {
				bIsSafepoint = THitboxSafety::IsSafePoint(PointObject->vecHigher, pStudioSet, BoxObject->Index, vecEyePosition, recAnimation, EntityObject);

				bool bShouldForceSafepoint = THitboxSafety::SafepointEnabledOnHitbox(BoxObject->Index);

				if (!bIsSafepoint && bShouldForceSafepoint)
					continue;
			}

				switch (BoxObject->Index)
				{
				case HITBOX_HEAD:
				{
					if (bIsSafepoint)
					{
						TTracePoint* TraceInfoObject = arrHeadSafePointList.add_back(TTracePoint());
						TraceInfoObject->Index = HITBOX_HEAD;
						TraceInfoObject->flDamage = 0;
						TraceInfoObject->PointInfo = *PointObject;
						TraceInfoObject->bSafePoint = true;

					}
					else
					{
						TTracePoint* TraceInfoObject = arrHeadPointList.add_back(TTracePoint());
						TraceInfoObject->Index = HITBOX_HEAD;
						TraceInfoObject->flDamage = 0;
						TraceInfoObject->PointInfo = *PointObject;
						TraceInfoObject->bSafePoint = false;
					}

					break;
				}
				case HITBOX_STOMACH:
				case HITBOX_PELVIS:
				case HITBOX_UPPER_CHEST:
				case HITBOX_CHEST:
				{
					if (bIsSafepoint)
					{
						TTracePoint* TraceInfoObject = arrUpperBodySafePointList.add_back(TTracePoint());
						TraceInfoObject->Index = BoxObject->Index;
						TraceInfoObject->flDamage = 0;
						TraceInfoObject->PointInfo = *PointObject;
						TraceInfoObject->bSafePoint = true;

					}
					else
					{
						TTracePoint* TraceInfoObject = arrUpperBodyPointList.add_back(TTracePoint());
						TraceInfoObject->Index = BoxObject->Index;
						TraceInfoObject->flDamage = 0;
						TraceInfoObject->PointInfo = *PointObject;
						TraceInfoObject->bSafePoint = false;
					}
					break;
				}
				case HITBOX_LEFT_UPPER_ARM:
				case HITBOX_RIGHT_UPPER_ARM:
				case HITBOX_LEFT_FOREARM:
				case HITBOX_RIGHT_FOREARM:
				case HITBOX_LEFT_CALF:
				case HITBOX_RIGHT_CALF:
				case HITBOX_LEFT_FOOT:
				case HITBOX_RIGHT_FOOT:
				{
					if (bIsSafepoint)
					{
						TTracePoint* TraceInfoObject = arrLowerBodySafePointList.add_back(TTracePoint());
						TraceInfoObject->Index = BoxObject->Index;
						TraceInfoObject->flDamage = 0;
						TraceInfoObject->PointInfo = *PointObject;
						TraceInfoObject->bSafePoint = true;

					}
					else
					{
						TTracePoint* TraceInfoObject = arrLowerBodyPointList.add_back(TTracePoint());
						TraceInfoObject->Index = BoxObject->Index;
						TraceInfoObject->flDamage = 0;
						TraceInfoObject->PointInfo = *PointObject; 
						TraceInfoObject->bSafePoint = false;
					}
					break;
				}
				default:
				{
					break;
				}
				}

		}
	}

	bool bPreferBodyAim = TVariables::TRagebot::PreferBaim.bEnabled || (EntityObject->pEntity->GetHealth() < pWeaponData->iDamage);

	if (!bPreferBodyAim)
	{
		if (!arrHeadSafePointList.empty())
			for (int Current = 0; Current < arrHeadSafePointList.size(); Current++)
				arrAdjustedList.add_back(*arrHeadSafePointList[Current]);
			
		if (!arrHeadPointList.empty())
			for (int Current = 0; Current < arrHeadPointList.size(); Current++)
				arrAdjustedList.add_back(*arrHeadPointList[Current]);

	}

	if (!arrUpperBodySafePointList.empty())
		for (int Current = 0; Current < arrUpperBodySafePointList.size(); Current++)
			arrAdjustedList.add_back(*arrUpperBodySafePointList[Current]);

	if (!arrUpperBodyPointList.empty())
		for (int Current = 0; Current < arrUpperBodyPointList.size(); Current++)
			arrAdjustedList.add_back(*arrUpperBodyPointList[Current]);

	if (!arrLowerBodySafePointList.empty())
		for (int Current = 0; Current < arrLowerBodySafePointList.size(); Current++)
			arrAdjustedList.add_back(*arrLowerBodySafePointList[Current]);

	if (bPreferBodyAim)
	{
		if (!arrHeadSafePointList.empty())
			for (int Current = 0; Current < arrHeadSafePointList.size(); Current++)
				arrAdjustedList.add_back(*arrHeadSafePointList[Current]);

		if (!arrHeadPointList.empty())
			for (int Current = 0; Current < arrHeadPointList.size(); Current++)
				arrAdjustedList.add_back(*arrHeadPointList[Current]);

	}

	if (!arrLowerBodyPointList.empty())
		for (int Current = 0; Current < arrLowerBodyPointList.size(); Current++)
			arrAdjustedList.add_back(*arrLowerBodyPointList[Current]);

	
	if (!arrAdjustedList.empty())
	{
		for (int Current = 0; Current < arrAdjustedList.size(); Current++)
		{
			TTracePoint* PointObject = arrAdjustedList[Current];

			TTracePoint PointTraceInfo = TracePoint(PointObject->PointInfo, PointObject->Index, EntityObject, vecEyePosition, TempRecord);

			if (PointTraceInfo.flDamage < flMinimumDamage)
				continue;

			return TTraceInfo{ PointObject->PointInfo.vecHigher,PointTraceInfo.flDamage,PointObject->Index };
		}
	}

	return TTraceInfo{Vector3D(),-1};
}

void  TRageBot::TMultipointSystem::TraceMultipoints(TEntityObject* EntityObject, mstudiohitboxset_t* pStudioSet, Vector3D vecEyePosition, TTraceInfo& TraceInfo, TAnimationRecord* recLatest, TAnimationRecord* recOldest, TTempAnimationRecord* TempRecord, bool bAdjustForSingleScan, float flMinimumDamage)
{
	if (stkLatestAdjustedInfo.empty())
		return;

	TempRecord->Apply(recLatest);
	TraceInfo = TraceList(recLatest, pStudioSet,stkLatestAdjustedInfo, EntityObject, vecEyePosition, TempRecord, bAdjustForSingleScan, flMinimumDamage);
	TraceInfo.AnimationRecord = recLatest;
	
	if (bAdjustForSingleScan && TraceInfo.flDamage >= 1)
		return;

	if (stkOldAdjustedInfo.empty())
		return;

	if (TraceInfo.bSafePoint && TraceInfo.Index == HITBOX_HEAD)
		return;

	TempRecord->Apply(recOldest);
	TTraceInfo OldTraceInfo = TraceList(recOldest, pStudioSet,stkOldAdjustedInfo, EntityObject, vecEyePosition, TempRecord, bAdjustForSingleScan, flMinimumDamage);

	if (!OldTraceInfo.flDamage)
		return;

	if (OldTraceInfo.bSafePoint && OldTraceInfo.Index == HITBOX_HEAD) {
		TraceInfo = OldTraceInfo;
		return;
	}

	if (TraceInfo.flDamage >= OldTraceInfo.flDamage)
		return;

	TraceInfo = OldTraceInfo;
	TraceInfo.AnimationRecord = recOldest;

	return;
}

