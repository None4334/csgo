#include "../../includes.h"
#include "animationsmanager.h"
#include "localanimmanager.h"
#include "entityhandler.h"
#include "../gvars.h"
#include "../sdk.h"
#include "../math.h"
#include "convarmanager.h"
#include "../sdk/bonesetuprebuilt.h"
#include "../handlers/packethandler.h"
#include "eventlogger.h"

std::array<CAnimationLayer, ANIMATION_LAYER_COUNT > TLocalAnimation::GetAnimationLayers(CBaseEntity* pLocalPlayer)
{
	static std::array< CAnimationLayer, ANIMATION_LAYER_COUNT > arrOutput;
	RtlCopyMemory(arrOutput.data(), pLocalPlayer->GetAnimationOverlays().Base(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);
	RtlCopyMemory(&arrOutput.at(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL), &arrAnimationLayers.at(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL), sizeof(CAnimationLayer));
	RtlCopyMemory(&arrOutput.at(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB), &arrAnimationLayers.at(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB), sizeof(CAnimationLayer));
	RtlCopyMemory(&arrOutput.at(ANIMATION_LAYER_ALIVELOOP), &arrAnimationLayers.at(ANIMATION_LAYER_ALIVELOOP), sizeof(CAnimationLayer));
	RtlCopyMemory(&arrOutput.at(ANIMATION_LAYER_LEAN), &arrAnimationLayers.at(ANIMATION_LAYER_LEAN), sizeof(CAnimationLayer));
	RtlCopyMemory(&arrOutput.at(ANIMATION_LAYER_MOVEMENT_MOVE), &arrAnimationLayers.at(ANIMATION_LAYER_MOVEMENT_MOVE), sizeof(CAnimationLayer));
	RtlCopyMemory(&arrOutput.at(ANIMATION_LAYER_MOVEMENT_STRAFECHANGE), &arrAnimationLayers.at(ANIMATION_LAYER_MOVEMENT_STRAFECHANGE), sizeof(CAnimationLayer));
	return arrOutput;
}

std::array< CAnimationLayer, ANIMATION_LAYER_COUNT > TLocalAnimation::GetFakeAnimationLayers(CBaseEntity* pLocalPlayer)
{
	static std::array< CAnimationLayer, ANIMATION_LAYER_COUNT > arrOutput;
	RtlCopyMemory(arrOutput.data(), pLocalPlayer->GetAnimationOverlays().Base(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);
	RtlCopyMemory(&arrOutput.at(ANIMATION_LAYER_MOVEMENT_MOVE), &arrFakeAnimationLayers.at(ANIMATION_LAYER_MOVEMENT_MOVE), sizeof(CAnimationLayer));
	RtlCopyMemory(&arrOutput.at(ANIMATION_LAYER_MOVEMENT_STRAFECHANGE), &arrFakeAnimationLayers.at(ANIMATION_LAYER_MOVEMENT_STRAFECHANGE), sizeof(CAnimationLayer));
	RtlCopyMemory(&arrOutput.at(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL), &arrFakeAnimationLayers.at(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL), sizeof(CAnimationLayer));
	RtlCopyMemory(&arrOutput.at(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB), &arrFakeAnimationLayers.at(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB), sizeof(CAnimationLayer));
	RtlCopyMemory(&arrOutput.at(ANIMATION_LAYER_ALIVELOOP), &arrFakeAnimationLayers.at(ANIMATION_LAYER_ALIVELOOP), sizeof(CAnimationLayer));
	RtlCopyMemory(&arrOutput.at(ANIMATION_LAYER_LEAN), &arrFakeAnimationLayers.at(ANIMATION_LAYER_LEAN), sizeof(CAnimationLayer));
	return arrOutput;
}

void TLocalAnimation::UpdateAnimationOverlay(CBaseEntity* pLocalPlayer, int nType)
{
	CAnimationLayer* pLandOrClimbLayer = pLocalPlayer->GetAnimationLayer(ANIMATION_LAYER_MOVEMENT_LAND_OR_CLIMB);

	if (!pLandOrClimbLayer)
		return;

	CAnimationLayer* pJumpOrFallLayer = pLocalPlayer->GetAnimationLayer(ANIMATION_LAYER_MOVEMENT_JUMP_OR_FALL);

	if (!pJumpOrFallLayer)
		return;

	if (iMoveType[nType] != MOVETYPE_LADDER && pLocalPlayer->GetMoveType() == MOVETYPE_LADDER)
		pLocalPlayer->GetAnimationState()->SetLayerSequence(pLandOrClimbLayer, ACT_CSGO_CLIMB_LADDER);
	else if (iMoveType[nType] == MOVETYPE_LADDER && pLocalPlayer->GetMoveType() != MOVETYPE_LADDER)
		pLocalPlayer->GetAnimationState()->SetLayerSequence(pJumpOrFallLayer, ACT_CSGO_FALL);
	else
	{
		if (pLocalPlayer->GetFlags() & FL_ONGROUND)
		{
			if (!(iFlags[nType] & FL_ONGROUND))
				pLocalPlayer->GetAnimationState()->SetLayerSequence(pLandOrClimbLayer, pLocalPlayer->GetAnimationState()->m_flDurationInAir > 1.0f && nType == 0 ? ACT_CSGO_LAND_HEAVY : ACT_CSGO_LAND_LIGHT);
		}
		else if (iFlags[nType] & FL_ONGROUND)
		{
			if (pLocalPlayer->GetVelocity().z > 0.0f)
				pLocalPlayer->GetAnimationState()->SetLayerSequence(pJumpOrFallLayer, ACT_CSGO_JUMP);
			else
				pLocalPlayer->GetAnimationState()->SetLayerSequence(pJumpOrFallLayer, ACT_CSGO_FALL);
		}
	}

	iMoveType[nType] = pLocalPlayer->GetMoveType();
	iFlags[nType] = pLocalPlayer->GetFlags();
}

void TLocalAnimation::OnCommandEnd(CUserCmd* pCmd, CBaseEntity* pLocalPlayer, bool bSendPacket) {

	static IGlobalVarsBase Globals;
	static CCSGOPlayerAnimState FakeAnimationState;
	static CCSGOPlayerAnimState OldAnimationState;

	bInAnimUpdate = true;

	if (bSendPacket)
		vecAnglesSent = pCmd->angViewPoint;
	
     vecAngles = pCmd->angViewPoint;

	memcpy(&Globals, TInterfaces::Globals, sizeof IGlobalVarsBase);

	CCSGOPlayerAnimState* pAnimState = pLocalPlayer->GetAnimationState();

	static float flGoalFeetYaw = pAnimState->m_flFootYaw;

	float flMaxFeetYaw = pLocalPlayer->GetMaxAnimationDelta() * -1.f;

	Vector3D vecNormalizedAngle = vecAngles - vecAnglesSent;
	vecNormalizedAngle.NormalizeAngles();

	float flGoalFeet = floor(vecNormalizedAngle.y);

	bool bFrozen = TInterfaces::GetGameRules()->IsFreezePeriod() || (pLocalPlayer->GetFlags() & FL_FROZEN);
	
	bool bUseOriginal = (bSendPacket && TPacketHandler::bLastPacketState) || bFrozen;

	if (!vecNormalizedAngle.IsZero()) {

		if (flGoalFeet < -28)
			flGoalFeet += flMaxFeetYaw;
		else if (flGoalFeet > 28)
			flGoalFeet -= flMaxFeetYaw;

		flGoalFeetYaw = floor(flGoalFeet);
	} 



	if (pLocalPlayer->GetSpawnTime() != flSpawnTime)
	{
		iFlags[0] = iFlags[1] = pLocalPlayer->GetFlags();
		iMoveType[0] = iMoveType[1] = pLocalPlayer->GetMoveType();
		flSpawnTime = pLocalPlayer->GetSpawnTime();

		memcpy(&FakeAnimationState, pAnimState, sizeof(CCSGOPlayerAnimState));
		memcpy(arrFakeAnimationLayers.data(), pLocalPlayer->GetAnimationOverlays().Base(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);
		memcpy(arrFakePoseParameters.data(), pLocalPlayer->GetPoseParameter().data(), sizeof(float_t) * 24);
	}

	if (!bFrozen)
		memcpy(pLocalPlayer->GetAnimationOverlays().Base(), GetAnimationLayers(pLocalPlayer).data(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);

	int iOldFlags = pLocalPlayer->GetFlags();
	float flOldLowerBodyYaw = pLocalPlayer->GetLowerBodyYaw();
	float flOldDuckSpeed = pLocalPlayer->GetDuckSpeed();
	float flOldDuckAmount = pLocalPlayer->GetDuckAmount();

	TInterfaces::Globals->flCurrentTime = TICKS_TO_TIME(pCmd->iTickCount);
	TInterfaces::Globals->flRealTime = TICKS_TO_TIME(pCmd->iTickCount);
	TInterfaces::Globals->flAbsFrameTime = TInterfaces::Globals->flIntervalPerTick;
	TInterfaces::Globals->flFrameTime = TInterfaces::Globals->flIntervalPerTick;
	TInterfaces::Globals->iTickCount = pCmd->iTickCount;
	TInterfaces::Globals->iFrameCount = pCmd->iTickCount;
	TInterfaces::Globals->flInterpolationAmount = 1.0f;

	if (pLocalPlayer->GetFlags() & FL_ONGROUND) {
		pLocalPlayer->GetAnimationOverlays()[6].flWeight = 0.f;
		pLocalPlayer->GetPoseParameter()[7] = 1.f / pLocalPlayer->GetSimulationTime();
	}

	//if (!bUseOriginal)
		pAnimState->m_flFootYaw = vecAngles.y + flGoalFeetYaw;

	pAnimState->m_flPrimaryCycle = arrAnimationLayers[ANIMATION_LAYER_MOVEMENT_MOVE].flCycle;
	pAnimState->m_flMoveWeight = arrAnimationLayers[ANIMATION_LAYER_MOVEMENT_MOVE].flWeight;
	pAnimState->m_nStrafeSequence = arrAnimationLayers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].nSequence;
	pAnimState->m_flStrafeChangeCycle = arrAnimationLayers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].flCycle;
	pAnimState->m_flStrafeChangeWeight = arrAnimationLayers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].flWeight;
	pAnimState->m_flAccelerationWeight = arrAnimationLayers[ANIMATION_LAYER_LEAN].flWeight;

	pLocalPlayer->GetAbsVelocity() = pLocalPlayer->GetVelocity();

	if (pAnimState->m_nLastUpdateFrame > TInterfaces::Globals->iFrameCount - 1)
		pAnimState->m_nLastUpdateFrame = TInterfaces::Globals->iFrameCount - 1;

	if (!bFrozen)
		UpdateAnimationOverlay(pLocalPlayer, 0);

	for (int iLayer = 0; iLayer < ANIMATION_LAYER_COUNT; iLayer++)
		pLocalPlayer->GetAnimationLayer(iLayer)->pOwner = pLocalPlayer;
	
	bool bClientSideAnimation = pLocalPlayer->IsClientSideAnimation();
	pLocalPlayer->IsClientSideAnimation() = true;

	TGlobalVars::bUpdatingAnimations = true;
	pLocalPlayer->UpdateClientSideAnimations();
	TGlobalVars::bUpdatingAnimations = false;

	pLocalPlayer->IsClientSideAnimation() = bClientSideAnimation;
	
	memcpy(arrPoseParameters.data(), pLocalPlayer->GetPoseParameter().data(), sizeof(float_t) * 24);
	memcpy(arrAnimationLayers.data(), pLocalPlayer->GetAnimationOverlays().Base(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);

	pLocalPlayer->GetFlags() = iOldFlags;
	pLocalPlayer->GetDuckAmount() = flOldDuckAmount;
	pLocalPlayer->GetDuckSpeed() = flOldDuckSpeed;

	if (bSendPacket)
	{
		static CCSGOPlayerAnimState AnimationState;
		memcpy(&AnimationState, pAnimState, sizeof(CCSGOPlayerAnimState));

		TAnimationUpdate::TAnimationCorrection::SetupBones(pLocalPlayer, matModel, BONE_USED_BY_ANYTHING, pLocalPlayer->GetSimulationTime(), true);

		memcpy(&OldAnimationState, pAnimState, sizeof(CCSGOPlayerAnimState));
		memcpy(arrOldAnimationLayers.data(), pLocalPlayer->GetAnimationOverlays().Base(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);
		memcpy(arrOldPoseParameters.data(), pLocalPlayer->GetPoseParameter().data(), sizeof(float_t) * 24);

		if (!bFrozen)
			memcpy(pLocalPlayer->GetAnimationOverlays().Base(), GetFakeAnimationLayers(pLocalPlayer).data(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);

		memcpy(pAnimState, &FakeAnimationState, sizeof(CCSGOPlayerAnimState));

		if (!bFrozen)
			memcpy(pLocalPlayer->GetPoseParameter().data(), arrFakePoseParameters.data(), sizeof(float_t) * 24);

		pAnimState->m_flPrimaryCycle = arrFakeAnimationLayers[ANIMATION_LAYER_MOVEMENT_MOVE].flCycle;
		pAnimState->m_flMoveWeight = arrFakeAnimationLayers[ANIMATION_LAYER_MOVEMENT_MOVE].flWeight;
		pAnimState->m_nStrafeSequence = arrFakeAnimationLayers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].nSequence;
		pAnimState->m_flStrafeChangeCycle = arrFakeAnimationLayers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].flCycle;
		pAnimState->m_flStrafeChangeWeight = arrFakeAnimationLayers[ANIMATION_LAYER_MOVEMENT_STRAFECHANGE].flWeight;
		pAnimState->m_flAccelerationWeight = arrFakeAnimationLayers[ANIMATION_LAYER_LEAN].flWeight;

		float flPrevZAngle = vecAnglesSent.z;
		vecAnglesSent.z = 0;

		int iSimulationTicks = TInterfaces::ClientState->nChokedCommands + 1;
		for (int iSimulationTick = 1; iSimulationTick <= iSimulationTicks; iSimulationTick++)
		{
			int iTickCount = pCmd->iTickCount - (iSimulationTicks - iSimulationTick);
			TInterfaces::Globals->flCurrentTime = TICKS_TO_TIME(iTickCount);
			TInterfaces::Globals->flRealTime = TICKS_TO_TIME(iTickCount);
			TInterfaces::Globals->flAbsFrameTime = TInterfaces::Globals->flIntervalPerTick;
			TInterfaces::Globals->flFrameTime = TInterfaces::Globals->flIntervalPerTick;
			TInterfaces::Globals->iTickCount = iTickCount;
			TInterfaces::Globals->iFrameCount = iTickCount;

			pLocalPlayer->GetAbsVelocity() = pLocalPlayer->GetVelocity();

			bForceSentAngles = false;

			if (pAnimState->m_nLastUpdateFrame == TInterfaces::Globals->iFrameCount)
				pAnimState->m_nLastUpdateFrame = TInterfaces::Globals->iFrameCount - 1;

			if (!bFrozen)
				UpdateAnimationOverlay(pLocalPlayer, 1);

			for (int iLayer = 0; iLayer < ANIMATION_LAYER_COUNT; iLayer++)
				pLocalPlayer->GetAnimationOverlays()[iLayer].pOwner = pLocalPlayer;

			bool bClientSideAnimation = pLocalPlayer->IsClientSideAnimation();

			pLocalPlayer->IsClientSideAnimation() = true;

			TGlobalVars::bUpdatingAnimations = true;
			pLocalPlayer->UpdateClientSideAnimations();
			TGlobalVars::bUpdatingAnimations = false;

			pLocalPlayer->IsClientSideAnimation() = bClientSideAnimation;

			bForceSentAngles = true;
		}

		TAnimationUpdate::TAnimationCorrection::SetupBones(pLocalPlayer, matDesync, BONE_USED_BY_ANYTHING, pLocalPlayer->GetSimulationTime(), true);
		memcpy(matLag, matDesync, sizeof(matrix3x4_t) * MAXSTUDIOBONES);

		vecAnglesSent.z = flPrevZAngle;

		memcpy(&FakeAnimationState, pAnimState, sizeof(CCSGOPlayerAnimState));
		memcpy(arrFakeAnimationLayers.data(), pLocalPlayer->GetAnimationOverlays().Base(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);
		memcpy(arrFakePoseParameters.data(), pLocalPlayer->GetPoseParameter().data(), sizeof(float_t) * 24);

		if (!bFrozen)
			memcpy(pLocalPlayer->GetAnimationOverlays().Base(), GetAnimationLayers(pLocalPlayer).data(), sizeof(CAnimationLayer) * ANIMATION_LAYER_COUNT);

		memcpy(pAnimState, &AnimationState, sizeof(CCSGOPlayerAnimState));

		if (!bFrozen)
			memcpy(pLocalPlayer->GetPoseParameter().data(), arrPoseParameters.data(), sizeof(float_t) * 24);

		for (int i = 0; i < MAXSTUDIOBONES; i++)
			arrBoneOrigins[i] = pLocalPlayer->GetAbsOrigin() - matModel[i].GetOrigin();

		for (int i = 0; i < MAXSTUDIOBONES; i++)
			arrFakeBoneOrigins[i] = pLocalPlayer->GetAbsOrigin() - matDesync[i].GetOrigin();

	}

	pLocalPlayer->GetFlags() = iOldFlags;
	pLocalPlayer->GetDuckAmount() = flOldDuckAmount;
	pLocalPlayer->GetDuckSpeed() = flOldDuckSpeed;
	pLocalPlayer->GetLowerBodyYaw() = flOldLowerBodyYaw;

	memcpy(TInterfaces::Globals, &Globals, sizeof IGlobalVarsBase);

	bInAnimUpdate = false;
}


