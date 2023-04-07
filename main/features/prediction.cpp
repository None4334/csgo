#include "../../includes.h"
#include "../handlers/localengine.h"
#include "../handlers/entityhandler.h"
#include "../gvars.h"
#include "../sdk.h"
#include "../math.h"
#include "../handlers/convarmanager.h"
#include "../ui/vars.h"
#include "prediction.h"

void TPrediction::Initialize()
{
	bOverrideModifier = true;

	if (!uPredictionRandomSeed)
    	uPredictionRandomSeed = *reinterpret_cast<unsigned int**>(TUtil::FindPattern(CLIENT_STR, TSTRING("8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04")) + 0x2);
	
	if (!pPredictionPlayer)
	     pPredictionPlayer = *reinterpret_cast<CBaseEntity***>(TUtil::FindPattern(CLIENT_STR, TSTRING("89 35 ? ? ? ? F3 0F 10 48 20")) + 0x2);

	if (flVelocityModifier < 1.f)
		TInterfaces::Prediction->DoPred();
	
	if (TInterfaces::ClientState->iDeltaTick > 0)
		TInterfaces::Prediction->Update(TInterfaces::ClientState->iDeltaTick, 
			TInterfaces::ClientState->iDeltaTick > 0, TInterfaces::ClientState->iLastCommandAck,
			TInterfaces::ClientState->iLastOutgoingCommand + TInterfaces::ClientState->nChokedCommands);
}

void TPrediction::Start(CUserCmd* pCmd, CBaseEntity* pLocal) {

	if (!pLocal->IsAlive())
		return;

	Initialize();

	iOldMoveType = pLocal->GetMoveParent();
	iOldFlags = pLocal->GetFlags();
	vecOldVelocity = pLocal->GetVelocity();

	*pLocal->GetCurrentCommand() = pCmd;
	pLocal->GetLastCommand() = *pCmd;

	*uPredictionRandomSeed = pCmd->iRandomSeed;
	*pPredictionPlayer = pLocal;

	flOldCurrentTime = TInterfaces::Globals->flCurrentTime;
	flOldFrameTime = TInterfaces::Globals->flFrameTime;
	iOldTickCount = TInterfaces::Globals->iTickCount;

	const int iOldTickBase = pLocal->GetTickBase();
	const bool bOldIsFirstPrediction = TInterfaces::Prediction->bIsFirstTimePredicted;
	const bool bOldInPrediction = TInterfaces::Prediction->bInPrediction;

	TInterfaces::Globals->flCurrentTime = TICKS_TO_TIME(GetTickBase(pCmd, pLocal));
	TInterfaces::Globals->flFrameTime = TInterfaces::Prediction->bEnginePaused ? 0.f : TICK_INTERVAL;
	TInterfaces::Globals->iTickCount = GetTickBase(pCmd, pLocal);

	TInterfaces::Prediction->bIsFirstTimePredicted = false;
	TInterfaces::Prediction->bInPrediction = true;

	pCmd->iButtons |= pLocal->GetButtonForced();
	pCmd->iButtons &= ~(pLocal->GetButtonDisabled());

	TInterfaces::GameMovement->StartTrackPredictionErrors(pLocal);

	const int iButtons = pCmd->iButtons;
	const int nLocalButtons = *pLocal->GetButtons();
	const int nButtonsChanged = iButtons ^ nLocalButtons;

	pLocal->GetButtonLast() = nLocalButtons;
	*pLocal->GetButtons() = iButtons;
	pLocal->GetButtonPressed() = nButtonsChanged & iButtons;
	pLocal->GetButtonReleased() = nButtonsChanged & (~iButtons);
	TInterfaces::Prediction->CheckMovingGround(pLocal, TInterfaces::Globals->flFrameTime);
	TInterfaces::Prediction->SetLocalViewAngles(pCmd->angViewPoint);

	if (pLocal->PhysicsRunThink(THINK_FIRE_ALL_FUNCTIONS))
		pLocal->PreThink();

	if (int* iNextThinkTick = pLocal->GetNextThinkTick(); *iNextThinkTick > 0 && *iNextThinkTick <= GetTickBase(pCmd, pLocal))
	{
		*iNextThinkTick = TICK_NEVER_THINK;

		pLocal->Think();
	}

	TInterfaces::MoveHelper->SetHost(pLocal);

	TInterfaces::Prediction->SetupMove(pLocal, pCmd, TInterfaces::MoveHelper, &moveData);
	TInterfaces::GameMovement->ProcessMovement(pLocal, &moveData);

	TInterfaces::Prediction->FinishMove(pLocal, pCmd, &moveData);
	TInterfaces::MoveHelper->ProcessImpacts();

	pLocal->PostThink();

	CBaseCombatWeapon* pBaseWeapon = TBaseEntity::GetWeapon(pLocal);

	if (pBaseWeapon)
	{

		Vector3D vecBackupAbsOrigin = pLocal->GetAbsOrigin();
		Vector3D vecBackupAbsVelocity = pLocal->GetAbsVelocity();

		pLocal->SetAbsOrigin(pLocal->GetOrigin());
		pLocal->GetAbsVelocity() = pLocal->GetVelocity();

		pBaseWeapon->UpdateAccuracyPenalty();

		pLocal->SetAbsOrigin(vecBackupAbsOrigin);
		pLocal->GetAbsVelocity() = vecBackupAbsVelocity;
	}
	
	pLocal->GetTickBase() = iOldTickBase;
	TInterfaces::Prediction->bInPrediction = bOldInPrediction;
	TInterfaces::Prediction->bIsFirstTimePredicted = bOldIsFirstPrediction;
}

