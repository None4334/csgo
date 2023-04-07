#include "../../includes.h"
#include "animationsmanager.h"
#include "entityhandler.h"
#include "../gvars.h"
#include "../sdk.h"
#include "../math.h"
#include "convarmanager.h"
#include "../sdk/bonesetuprebuilt.h"
#include "../handlers/localengine.h"
#include "eventlogger.h"

void TAnimationRecord::FillRecord() {

	if (!OwningInstance)
		return;

	if (OwningInstance->pAnimationEntity && OwningInstance->pAnimationEntity->IsAlive())
	{
		flSimulationTime = OwningInstance->pAnimationEntity->GetSimulationTime();
		flCorrectSimulationTime = OwningInstance->pAnimationEntity->GetSimulationTime();
		Flags = OwningInstance->pAnimationEntity->GetFlags();
		vecOrigin = OwningInstance->pAnimationEntity->GetOrigin();
		vecVelocity = OwningInstance->pAnimationEntity->GetVelocity();
		flDuckAmount = OwningInstance->pAnimationEntity->GetDuckAmount();
		flLowerBodyYaw = OwningInstance->pAnimationEntity->GetLowerBodyYaw();
		vecOriginFixed = OwningInstance->pAnimationEntity->GetOrigin();
		vecVelocityFixed = OwningInstance->pAnimationEntity->GetVelocity();
		vecAbsAngles = OwningInstance->pAnimationEntity->GetAbsAngles();
		memcpy(AnimationLayers,OwningInstance->pAnimationEntity->GetAnimationOverlays().Base(),sizeof CAnimationLayer * 13);
		bFilled = true;
		bValid = true;
	}
}

void TAnimationRecord::CopyRecord(TAnimationRecord& Other) {

	if (!Other.bFilled)
		return;

	nSimulationDifference = Other.nSimulationDifference;
	flSimulationTime = Other.flSimulationTime;
	flCorrectSimulationTime = Other.flCorrectSimulationTime;
	Flags = Other.Flags;
	vecOrigin = Other.vecOrigin;
	vecVelocity = Other.vecVelocity;
	flDuckAmount = Other.flDuckAmount;
	flLowerBodyYaw = Other.flLowerBodyYaw;
	bValid = Other.bValid;
	vecOriginFixed =Other.vecOriginFixed;
	vecVelocityFixed =Other.vecVelocityFixed;
	vecAbsAngles = Other.vecAbsAngles;
	memcpy(AnimationLayers, Other.AnimationLayers, sizeof CAnimationLayer * 13);
	memcpy(matModel, Other.matModel, sizeof(matrix3x4_t) * MAXSTUDIOBONES);
	memcpy(matLeft, Other.matLeft, sizeof(matrix3x4_t) * MAXSTUDIOBONES);
	memcpy(matCenter, Other.matCenter, sizeof(matrix3x4_t) * MAXSTUDIOBONES);
	memcpy(matRight, Other.matRight, sizeof(matrix3x4_t) * MAXSTUDIOBONES);
	bFilled = true;
}

