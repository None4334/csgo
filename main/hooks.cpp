#include "../includes.h"
#include "hooks.h"
#include "../main/handlers/entityhandler.h"

#include "../dependencies/imgui/imgui.h"
#include "../dependencies/imgui/backends/imgui_impl_dx9.h"
#include "../dependencies/imgui/backends/imgui_impl_win32.h"
#include "../dependencies/imgui/imgui_internal.h"

#include "ui/vars.h"
#include "ui/ui.h"
#include "features/visuals.h"
#include "gvars.h"
#include "math.h"

#include "sdk.h"
#include "handlers/animationsmanager.h"
#include "handlers/convarmanager.h"
#include "handlers/localengine.h"
#include "sdk/bonesetuprebuilt.h"
#include "renderer.h"
#include "handlers/firebulletmanager.h"
#include "handlers/eventlogger.h"
#include "features/nadepred.h"
#include "handlers/cmdmgr.h"
#include "features/chams.h"
#include "features/skins/skins.h"
#include "features/misc.h"
#include "features/prediction.h"
#include "handlers/packethandler.h"
#include "handlers/localanimmanager.h"
#include "features/antiaim.h"
#include "features/ragebot.h"

void* __fastcall THooks::hkAllocKeyValuesMemory(IKeyValuesSystem* thisptr, int edx, int iSize)
{
	static auto oAllocKeyValuesMemory = dtrAllocKeyValuesMemory.GetOriginal<decltype(&hkAllocKeyValuesMemory)>();

	static const std::uintptr_t uAllocKeyValuesEngine = TUtil::GetAbsoluteAddress(TUtil::FindPattern(ENGINE_STR, TSTRING("E8 ? ? ? ? 83 C4 08 84 C0 75 10 FF 75 0C")) + 0x1) + 0x4A;
	static const std::uintptr_t uAllocKeyValuesClient = TUtil::GetAbsoluteAddress(TUtil::FindPattern(CLIENT_STR, TSTRING("E8 ? ? ? ? 83 C4 08 84 C0 75 10")) + 0x1) + 0x3E;

	if (const std::uintptr_t uReturnAddress = reinterpret_cast<std::uintptr_t>(_ReturnAddress()); uReturnAddress == uAllocKeyValuesEngine || uReturnAddress == uAllocKeyValuesClient)
		return nullptr;

	return oAllocKeyValuesMemory(thisptr, edx, iSize);
}

static void __stdcall CreateMove(int nSequenceNumber, float flInputSampleFrametime, bool bIsActive, bool& bSendPacket)
{
	static auto oCreateMove = THooks::dtrCreateMoveProxy.GetOriginal<decltype(&THooks::hkCreateMoveProxy)>();

	oCreateMove(TInterfaces::BaseClient, 0, nSequenceNumber, flInputSampleFrametime, bIsActive);

	TLocalEngine::bInCreateMove = true; 

	CUserCmd* pCmd = TInterfaces::Input->GetUserCmd(nSequenceNumber);
	CVerifiedUserCmd* pVerifiedCmd = TInterfaces::Input->GetVerifiedCmd(nSequenceNumber);

	if (!pCmd || !pVerifiedCmd || !bIsActive) {
		TLocalEngine::bInCreateMove = false;
		return;
	}

	TGlobalVars::LocalIndex = TInterfaces::EngineClient->GetLocalPlayer();
	TGlobalVars::pLocalEntity = TInterfaces::ClientEntityList->Get<CBaseEntity>(TGlobalVars::LocalIndex);

	if (!TGlobalVars::pLocalEntity || !TGlobalVars::pLocalEntity->IsAlive()) {
		TLocalEngine::bInCreateMove = false;
		return;
	}

	if (TVariables::bMenuOpen) {
		pCmd->iButtons &= ~IN_ATTACK;
		pCmd->iButtons &= ~IN_SECOND_ATTACK;
	}

	TPacketHandler::OnCommandStart(pCmd, TGlobalVars::pLocalEntity);
	TCommandManager::OnNewCommand(pCmd->angViewPoint, pCmd);

	INetChannel* pNetChannel = TInterfaces::ClientState->pNetChannel;

	Vector3D angOldViewPoint = pCmd->angViewPoint;
	TLocalEngine::vecViewAngles = pCmd->angViewPoint;

	TCommandInfo* CmdObject = TCommandManager::GetCommandData(pCmd->iCommandNumber);

	if (!CmdObject)
		return;

	TMisc::TMovement::InfiniteDuck(pCmd);
	TMisc::TMovement::BunnyHop(pCmd, TGlobalVars::pLocalEntity);
	TMisc::TMovement::Quickstop(pCmd, TGlobalVars::pLocalEntity);

	TNadePrediction::SetupAndRunPrediction(TGlobalVars::pLocalEntity);

	TPrediction::Start(pCmd, TGlobalVars::pLocalEntity);

	CmdObject->vecEyePosition = TGlobalVars::pLocalEntity->GetEyePosition();
	 
	TMisc::TMovement::AirStrafe(pCmd, TGlobalVars::pLocalEntity);

	TRageBot::Run(pCmd, TGlobalVars::pLocalEntity);
	TAntiAim::Run(pCmd, TGlobalVars::pLocalEntity, bSendPacket);

	TPrediction::End(pCmd, TGlobalVars::pLocalEntity);

	if (TVariables::TMisc::AntiUntrusted.bEnabled) {
		pCmd->angViewPoint.NormalizeAngles();
		pCmd->angViewPoint.ClampAngles();
	}

	TMisc::TMovement::FixMovement(angOldViewPoint, pCmd);

	TMisc::TBuyBot::Run(false);

	TPacketHandler::OnCommandEnd(pCmd, TGlobalVars::pLocalEntity, bSendPacket);
	TCommandManager::OnCommandEnd(pCmd->angViewPoint, pCmd, bSendPacket);

	TLocalAnimation::OnCommandEnd(pCmd, TGlobalVars::pLocalEntity, bSendPacket);

	TPacketHandler::TProcessPacket::FinishCommandProcess(pCmd,bSendPacket);

	pVerifiedCmd->userCmd = *pCmd;
	pVerifiedCmd->uHashCRC = pCmd->GetChecksum();

	TLocalEngine::bInCreateMove = false;
}

__declspec(naked) void __fastcall THooks::hkCreateMoveProxy(IBaseClientDll* thisptr, int edx, int nSequenceNumber, float flInputSampleFrametime, bool bIsActive)
{
	__asm
	{
		push	ebp
		mov		ebp, esp; 
		push	ebx; 
		push	esp; 
		push	dword ptr[bIsActive]; 
		push	dword ptr[flInputSampleFrametime]; 
		push	dword ptr[nSequenceNumber]; 
		call	CreateMove
		pop		ebx
		pop		ebp
		retn	0Ch
	}
}

void __fastcall THooks::hkRunCommand(IPrediction* thisptr, int edx, CBaseEntity* pEntity, CUserCmd* pCmd, IMoveHelper* pMoveHelper)
{
	static auto oRunCommand = dtrRunCommand.GetOriginal<decltype(&hkRunCommand)>();

	if (pEntity != TGlobalVars::pLocalEntity || !pEntity || !pEntity->IsAlive())
		return oRunCommand(thisptr, edx, pEntity, pCmd, pMoveHelper);

	int TickRate = ((int)(1.0f / TInterfaces::Globals->flIntervalPerTick));

	if (TInterfaces::Globals->iTickCount + TickRate + 8 <= pCmd->iTickCount)
	{
		pCmd->bHasBeenPredicted = true;

		if (TInterfaces::ClientState->pNetChannel)
		{
			INetChannelInfo* pNetInfo = (INetChannelInfo*)TInterfaces::ClientState->pNetChannel;
			int serverTickcount = TInterfaces::Globals->iTickCount + ((pNetInfo->GetLatency(FLOW_OUTGOING) / TInterfaces::Globals->flIntervalPerTick) + 3);
			serverTickcount += (pNetInfo->GetLatency(FLOW_OUTGOING) / TInterfaces::Globals->flIntervalPerTick) + 3;
			pEntity->GetFinalPredictedTick() = serverTickcount;
		}

		pEntity->SetAbsOrigin(pEntity->GetOrigin());

		if (TInterfaces::Globals->flFrameTime > 0.0f && !TInterfaces::Prediction->bEnginePaused)
		{
			++pEntity->GetTickBase();
			TInterfaces::Globals->flCurrentTime = TICKS_TO_TIME(pEntity->GetTickBase());
		}

		return;
	}

	TPrediction::bOverrideModifier = true;

	auto flBackupVelocityModifier = pEntity->GetVelocityModifier();

	oRunCommand(thisptr, edx, pEntity, pCmd, pMoveHelper);
	
	pEntity->GetVelocityModifier() = TPrediction::GetFixedVelocityModifier(pEntity, pCmd->iCommandNumber, false);
}