void TPrediction::End(CUserCmd* pCmd, CBaseEntity* pLocal) {

	TInterfaces::GameMovement->FinishTrackPredictionErrors(pLocal);

	TInterfaces::MoveHelper->SetHost(nullptr);

	TInterfaces::Globals->flCurrentTime = flOldCurrentTime;
	TInterfaces::Globals->flFrameTime = flOldFrameTime;
	TInterfaces::Globals->iTickCount = iOldTickCount;

	*pLocal->GetCurrentCommand() = nullptr;
	*uPredictionRandomSeed = -1;

	*pPredictionPlayer = nullptr;

	TInterfaces::GameMovement->Reset();

	bOverrideModifier = false;
}

float TPrediction::GetFixedVelocityModifier(CBaseEntity* pBasePlayer, int nCommandNumber, bool bBefore)
{
	float flVelocityModifier = TLocalEngine::flVelocityModifier;
	int iDelta = nCommandNumber - TLocalEngine::iVelocityModifierTick;

	if (bBefore)
		--iDelta;

	if (iDelta < 0 || TLocalEngine::flVelocityModifier == 1.0f)
		flVelocityModifier = pBasePlayer->GetVelocityModifier();
	else if (iDelta)
	{
		flVelocityModifier = TLocalEngine::flVelocityModifier + (TInterfaces::Globals->flIntervalPerTick * (float)iDelta * 0.4f);
		flVelocityModifier = std::clamp(flVelocityModifier, 0.0f, 1.0f);
	}

	return flVelocityModifier;
}

void TPrediction::UpdateVelocityModifier()
{
	if (!TGlobalVars::pLocalEntity || !TGlobalVars::pLocalEntity->IsAlive())
		return;

	static int iLastCmdAck = 0;
	static float flNextCmdTime = 0.f;

	if (iLastCmdAck != TInterfaces::ClientState->iLastCommandAck || flNextCmdTime != TInterfaces::ClientState->flNextCmdTime)
	{
		if (flVelocityModifier != TGlobalVars::pLocalEntity->GetVelocityModifier())
		{
			*reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(TInterfaces::Prediction + 0x24)) = true;
			flVelocityModifier = TGlobalVars::pLocalEntity->GetVelocityModifier();
		}

		iLastCmdAck = TInterfaces::ClientState->iLastCommandAck;
		flNextCmdTime = TInterfaces::ClientState->flNextCmdTime;
	}
}

