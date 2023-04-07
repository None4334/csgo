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
#include "misc.h"
#include "prediction.h"
#include "../handlers/cmdmgr.h"

void TMisc::TOther::PreserveDeathNotices(CBaseEntity* pLocalPlayer) {

	static auto uOffsetClearDeathList = std::uintptr_t(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 EC 0C 53 56 8B 71")));

	static uintptr_t* pHudDeathNotice = nullptr;

	if (!pLocalPlayer || !pLocalPlayer->IsAlive())
	{
		pHudDeathNotice = nullptr;
		return;
	}

	if (!pHudDeathNotice)
	{
		pHudDeathNotice = TUtil::FindHudElement(TSTRING("CCSGO_HudDeathNotice"));
		return;
	}

	if (!pHudDeathNotice)
		return;

	PFLOAT pNoticeExpireTime = (PFLOAT)((DWORD)(pHudDeathNotice) + 0x50);

	if (pNoticeExpireTime)
		*pNoticeExpireTime = TVariables::TMisc::PreserveDeathNotice.bEnabled ? FLT_MAX : 1.5f;

	if (bClearNotices)
		((void(__thiscall*)(DWORD))(uOffsetClearDeathList))((DWORD)(pHudDeathNotice)-20);

	bClearNotices = false;
}


void TMisc::TMovement::FixMovement(Vector3D& angViewWish, CUserCmd* pCmd)
{
	Vector3D vecViewForward, vecViewRight, vecViewUp, vecCmdForward, vecCmdRight, vecCmdUp;
	Vector3D vecViewAngles = pCmd->angViewPoint;
	vecViewAngles.Normalized();

	TMath::AngleVectors(angViewWish, &vecViewForward, &vecViewRight, &vecViewUp);
	TMath::AngleVectors(vecViewAngles, &vecCmdForward, &vecCmdRight, &vecCmdUp);

	float v8 = sqrtf((vecViewForward.x * vecViewForward.x) + (vecViewForward.y * vecViewForward.y));
	float v10 = sqrtf((vecViewRight.x * vecViewRight.x) + (vecViewRight.y * vecViewRight.y));
	float v12 = sqrtf(vecViewUp.z * vecViewUp.z);

	Vector3D vecNormalViewForward((1.f / v8) * vecViewForward.x, (1.f / v8) * vecViewForward.y, 0.f);
	Vector3D vecNormalViewRight((1.f / v10) * vecViewRight.x, (1.f / v10) * vecViewRight.y, 0.f);
	Vector3D vecNormalViewUp(0.f, 0.f, (1.f / v12) * vecViewUp.z);

	float v14 = sqrtf((vecCmdForward.x * vecCmdForward.x) + (vecCmdForward.y * vecCmdForward.y));
	float v16 = sqrtf((vecCmdRight.x * vecCmdRight.x) + (vecCmdRight.y * vecCmdRight.y));
	float v18 = sqrtf(vecCmdUp.z * vecCmdUp.z);

	Vector3D vecNormalForward((1.f / v14) * vecCmdForward.x, (1.f / v14) * vecCmdForward.y, 0.f);
	Vector3D vecNormalRight((1.f / v16) * vecCmdRight.x, (1.f / v16) * vecCmdRight.y, 0.f);
	Vector3D vecNormalUp(0.f, 0.f, (1.f / v18) * vecCmdUp.z);

	float v22 = vecNormalViewForward.x * pCmd->flForwardMove;
	float v26 = vecNormalViewForward.y * pCmd->flForwardMove;
	float v28 = vecNormalViewForward.z * pCmd->flForwardMove;
	float v24 = vecNormalViewRight.x * pCmd->flSideMove;
	float v23 = vecNormalViewRight.y * pCmd->flSideMove;
	float v25 = vecNormalViewRight.z * pCmd->flSideMove;
	float v30 = vecNormalViewUp.x * pCmd->flUpMove;
	float v27 = vecNormalViewUp.z * pCmd->flUpMove;
	float v29 = vecNormalViewUp.y * pCmd->flUpMove;

	pCmd->flForwardMove = ((((vecNormalForward.x * v24) + (vecNormalForward.y * v23)) + (vecNormalForward.z * v25))
		+ (((vecNormalForward.x * v22) + (vecNormalForward.y * v26)) + (vecNormalForward.z * v28)))
		+ (((vecNormalForward.y * v30) + (vecNormalForward.x * v29)) + (vecNormalForward.z * v27));
	pCmd->flSideMove = ((((vecNormalRight.x * v24) + (vecNormalRight.y * v23)) + (vecNormalRight.z * v25))
		+ (((vecNormalRight.x * v22) + (vecNormalRight.y * v26)) + (vecNormalRight.z * v28)))
		+ (((vecNormalRight.x * v29) + (vecNormalRight.y * v30)) + (vecNormalRight.z * v27));
	pCmd->flUpMove = ((((vecNormalUp.x * v23) + (vecNormalUp.y * v24)) + (vecNormalUp.z * v25))
		+ (((vecNormalUp.x * v26) + (vecNormalUp.y * v22)) + (vecNormalUp.z * v28)))
		+ (((vecNormalUp.x * v30) + (vecNormalUp.y * v29)) + (vecNormalUp.z * v27));

	pCmd->flForwardMove = std::clamp(pCmd->flForwardMove, -TConvarManager::varForwardSpeed.pBase->GetFloat(), TConvarManager::varForwardSpeed.pBase->GetFloat());
	pCmd->flSideMove = std::clamp(pCmd->flSideMove, -TConvarManager::varSideSpeed.pBase->GetFloat(), TConvarManager::varSideSpeed.pBase->GetFloat());
	pCmd->flUpMove = std::clamp(pCmd->flUpMove, -TConvarManager::varUpSpeed.pBase->GetFloat(), TConvarManager::varUpSpeed.pBase->GetFloat());
}

void TMisc::TMovement::AirStrafe(CUserCmd* pCmd, CBaseEntity* pLocalPlayer) {
	//removed
}

void TMisc::TMovement::Quickstop(CUserCmd* pCmd, CBaseEntity* pLocalPlayer) {

	if (!(pLocalPlayer->GetFlags() & FL_ONGROUND && TPrediction::iOldFlags & FL_ONGROUND))
		return;

	bool bPressedMoveKey = pCmd->iButtons & IN_FORWARD || pCmd->iButtons & IN_MOVELEFT || pCmd->iButtons & IN_BACK || pCmd->iButtons & IN_MOVERIGHT || pCmd->iButtons & IN_JUMP;

	if (bPressedMoveKey)
		return;

	Vector3D vecVelocity = pLocalPlayer->GetVelocity();

	if (vecVelocity.Length2D() > 20.0f)
	{
		Vector3D vecDirection;
		Vector3D vecRealView;

		TMath::VectorAngles(vecVelocity, vecDirection);
		TInterfaces::EngineClient->GetViewAngles(vecRealView);

		vecDirection.y = vecRealView.y - vecDirection.y;

		Vector3D vecForward;
		TMath::AngleVectors(vecDirection, &vecForward);

		pCmd->flForwardMove = (vecForward * (-TConvarManager::varForwardSpeed.pBase->GetFloat())).x;
		pCmd->flSideMove = (vecForward * (-TConvarManager::varSideSpeed.pBase->GetFloat())).y;
	}
}

void TMisc::TMovement::BunnyHop(CUserCmd* pCmd, CBaseEntity* pLocalPlayer) {

	if (!TVariables::TMisc::BunnyHop.bEnabled)
		return;

	if (pLocalPlayer->GetMoveType() == MOVETYPE_LADDER)
		return;

	static auto bLastJumped = false;
	static auto bShouldJump = false;

	if (!bLastJumped && bShouldJump)
	{
		bShouldJump = false;
		pCmd->iButtons |= IN_JUMP;
	}
	else if (pCmd->iButtons & IN_JUMP)
	{
		if (pLocalPlayer->GetFlags() & FL_ONGROUND || !pLocalPlayer->GetVelocity().z)
		{
			bLastJumped = true;
			bShouldJump = true;
		}
		else
		{
			pCmd->iButtons &= ~IN_JUMP;
			bLastJumped = false;
		}
	}
	else
	{
		bLastJumped = false;
		bShouldJump = false;
	}
}


void TMisc::TMovement::InfiniteDuck(CUserCmd* pCmd) {

	if (!TVariables::TMisc::InfiniteDuck.bEnabled)
		return;

	if (TInterfaces::GetGameRules()->IsValveDS() && TVariables::TMisc::AntiUntrusted.bEnabled)
		return;

	pCmd->iButtons |= IN_BULLRUSH;
}

void TMisc::TBuyBot::Run(bool bReset) {

	if (!TVariables::TMisc::BuyBot.bEnabled)
		return;

	static bool bRunBuyBot = false;

	if (bReset) {
		bRunBuyBot = true;
		return;
	}

	if (!bRunBuyBot)
		return;

	bRunBuyBot = false;

	if (TVariables::TMisc::SecondaryWeapon.Value != 0) {
		switch (TVariables::TMisc::SecondaryWeapon.Value)
		{
		case 1:
			TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy glock"));
			break;
		case 2:
			TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy hkp2000"));
			break;
		case 3:
			TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy usp_silencer"));
			break;
		case 4:
			TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy elite"));
			break;
		case 5:
			TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy p250"));
			break;
		case 6:
			TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy tec9"));
			break;
		case 7:
			TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy fn57"));
			break;
		case 8:
			TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy deagle"));
			break;
		default:
			break;
		}
	}

	if (TVariables::TMisc::Defuser.bEnabled)
		TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy defuser"));

	if (TVariables::TMisc::PrimaryWeapon.Value != 0) {
		switch (TVariables::TMisc::PrimaryWeapon.Value) 
		{
		case 1:
			TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy galilar"));
			break;
		case 2:
			TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy famas"));
			break;
		case 3:
	         TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy ak47"));
			 break;
		case 4:
		     TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy m4a1"));
			 break;
	    case 5:
		     TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy m4a1_silencer"));
			 break;
	    case 6:
		     TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy ssg08"));
			 break;
	    case 7:
		     TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy aug"));
			 break;
	    case 8:
	 	     TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy sg556"));
			 break;
	    case 9:
		     TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy awp"));
			 break;
    	case 10:
		     TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy scar20"));
			 break;
     	case 11:
		     TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy g3sg1"));
			 break;
    	case 12:
		     TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy nova"));
			 break;
    	case 13:
		     TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy xm1014"));
			 break;
    	case 14:
		     TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy mag7"));
			 break;
    	case 15:
	    	TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy m249"));
			break;
	    case 16:
	    	TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy negev"));
			break;
    	case 17:
     		TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy mac10"));
			break;
    	case 18:
	    	TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy mp9"));
			break;
    	case 19:
	    	TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy mp7"));
			break;
	    case 20:
	    	TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy ump45"));
			break;
	    case 21:
	    	TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy p90"));
			break;
    	case 22:
	    	TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy bizon"));
			break;
		default:
			break;
		}
	}

	if (TVariables::TMisc::ArmorType.Value != 0) {
		switch (TVariables::TMisc::ArmorType.Value)
		{
		case 1:
			TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy vest"));
		break;
		case 2:
			TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy vesthelm"));
		break;
		case 3:
			TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy heavyarmor"));
		break;
		default:
			break;
		}
	}

	if (TVariables::TMisc::MapGrenades.mapValues[0])
		TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy molotov"));
	
	if (TVariables::TMisc::MapGrenades.mapValues[1])
		TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy incgrenade"));
		
	if (TVariables::TMisc::MapGrenades.mapValues[2])
		TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy hegrenade"));
	
	if (TVariables::TMisc::MapGrenades.mapValues[3])
		TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy smokegrenade"));
	
	if (TVariables::TMisc::MapGrenades.mapValues[4])
		TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy flashbang"));
	
	if (TVariables::TMisc::MapGrenades.mapValues[5])
		TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy decoy"));
	
	if (TVariables::TMisc::Taser.bEnabled)
		TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("buy taser"));
}