void __fastcall THooks::hkFrameStageNotify(IBaseClientDll* thisptr, int edx,EClientFrameStage curStage)
{
	static auto oFrameStageNotify = THooks::dtrFrameStageNotify.GetOriginal<decltype(&THooks::hkFrameStageNotify)>();
	
	TGlobalVars::LocalIndex = TInterfaces::EngineClient->GetLocalPlayer();
	TGlobalVars::pLocalEntity = TInterfaces::ClientEntityList->Get<CBaseEntity>(TGlobalVars::LocalIndex);

	if (!TInterfaces::EngineClient->IsInGame())
		return oFrameStageNotify(thisptr, edx, curStage);

	if (!TGlobalVars::pLocalEntity)
		return oFrameStageNotify(thisptr, edx, curStage);

	TSkinChanger::ModelChanger(curStage);

	switch (curStage)
	{
	case FRAME_START: {
		TVariables::UpdateKeybindStates();
		break;
	}
	case FRAME_NET_UPDATE_START:
	{
		TConvarManager::flLerp = TConvarManager::GetLerp();
		TEntityListener::TOnUpdate::ValidateEntityList();
		TEntityListener::TOnUpdate::FillEntityList();
		break;
	}
	case FRAME_NET_UPDATE_POSTDATAUPDATE_START:
	{
		TSkinChanger::Start(false);
		TSkinChanger::Start(true);

		static auto flOldNextCmdTime = 0.0f;
		static auto iOldLastCommandAck = 0;

		auto iLastCommandAck = TInterfaces::ClientState->iLastCommandAck;
		auto flNextCmdTime = TInterfaces::ClientState->flNextCmdTime;

		if (flOldNextCmdTime != flNextCmdTime || iOldLastCommandAck != iLastCommandAck)
		{
			auto m_flVelocityModifier = TGlobalVars::pLocalEntity->GetVelocityModifier();

			if (TLocalEngine::flVelocityModifier != m_flVelocityModifier) 
			{
			    TInterfaces::Prediction->DoPred();
				TLocalEngine::flVelocityModifier = m_flVelocityModifier;
				TLocalEngine::iVelocityModifierTick = TInterfaces::ClientState->iCommandAck;
			}

			flOldNextCmdTime = flNextCmdTime;
			iOldLastCommandAck = iLastCommandAck;
		}

		break;
	}
	case FRAME_NET_UPDATE_POSTDATAUPDATE_END: {
		break;
	}
	case FRAME_NET_UPDATE_END: {
		TAnimationUpdate::OnNetUpdateEnd();
		TFireBullet::Routine();
		break;
	}
	case FRAME_RENDER_START:
	{
		TVisuals::TLocal::RemoveSmoke();

		static IMaterial* pBlurOverlay = TInterfaces::MaterialSystem->FindMaterial(TSTRING("dev/scope_bluroverlay"), TEXTURE_GROUP_OTHER);
		static IMaterial* pScopeDirt = TInterfaces::MaterialSystem->FindMaterial(TSTRING("models/weapons/shared/scope/scope_lens_dirt"), TEXTURE_GROUP_OTHER);

		pBlurOverlay->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);
		pScopeDirt->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

		break;
	}
	case FRAME_RENDER_END: {
		break;
	}
	case FRAME_NET_FULL_FRAME_UPDATE_ON_REMOVE: {
		break;
	}
	default: {
		break;
	}
	}

	TSkinChanger::CheckSkinUpdate();
	//TModelChams::Initialize();
	oFrameStageNotify(thisptr, edx, curStage);

	TMisc::TOther::PreserveDeathNotices(TGlobalVars::pLocalEntity);

	if (!TGlobalVars::pLocalEntity->IsAlive())
		return;

	TLocalEngine::vecOrigin = TGlobalVars::pLocalEntity->GetOrigin();
	TLocalEngine::vecAbsOrigin = TGlobalVars::pLocalEntity->GetAbsOrigin();
	TInterfaces::EngineClient->GetViewAngles(TLocalEngine::vecViewAngles);
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
long __stdcall hkWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	IPT::Process(uMsg, wParam, lParam);

	if (TVariables::iMenuKey > 0 && IPT::IsKeyReleased(TVariables::iMenuKey))
		TVariables::bMenuOpen = !TVariables::bMenuOpen;

	//TInterfaces::InputSystem->EnableInput(!TVariables::bMenuOpen);

	if (TVariables::bMenuInitialized && TVariables::bMenuOpen && ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return 1L;

	return CallWindowProcW(IPT::pOldWndProc, hWnd, uMsg, wParam, lParam);
}

long D3DAPI THooks::hkReset(IDirect3DDevice9* pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	static auto oReset = dtrReset.GetOriginal<decltype(&hkReset)>();

	if (!TVariables::bMenuInitialized)
		return oReset(pDevice, pPresentationParameters);

	ImGui_ImplDX9_InvalidateDeviceObjects();

	const HRESULT hReset = oReset(pDevice, pPresentationParameters);

	if (hReset == D3D_OK)
		ImGui_ImplDX9_CreateDeviceObjects();

	return hReset;
}

long D3DAPI THooks::hkEndScene(IDirect3DDevice9* pDevice)
{
	static auto oEndScene = dtrEndScene.GetOriginal<decltype(&hkEndScene)>();
	static void* pUsedAddress = nullptr;

	if (pUsedAddress == nullptr)
	{
			MEMORY_BASIC_INFORMATION memInfo;
			VirtualQuery(_ReturnAddress(), &memInfo, sizeof(MEMORY_BASIC_INFORMATION));

			char chModuleName[MAX_PATH];
			GetModuleFileName(static_cast<HMODULE>(memInfo.AllocationBase), chModuleName, MAX_PATH);

			if (strstr(chModuleName, GAMEOVERLAYRENDERER_STR) != nullptr)
				pUsedAddress = _ReturnAddress();
	}

	if (_ReturnAddress() == pUsedAddress)
	{
		if (!TVariables::bMenuInitialized)
			TUserInterface::Setup(pDevice);

		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		TUserInterface::Render(pDevice);

		ImGui::EndFrame();
		ImGui::Render();

		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	}

		return oEndScene(pDevice);
}

void __fastcall THooks::hkPaintTraverse(ISurface* thisptr, int edx, unsigned int uPanel, bool bForceRepaint, bool bForce)
{
	static auto oPaintTraverse = dtrPaintTraverse.GetOriginal<decltype(&hkPaintTraverse)>();
	const FNV1A_t uPanelHash = FNV1A::Hash(TInterfaces::Panel->GetName(uPanel));

	if (!TInterfaces::EngineClient->IsTakingScreenshot() && TVariables::TVisuals::TOthers::Removals.mapValues[REMOVAL_NOSCOPE] && uPanelHash == FNV1A::HashConst("HudZoom"))
		return;

	oPaintTraverse(thisptr, edx, uPanel, bForceRepaint, bForce);

	if (uPanelHash == FNV1A::HashConst(TSTRING("FocusOverlayPanel")))
	{
		TEventLogger::Routine();

		TVisuals::RunVisuals( thisptr, uPanel, bForceRepaint, bForce);

		TDirectXRender::SwapDrawData();
	}
}


bool __fastcall THooks::hkSetupBones(CBaseAnimating* pAnimating, void* edx, matrix3x4_t* matBoneToWorld, int nMaxBones, int nBoneMask, float flCurrentTime)
{
	auto oSetupBones = dtrSetupBones.GetOriginal<bool(__thiscall*)(void*, matrix3x4_t*, int, int, float)>();

	CBaseEntity* pBasePlayer = (CBaseEntity*)((uintptr_t)pAnimating - 0x4);

	if (!pBasePlayer)
		return oSetupBones(pAnimating, matBoneToWorld, nMaxBones, nBoneMask, flCurrentTime);

	if (!TGlobalVars::pLocalEntity || pBasePlayer->GetClientClass()->nClassID != EClassIndex::CCSPlayer || !pBasePlayer->IsPlayer() || !pBasePlayer->IsAlive() || !pBasePlayer->GetCachedBoneData().Count())
		return oSetupBones(pAnimating, matBoneToWorld, nMaxBones, nBoneMask, flCurrentTime);

	if (!matBoneToWorld && !TGlobalVars::pLocalEntity->IsAlive())
		return oSetupBones(pAnimating, matBoneToWorld, nMaxBones, nBoneMask, flCurrentTime);

	if (pBasePlayer == TGlobalVars::pLocalEntity && TGlobalVars::pLocalEntity) {

		if (!TGlobalVars::bCallingSetupBones)
		{
			if (nMaxBones > 0 && matBoneToWorld) {
				for (int i = 0; i < MAXSTUDIOBONES; i++)
					TLocalAnimation::matModel[i].SetOrigin(pBasePlayer->GetAbsOrigin() - TLocalAnimation::arrBoneOrigins[i]);

				memcpy(matBoneToWorld, TLocalAnimation::matModel, nMaxBones * sizeof(matrix3x4_t));
			}
			return true;
		}

		return oSetupBones(pAnimating, matBoneToWorld, nMaxBones, nBoneMask, flCurrentTime);
	}
	else {

		TEntityObject* EntityObject = TEntityListener::GetEntityObject(pBasePlayer);

		if (!EntityObject)
			return oSetupBones(pAnimating, matBoneToWorld, nMaxBones, nBoneMask, flCurrentTime);

		if (!TGlobalVars::bCallingSetupBones)
		{
			if (EntityObject->AnimationInstance->iSetupBonesTick == 0)
				return oSetupBones(pAnimating, matBoneToWorld, nMaxBones, nBoneMask, flCurrentTime);

			if (nMaxBones > 0 && matBoneToWorld) {
				for (int i = 0; i < MAXSTUDIOBONES; i++)
					EntityObject->AnimationInstance->matCached[i].SetOrigin(pBasePlayer->GetAbsOrigin() - EntityObject->AnimationInstance->vecBoneOrigin[i]);

				memcpy(matBoneToWorld, EntityObject->AnimationInstance->matCached, pBasePlayer->GetCachedBoneData().Count() * sizeof(matrix3x4_t));
			}
			return true;
		}


		if (EntityObject && EntityObject->AnimationInstance)
		{
			bool bReturn = oSetupBones(pAnimating, matBoneToWorld, nMaxBones, nBoneMask, flCurrentTime);
			memcpy(EntityObject->AnimationInstance->matCached, matBoneToWorld, nMaxBones * sizeof(matrix3x4_t));

			if (!TGlobalVars::bSettingUpPredictionBones) {
				for (int i = 0; i < MAXSTUDIOBONES; i++)
					EntityObject->AnimationInstance->vecBoneOrigin[i] = pBasePlayer->GetOrigin() - EntityObject->AnimationInstance->matCached[i].GetOrigin();
			}

			EntityObject->AnimationInstance->iSetupBonesTick = TIME_TO_TICKS(pBasePlayer->GetSimulationTime());
			return bReturn;
		}
	}

	return true;
}

void __fastcall THooks::hkUpdateClientSideAnimations(CBaseEntity* pPlayer, uint32_t u)
{
	auto oUpdateClientSideAnimations = dtrUpdateClientSideAnimations.GetOriginal<void(__fastcall*)(CBaseEntity*)>();

	if (!pPlayer || !pPlayer->IsPlayer() || pPlayer->GetClientClass()->nClassID != EClassIndex::CCSPlayer || !TGlobalVars::pLocalEntity || TGlobalVars::bUpdatingAnimations)
		return oUpdateClientSideAnimations(pPlayer);

	if (pPlayer == TGlobalVars::pLocalEntity || pPlayer->GetIndex() == TGlobalVars::LocalIndex)
	{
		for (int i = 0; i < MAXSTUDIOBONES; i++)
			TLocalAnimation::matModel[i].SetOrigin(pPlayer->GetAbsOrigin() - TLocalAnimation::arrBoneOrigins[i]);

		for (int i = 0; i < MAXSTUDIOBONES; i++)
			TLocalAnimation::matDesync[i].SetOrigin(pPlayer->GetAbsOrigin() - TLocalAnimation::arrFakeBoneOrigins[i]);

		memcpy(pPlayer->GetCachedBoneData().Base(), TLocalAnimation::matModel, pPlayer->GetCachedBoneData().Count() * sizeof(matrix3x4_t));
		
		if (pPlayer->GetBoneAccessor()->GetBoneArrayForWrite())
		    memcpy(pPlayer->GetBoneAccessor()->GetBoneArrayForWrite(), TLocalAnimation::matModel, sizeof(matrix3x4_t) * pPlayer->GetCachedBoneData().Count());
		
		pPlayer->AttachmentHelper();

		return;
	}
	else {
		TEntityObject* EntityObject = TEntityListener::GetEntityObject(pPlayer);

		if (EntityObject && EntityObject->AnimationInstance)
		{
			for (int i = 0; i < MAXSTUDIOBONES; i++)
				EntityObject->AnimationInstance->matCached[i].SetOrigin(pPlayer->GetAbsOrigin() - EntityObject->AnimationInstance->vecBoneOrigin[i]);

			memcpy(pPlayer->GetCachedBoneData().Base(), EntityObject->AnimationInstance->matCached, pPlayer->GetCachedBoneData().Count() * sizeof(matrix3x4_t));
			
			if (pPlayer->GetBoneAccessor()->GetBoneArrayForWrite())
			    memcpy(pPlayer->GetBoneAccessor()->GetBoneArrayForWrite(), EntityObject->AnimationInstance->matCached, sizeof(matrix3x4_t) * pPlayer->GetCachedBoneData().Count());
			
			pPlayer->AttachmentHelper();

			return;
		}
	}

	oUpdateClientSideAnimations(pPlayer);
}