void TPrediction::TNetvarFix::DetectPredictionError(TNetvarData* netData, int iTick)
{
	if (!netData || netData->nCommandNumber != iTick || !netData->bFilled || netData->iTick > (TInterfaces::Globals->iTickCount + 8))
		return;

	static auto IsOutOfEpsilonFloat = [](float a, float b, float m_epsilon) -> bool {
		return std::fabsf(a - b) > m_epsilon;
	};

	static auto IsOutOfEpsilonVector = [](Vector3D a, Vector3D b, float m_epsilon) -> bool {
		return std::fabsf(a.x - b.x) > m_epsilon || std::fabsf(a.y - b.y) > m_epsilon || std::fabsf(a.z - b.z) > m_epsilon;
	};

	static auto IsOutOfEpsilonAngle = [](Vector3D a, Vector3D b, float m_epsilon) -> bool {
		return std::fabsf(a.x - b.x) > m_epsilon || std::fabsf(a.y - b.y) > m_epsilon || std::fabsf(a.z - b.z) > m_epsilon;
	};

	if (IsOutOfEpsilonAngle(TGlobalVars::pLocalEntity->GetPunch(), netData->vecPunch, 0.5f))
	{
		netData->vecPunch = TGlobalVars::pLocalEntity->GetPunch();
		TInterfaces::Prediction->ForceRepredict();
	}
	else
		TGlobalVars::pLocalEntity->GetPunch() = netData->vecPunch;

	if (IsOutOfEpsilonAngle(TGlobalVars::pLocalEntity->GetPunchAngleVelocity(), netData->vecPunchVelocity, 0.5f))
	{
		netData->vecPunchVelocity = TGlobalVars::pLocalEntity->GetPunchAngleVelocity();
		TInterfaces::Prediction->ForceRepredict();
	}
	else
		TGlobalVars::pLocalEntity->GetPunchAngleVelocity() = netData->vecPunchVelocity;

	if (IsOutOfEpsilonFloat(TGlobalVars::pLocalEntity->GetViewOffset().z, netData->vecViewOffset.z, 0.5f))
	{
		netData->vecViewOffset.z = TGlobalVars::pLocalEntity->GetViewOffset().z;
		TInterfaces::Prediction->ForceRepredict();
	}
	else
		TGlobalVars::pLocalEntity->GetViewOffset().z = netData->vecViewOffset.z;

	if (IsOutOfEpsilonVector(TGlobalVars::pLocalEntity->GetVelocity(), netData->vecVelocity, 0.5f))
	{
		netData->vecVelocity = TGlobalVars::pLocalEntity->GetVelocity();
		TInterfaces::Prediction->ForceRepredict();
	}
	else
		TGlobalVars::pLocalEntity->GetVelocity() = netData->vecVelocity;

	if (IsOutOfEpsilonFloat(TGlobalVars::pLocalEntity->GetVelocityModifier(), netData->flVelocityModifier, 0.00625f))
	{
		netData->flVelocityModifier = TGlobalVars::pLocalEntity->GetVelocityModifier();
		TInterfaces::Prediction->ForceRepredict();
	}
	else
		TGlobalVars::pLocalEntity->GetVelocityModifier() = netData->flVelocityModifier;

	if ((TGlobalVars::pLocalEntity->GetOrigin() - netData->vecOrigin).LengthSqr() >= 1.f)
	{
		netData->vecOrigin = TGlobalVars::pLocalEntity->GetOrigin();
		TInterfaces::Prediction->ForceRepredict();
	}
}

void TPrediction::TNetvarFix::Store(TNetvarData* NetData, CCommandContext* CmdContext, int iCommand) {

	NetData->nTickBase = TGlobalVars::pLocalEntity->GetTickBase();
	NetData->nCommandNumber = CmdContext->Cmd.iCommandNumber;
	NetData->vecPunch = TGlobalVars::pLocalEntity->GetPunch();
	NetData->vecPunchVelocity = TGlobalVars::pLocalEntity->GetPunchAngleVelocity();
	NetData->vecViewPunchAngle = TGlobalVars::pLocalEntity->GetViewPunch();
	NetData->vecViewOffset = TGlobalVars::pLocalEntity->GetViewOffset();
	NetData->vecViewOffset.z = std::fminf(std::fmaxf(NetData->vecViewOffset.z, 46.0f), 64.0f);
	NetData->vecVelocity = TGlobalVars::pLocalEntity->GetVelocity();
	NetData->vecOrigin = TGlobalVars::pLocalEntity->GetOrigin();
	NetData->flFallVelocity = *TGlobalVars::pLocalEntity->GetFallVelocity();
	NetData->flThirdpersonRecoil = TGlobalVars::pLocalEntity->GetThirdPersonRecoil();
	NetData->fllDuckAmount = TGlobalVars::pLocalEntity->GetDuckAmount();
	NetData->flVelocityModifier = TGlobalVars::pLocalEntity->GetVelocityModifier();
	NetData->iTick = iCommand;
	NetData->bFilled = true;

}