bool TAnimationRecord::IsValid() {
	
	if (!bFilled)
		return false;

	if (TConvarManager::varLagCompensation.pBase->GetInt() == 0)
		return true;

	if (!bValid)
		return false;

	if (!TInterfaces::ClientState->pNetChannel)
		return false;

	int nTickbase = (TGlobalVars::pLocalEntity && TGlobalVars::pLocalEntity->IsAlive()) ? TGlobalVars::pLocalEntity->GetTickBase() : TIME_TO_TICKS(TInterfaces::Globals->flCurrentTime);

	if (nTickbase <= 0)
		return false;
	
	auto pNetChannel = (INetChannelInfo*)TInterfaces::ClientState->pNetChannel;

	if (!pNetChannel)
		return false;

	auto flOutGoing = pNetChannel->GetLatency(FLOW_OUTGOING);
	auto flIncoming = pNetChannel->GetLatency(FLOW_INCOMING);

	auto flCorrect = std::clamp(flOutGoing + flIncoming + TConvarManager::flLerp, 0.0f, 1.0f);

	auto flCurrentTime = TICKS_TO_TIME(nTickbase);

	auto flDelta = flCorrect - (flCurrentTime - flCorrectSimulationTime);

	if (fabs(flDelta) > 0.2f)
		return false;

	auto nExtraChoke = 0;

	if (TLocalEngine::bInFakeDuck)
		nExtraChoke = 14 - TInterfaces::ClientState->nChokedCommands;

	auto nServerTickCount = nExtraChoke + TInterfaces::Globals->iTickCount + TIME_TO_TICKS(flOutGoing + flIncoming);
	auto flDeadTime = (int)(TICKS_TO_TIME(nServerTickCount) - 0.2f);

	if (flCorrectSimulationTime < (float)flDeadTime)
		return false;

	return true;
}

void TAnimationInstance::ResetRecords() {

	PredictedRecord.bFilled = false;

	for (int Current = 0; Current < TDEF_MAXANIMRECORDS; Current++)
		AnimationRecords[Current].bFilled = false;

}

void TAnimationUpdate::OnNetUpdateEnd() {

	if (TEntityListener::vecEntities.empty())
		return;

	if (!TGlobalVars::pLocalEntity)
		return;

	for (auto CurrentObject : TEntityListener::vecEntities)
	{
		if (!CurrentObject->AnimationInstance)
			continue;

		if (!CurrentObject->pEntity || !CurrentObject->pEntity->IsPlayer() || CurrentObject->pEntity->GetClientClass()->nClassID != EClassIndex::CCSPlayer
			|| CurrentObject->pEntity == TGlobalVars::pLocalEntity || CurrentObject->nIndex == TGlobalVars::LocalIndex)
			continue;

		if (!CurrentObject->pEntity->IsAlive())
		{
			CurrentObject->AnimationInstance->ResetRecords();
			continue;
		}

		if (CurrentObject->pEntity->IsDormant())
		     CurrentObject->AnimationInstance->bDormant = true;

		UpdateAnimations(CurrentObject->AnimationInstance);

	}
}

void TAnimationUpdate::UpdateAnimations(TAnimationInstance* AnimationInstance) {

	bool bLagCompensation = TConvarManager::varLagCompensation.pBase->GetInt() != 0;

	if (bLagCompensation) {

		if (AnimationInstance->pAnimationEntity->GetSimulationTime() <= AnimationInstance->pAnimationEntity->GetOldSimulationTime())
			return;

		if (AnimationInstance->AnimationRecords[0].bFilled) {

			if (AnimationInstance->pAnimationEntity->GetSimulationTime() == AnimationInstance->AnimationRecords[0].flSimulationTime)
				return;

			if (AnimationInstance->pAnimationEntity->GetAnimationLayer(11)->flCycle == AnimationInstance->AnimationRecords[0].AnimationLayers[11].flCycle)
			{
				AnimationInstance->pAnimationEntity->GetSimulationTime() = AnimationInstance->pAnimationEntity->GetOldSimulationTime();
				return;
			}
		}
	}
	else {

		if (AnimationInstance->pAnimationEntity->GetSimulationTime() == AnimationInstance->pAnimationEntity->GetOldSimulationTime())
			return;

		if (AnimationInstance->AnimationRecords[0].bFilled) {
			if (AnimationInstance->pAnimationEntity->GetSimulationTime() == AnimationInstance->AnimationRecords[0].flSimulationTime)
				return;
		}
	}
	if (AnimationInstance->AnimationRecords[0].bFilled) {
		if ((AnimationInstance->pAnimationEntity->GetOrigin() - AnimationInstance->AnimationRecords[0].vecOrigin).Length2DSqr() > LAG_COMPENSATION_TELEPORTED_DISTANCE_SQR) 
		{
			AnimationInstance->bCanLagCompensate = false;

			for (int Current = 0; Current < TDEF_MAXANIMRECORDS; Current++)
				if (AnimationInstance->AnimationRecords[Current].bFilled)
					AnimationInstance->AnimationRecords[Current].bValid = false;
			

		}
		else
			AnimationInstance->bCanLagCompensate = true;
	}

	ValidateAndSortRecords(AnimationInstance);
	TAnimationCorrection::OnUpdateAnimation(AnimationInstance);
}