void __fastcall THooks::hkClampBonesInBBox(CBaseEntity* player, void* edx, matrix3x4_t* matrix, int mask)
{
	auto ClampBonesInBBox = dtrClampBonesInBox.GetOriginal<void(__thiscall*) (void*, matrix3x4_t*, int)>();
	
	if (TGlobalVars::bCallingSetupBones && matrix && TGlobalVars::bCallingLocalBoneSetup) {

		matrix3x4_t matCache[MAXSTUDIOBONES];
		RtlCopyMemory(matCache, matrix, sizeof(matrix3x4_t) * MAXSTUDIOBONES);

		float flOldCurrentTime = TInterfaces::Globals->flCurrentTime;

		if (player == TGlobalVars::pLocalEntity && player)
			TInterfaces::Globals->flCurrentTime = TICKS_TO_TIME(TInterfaces::Globals->iTickCount);

		ClampBonesInBBox(player, matrix, mask);

		TInterfaces::Globals->flCurrentTime = flOldCurrentTime;

		for (int CurrentBone = 0; CurrentBone < MAXSTUDIOBONES; CurrentBone++)
			matrix[CurrentBone].SetZDelta(matCache[CurrentBone].GetZDelta());
			
	}
}

void __fastcall THooks::hkLockCursor(ISurface* thisptr, int edx)
{
	static auto oLockCursor = dtrLockCursor.GetOriginal<decltype(&hkLockCursor)>();

	if (TVariables::bMenuOpen)
	{
		TInterfaces::Surface->UnLockCursor();
		return;
	}

	oLockCursor(thisptr, edx);
}


float __fastcall THooks::hkGetViewModelFov(IClientModeShared* thisptr, int edx) {

	static auto ohkViewModel = dtrGetViewModelFov.GetOriginal<decltype(&hkGetViewModelFov)>();

	if (TVariables::TVisuals::TLocalPlayer::ViewModelFov.bEnabled && TVariables::TVisuals::bEnabled)
		return ohkViewModel(thisptr, edx) + TVariables::TVisuals::TLocalPlayer::ViewModelFov.Value;

	return ohkViewModel(thisptr, edx);
}

float  __fastcall THooks::hkGetAspectRatio(IEngineClient* thisptr, void* edx, int iWidth, int iHeight)
{
	static auto oGetAspectRatio = dtrGetAspectRatio.GetOriginal<decltype(&hkGetAspectRatio)>();
	
	if (TVariables::TVisuals::TLocalPlayer::AspectRatio.bEnabled && TVariables::TVisuals::bEnabled)
		return static_cast<float>(TVariables::TVisuals::TLocalPlayer::AspectRatio.Value / 100.f);

	return oGetAspectRatio(thisptr, edx, iWidth, iHeight);
}

int __fastcall THooks::hkDoPostScreenEffects(IClientModeShared* thisptr, int edx, CViewSetup* pSetup)
{
	static auto oDoPostScreenEffects = dtrDoPostScreenEffects.GetOriginal<decltype(&hkDoPostScreenEffects)>();

	if (!TInterfaces::EngineClient->IsInGame() || TInterfaces::EngineClient->IsTakingScreenshot() || !TGlobalVars::pLocalEntity)
		return oDoPostScreenEffects(thisptr, edx, pSetup);

	TVisuals::DoPostScreenEffects(thisptr, pSetup);
	TFireBullet::RenderRecords();

	return oDoPostScreenEffects(thisptr, edx, pSetup);
}


void __fastcall THooks::hkCalcViewModelBob(CBaseEntity* pEntity, void* edx, Vector3D& vecPosition)
{
	static auto oCalcViewModelBob = dtrCalcViewModelBob.GetOriginal<decltype(&hkCalcViewModelBob)>();

	if (!TVariables::TVisuals::TOthers::Removals.mapValues[REMOVAL_LANDING_BOB])
		return oCalcViewModelBob(pEntity, edx, vecPosition);

	return;
}

void __fastcall THooks::hkModifyEyePosition(CCSGOPlayerAnimState* pAnimState, void* pEdx, Vector3D& vecAngles) {

	static auto oModifyEyePosition = dtrModifyEyePosition.GetOriginal<decltype(&hkModifyEyePosition)>();

	if (!pAnimState || !pAnimState->m_pBasePlayer || pAnimState->m_pBasePlayer != TGlobalVars::pLocalEntity)
		return;

	if (!TLocalEngine::bInCreateMove)
		return;

	if (!pAnimState->m_bHitGroundAnimation && pAnimState->m_flAnimDuckAmount == 0.0f)
	{
		pAnimState->m_bSmoothHeightValid = false;
		return;
	}

	Vector3D vecHeadPos = Vector3D
	(
		pAnimState->m_pBasePlayer->GetCachedBoneData()[8][0][3],
		pAnimState->m_pBasePlayer->GetCachedBoneData()[8][1][3],
		pAnimState->m_pBasePlayer->GetCachedBoneData()[8][2][3] + 1.7f
	);

	if (vecHeadPos.z > vecAngles.z)
		return;

	static auto RemapValClamped = [](float val, float A, float B, float C, float D) ->float
	{
		if (A == B)
			return val >= B ? D : C;
		float cVal = (val - A) / (B - A);
		cVal = std::clamp(cVal, 0.0f, 1.0f);

		return C + (D - C) * cVal;
	};

	float flLerp = RemapValClamped(abs(vecAngles.z - vecHeadPos.z),
		4.0f,
		10.0f,
		0.0f, 1.0f);

	vecAngles.z = TMath::Lerp(flLerp, vecAngles.z, vecHeadPos.z);
}


void __fastcall THooks::hkOverrideView(IClientModeShared* thisptr, int edx, CViewSetup* pSetup)
{
	static auto oOverrideView = dtrOverrideView.GetOriginal<decltype(&hkOverrideView)>();

	if (!TInterfaces::EngineClient->IsInGame() || TInterfaces::EngineClient->IsTakingScreenshot())
		return oOverrideView(thisptr, edx, pSetup);

	if (!TGlobalVars::pLocalEntity)
		return oOverrideView(thisptr, edx, pSetup);

	if (TGlobalVars::pLocalEntity->IsScoped() && TVariables::TVisuals::TOthers::Removals.mapValues[REMOVAL_NOSCOPEZOOM])
		pSetup->flFOV = 90.f + TVariables::TVisuals::TLocalPlayer::ViewFov.Value;

	if (TVariables::TVisuals::TLocalPlayer::ViewFov.bEnabled && !TGlobalVars::pLocalEntity->IsScoped())
		pSetup->flFOV = 90.f + TVariables::TVisuals::TLocalPlayer::ViewFov.Value;

	if (TVariables::TVisuals::TOthers::Removals.mapValues[REMOVAL_NO_VISUAL_RECOIL])
	{
		Vector3D viewPunch = TGlobalVars::pLocalEntity->GetViewPunch();
		Vector3D aimPunch = TGlobalVars::pLocalEntity->GetPunch();
		pSetup->angView[0] -= (viewPunch[0] + (aimPunch[0] * 2 * 0.4499999f));
		pSetup->angView[1] -= (viewPunch[1] + (aimPunch[1] * 2 * 0.4499999f));
		pSetup->angView[2] -= (viewPunch[2] + (aimPunch[2] * 2 * 0.4499999f));
	}


	if (TGlobalVars::pLocalEntity->IsAlive()) {
		if (TVariables::TVisuals::TLocalPlayer::ThirdPerson.bEnabled &&
			TVariables::TVisuals::TLocalPlayer::ThirdPerson.KeyState.bState) {

			float flThirdPersonDistance = TVariables::TVisuals::TLocalPlayer::ThirdPersonDistance.Value;

			Vector3D vecInversedAngles, vecAngles;
			TInterfaces::EngineClient->GetViewAngles(vecInversedAngles);

			vecAngles = vecInversedAngles;
			vecInversedAngles.z = 150.f;
			vecInversedAngles.z -= 10;
			Vector3D vecForward, vecRight, vecUp;
			TMath::AngleVectors(vecInversedAngles, &vecForward, &vecRight, &vecUp);

			auto vecEyePosition = TLocalEngine::vecAbsOrigin + Vector3D(0.0f, 0.0f, TInterfaces::GameMovement->GetPlayerViewOffset(false).z + 0.064f);

			auto vecOffset = vecEyePosition + vecForward * -flThirdPersonDistance + vecRight + vecUp;

			Ray_t pRay(vecEyePosition, vecOffset, Vector3D(-10.0f, -10.0f, -10.0f), Vector3D(10.0f, 10.0f, 10.0f));
			Trace_t pEngineTrace = {};
			CTraceFilter pTraceFilter(nullptr, TRACE_WORLD_ONLY);

			TInterfaces::EngineTrace->TraceRay(pRay, MASK_SOLID, &pTraceFilter, &pEngineTrace);

			vecAngles.z = flThirdPersonDistance * pEngineTrace.flFraction;
			TInterfaces::Input->vecCameraOffset = vecAngles;
			TInterfaces::Input->bCameraInThirdPerson = true;
		}
		else {
			TInterfaces::Input->bCameraInThirdPerson = false;
		}
	}

	static bool bRequireObserverModeReset = false;

	if (TGlobalVars::pLocalEntity->IsAlive())
	{
		bRequireObserverModeReset = false;
	}
	else {
		if (TVariables::TVisuals::TLocalPlayer::ThirdPersonWhileSpecating.bEnabled) {
			if (bRequireObserverModeReset)
				*TGlobalVars::pLocalEntity->GetObserverMode() = OBS_MODE_CHASE;

			if (*TGlobalVars::pLocalEntity->GetObserverMode() == OBS_MODE_IN_EYE)
				bRequireObserverModeReset = true;
		}
	}

	static bool* bDisablePostProcessing = *reinterpret_cast<bool**>(TUtil::FindPattern(CLIENT_STR,TSTRING("83 EC 4C 80 3D")) + 0x5);

	if (TVariables::TVisuals::TOthers::Removals.mapValues[REMOVAL_POST_PROCESSING])
		*bDisablePostProcessing = true;
	else
		*bDisablePostProcessing = false;

	static auto bFakeDucking = false;

	if (!bFakeDucking && TVariables::TAntiAim::TOthers::Fakeduck.KeyState.bState)
		bFakeDucking = true;
	else if (bFakeDucking && !TVariables::TAntiAim::TOthers::Fakeduck.KeyState.bState
		&& (!TGlobalVars::pLocalEntity->GetAnimationState()->m_flAnimDuckAmount || 
			TGlobalVars::pLocalEntity->GetAnimationState()->m_flAnimDuckAmount == 1.0f))
		bFakeDucking = false;

	if (!TGlobalVars::pLocalEntity->IsAlive())
		bFakeDucking = false;

	if (bFakeDucking)
		pSetup->vecOrigin = TGlobalVars::pLocalEntity->GetAbsOrigin() + Vector3D(0.0f, 0.0f, TInterfaces::GameMovement->GetPlayerViewOffset(false).z + 0.064f);

	oOverrideView(thisptr, edx, pSetup);
}