void TPrediction::TNetvarFix::Fix(int iCommand) {
	
	TNetvarData* netData = &arrNetvarData[iCommand % MULTIPLAYER_BACKUP];
	
	if (!netData || !netData->bFilled || netData->nCommandNumber != iCommand || (iCommand - netData->nCommandNumber) > MULTIPLAYER_BACKUP)
		return;

	Vector3D vecAimPunchDelta = TGlobalVars::pLocalEntity->GetPunch() - netData->vecPunch;
	Vector3D vecAimPunchVelocityDelta = TGlobalVars::pLocalEntity->GetPunchAngleVelocity() - netData->vecPunchVelocity;
	Vector3D vecViewPunchDelta = TGlobalVars::pLocalEntity->GetViewPunch() - netData->vecViewPunchAngle;

	Vector3D vecViewOffsetDelta = TGlobalVars::pLocalEntity->GetViewOffset() - netData->vecViewOffset;
	Vector3D vecVelocityDelta = TGlobalVars::pLocalEntity->GetVelocity() - netData->vecVelocity;

	if (std::abs(vecAimPunchDelta.x) <= 0.03125f && std::abs(vecAimPunchDelta.y) <= 0.03125f && std::abs(vecAimPunchDelta.z) <= 0.03125f)
		TGlobalVars::pLocalEntity->GetPunch() = netData->vecPunch;

	if (std::abs(vecAimPunchVelocityDelta.x) <= 0.03125f && std::abs(vecAimPunchVelocityDelta.y) <= 0.03125f && std::abs(vecAimPunchVelocityDelta.z) <= 0.03125f)
		TGlobalVars::pLocalEntity->GetPunchAngleVelocity() = netData->vecPunchVelocity;

	if (std::abs(vecViewOffsetDelta.z) <= 0.03125f)
		TGlobalVars::pLocalEntity->GetViewOffset().z = netData->vecViewOffset.z;

	if (std::abs(vecViewPunchDelta.x) <= 0.03125f && std::abs(vecViewPunchDelta.y) <= 0.03125f && std::abs(vecViewPunchDelta.z) <= 0.03125f)
		TGlobalVars::pLocalEntity->GetViewPunch() = netData->vecViewPunchAngle;

	if (std::abs(vecVelocityDelta.x) <= 0.03125f && std::abs(vecVelocityDelta.y) <= 0.03125f && std::abs(vecVelocityDelta.z) <= 0.03125f)
		TGlobalVars::pLocalEntity->GetVelocity() = netData->vecVelocity;

	if (std::abs(TGlobalVars::pLocalEntity->GetThirdPersonRecoil() - netData->flThirdpersonRecoil) <= 0.03125f)
		TGlobalVars::pLocalEntity->GetThirdPersonRecoil() = netData->flThirdpersonRecoil;

	if (std::abs(TGlobalVars::pLocalEntity->GetDuckAmount() - netData->fllDuckAmount) <= 0.03125f)
		TGlobalVars::pLocalEntity->GetDuckAmount() = netData->fllDuckAmount;

	if (std::abs(*TGlobalVars::pLocalEntity->GetFallVelocity() - netData->flFallVelocity) <= 0.03125f)
		*TGlobalVars::pLocalEntity->GetFallVelocity() = netData->flFallVelocity;

	if (std::abs(TGlobalVars::pLocalEntity->GetVelocityModifier() - netData->flVelocityModifier) <= 0.00625f)
		TGlobalVars::pLocalEntity->GetVelocityModifier() = netData->flVelocityModifier;
}

int TPrediction::GetTickBase(CUserCmd* pCmd, CBaseEntity* pLocal) {

	static int iTick = 0;

	if (pCmd != nullptr)
	{
		static CUserCmd* pLastCmd = nullptr;

		if (pLastCmd == nullptr || pLastCmd->bHasBeenPredicted)
			iTick = pLocal->GetTickBase();
		else
			iTick++;

		pLastCmd = pCmd;
	}

	return iTick;
}