void TAnimationUpdate::TAnimationCorrection::UpdateLayersAndAnimationState(TAnimationInstance* AnimationInstance) {

	//removed
}

void TAnimationUpdate::TAnimationCorrection::PredictAnimations(TAnimationInstance* AnimationInstance) 
{
	//not complete

	auto pNetChannel = (INetChannelInfo*)TInterfaces::ClientState->pNetChannel;

	if (!pNetChannel)
		return;

	if (!AnimationInstance->AnimationRecords[1].bFilled)
		return;

	auto flOutGoing = pNetChannel->GetLatency(FLOW_OUTGOING);
	auto flIncoming = pNetChannel->GetLatency(FLOW_INCOMING);
    int nTickbase = TIME_TO_TICKS(TInterfaces::Globals->flCurrentTime);

	if (nTickbase <= 0)
		return;

	int iTicksToPredict = TIME_TO_TICKS(flOutGoing + flIncoming);

	AnimationInstance->AnimationRecords[0].nSimulationDifference = TIME_TO_TICKS(AnimationInstance->pAnimationEntity->GetSimulationTime() - AnimationInstance->AnimationRecords[1].flSimulationTime);
	
	if (TConvarManager::varLagCompensation.pBase->GetInt() != 0 && AnimationInstance->AnimationRecords[0].nSimulationDifference <= 0)
		return;

	if (iTicksToPredict <= AnimationInstance->AnimationRecords[0].nSimulationDifference)
		return; 
		

	if (iTicksToPredict <= AnimationInstance->AnimationRecords[1].nSimulationDifference)
		return;

	if (iTicksToPredict <= 0 || iTicksToPredict > 28)
		return;

	int iSimulationDifference = AnimationInstance->AnimationRecords[0].nSimulationDifference;

	Vector3D vecOriginDeltaPerTick = (AnimationInstance->AnimationRecords[0].vecOrigin - AnimationInstance->AnimationRecords[1].vecOrigin) / (float)iSimulationDifference;

	if (vecOriginDeltaPerTick.IsZero())
		return;

	float flDuckAmountPreviousTick =  AnimationInstance->AnimationRecords[1].flDuckAmount - AnimationInstance->AnimationRecords[0].flDuckAmount;

	 float flOldEFlags = AnimationInstance->pAnimationEntity->GetEFlags();
	 Vector3D vecOldAbsVelocity = AnimationInstance->pAnimationEntity->GetAbsVelocity();

	 AnimationInstance->pAnimationEntity->GetVelocity() = AnimationInstance->pAnimationEntity->GetVelocity() * TInterfaces::Globals->flIntervalPerTick;
	 AnimationInstance->pAnimationEntity->GetEFlags() &= ~0x1800;
	 AnimationInstance->pAnimationEntity->GetAbsVelocity() = AnimationInstance->AnimationRecords[0].vecVelocity;

	for (int iCurrentTick = 1; iCurrentTick <= iTicksToPredict; iCurrentTick++)
	{
		AnimationInstance->pAnimationEntity->GetSimulationTime() = AnimationInstance->AnimationRecords[0].flSimulationTime + TICKS_TO_TIME(iCurrentTick);

		TInterfaces::Globals->flCurrentTime = AnimationInstance->pAnimationEntity->GetSimulationTime();
		TInterfaces::Globals->flRealTime = AnimationInstance->pAnimationEntity->GetSimulationTime();
		TInterfaces::Globals->iFrameCount = TIME_TO_TICKS(AnimationInstance->pAnimationEntity->GetSimulationTime());
		TInterfaces::Globals->iTickCount = TIME_TO_TICKS(AnimationInstance->pAnimationEntity->GetSimulationTime());
		TInterfaces::Globals->flFrameTime = TInterfaces::Globals->flIntervalPerTick;
		TInterfaces::Globals->flAbsFrameTime = TInterfaces::Globals->flIntervalPerTick;
		TInterfaces::Globals->flInterpolationAmount = 0.0f;

		TAnimationUpdate::TAnimationCorrection::UpdateClientSideAnimations(AnimationInstance);

		AnimationInstance->pAnimationEntity->GetOrigin() += AnimationInstance->pAnimationEntity->GetVelocity();


	}	 
	AnimationInstance->pAnimationEntity->GetAbsVelocity() = vecOldAbsVelocity;
	 AnimationInstance->pAnimationEntity->GetEFlags() = flOldEFlags;
	AnimationInstance->AnimationRecords[0].vecVelocityFixed = AnimationInstance->pAnimationEntity->GetVelocity();
	AnimationInstance->AnimationRecords[0].vecOriginFixed = AnimationInstance->pAnimationEntity->GetOrigin();
	AnimationInstance->AnimationRecords[0].flCorrectSimulationTime = AnimationInstance->pAnimationEntity->GetSimulationTime();
	

	TEventLogger::PushString(std::to_string(iTicksToPredict));
}

