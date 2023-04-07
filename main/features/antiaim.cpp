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
#include "antiaim.h"
#include "prediction.h"
#include "../handlers/cmdmgr.h"
#include "../handlers/packethandler.h"

void TAntiAim::Run(CUserCmd* pCmd, CBaseEntity* pLocalPlayer, bool& bSendPacket) {

	CBaseCombatWeapon* pBaseWeapon = TBaseEntity::GetWeapon(pLocalPlayer);

	CCSWeaponData* pWeaponData = nullptr;

	if (pBaseWeapon)
		pWeaponData = TBaseEntity::GetWeaponData(pBaseWeapon);

	if (!pBaseWeapon || !pWeaponData)
		return;

	if (TInterfaces::GetGameRules()->IsFreezePeriod())
		return;


	Fakelag(pCmd, pLocalPlayer, bSendPacket);
	TPacketHandler::OnCommandEnd(pCmd, pLocalPlayer, bSendPacket, true);

	if (pLocalPlayer->GetMoveType() == MOVETYPE_LADDER ||
		pLocalPlayer->GetMoveType() == MOVETYPE_NOCLIP || 
		TPrediction::iOldMoveType == MOVETYPE_LADDER ||
		TPrediction::iOldMoveType == MOVETYPE_NOCLIP)
		return;

	RegisterMovement(pCmd, pLocalPlayer);
	RegisterDuck(pCmd, pLocalPlayer, bSendPacket);

	if (pBaseWeapon->GetItemDefinitionIndex() != WEAPON_HEALTHSHOT)
	{
		if (pWeaponData->nWeaponType == WEAPONTYPE_GRENADE)
		{
			CBaseCSGrenade* pGrenade = (CBaseCSGrenade*)pBaseWeapon;

			if (pGrenade->GetThrowTime())
				return;
		}
		else if (pWeaponData->nWeaponType == WEAPONTYPE_KNIFE)
		{
			if ((pCmd->iButtons & IN_ATTACK || pCmd->iButtons & IN_SECOND_ATTACK))
				return;
		}
		else if (pWeaponData->IsGun())
		{
			if (pBaseWeapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
			{
				const float flServerTime = TICKS_TO_TIME(pLocalPlayer->GetTickBase());

				CWeaponCSBase* pWeaponBase = static_cast<CWeaponCSBase*>(pBaseWeapon);

				float flFireReadyTimeDifference = pWeaponBase->GetFireReadyTime() - flServerTime;

				if (pCmd->iButtons & IN_ATTACK && flFireReadyTimeDifference < 0.0f)
					return;
			}
			else
			{
				if (pCmd->iButtons & IN_ATTACK)
					return;
			}
		}
	}


	if (TVariables::TAntiAim::TFakeAngles::bEnabled) {
		RegisterPitch(pCmd, pLocalPlayer, bSendPacket);
		RegisterYaw(pCmd, pLocalPlayer, bSendPacket);
		RegisterRoll(pCmd, pLocalPlayer, bSendPacket);
	}

}

void TAntiAim::Fakelag(CUserCmd* pCmd, CBaseEntity* pLocalPlayer, bool& bSendPacket) {

	static bool bWasHigher = false;
	static Vector3D vecLastVelocity;
	static float flLastVelocityDelta = 0.0f;

	int iCurrentChoke = 0;

	if (!TVariables::TAntiAim::TFakeAngles::bEnabled && !TVariables::TAntiAim::TFakelag::bFakelagEnabled)
		return;

	if (TVariables::TAntiAim::TFakeAngles::bEnabled)
		iCurrentChoke = 2;
	
	if (TVariables::TAntiAim::TFakelag::bFakelagEnabled)
		iCurrentChoke = TVariables::TAntiAim::TFakelag::iFakelagValue;

	float flAddVariance = !(pLocalPlayer->GetFlags() & FL_ONGROUND) ? 2.F : 0.F;
	if ((pLocalPlayer->GetVelocity() - vecLastVelocity).Length2D() > flLastVelocityDelta + flAddVariance)
	{
		if (!bWasHigher)
		{
			iCurrentChoke = 2;
			bWasHigher = true;
		}
	}
	else {
		if (bWasHigher)
		{
			iCurrentChoke = 2;
			bWasHigher = false;
		}
	}

	flLastVelocityDelta = (pLocalPlayer->GetVelocity() - vecLastVelocity).Length2D();
	vecLastVelocity = pLocalPlayer->GetVelocity();

	bSendPacket = TInterfaces::ClientState->nChokedCommands >= iCurrentChoke;
}

void TAntiAim::RegisterPitch(CUserCmd* pCmd, CBaseEntity* pLocalPlayer, bool& bSendPacket) {

	switch (TVariables::TAntiAim::TFakeAngles::iPitchMode)
	{
	case 1:
		pCmd->angViewPoint.x = 89.f;
		break;
	case 2:
		pCmd->angViewPoint.x = 0.f;
		break;
	case 3:
		pCmd->angViewPoint.x = -89.f;
		break;
	default:
		break;
	}
}

void TAntiAim::RegisterYaw(CUserCmd* pCmd, CBaseEntity* pLocalPlayer, bool& bSendPacket) 
{

	if (pCmd->iButtons & IN_USE && !pLocalPlayer->IsDefusing())
		return;

	float flFinalYaw = pCmd->angViewPoint.y;

	TCommandInfo* CmdObject = TCommandManager::GetCommandData(pCmd->iCommandNumber);

	if (!CmdObject)
		return;

	static bool bFlipJitter = false;

	if (bSendPacket == true)
		bFlipJitter = !bFlipJitter;

	switch (TVariables::TAntiAim::TFakeAngles::iYawMode)
	{
	case 1:
	case 3:
		flFinalYaw += TVariables::TAntiAim::TFakeAngles::iYawValue;
		break;
	case 2:
	case 4:
	{
		float flBestFovDistance = 180;
		Vector3D vecTargetAimPos;

		for (auto& CurrentObject : TEntityListener::vecEntities)
		{
			if (CurrentObject->nClass != (int)EClassIndex::CCSPlayer)
				continue;

			if (!CurrentObject->pEntity->IsAlive())
				continue;

			if (!TGlobalVars::pLocalEntity->IsEnemy(CurrentObject->pEntity))
				continue;

			Vector3D vecHeadPosition = TBaseEntity::GetBonePosition(CurrentObject,BONE_HEAD);

			if (vecHeadPosition.IsZero())
				continue;

			Vector3D vecAimAngle= TMath::CalculateAngle(CmdObject->vecEyePosition, vecHeadPosition);
		
			float flFov = TMath::GetFov(CmdObject->Data.UserCmd.angViewPoint,vecAimAngle);
			
			if (flFov < flBestFovDistance)
			{
				flBestFovDistance = flFov;
				vecTargetAimPos = vecAimAngle;
			}
		}

		if (!vecTargetAimPos.IsZero())
			flFinalYaw = vecTargetAimPos.y;

		flFinalYaw += TVariables::TAntiAim::TFakeAngles::iYawValue;
	}
		break;
	default:
		break;
	}

	if (TVariables::TAntiAim::TFakeAngles::iYawMode >= 3)
		flFinalYaw += 180.f;

	switch (TVariables::TAntiAim::TFakeAngles::iYawJitterMode)
	{
	case 1: {
		if (bFlipJitter)
			flFinalYaw += TVariables::TAntiAim::TFakeAngles::iYawJitterValue;
		break;
	}
	case 2: {
		if (bFlipJitter)
			flFinalYaw += TVariables::TAntiAim::TFakeAngles::iYawJitterValue;
		else
			flFinalYaw -= TVariables::TAntiAim::TFakeAngles::iYawJitterValue;
		break;
	}
	default:
		break;
	}

	if (bSendPacket == false) {
		float flMaxDesyncDelta = pLocalPlayer->GetMaxAnimationDelta();

		if (flMaxDesyncDelta < 0.0f)
			flMaxDesyncDelta *= -1.f;

		if (TVariables::TAntiAim::TFakeAngles::iFakeMode > 0) {
			const float flYawMax = flMaxDesyncDelta;
			if (TVariables::TAntiAim::TFakeAngles::iFakeMode == 2) {
				if (TPrediction::vecOldVelocity.Length2D() > 4.0f)
					flMaxDesyncDelta = 0.0f;
			}

			float flFakeAmount = flMaxDesyncDelta + TVariables::TAntiAim::TFakeAngles::iFakeValue;

			if (TVariables::TAntiAim::TFakeAngles::Fake.KeyState.bState)
				flFinalYaw -= flFakeAmount;
			else
				flFinalYaw += flFakeAmount;

			if (TVariables::TAntiAim::TFakeAngles::iFakeMode == 1 && !pCmd->IsTryingToMove())
			{
				static bool bSwitch = false;

				if (bSwitch)
					pCmd->flSideMove += 8.0f;
				else
					pCmd->flSideMove -= 8.0f;

				bSwitch = !bSwitch;
			}
		}
	}
	flFinalYaw = TMath::NormalizeYaw(flFinalYaw);

	//flFinalYaw = TMath::NormalizeYaw(flFinalYaw);

	pCmd->angViewPoint.y = flFinalYaw;
}

void TAntiAim::RegisterRoll(CUserCmd* pCmd, CBaseEntity* pLocalPlayer, bool& bSendPacket) {

}

void TAntiAim::RegisterMovement(CUserCmd* pCmd, CBaseEntity* pLocalPlayer) {

	if (!TVariables::TAntiAim::TOthers::Slowmotion.KeyState.bState)
		return;

	float flDesiredSpeed = TVariables::TAntiAim::TOthers::iSlowmotionSpeed;
	float flMinimalSpeed = std::sqrt((pCmd->flForwardMove * pCmd->flForwardMove) + (pCmd->flSideMove * pCmd->flSideMove));

	if (flMinimalSpeed <= 0)
		return;

	if (pCmd->iButtons & IN_DUCK)
		flDesiredSpeed = flDesiredSpeed * 2.94117647;

	if (flMinimalSpeed <= flDesiredSpeed)
		return;

	float flSpeedFactor = flDesiredSpeed / flMinimalSpeed;
	pCmd->flForwardMove = pCmd->flForwardMove * flSpeedFactor;
	pCmd->flSideMove = pCmd->flSideMove * flSpeedFactor;
}

void TAntiAim::RegisterDuck(CUserCmd* pCmd, CBaseEntity* pLocalPlayer, bool& bSendPacket) 
{
	if ((pLocalPlayer->GetFlags() & FL_FROZEN) || TInterfaces::GetGameRules()->IsFreezePeriod())
		return;

	if (!TVariables::TAntiAim::TOthers::Fakeduck.KeyState.bState)
	{
		TLocalEngine::bInFakeDuck = false;
		bVisualDuck = false;
		return;
	}

	bool bShouldFakeDuck = TVariables::TAntiAim::TOthers::Fakeduck.KeyState.bState && (pLocalPlayer->GetFlags() & FL_ONGROUND);

	if (bShouldFakeDuck)
		bVisualDuck = true;
	else if (pLocalPlayer->GetDuckAmount() == 0.0f || pLocalPlayer->GetDuckAmount() >= 1.0f)
		bVisualDuck = false;

	TLocalEngine::bInFakeDuck = bShouldFakeDuck;
	if (!TLocalEngine::bInFakeDuck && bShouldFakeDuck)
	{
		TLocalEngine::bInFakeDuck = true;
		if (TInterfaces::ClientState->nChokedCommands)
			TLocalEngine::bInFakeDuck = false;
		else
			pCmd->iButtons &= ~IN_DUCK;

		return;
	}
	else if (!bShouldFakeDuck)
	{
		float m_flAwaitedDuck = (pCmd->iButtons & IN_DUCK) ? 1.0f : 0.0f;
		if (m_flAwaitedDuck != pLocalPlayer->GetDuckAmount())
		{
			pCmd->iButtons |= IN_BULLRUSH;

			if (m_flAwaitedDuck < pLocalPlayer->GetDuckAmount())
				pCmd->iButtons &= ~IN_DUCK;
			else
				pCmd->iButtons |= IN_DUCK;

			TLocalEngine::bInFakeDuck = true;
		}

		if (!TLocalEngine::bInFakeDuck)
			return;
	}
	else
	{
		pCmd->iButtons |= IN_BULLRUSH;

		if (TInterfaces::ClientState->nChokedCommands < 7)
			pCmd->iButtons &= ~IN_DUCK;
		else
			pCmd->iButtons |= IN_DUCK;
	}

	if (!bShouldFakeDuck)
	{
		TLocalEngine::bInFakeDuck = false;
		return;
	}

}