bool __fastcall THooks::hkShouldDrawFog(IClientModeShared* pClientMode) {
	return !TVariables::TVisuals::TOthers::Removals.mapValues[REMOVAL_FOG];
}

void THooks::hkGetFlashDuration(const CRecvProxyData* Data, void* pStruct, float* flFlashDuration)
{
	static auto oGetFlashDuration = recGetFlashDuration->GetOriginal();

	if (TVariables::TVisuals::TOthers::Removals.mapValues[REMOVAL_NO_FLASH])
	{
		*flFlashDuration = 0.0f;
		return;
	}

	return oGetFlashDuration(Data, pStruct, flFlashDuration);
}

int THooks::hkProcessInterpolatedList()
{
	auto oProcessInterpolatedList = dtrInterpolatedList.GetOriginal<decltype(&hkProcessInterpolatedList)>();

	static auto ubAllowExtrapolation = *(bool**)(TUtil::FindPattern(CLIENT_STR,TSTRING("A2 ? ? ? ? 8B 45 E8")) + 0x1);

	if (ubAllowExtrapolation)
		*ubAllowExtrapolation = false;
	
	return oProcessInterpolatedList();
}

void THooks::hkImpactCallback(const CEffectData& data) {

	oImpactCallback(data);
}

bool __fastcall THooks::hkOverrideConfig(void* pThis, void* edx, MaterialSystemConfig_t* pConfig, bool bUpdate)
{
	static auto oOverrideConfig = dtrOverrideConfig.GetOriginal<decltype(&hkOverrideConfig)>();

	if (TVariables::TVisuals::TOthers::Fullbright.bEnabled)
		pConfig->uFullbright = true;

	static TConfigVarBool SaveNightMode;
	static TConfigVarBool SavePropCol;

	using fnLoadSkybox = bool(__fastcall*)(const char*);

	static auto oLoadSkybox = reinterpret_cast<fnLoadSkybox>(TUtil::FindPattern(ENGINE_STR,TSTRING("55 8B EC 81 EC ? ? ? ? 56 57 8B F9 C7 45")));

	TConvarManager::var3Dsky.pBase->SetValue(false);

	oLoadSkybox(TSTRING("sky_csgo_night02b"));

	if (memcmp(&SaveNightMode, &TVariables::TVisuals::TOthers::Nightmode, sizeof TConfigVarBool) != 0 || (memcmp(&SavePropCol, &TVariables::TVisuals::TOthers::Propscol, sizeof TConfigVarBool) != 0 && TVariables::TVisuals::TOthers::Nightmode.bEnabled))
	{
		static std::deque< IMaterial* > vecWorld, vecProps, vecSky;

		vecWorld.clear();
		vecProps.clear();
		vecSky.clear();

		for (
			uint16_t h = TInterfaces::MaterialSystem->FirstMaterial();
			h != TInterfaces::MaterialSystem->InvalidMaterial();
			h = TInterfaces::MaterialSystem->NextMaterial(h))
		{
			IMaterial* pMaterial = TInterfaces::MaterialSystem->GetMaterial(h);
			if (!pMaterial || pMaterial->IsErrorMaterial())
				continue;

			switch (FNV1A::Hash(pMaterial->GetTextureGroupName()))
			{
			case FNV1A::HashConst(TSTRING("World textures")):
				vecWorld.push_back(pMaterial);
				break;

			case FNV1A::HashConst(TSTRING("StaticProp textures")):
				vecProps.push_back(pMaterial);
				break;

			case FNV1A::HashConst(TSTRING("SkyBox textures")):
				vecSky.push_back(pMaterial);
				break;
			}
		}

		if (TVariables::TVisuals::TOthers::Nightmode.bEnabled)
		{
			TConvarManager::varFogOverride.pBase->SetValue(0);
			TConvarManager::varFogMaxDensity.pBase->SetValue(0);
			TConvarManager::varFogStart.pBase->SetValue(0);
			TConvarManager::varFogEnd.pBase->SetValue(0);
			TConvarManager::varDrawSpecificStaticProp.pBase->SetValue(1);

			if (!vecWorld.empty())
				for (const auto& hWorld : vecWorld) {
					hWorld->ColorModulate(TVariables::TVisuals::TOthers::Nightmode.Color.flColors[0],
						TVariables::TVisuals::TOthers::Nightmode.Color.flColors[1],
						TVariables::TVisuals::TOthers::Nightmode.Color.flColors[2]);

					hWorld->AlphaModulate(TVariables::TVisuals::TOthers::Nightmode.Color.flColors[3]);
				}

			if (!vecProps.empty())
				for (const auto& hProp : vecProps) {
					hProp->ColorModulate(TVariables::TVisuals::TOthers::Propscol.Color.flColors[0],
						TVariables::TVisuals::TOthers::Propscol.Color.flColors[1],
						TVariables::TVisuals::TOthers::Propscol.Color.flColors[2]);

					hProp->AlphaModulate(TVariables::TVisuals::TOthers::Propscol.Color.flColors[3]);
				}

		}
		else
		{
			TConvarManager::varFogMaxDensity.Restore();
			TConvarManager::varFogOverride.Restore();
			TConvarManager::varDrawSpecificStaticProp.Restore();
			TConvarManager::varFogStart.Restore();
			TConvarManager::varFogEnd.Restore();

			if (!vecWorld.empty())
				for (const auto& hWorld : vecWorld) {
					hWorld->ColorModulate(1.f, 1.f, 1.f);
					hWorld->AlphaModulate(1.f);
				}

			if (!vecProps.empty())
				for (const auto& hProp : vecProps) {
					hProp->ColorModulate(1.f, 1.f, 1.f);
					hProp->AlphaModulate(1.f);
				}
		}

		RtlCopyMemory(&SaveNightMode, &TVariables::TVisuals::TOthers::Nightmode, sizeof TConfigVarBool);
		RtlCopyMemory(&SavePropCol, &TVariables::TVisuals::TOthers::Propscol, sizeof TConfigVarBool);
	}

	return oOverrideConfig(pThis,edx, pConfig, bUpdate);
}