bool TAnimationUpdate::TAnimationCorrection::IsPredictionRequired(TAnimationInstance* AnimationInstance) {

	if (TConvarManager::varLagCompensation.pBase->GetInt() == 0)
		return true;

	if (!TInterfaces::ClientState->pNetChannel)
		return false;

	int nTickbase = TIME_TO_TICKS(TInterfaces::Globals->flCurrentTime);
	
	if (nTickbase <= 0)
		return false;

	auto pNetChannel = (INetChannelInfo*)TInterfaces::ClientState->pNetChannel;

	if (!pNetChannel)
		return false;

	auto flOutGoing = pNetChannel->GetLatency(FLOW_OUTGOING);
	auto flIncoming = pNetChannel->GetLatency(FLOW_INCOMING);

	auto flCorrect = std::clamp(flOutGoing + flIncoming + TConvarManager::flLerp, 0.0f, 1.0f);

	auto flCurrentTime = TICKS_TO_TIME(nTickbase);

	auto flDelta = flCorrect - (flCurrentTime - AnimationInstance->pAnimationEntity->GetSimulationTime());

	if (fabs(flDelta) > 0.2f)
		return true;

	return false;

}
void TAnimationUpdate::TAnimationCorrection::OnUpdateAnimation(TAnimationInstance* AnimationInstance) {

	CalculateVelocity(AnimationInstance);
	static IGlobalVarsBase Globals;
	memcpy(&Globals, TInterfaces::Globals, sizeof IGlobalVarsBase);
	TAnimationCorrection::UpdateLayersAndAnimationState(AnimationInstance);
	memcpy(TInterfaces::Globals, &Globals, sizeof IGlobalVarsBase);

	AnimationInstance->AnimationRecords[0].vecOriginFixed = AnimationInstance->AnimationRecords[0].vecOrigin;
	AnimationInstance->AnimationRecords[0].vecVelocityFixed = AnimationInstance->AnimationRecords[0].vecVelocity;

	bool bPredictionRequired = TAnimationUpdate::TAnimationCorrection::IsPredictionRequired(AnimationInstance);

	bPredictionRequired = true;

	if (bPredictionRequired)
	{
		Vector3D vecOldVelocity = AnimationInstance->pAnimationEntity->GetVelocity();
		Vector3D vecOldOrigin = AnimationInstance->pAnimationEntity->GetOrigin();
		float flOldDuckAmount = AnimationInstance->pAnimationEntity->GetDuckAmount();
		float flOldSimulationTime = AnimationInstance->pAnimationEntity->GetSimulationTime();

		TAnimationCorrection::PredictAnimations(AnimationInstance);
		memcpy(TInterfaces::Globals, &Globals, sizeof IGlobalVarsBase);
		TGlobalVars::bSettingUpPredictionBones = true;
		TAnimationCorrection::SetupBones(AnimationInstance->pAnimationEntity, AnimationInstance->AnimationRecords[0].matModel, BONE_USED_BY_ANYTHING, AnimationInstance->pAnimationEntity->GetSimulationTime(), true);
		TGlobalVars::bSettingUpPredictionBones = false;

		for (int i = 0; i < MAXSTUDIOBONES; i++)
			AnimationInstance->vecBoneOrigin[i] = AnimationInstance->pAnimationEntity->GetOrigin() - AnimationInstance->matCached[i].GetOrigin();

		AnimationInstance->pAnimationEntity->GetOrigin() = vecOldOrigin;
		AnimationInstance->pAnimationEntity->GetDuckAmount() = flOldDuckAmount;
		AnimationInstance->pAnimationEntity->GetSimulationTime() = flOldSimulationTime;
		AnimationInstance->pAnimationEntity->GetVelocity() = vecOldVelocity;
	}
	else
	   TAnimationCorrection::SetupBones(AnimationInstance->pAnimationEntity, AnimationInstance->AnimationRecords[0].matModel, BONE_USED_BY_ANYTHING, AnimationInstance->pAnimationEntity->GetSimulationTime(), true);
}


void TAnimationUpdate::TAnimationCorrection::CalculateVelocity(TAnimationInstance* AnimationInstance) {
	//removed
}

void TAnimationUpdate::TAnimationCorrection::UpdateClientSideAnimations(TAnimationInstance* AnimationInstance) {

	if (AnimationInstance->pAnimationEntity->GetAnimationState()->m_nLastUpdateFrame > TInterfaces::Globals->iFrameCount - 1)
		AnimationInstance->pAnimationEntity->GetAnimationState()->m_nLastUpdateFrame = TInterfaces::Globals->iFrameCount - 1;

	bool bClientSideAnimation = AnimationInstance->pAnimationEntity->IsClientSideAnimation();
	AnimationInstance->pAnimationEntity->IsClientSideAnimation() = true;

	for (int iLayer = 0; iLayer < ANIMATION_LAYER_COUNT; iLayer++)
		AnimationInstance->pAnimationEntity->GetAnimationLayer(iLayer)->pOwner = AnimationInstance->pAnimationEntity;
	
	TGlobalVars::bUpdatingAnimations = true;
	AnimationInstance->pAnimationEntity->UpdateClientSideAnimations();
	TGlobalVars::bUpdatingAnimations = false;

	AnimationInstance->pAnimationEntity->IsClientSideAnimation() = bClientSideAnimation;
}

void TAnimationUpdate::TAnimationCorrection::SetupBones(CBaseEntity* pBaseEntity, matrix3x4_t* matBoneToWorld, int BoneMask, float flCurrentTime, bool bDisableInterpolation) 
{
	//removed
}

void TAnimationUpdate::ValidateAndSortRecords(TAnimationInstance* AnimationInstance) {

	static TAnimationRecord Records[TDEF_MAXANIMRECORDS];

	for (int Current = 0; Current < TDEF_MAXANIMRECORDS; Current++) {
		Records[Current].bFilled = false;
		Records[Current].CopyRecord(AnimationInstance->AnimationRecords[Current]);
	}

	for (int Current = 1; Current < TDEF_MAXANIMRECORDS; Current++) {
		AnimationInstance->AnimationRecords[Current].bFilled = false;
		AnimationInstance->AnimationRecords[Current].CopyRecord(Records[Current - 1]);
	}

	AnimationInstance->AnimationRecords[0].FillRecord();
}