void __fastcall THooks::hkDrawModel(IStudioRender* thisptr, int edx, DrawModelResults_t* pResults, const DrawModelInfo_t& info, matrix3x4_t* pBoneToWorld, float* flFlexWeights, float* flFlexDelayedWeights, const Vector3D& vecModelOrigin, int nFlags)
{
	static auto oDrawModel = dtrDrawModel.GetOriginal<decltype(&hkDrawModel)>();

	if (!TInterfaces::EngineClient->IsInGame() || TInterfaces::EngineClient->IsTakingScreenshot() || TGlobalVars::bCallingDrawModelExecute)
		return oDrawModel(thisptr, edx, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

	bool bClearOverride = TModelChams::OnRenderModel(pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

	oDrawModel(thisptr, edx, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

	if (bClearOverride)
		TInterfaces::StudioRender->ForcedMaterialOverride(nullptr);

}

#define RandomInt(nMin, nMax) (rand() % (nMax - nMin + 1) + nMin);

void THooks::hkRecvProxyViewModel(CRecvProxyData* pData, void* pStruct, void* pOut)
{
	static int default_t = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_default_t.mdl"));
	static int default_ct = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_default_ct.mdl"));
	static int iBayonet = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_bayonet.mdl"));
	static int iButterfly = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_butterfly.mdl"));
	static int iFlip = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_flip.mdl"));
	static int iGut = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_gut.mdl"));
	static int iKarambit = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_karam.mdl"));
	static int iM9Bayonet = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_m9_bay.mdl"));
	static int iHuntsman = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_tactical.mdl"));
	static int iFalchion = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_falchion_advanced.mdl"));
	static int iDagger = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_push.mdl"));
	static int iBowie = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_survival_bowie.mdl"));
	static int iUrsus = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_ursus.mdl"));
	static int iNavaja = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_gypsy_jackknife.mdl"));
	static int iStiletto = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_stiletto.mdl"));
	static int iTalon = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_widowmaker.mdl"));
	static int iNomad = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_outdoor.mdl"));
	static int iSkeleton = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_skeleton.mdl"));
	static int iSurvival = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_canis.mdl"));
	static int iParacord = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_cord.mdl"));
	static int iClassic = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_knife_css.mdl"));

	if (TGlobalVars::pLocalEntity && TVariables::TSkins::KnifeModel.Value > 0)
	{
		if (TGlobalVars::pLocalEntity->IsAlive() && (pData->Value.Int == default_ct
			|| pData->Value.Int == default_t
			|| pData->Value.Int == iGut
			|| pData->Value.Int == iKarambit
			|| pData->Value.Int == iM9Bayonet
			|| pData->Value.Int == iHuntsman
			|| pData->Value.Int == iFalchion
			|| pData->Value.Int == iDagger
			|| pData->Value.Int == iBowie
			|| pData->Value.Int == iButterfly
			|| pData->Value.Int == iFlip
			|| pData->Value.Int == iBayonet
			|| pData->Value.Int == iUrsus
			|| pData->Value.Int == iNavaja
			|| pData->Value.Int == iStiletto
			|| pData->Value.Int == iTalon
			|| pData->Value.Int == iNomad
			|| pData->Value.Int == iSkeleton
			|| pData->Value.Int == iSurvival
			|| pData->Value.Int == iParacord
			|| pData->Value.Int == iClassic))
		{
			pData->Value.Int = TInterfaces::ModelInfo->GetModelIndex(TSkinChanger::charIndex);


		}
	}


	oRecvnModelIndex(pData, pStruct, pOut);
}


void THooks::hkSetViewModelSequence(const CRecvProxyData* pDataConst, void* pStruct, void* pOut) {

	CRecvProxyData* pData = const_cast<CRecvProxyData*>(pDataConst);

	CBaseViewModel* pViewModel = (CBaseViewModel*)pStruct;

	if (pViewModel) {
		auto pOwner = reinterpret_cast<CBaseEntity*>(TInterfaces::ClientEntityList->GetClientEntityFromHandle(uintptr_t(pViewModel->GetOwnerHandle())));

		if (pOwner && pOwner->GetIndex() == TGlobalVars::LocalIndex) {

			const void* pModel = TInterfaces::ModelInfo->GetModel(pViewModel->GetVModelIndex());

			const char* szModel = TInterfaces::ModelInfo->GetModelName((Model_t*)pModel);

			int m_nSequence = pData->Value.Int;


			if (!strcmp(szModel, TSTRING("models/weapons/v_knife_butterfly.mdl"))) {
				switch (m_nSequence) {
				case SEQUENCE_DEFAULT_DRAW:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2); break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_LOOKAT01, SEQUENCE_BUTTERFLY_LOOKAT03); break;
				default:
					m_nSequence++;
				}
			}
			else if (!strcmp(szModel, TSTRING("models/weapons/v_knife_falchion_advanced.mdl"))) {
				switch (m_nSequence) {
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_FALCHION_IDLE1; break;
				case SEQUENCE_DEFAULT_HEAVY_MISS1:
					m_nSequence = RandomInt(SEQUENCE_FALCHION_HEAVY_MISS1, SEQUENCE_FALCHION_HEAVY_MISS1_NOFLIP); break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(SEQUENCE_FALCHION_LOOKAT01, SEQUENCE_FALCHION_LOOKAT02); break;
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				default:
					m_nSequence--;
				}
			}
			else if (!strcmp(szModel, TSTRING("models/weapons/v_knife_push.mdl"))) {
				switch (m_nSequence) {
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_DAGGERS_IDLE1; break;
				case SEQUENCE_DEFAULT_LIGHT_MISS1:
				case SEQUENCE_DEFAULT_LIGHT_MISS2:
					m_nSequence = RandomInt(SEQUENCE_DAGGERS_LIGHT_MISS1, SEQUENCE_DAGGERS_LIGHT_MISS5); break;
				case SEQUENCE_DEFAULT_HEAVY_MISS1:
					m_nSequence = RandomInt(SEQUENCE_DAGGERS_HEAVY_MISS2, SEQUENCE_DAGGERS_HEAVY_MISS1); break;
				case SEQUENCE_DEFAULT_HEAVY_HIT1:
				case SEQUENCE_DEFAULT_HEAVY_BACKSTAB:
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence += 3; break;
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				default:
					m_nSequence += 2;
				}
			}
			else if (!strcmp(szModel, TSTRING("models/weapons/v_knife_survival_bowie.mdl"))) {
				switch (m_nSequence) {
				case SEQUENCE_DEFAULT_DRAW:
				case SEQUENCE_DEFAULT_IDLE1:
					break;
				case SEQUENCE_DEFAULT_IDLE2:
					m_nSequence = SEQUENCE_BOWIE_IDLE1; break;
				default:
					m_nSequence--;
				}
			}
			else if (!strcmp(szModel, TSTRING("models/weapons/v_knife_ursus.mdl")))
			{
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_DRAW:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
					break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
					break;
				default:
					m_nSequence++;
					break;
				}
			}
			else if (!strcmp(szModel, TSTRING("models/weapons/v_knife_stiletto.mdl")))
			{
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(12, 13);
					break;
				}
			}
			else if (!strcmp(szModel, TSTRING("models/weapons/v_knife_widowmaker.mdl")))
			{
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(14, 15);
					break;
				}
			}
			else if (!strcmp(szModel, TSTRING("models/weapons/v_knife_css.mdl")))
			{
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = 15;
					break;
				}
			}
			else if (!strcmp(szModel, TSTRING("models/weapons/v_knife_cord.mdl")) ||
				!strcmp(szModel, TSTRING("models/weapons/v_knife_canis.mdl")) ||
				!strcmp(szModel, TSTRING("models/weapons/v_knife_outdoor.mdl")) ||
				!strcmp(szModel, TSTRING("models/weapons/v_knife_skeleton.mdl")))
			{
				switch (m_nSequence)
				{
				case SEQUENCE_DEFAULT_DRAW:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_DRAW, SEQUENCE_BUTTERFLY_DRAW2);
					break;
				case SEQUENCE_DEFAULT_LOOKAT01:
					m_nSequence = RandomInt(SEQUENCE_BUTTERFLY_LOOKAT01, 14);
					break;
				default:
					m_nSequence++;
				}
			}
			pData->Value.Int = m_nSequence;
		}
	}

	fnSequenceProxyFn(pData, pStruct, pOut);
}

void THooks::AnimationHook() {
	CBaseClient* pClass = TInterfaces::BaseClient->GetAllClasses();
	while (pClass)
	{
		const char* pszName = pClass->pRecvTable->szNetTableName;

		if (!strcmp(pszName, TSTRING("DT_BaseViewModel"))) {
			RecvTable_t* pClassTable = pClass->pRecvTable;

			for (int nIndex = 0; nIndex < pClass->pRecvTable->nProps; nIndex++) {
				RecvProp_t* pProp = &(pClass->pRecvTable->pProps[nIndex]);

				if (!pProp || strcmp(pProp->szVarName, TSTRING("m_nSequence")))
					continue;

				fnSequenceProxyFn = (RecvVarProxyFn)pProp->oProxyFn;

				pProp->oProxyFn = (RecvVarProxyFn)hkSetViewModelSequence;
			}
		}

		if (!strcmp(pszName, TSTRING("DT_BaseViewModel")))
		{
			for (int i = 0; i < pClass->pRecvTable->nProps; i++)
			{
				RecvProp_t* pProp = &(pClass->pRecvTable->pProps[i]);
				const char* name = pProp->szVarName;

				if (!strcmp(name, TSTRING("m_nModelIndex")))
				{
					oRecvnModelIndex = (RecvVarProxyFn)pProp->oProxyFn;
					pProp->oProxyFn = (RecvVarProxyFn)hkRecvProxyViewModel;
				}
			}
		}
		pClass = pClass->pNext;
	}
}

void THooks::AnimationUnHook() {
	for (CBaseClient* pClass = TInterfaces::BaseClient->GetAllClasses(); pClass; pClass = pClass->pNext) {
		if (!strcmp(pClass->szNetworkName, TSTRING("CBaseViewModel"))) {
			RecvTable_t* pClassTable = pClass->pRecvTable;

			for (int nIndex = 0; nIndex < pClassTable->nProps; nIndex++) {
				RecvProp_t* pProp = &pClassTable->pProps[nIndex];

				if (!pProp || strcmp(pProp->szVarName, TSTRING("m_nSequence")))
					continue;

				pProp->oProxyFn = fnSequenceProxyFn;

				break;
			}

			break;
		}
	}

	for (CBaseClient* pClass =TInterfaces::BaseClient->GetAllClasses(); pClass; pClass = pClass->pNext) {
		if (!strcmp(pClass->szNetworkName, TSTRING("CBaseViewModel"))) {
			RecvTable_t* pClassTable = pClass->pRecvTable;

			for (int nIndex = 0; nIndex < pClassTable->nProps; nIndex++) {
				RecvProp_t* pProp = &pClassTable->pProps[nIndex];

				if (!pProp || strcmp(pProp->szVarName, TSTRING("m_nModelIndex")))
					continue;

				pProp->oProxyFn = oRecvnModelIndex;

				break;
			}

			break;
		}
	}
}

int __fastcall THooks::hkListLeavesInBox(void* thisptr, int edx, const Vector3D& vecMins, const Vector3D& vecMaxs, unsigned short* puList, int nListMax)
{
	static auto oListLeavesInBox = dtrListLeavesInBox.GetOriginal<decltype(&hkListLeavesInBox)>();

	static std::uintptr_t uInsertIntoTree = (TUtil::FindPattern(CLIENT_STR, TSTRING("56 52 FF 50 18")) + 0x5); 

	if (reinterpret_cast<std::uintptr_t>(_ReturnAddress()) == uInsertIntoTree)
	{
		if (const auto pInfo = *reinterpret_cast<EntityRenderableInfo_t**>(reinterpret_cast<std::uintptr_t>(_AddressOfReturnAddress()) + 0x14); pInfo != nullptr)
		{
			if (const auto pRenderable = pInfo->pRenderable; pRenderable != nullptr)
			{
				if (const auto pEntity = pRenderable->GetIClientUnknown()->GetBaseEntity(); pEntity != nullptr && pEntity->IsPlayer())
				{
					pInfo->uFlags &= ~RENDER_FLAGS_FORCE_OPAQUE_PASS;
					pInfo->uFlags2 |= RENDER_FLAGS_BOUNDS_ALWAYS_RECOMPUTE;

					constexpr Vector3D vecMapMin(MIN_COORD_FLOAT, MIN_COORD_FLOAT, MIN_COORD_FLOAT);
					constexpr Vector3D vecMapMax(MAX_COORD_FLOAT, MAX_COORD_FLOAT, MAX_COORD_FLOAT);
					return oListLeavesInBox(thisptr, edx, vecMapMin, vecMapMax, puList, nListMax);
				}
			}
		}
	}

	return oListLeavesInBox(thisptr, edx, vecMins, vecMaxs, puList, nListMax);
}

int __fastcall THooks::hkRetrieveMessage(ISteamGameCoordinator* thisptr, int edx, std::uint32_t* puMsgType, void* pDest, std::uint32_t uDest, std::uint32_t* puMsgSize)
{
	static auto oRetrieveMessage = dtrRetrieveMessage.GetOriginal<decltype(&hkRetrieveMessage)>();
	const int iStatus = oRetrieveMessage(thisptr, edx, puMsgType, pDest, uDest, puMsgSize);

	if (iStatus != EGCResultOK)
		return iStatus;

	const std::uint32_t uMessageType = *puMsgType & 0x7FFFFFFF;

	if (uMessageType == 9177)
	{
		if (TVariables::TMisc::AutoAcceptMatchMaking.bEnabled) {
			static auto oSetLocalPlayerReady = reinterpret_cast<void(__stdcall*)(const char*)>(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12")));
			oSetLocalPlayerReady(TSTRING(""));
			FLASHWINFO fwInfo = { };
			fwInfo.cbSize = sizeof(FLASHWINFO);
			fwInfo.hwnd = IPT::hWindow;
			fwInfo.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
			fwInfo.uCount = 0;
			fwInfo.dwTimeout = 0;
			FlashWindowEx(&fwInfo);
		}
	}

	return iStatus;
}


bool __fastcall THooks::hkIsConnected(IEngineClient* thisptr, int edx)
{
	static auto oIsConnected = dtrIsConnected.GetOriginal<decltype(&hkIsConnected)>();

	static auto uLoadoutAllowedReturn = TUtil::FindPattern(CLIENT_STR, TSTRING("84 C0 75 05 B0 01 5F"));

	if (_ReturnAddress() == (void*)uLoadoutAllowedReturn && TVariables::TMisc::UnlockInventory.bEnabled)
		return false;

	return oIsConnected(thisptr, edx);
}


int __fastcall THooks::hkCanLoadThirdPartyFiles(void* ecx, void* edx)
{
	static auto oCanLoadThirdPartyFiles = vmtFileSystem->GetOriginal<int(__thiscall*)(void*)>(128);

	if (TVariables::TMisc::ServerPureBypass.bEnabled)
		return 1;
	else
		return oCanLoadThirdPartyFiles(ecx);
}

int __stdcall THooks::hkGetUnverifiedFileHashes(void* ecx, void* ptr, int nMaxFiles)
{
	return 0;
}

void __fastcall THooks::hkPhysicsSimulate(CBaseEntity* pBasePlayer, void* edx)
{
	auto oPhysicsSimulate = dtrPhysicsSimulate.GetOriginal<void(__fastcall*)(CBaseEntity*)>();

	int nSimulationTick = *(int*)((uintptr_t)pBasePlayer + 0x2AC); 
	CCommandContext* CommandContext = reinterpret_cast<CCommandContext*>(uintptr_t(pBasePlayer) + 0x350C);
	CUtlVector<CCommandContext>* CommandContextServer = reinterpret_cast<CUtlVector<CCommandContext>*>(uintptr_t(pBasePlayer) + 0x350C);

	if (pBasePlayer != TGlobalVars::pLocalEntity || !pBasePlayer || !pBasePlayer->IsAlive() || TInterfaces::Globals->iTickCount == nSimulationTick || !CommandContext->bNeedsProcessing || TInterfaces::EngineClient->IsPlayingDemo() || TInterfaces::EngineClient->IsHLTV() || pBasePlayer->GetFlags() & 0x40)
	{
		oPhysicsSimulate(pBasePlayer);
		return;
	}

	if (CommandContext->Cmd.iTickCount == 0x7FFFFFFF)
	{
		TNetvarData* NetData = &TPrediction::TNetvarFix::arrNetvarData[CommandContext->Cmd.iCommandNumber % MULTIPLAYER_BACKUP];
		TPrediction::TNetvarFix::Store(NetData, CommandContext, CommandContext->Cmd.iTickCount);
		nSimulationTick = TInterfaces::Globals->iTickCount;
		CommandContext->bNeedsProcessing = false;
		return;
	}
	else
	{
		TNetvarData* netData = &TPrediction::TNetvarFix::arrNetvarData[(CommandContext->Cmd.iCommandNumber - 1) % MULTIPLAYER_BACKUP];
		if (netData && netData->nCommandNumber == (CommandContext->Cmd.iCommandNumber - 1) && std::abs(netData->nCommandNumber - CommandContext->Cmd.iCommandNumber) <= MULTIPLAYER_BACKUP)
			TPrediction::TNetvarFix::Fix(CommandContext->Cmd.iCommandNumber - 1);

		if (CommandContext->Cmd.iCommandNumber == (TInterfaces::ClientState->iCommandAck + 1) && TPrediction::bOverrideModifier)
			pBasePlayer->GetVelocityModifier() = TPrediction::flVelocityModifier;

		if (netData && netData->nCommandNumber == (CommandContext->Cmd.iCommandNumber - 1) && std::abs(netData->nCommandNumber - CommandContext->Cmd.iCommandNumber) <= MULTIPLAYER_BACKUP)
			TPrediction::TNetvarFix::DetectPredictionError(netData, CommandContext->Cmd.iCommandNumber - 1);

		oPhysicsSimulate(pBasePlayer);

		netData = &TPrediction::TNetvarFix::arrNetvarData[CommandContext->Cmd.iCommandNumber % MULTIPLAYER_BACKUP];
		TPrediction::TNetvarFix::Store(netData, CommandContext,TInterfaces::Globals->iTickCount);
	}
}

void __fastcall THooks::hkBuildTransformations(CBaseEntity* pPlayer, uint32_t edx, CStudioHdr* pHdr, Vector3D* vecPosition, Quaternion* Quat, const matrix3x4_t& matTransform, int iMask, byte* byteBoneComputed)
{
	auto oBuildTransformations = dtrBuildTransformations.GetOriginal<void(__thiscall*)(CBaseEntity*, CStudioHdr*, Vector3D*, Quaternion*, const matrix3x4_t&, int, byte*)>();

	const auto iBackupFlags = pHdr->vecBoneFlags;

	for (auto index = 0; index < pHdr->vecBoneFlags.Count(); index++)
	{
		pHdr->vecBoneFlags.Element(index) &= ~BONE_ALWAYS_PROCEDURAL;
	}

	oBuildTransformations(pPlayer, pHdr, vecPosition, Quat, matTransform, iMask, byteBoneComputed);
	pHdr->vecBoneFlags = iBackupFlags;
}

Vector3D* __fastcall THooks::hkGetEyeAngles(CBaseEntity* pPlayer, void* edx) {

	static auto oGetEyeAngles = dtrGetEyeAngles.GetOriginal<Vector3D* (__thiscall*)(void*)>();

	static const auto First = reinterpret_cast<void*>(TUtil::FindPattern(CLIENT_STR,TSTRING("F3 0F 10 55 ? 51 8B 8E ? ? ? ?")));
	static const auto Second = reinterpret_cast<void*>(TUtil::FindPattern(CLIENT_STR, TSTRING("8B 55 0C 8B C8 E8 ? ? ? ? 83 C4 08 5E 8B E5")));
	static const auto Third = reinterpret_cast<void*>(TUtil::FindPattern(CLIENT_STR, TSTRING("8B CE F3 0F 10 00 8B 06 F3 0F 11 45 ? FF 90 ? ? ? ? F3 0F 10 55 ?")));

	if (!pPlayer || !TGlobalVars::pLocalEntity || pPlayer != TGlobalVars::pLocalEntity ||
		!TLocalAnimation::bInAnimUpdate || (
			_ReturnAddress() != First &&
			_ReturnAddress() != Second
			&& _ReturnAddress() != Third))
		return oGetEyeAngles(pPlayer);

	if (TLocalAnimation::bForceSentAngles)
		return &TLocalAnimation::vecAnglesSent;
	else
		return &TLocalAnimation::vecAngles;
}

void __fastcall	THooks::hkStandardBlendingRules(CBaseEntity* pPlayer, int i, studiohdr_t* pHdr, Vector3D* vecPosition, Quaternion* Quat, float flCurrentTime, int boneMask)
{
	static auto oStandardBlendingRules = dtrStandardBlendingRules.GetOriginal<void(__thiscall*)(CBaseEntity*, studiohdr_t*, Vector3D*, Quaternion*, float, int)>();
	
	boneMask |= BONE_USED_BY_HITBOX;

	oStandardBlendingRules(pPlayer, pHdr, vecPosition, Quat, flCurrentTime, boneMask);
}

bool __fastcall  THooks::hkShouldSkipAnimationFrame(void* ecx, void* edx)
{
	return false;
}

bool __fastcall THooks::hkSvCheatsGetBool(CConVar* thisptr, int edx)
{
	static auto oSvCheatsGetBool = dtrCheatsGetBool.GetOriginal<decltype(&hkSvCheatsGetBool)>();
	static std::uintptr_t uCAM_ThinkReturn = (TUtil::FindPattern(CLIENT_STR, TSTRING("85 C0 75 30 38 87"))); 

	if (reinterpret_cast<std::uintptr_t>(_ReturnAddress()) == uCAM_ThinkReturn)
		return true;

	return oSvCheatsGetBool(thisptr, edx);
}

void __fastcall THooks::hkPacketStart(void* ecx, void*, int incoming, int outgoing)
{
	static auto oPacketStart = pNetChannelHook->GetOriginal<void(__thiscall*)(void*, int, int)>(5);

	if (!TInterfaces::EngineClient->IsConnected())
		return oPacketStart(ecx, incoming, outgoing);

	if (!TInterfaces::ClientState->pNetChannel)
		return oPacketStart(ecx, incoming, outgoing);

	if (!TGlobalVars::pLocalEntity || !TGlobalVars::pLocalEntity->IsAlive())
		return oPacketStart(ecx, incoming, outgoing);

	if (TPacketHandler::TProcessPacket::dqCommandFixData.empty())
		return oPacketStart(ecx, incoming, outgoing);

	if (TInterfaces::GetGameRules()->IsValveDS())
		return oPacketStart(ecx, incoming, outgoing);

	for (auto it = TPacketHandler::TProcessPacket::dqCommandFixData.rbegin(); it != TPacketHandler::TProcessPacket::dqCommandFixData.rend(); ++it)
	{
		if (!it->bIsOutgoing)
			continue;

		if (it->iCommandNumber == outgoing || outgoing > it->iCommandNumber && (!it->bIsUsed || it->iPreviousCommandNumber == outgoing))
		{
			it->iPreviousCommandNumber = outgoing;
			it->bIsUsed = true;
			oPacketStart(ecx, incoming, outgoing);
			break;
		}
	}

	bool bResult = false;

	for (auto it = TPacketHandler::TProcessPacket::dqCommandFixData.begin(); it != TPacketHandler::TProcessPacket::dqCommandFixData.end();)
	{
		if (outgoing == it->iCommandNumber || outgoing == it->iPreviousCommandNumber)
			bResult = true;

		if (outgoing > it->iCommandNumber && outgoing > it->iPreviousCommandNumber)
			it = TPacketHandler::TProcessPacket::dqCommandFixData.erase(it);
		else
			++it;
	}

	if (!bResult)
		oPacketStart(ecx, incoming, outgoing);
}

void  __fastcall  THooks::hkPacketEnd(void* ecx, void* edx) {

	static auto oPacketEnd = pNetChannelHook->GetOriginal<void(__thiscall*)(void*)>(6);

	if (!TGlobalVars::pLocalEntity || !TGlobalVars::pLocalEntity->IsAlive())
	{
		TPacketHandler::TProcessPacket::dqCommandCorrectionData.clear();
		return oPacketEnd(ecx);
	}

	if (*(int*)((uintptr_t)ecx + 0x164) == *(int*)((uintptr_t)ecx + 0x16C))
	{
		int iCommandAck = *(int*)((uintptr_t)ecx + 0x4D2C);
		auto itCorrect = std::find_if(TPacketHandler::TProcessPacket::dqCommandCorrectionData.begin(), TPacketHandler::TProcessPacket::dqCommandCorrectionData.end(),
			[&iCommandAck](const TCommandCorrectionData& other_data)
			{
				return other_data.iCommandNumber == iCommandAck;
			}
		);

		auto pNetChannelInfo = (INetChannelInfo*)TInterfaces::ClientState->pNetChannel;

		if (pNetChannelInfo && itCorrect != TPacketHandler::TProcessPacket::dqCommandCorrectionData.end())
		{
			static float flLastVelocityModifier = 0.0f;

			if (flLastVelocityModifier > TGlobalVars::pLocalEntity->GetVelocityModifier() + 0.1f)
			{
				auto pBaseWeapon = TBaseEntity::GetWeapon(TGlobalVars::pLocalEntity);
				
				CCSWeaponData* pBaseWeaponData = nullptr;

				if (pBaseWeapon)
					pBaseWeaponData = TBaseEntity::GetWeaponData(pBaseWeapon);

				if (!pBaseWeapon || !pBaseWeaponData || pBaseWeapon->GetItemDefinitionIndex() != WEAPON_REVOLVER && pBaseWeaponData->nWeaponType!= WEAPONTYPE_GRENADE) 
				{
					for (auto& nCurrent : TPacketHandler::TProcessPacket::dqChokedNumber)
					{
						CUserCmd* pCmd = &TInterfaces::Input->pCommands[nCurrent % MULTIPLAYER_BACKUP];
						CVerifiedUserCmd* pVerifiedCommand = &TInterfaces::Input->pVerifiedCommands[nCurrent % MULTIPLAYER_BACKUP];

						if (pCmd->iButtons & (IN_ATTACK | IN_SECOND_ATTACK))
						{
							pCmd->iButtons &= ~IN_ATTACK;
							pVerifiedCommand->userCmd = *pCmd;
							pVerifiedCommand->uHashCRC = pCmd->GetChecksum();
						}
					}
				}
			}

			flLastVelocityModifier = TGlobalVars::pLocalEntity->GetVelocityModifier();
		}
	}

	return oPacketEnd(ecx);
}
bool __fastcall THooks::hkNetShowFragmentsGetBool(CConVar* thisptr, int edx)
{
	static auto oNetShowFragements = dtrShowFragments.GetOriginal<decltype(&hkNetShowFragmentsGetBool)>();

	if (!TInterfaces::EngineClient->IsInGame())
		return oNetShowFragements(thisptr, edx);

	static auto read_sub_channel_data_ret = TUtil::FindPattern(ENGINE_STR, TSTRING("85 C0 74 12 53 FF 75 0C 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 0C"));
	static auto check_receiving_list_ret = TUtil::FindPattern(ENGINE_STR, TSTRING("8B 1D ? ? ? ? 85 C0 74 16 FF B6"));

	static uint32_t last_fragment = 0;

	if (_ReturnAddress() == reinterpret_cast<void*>(read_sub_channel_data_ret) && last_fragment > 0)
	{
		const auto data = &reinterpret_cast<uint32_t*>(TInterfaces::ClientState->pNetChannel)[0x56];
		const auto bytes_fragments = reinterpret_cast<uint32_t*>(data)[0x43];

		if (bytes_fragments == last_fragment)
		{
			auto& buffer = reinterpret_cast<uint32_t*>(data)[0x42];
			buffer = 0;
		}
	}

	if (_ReturnAddress() == (void*)check_receiving_list_ret)
	{
		const auto data = &reinterpret_cast<uint32_t*>(TInterfaces::ClientState->pNetChannel)[0x56];
		const auto bytes_fragments = reinterpret_cast<uint32_t*>(data)[0x43];

		last_fragment = bytes_fragments;
	}

	return oNetShowFragements(thisptr, edx);
}

bool __fastcall THooks::hkIsHltv(IEngineClient* thisptr, void* edx)
{
	static auto oIsHltv = dtrIsHltv.GetOriginal<decltype(&hkIsHltv)>();

	if (TGlobalVars::bCallingSetupBones || TGlobalVars::bUpdatingAnimations)
		return true;

	static auto uAccumulateLayers = TUtil::FindPattern(CLIENT_STR, TSTRING("84 C0 75 0D F6 87"));
	static auto uSetupVelocityCall = TUtil::FindPattern(CLIENT_STR, TSTRING("84 C0 75 38 8B 0D ? ? ? ? 8B 01 8B 80 ? ? ? ? FF D0"));

	if (uintptr_t(_ReturnAddress()) == uAccumulateLayers || uintptr_t(_ReturnAddress()) == uSetupVelocityCall)
		return true;

	return oIsHltv(thisptr, edx);
}

void _fastcall THooks::hkSetupAliveLoop(CCSGOPlayerAnimState* thisptr, void* edx)
{
	if (!thisptr->m_pBasePlayer || thisptr->m_pBasePlayer != TGlobalVars::pLocalEntity)
		return;

	CAnimationLayer* m_AliveLoop = &thisptr->m_pBasePlayer->GetAnimationOverlays()[ANIMATION_LAYER_ALIVELOOP];

	if (!m_AliveLoop)
		return;

	if (m_AliveLoop->nSequence != 8 && m_AliveLoop->nSequence != 9)
	{
		thisptr->SetLayerSequence(m_AliveLoop, ACT_CSGO_ALIVE_LOOP);
		thisptr->SetLayerCycle(m_AliveLoop, TInterfaces::RandomFloat(0.0f, 1.0f));
		thisptr->SetLayerRate(m_AliveLoop, thisptr->GetLayerSequenceCycleRate(m_AliveLoop, m_AliveLoop->nSequence) * TInterfaces::RandomFloat(0.8, 1.1f));
	}
	else
	{
		float_t flRetainCycle = m_AliveLoop->flCycle;
		if (thisptr->m_pWeapon != thisptr->m_pWeaponLast)
		{
			thisptr->SetLayerSequence(m_AliveLoop, ACT_CSGO_ALIVE_LOOP);
			thisptr->SetLayerCycle(m_AliveLoop, flRetainCycle);
		}
		else if (thisptr->IsLayerSequenceFinished(m_AliveLoop, thisptr->m_flLastUpdateIncrement))
			thisptr->SetLayerRate(m_AliveLoop, thisptr->GetLayerSequenceCycleRate(m_AliveLoop, m_AliveLoop->nSequence) * TInterfaces::RandomFloat(0.8, 1.1f));
		else
			thisptr->SetLayerWeight(m_AliveLoop, TMath::RemapValClamped(thisptr->m_flSpeedAsPortionOfRunTopSpeed, 0.55f, 0.9f, 1.0f, 0.0f));
	}

	return thisptr->IncrementLayerCycle(m_AliveLoop, true);
}

bool __fastcall THooks::hkInPrediction(IPrediction* thisptr, void* edx)
{
	static auto oIsHltv = dtrInPrediction.GetOriginal<decltype(&hkInPrediction)>();

	static const auto uToReturn = (void*)TUtil::FindPattern(CLIENT_STR,TSTRING("84 C0 74 0A F3 0F 10 05 ? ? ? ? EB 05"));

	if (_ReturnAddress() == uToReturn)
		return false;

	return oIsHltv(thisptr,edx);
}

LPVOID THooks::hkGetClientModelRenderable(void* thisptr, void* edx)
{
	return nullptr;
}

void _fastcall THooks::hkPlayerMove(IGameMovement* thisptr, void* edx)
{
	static auto oPlayerMove = dtrPlayerMove.GetOriginal<decltype(&hkPlayerMove)>();
	oPlayerMove(thisptr,edx);
	
	if (!TGlobalVars::pLocalEntity || !TGlobalVars::pLocalEntity->IsAlive() || TInterfaces::GameMovement->GetPlayer() != TGlobalVars::pLocalEntity )
		return;

	if (!(TGlobalVars::pLocalEntity ->GetFlags() & FL_DUCKING) && !TGlobalVars::pLocalEntity ->IsDucking() && !TGlobalVars::pLocalEntity ->IsDucked())
		TGlobalVars::pLocalEntity ->GetViewOffset() = VEC_VIEW;
	else if (TGlobalVars::pLocalEntity->DuckUntilOnGround())
	{
		Vector3D hullSizeNormal = VEC_HULL_MAX - VEC_HULL_MIN;
		Vector3D hullSizeCrouch = VEC_DUCK_HULL_MAX - VEC_DUCK_HULL_MIN;
		Vector3D lowerClearance = hullSizeNormal - hullSizeCrouch;
		Vector3D duckEyeHeight = TInterfaces::GameMovement->GetPlayerViewOffset(false) - lowerClearance;

		TGlobalVars::pLocalEntity ->GetViewOffset() = duckEyeHeight;
	}
	else if (TGlobalVars::pLocalEntity ->IsDucked() && !TGlobalVars::pLocalEntity ->IsDucking())
		TGlobalVars::pLocalEntity ->GetViewOffset() = VEC_DUCK_VIEW;

}

bool __fastcall THooks::hkClPredDoResetLatch(CConVar* thisptr, int edx)
{
	static auto oClPredDoResetLatch = dtrPredDoResetLatch.GetOriginal<decltype(&hkClPredDoResetLatch)>();
	
	static const auto uToReturn = (void*)TUtil::FindPattern(CLIENT_STR, TSTRING("85 C0 75 28 8B 0D ? ? ? ? 81"));

	if (_ReturnAddress() == uToReturn)
		return false;

	return oClPredDoResetLatch(thisptr,edx);
}

bool __fastcall THooks::hkProcessTempEntities(void* thisptr, void* edx, void* pMsg)
{
	static auto oProcessTempEntites = dtrProcessTempEntities.GetOriginal<decltype(&hkProcessTempEntities)>();

	int nMaxClients = TInterfaces::ClientState->nMaxClients;

	TInterfaces::ClientState->nMaxClients = 1;
	bool bResult = oProcessTempEntites(thisptr, edx,pMsg);
	TInterfaces::ClientState->nMaxClients = nMaxClients;

	return bResult;
}

void __fastcall THooks::hkProcessMovement(IGameMovement* thisptr, void*edx, CBaseEntity* pPlayer, CMoveData* m_MoveData)
{
	static auto oProcessMovement = dtrProcessMovement.GetOriginal<decltype(&hkProcessMovement)>();
	m_MoveData->bGameCodeMovedPlayer = false;
	return oProcessMovement(thisptr, edx, pPlayer, m_MoveData);
}

void THooks::hkClMove(float flFrameTime, bool bIsFinalTick)
{
	static auto oClMove = dtrClMove.GetOriginal<decltype(&hkClMove)>();
	oClMove(flFrameTime,bIsFinalTick);
}

bool THooks::Hook() {
	
	if (MH_Initialize() != MH_OK)
		return false;

	CClientEffectRegistration* pEffectsHead = **reinterpret_cast<CClientEffectRegistration***>(TUtil::FindPattern(CLIENT_STR,TSTRING("8B 35 ? ? ? ? 85 F6 0F 84 ? ? ? ? 0F 1F ? 8B 3E"))+ 2);

	for (CClientEffectRegistration* head = pEffectsHead; head; head = head->next)
	{
		if (strstr(head->effectName, TSTRING("Impact")) && strlen(head->effectName) <= 8) {
			oImpactCallback = head->function;
			head->function = &hkImpactCallback;
			break;
		}
	}

	IPT::Setup(reinterpret_cast<LONG_PTR>(hkWndProc));

	vmtClientModeHook = std::make_unique< VMTHook >();
	vmtClientModeHook->Setup(TInterfaces::ClientMode);
	vmtClientModeHook->Hook(17, &hkShouldDrawFog);

	vmtFileSystem = std::make_unique< VMTHook >();
	vmtFileSystem->Setup(TInterfaces::FileSystem);
	vmtFileSystem->Hook(128, &hkCanLoadThirdPartyFiles);
	vmtFileSystem->Hook(101, &hkGetUnverifiedFileHashes);

	pNetChannelHook = std::make_unique< VMTHook >();
	pNetChannelHook->Setup((IClientState*)(uint32_t(TInterfaces::ClientState) + 0x8));

	pNetChannelHook->Hook(5, &hkPacketStart);
	pNetChannelHook->Hook(6, &hkPacketEnd);

	dtrAllocKeyValuesMemory.Create(TUtil::GetVFunc(TInterfaces::KeyValueSystem, 2), &hkAllocKeyValuesMemory);
	dtrCreateMoveProxy.Create(TUtil::GetVFunc(TInterfaces::BaseClient, 22), &hkCreateMoveProxy);
	dtrRunCommand.Create(TUtil::GetVFunc(TInterfaces::Prediction, 19), &hkRunCommand);
	dtrFrameStageNotify.Create(TUtil::GetVFunc(TInterfaces::BaseClient, 37), &hkFrameStageNotify);
	dtrReset.Create(TUtil::GetVFunc(TInterfaces::DirectDevice, 16), &hkReset);
	dtrEndScene.Create(TUtil::GetVFunc(TInterfaces::DirectDevice, 42), &hkEndScene);
	dtrPaintTraverse.Create(TUtil::GetVFunc(TInterfaces::Panel, 41), &hkPaintTraverse);
	dtrLockCursor.Create(TUtil::GetVFunc(TInterfaces::Surface, 67), &hkLockCursor);

	dtrSetupBones.Create((PVOID)TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 57 8B F9 8B")), &hkSetupBones);
	dtrUpdateClientSideAnimations.Create((PVOID)TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 51 56 8B F1 80 BE ? ? ? ? ? 74 36")), &hkUpdateClientSideAnimations);

	dtrClampBonesInBox.Create((PVOID)TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 38 83 BF ? ? ? ? ? 75")), &hkClampBonesInBBox);

	dtrGetViewModelFov.Create(TUtil::GetVFunc(TInterfaces::ClientMode, 35), &hkGetViewModelFov);
	dtrGetAspectRatio.Create(TUtil::GetVFunc(TInterfaces::EngineClient, 101), &hkGetAspectRatio);
	dtrDoPostScreenEffects.Create(TUtil::GetVFunc(TInterfaces::ClientMode, 44), &hkDoPostScreenEffects);

	dtrOverrideView.Create(TUtil::GetVFunc(TInterfaces::ClientMode, 18), &hkOverrideView);
	
	recGetFlashDuration = new CRecvPropHook(TNetvarManager::mapProps[FNV1A::HashConst("CCSPlayer->m_flFlashDuration")].pRecvProp,
		(RecvVarProxyFn)&hkGetFlashDuration);

	dtrCalcViewModelBob.Create((void*)TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC A1 ? ? ? ? 83 EC 10 56 8B F1 B9")), &hkCalcViewModelBob);

	dtrInterpolatedList.Create((void*)TUtil::FindPattern(CLIENT_STR, TSTRING("53 0F B7 1D ? ? ? ? 56")), &hkProcessInterpolatedList);
	dtrModifyEyePosition.Create((void*)TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 E4 F8 83 EC 70 56 57 8B F9 89 7C 24 14")), &hkModifyEyePosition);

	dtrPhysicsSimulate.Create((void*)TUtil::FindPattern(CLIENT_STR, TSTRING("56 8B F1 8B 8E ? ? ? ? 83 F9 FF 74 23")), &hkPhysicsSimulate);

	dtrGetEyeAngles.Create((void*)TUtil::FindPattern(CLIENT_STR, TSTRING("56 8B F1 85 F6 74 32")), &hkGetEyeAngles);

	dtrBuildTransformations.Create((void*)TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 E4 F0 81 ? ? ? ? ? 56 57 8B F9 8B ? ? ? ? ? 89 7C 24 28")), &hkBuildTransformations);
	
	dtrStandardBlendingRules.Create((void*)TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 E4 F0 B8 ? ? ? ? E8 ? ? ? ? 56 8B 75 08 57 8B F9 85")), &hkStandardBlendingRules);
	dtrShouldSkipAnimationFrame.Create((void*)TUtil::FindPattern(CLIENT_STR, TSTRING("57 8B F9 8B 07 8B ? ? ? ? ? FF D0 84 C0 75 02")), &hkShouldSkipAnimationFrame);

	dtrOverrideConfig.Create((void*)TUtil::GetVFunc(TInterfaces::MaterialSystem, 21), &hkOverrideConfig);
	dtrDrawModel.Create((void*)TUtil::GetVFunc(TInterfaces::StudioRender, 29), &hkDrawModel);
	dtrListLeavesInBox.Create(TUtil::GetVFunc(TInterfaces::EngineClient->GetBSPTreeQuery(), 6), &hkListLeavesInBox);
	dtrRetrieveMessage.Create(TUtil::GetVFunc(TInterfaces::SteamGameCoordinator, 2), &hkRetrieveMessage);
	dtrIsConnected.Create(TUtil::GetVFunc(TInterfaces::EngineClient, 27), &hkIsConnected);
	dtrCheatsGetBool.Create(TUtil::GetVFunc(TConvarManager::varServerCheats.pBase, 13), &hkSvCheatsGetBool);

	dtrPredDoResetLatch.Create(TUtil::GetVFunc(TConvarManager::varPredDoResetLatch.pBase, 13), &hkClPredDoResetLatch);

	dtrShowFragments.Create(TUtil::GetVFunc(TConvarManager::varNetShowFragments.pBase, 13), &hkNetShowFragmentsGetBool);
	dtrIsHltv.Create(TUtil::GetVFunc(TInterfaces::EngineClient, 93), &hkIsHltv);

	dtrSetupAliveLoop.Create((void*)TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 51 56 8B 71 60 83 BE 9C 29 00 00 00 0F 84 93 00 00 00 8B")), &hkSetupAliveLoop);

	dtrInPrediction.Create(TUtil::GetVFunc(TInterfaces::Prediction, 14), &hkInPrediction);

	dtrPlayerMove.Create((void*)TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 EC 08 56 8B F1 8B 8E 54 0E 00 00 E8")), &hkPlayerMove);

   // dtrGetClientModeRenderable.Create((void*)TUtil::FindPattern(CLIENT_STR, TSTRING("56 8B F1 80 BE FC 26")), &hkGetClientModelRenderable);

	dtrProcessTempEntities.Create((void*)TUtil::FindPattern(ENGINE_STR, TSTRING("55 8B EC 83 E4 F8 83 EC 4C A1 ? ? ? ? 80 B8")), &hkProcessTempEntities);

	dtrProcessMovement.Create(TUtil::GetVFunc(TInterfaces::GameMovement, 1), &hkProcessMovement);

	dtrClMove.Create((void*)TUtil::FindPattern(ENGINE_STR, TSTRING("55 8B EC 81 EC 64 01 00 00 53 56 8A F9")), &hkClMove);

	AnimationHook();

	MH_EnableHook(MH_ALL_HOOKS);
	return dtrAllocKeyValuesMemory.IsHooked() && dtrCreateMoveProxy.IsHooked() && dtrFrameStageNotify.IsHooked() && dtrReset.IsHooked() 
		&& dtrEndScene.IsHooked() && dtrPaintTraverse.IsHooked() && dtrSetupBones.IsHooked() && dtrClampBonesInBox.IsHooked() 
		&& dtrGetViewModelFov.IsHooked() && dtrGetAspectRatio.IsHooked() && dtrDoPostScreenEffects.IsHooked() && dtrOverrideView.IsHooked() 
		&& dtrInterpolatedList.IsHooked() && dtrDrawModel.IsHooked() && dtrOverrideConfig.IsHooked() && dtrModifyEyePosition.IsHooked()
		&& dtrListLeavesInBox.IsHooked() && dtrRetrieveMessage.IsHooked() && dtrIsConnected.IsHooked() && dtrPhysicsSimulate.IsHooked()
		&& dtrGetEyeAngles.IsHooked() && dtrStandardBlendingRules.IsHooked() && dtrBuildTransformations.IsHooked() && dtrShowFragments.IsHooked()
		&& dtrIsHltv.IsHooked();
}

bool THooks::Unhook() {

	MH_DisableHook(MH_ALL_HOOKS);
	MH_RemoveHook(MH_ALL_HOOKS);

	MH_Uninitialize();

	CClientEffectRegistration* pEffectsHead = **reinterpret_cast<CClientEffectRegistration***>(TUtil::FindPattern(CLIENT_STR, TSTRING("8B 35 ? ? ? ? 85 F6 0F 84 ? ? ? ? 0F 1F ? 8B 3E")) + 2);

	for (CClientEffectRegistration* head = pEffectsHead; head; head = head->next)
	{
		if (strstr(head->effectName, TSTRING("Impact")) && strlen(head->effectName) <= 8) {
			head->function = oImpactCallback;
			break;
		}
	}

	AnimationUnHook();
	IPT::Restore();

	vmtClientModeHook->RestoreTable();
	delete vmtClientModeHook->uShadowVtable;
	vmtClientModeHook.release();

	vmtFileSystem->RestoreTable();
	delete vmtFileSystem->uShadowVtable;
	vmtFileSystem.release();

	pNetChannelHook->RestoreTable();
	delete pNetChannelHook->uShadowVtable;
	pNetChannelHook.release();


	recGetFlashDuration->Restore();
	delete recGetFlashDuration;

	return true;
}