TAnimationRecord* TAnimationUpdate::GetLatestValidRecord(TAnimationInstance* AnimationInstance) {

	for (int Current = 0; Current < TDEF_MAXANIMRECORDS; Current++)
	{
		if (AnimationInstance->AnimationRecords[Current].IsValid())
			return &AnimationInstance->AnimationRecords[Current];
	}
		
	if (AnimationInstance->AnimationRecords[0].bFilled)
		return &AnimationInstance->AnimationRecords[0];

	return nullptr;
}
TAnimationRecord* TAnimationUpdate::GetOldestValidRecord(TAnimationInstance* AnimationInstance) {

	if (TConvarManager::varLagCompensation.pBase->GetInt() == 0)
		return TAnimationUpdate::GetLatestValidRecord(AnimationInstance);

	for (int Current = TDEF_MAXANIMRECORDS - 1; Current >= 0; Current--)
	{
		if (AnimationInstance->AnimationRecords[Current].IsValid())
			return &AnimationInstance->AnimationRecords[Current];
	}

	return TAnimationUpdate::GetLatestValidRecord(AnimationInstance);
}

void TTempAnimationRecord::Apply(TAnimationRecord* AnimationRecord) {
	pAnimationEntity->GetSimulationTime() = AnimationRecord->flCorrectSimulationTime;
	pAnimationEntity->GetFlags() = AnimationRecord->Flags;
	pAnimationEntity->GetOrigin() = AnimationRecord->vecOriginFixed;
	pAnimationEntity->SetAbsOrigin(AnimationRecord->vecOriginFixed);
	pAnimationEntity->GetVelocity() = AnimationRecord->vecVelocityFixed;
	pAnimationEntity->GetAbsVelocity() = AnimationRecord->vecVelocityFixed;
	memcpy(pAnimationEntity->GetAnimationOverlays().Base(), AnimationRecord->AnimationLayers, sizeof CAnimationLayer * 13);
	memcpy(pAnimationEntity->GetCachedBoneData().Base(), AnimationRecord->matModel, sizeof(matrix3x4_t) * pAnimationEntity->GetCachedBoneData().Count());
}

void TTempAnimationRecord::ApplyMatrix(matrix3x4_t* matModel)
{
	memcpy(pAnimationEntity->GetCachedBoneData().Base(), matModel, sizeof(matrix3x4_t) * pAnimationEntity->GetCachedBoneData().Count());
}


void TTempAnimationRecord::Store(CBaseEntity* pBaseEntity) {
	pAnimationEntity = pBaseEntity;
	flSimulationTime = pBaseEntity->GetSimulationTime();
	Flags = pBaseEntity->GetFlags();
	vecOrigin = pBaseEntity->GetOrigin();
	vecAbsOrigin = pBaseEntity->GetAbsOrigin();
	vecVelocity = pBaseEntity->GetVelocity();
	vecAbsVelocity = pBaseEntity->GetAbsVelocity();
	memcpy(AnimationLayers, pBaseEntity->GetAnimationOverlays().Base(), sizeof CAnimationLayer * 13);
	memcpy(matBackup, pBaseEntity->GetCachedBoneData().Base(), sizeof(matrix3x4_t) * pBaseEntity->GetCachedBoneData().Count());
}

void TTempAnimationRecord::Restore() {
	pAnimationEntity->GetSimulationTime() = flSimulationTime;
	pAnimationEntity->GetFlags() = Flags;
	pAnimationEntity->GetOrigin() = vecOrigin;
	pAnimationEntity->SetAbsOrigin(vecAbsOrigin);
	pAnimationEntity->GetVelocity() = vecVelocity;
	pAnimationEntity->GetAbsVelocity() = vecAbsVelocity;
	memcpy(pAnimationEntity->GetAnimationOverlays().Base(), AnimationLayers, sizeof CAnimationLayer * 13);
	memcpy(pAnimationEntity->GetCachedBoneData().Base(), matBackup, sizeof(matrix3x4_t) * pAnimationEntity->GetCachedBoneData().Count());
}