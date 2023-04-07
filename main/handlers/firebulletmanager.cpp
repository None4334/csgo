#include "../../includes.h"
#include "entityhandler.h"
#include "firebulletmanager.h"
#include "../gvars.h"
#include "../sdk.h"
#include "../math.h"
#include "convarmanager.h"
#include "animationsmanager.h"
#include "eventlogger.h"
#include "../features/visuals.h"
#include "../ui/vars.h"
#include "cmdmgr.h"
#include "../features/misc.h"
#include "../features/chams.h"

void TFireBullet::Routine() {
	
	if (stkFireBullets.size())
	{
		while (stkFireBullets.first() && (stkFireBullets.first()->iTickcount + 75 < TInterfaces::Globals->iTickCount || stkFireBullets.first()->UsageInfo.FullyUsed() || !stkFireBullets.first()->stkImpacts.size()))
		{
			stkFireBullets.pop_front();
		}
	}

	if (!stkFireBullets.size())
		return;

	for (int iCurrent = stkFireBullets.size() - 1; iCurrent >= 0; iCurrent--)
	{
		TFireBulletInfo* Object = stkFireBullets[iCurrent];

			if (!Object->UsageInfo.FullyUsed()) {

				if (Object->pOwnerEntity == TGlobalVars::pLocalEntity && TGlobalVars::pLocalEntity) {

					if (!Object->UsageInfo.bUsedForHitmarker)
					{
						if (Object->PlayerHurtInfo.bHasHurtInfo)
						{

							Vector3D vecClosestPosition = TMath::GetClosestPosition(Object->stkImpacts.Base(), Object->stkImpacts.size(), Object->PlayerHurtInfo.vecEndHurtPos);

							if (!vecClosestPosition.IsZero())
								Object->PlayerHurtInfo.vecEndHurtPos = vecClosestPosition;

							stkPlayerHurt.add_back(Object->PlayerHurtInfo);
							Object->UsageInfo.bUsedForHitmarker = true;
						}

					}

				    if (!Object->UsageInfo.bUsedForTracerAndImpacts)
				    {

					    if (TVariables::TVisuals::bEnabled )
					    {
							if (TVariables::TVisuals::TOthers::ServerImpacts.bEnabled)
						       for (int Current = 0; Current < Object->stkImpacts.size();Current++)
							      TInterfaces::DebugOverlay->AddBoxOverlay(*Object->stkImpacts[Current], Vector3D(-2, -2, -2), Vector3D(2, 2, 2), Vector3D(0, 0, 0),
								      TVariables::ColorToVector(TVariables::TVisuals::TOthers::ServerImpacts.Color.flColors), 4.f);
						
							if (TVariables::TVisuals::TOthers::ClientImpacts.bEnabled) {
								static auto iLastCount = 0;
								auto& ClientImpactList = *(CUtlVector <TClientHitImpact>*)((uintptr_t)TGlobalVars::pLocalEntity + 0x11C50);

								for (auto i = ClientImpactList.Count(); i > iLastCount; --i)
									TInterfaces::DebugOverlay->AddBoxOverlay(ClientImpactList[i - 1].vecPos, Vector3D(-2, -2, -2), Vector3D(2, 2, 2), Vector3D(0, 0, 0),
										TVariables::ColorToVector(TVariables::TVisuals::TOthers::ClientImpacts.Color.flColors), 4.f);

								if (ClientImpactList.Count() != iLastCount)
									iLastCount = ClientImpactList.Count();
							}
					    }

						int iTotalTickCount = TIME_TO_TICKS(TCommandManager::GetTotalPacketTime());
						TCommandInfo* CmdObject = TCommandManager::GetSentCommandInfoFudge(TCommandManager::nCurrentCommand - iTotalTickCount, iTotalTickCount, true);

						if (CmdObject) {

							if (CmdObject->RecordInfo.bHasShotRecord && CmdObject->RecordInfo.EntityObject) {
								AddModelRecord(CmdObject->RecordInfo.EntityObject, &CmdObject->RecordInfo.AnimationRecord);
								CmdObject->RecordInfo.bHasShotRecord = false;
								CmdObject->RecordInfo.EntityObject = nullptr;
							}

							if (TVariables::TVisuals::TOthers::BulletTracers.bEnabled)
								TFireBullet::DrawTracer(CmdObject->vecEyePosition, *Object->stkImpacts.last());

						}

					    Object->UsageInfo.bUsedForTracerAndImpacts = true;
				    }



			    }

		    }
	}

}

void TFireBullet::DrawTracer(const Vector3D& vecStart, const Vector3D& vecEnd) {
	
	Vector4D vecColor = TVariables::ColorToVector(TVariables::TVisuals::TOthers::BulletTracers.Color.flColors);
	BeamInfo_t pBeamInfo;
	pBeamInfo.vecStart = vecStart;
	pBeamInfo.vecStart.z -= 2.0f;
	pBeamInfo.vecEnd = vecEnd;
	pBeamInfo.nType = TE_BEAMPOINTS;
	pBeamInfo.pszModelName = TSTRING("sprites/purplelaser1.vmt");
	pBeamInfo.pszHaloName = TSTRING("sprites/purplelaser1.vmt");
	pBeamInfo.nModelIndex = -1;
	pBeamInfo.flHaloScale = 0.0f;
	pBeamInfo.flLife = 4.f;
	pBeamInfo.flWidth = 3.0f;
	pBeamInfo.flEndWidth = 3.0f;
	pBeamInfo.flFadeLength = 0.0f;
	pBeamInfo.flAmplitude = 3.0f;
	pBeamInfo.flBrightness = vecColor.w;
	pBeamInfo.flSpeed = 0.2f;
	pBeamInfo.iStartFrame = 0;
	pBeamInfo.flFrameRate = 0.0f;
	pBeamInfo.flRed = vecColor.x;
	pBeamInfo.flGreen = vecColor.y;
	pBeamInfo.flBlue = vecColor.z;
	pBeamInfo.nSegments = 2;
	pBeamInfo.bRenderable = true;
	pBeamInfo.nFlags = 0;

	auto pBeam = TInterfaces::ViewRenderBeams->CreateBeamPoints(pBeamInfo);

	if (pBeam)
		TInterfaces::ViewRenderBeams->DrawBeam(pBeam);
}

void TFireBullet::OnEvent(IGameEvent* pEvent, FNV1A_t uNameHash) {

	static FNV1A_t PlayerHurt = FNV1A::HashConst("player_hurt");

	static FNV1A_t WeaponFire = FNV1A::HashConst("weapon_fire");

	static FNV1A_t BulletImpact = FNV1A::HashConst("bullet_impact");

	static FNV1A_t RoundEnd = FNV1A::HashConst("round_end");

	static FNV1A_t RoundStart = FNV1A::HashConst("round_start");

	static FNV1A_t ItemPurchaseEvent = FNV1A::HashConst("item_purchase");
	
	if (ItemPurchaseEvent == uNameHash)
	{
		if (TVariables::TMisc::ItemPurchaseLog.bEnabled) {
			CBaseEntity* pPurchaser = TInterfaces::ClientEntityList->Get<CBaseEntity>(
				TInterfaces::EngineClient->GetPlayerForUserID(pEvent->GetInt(TSTRING("userid"))));

			if (pPurchaser)
			{
				if (!TGlobalVars::pLocalEntity || (TGlobalVars::pLocalEntity != pPurchaser && !TGlobalVars::pLocalEntity->IsEnemy(pPurchaser)))
				{
					PlayerInfo_t Info;
					if (TInterfaces::EngineClient->GetPlayerInfo(pPurchaser->GetIndex(), &Info))
					{
						std::string szLog = Info.szName;

						szLog.append(TSTRING(" purchased "));
						szLog.append(pEvent->GetString());

						TEventLogger::PushString(szLog);
					}
				}
			}
		}
	}

	if (RoundStart == uNameHash)
	{
		TMisc::TBuyBot::Run(true);
		TInterfaces::GetGameRules(true);
		TMisc::TOther::bClearNotices = true;
	}

	if (WeaponFire == uNameHash)
	{
		CBaseEntity* pPlayer = TInterfaces::ClientEntityList->Get<CBaseEntity>(
			TInterfaces::EngineClient->GetPlayerForUserID(pEvent->GetInt(TSTRING("userid"))));

		if (pPlayer && pPlayer->GetClientClass()->nClassID == EClassIndex::CCSPlayer) {
			TFireBulletInfo FireBulletInfo;
			FireBulletInfo.iTickcount = TInterfaces::Globals->iTickCount;
			FireBulletInfo.pOwnerEntity = pPlayer;
			stkFireBullets.add_back(FireBulletInfo);
		}
	}

	if (BulletImpact == uNameHash)
	{
		CBaseEntity* pPlayer = TInterfaces::ClientEntityList->Get<CBaseEntity>(
			TInterfaces::EngineClient->GetPlayerForUserID(pEvent->GetInt(TSTRING("userid"))));

		TFireBulletInfo* pShotInfo = stkFireBullets.last();

		if (pShotInfo && pShotInfo->pOwnerEntity == pPlayer) {
			Vector3D vecPosition = Vector3D(pEvent->GetFloat(TSTRING("x")), pEvent->GetFloat(TSTRING("y")), pEvent->GetFloat(TSTRING("z")));
			pShotInfo->stkImpacts.add_back(vecPosition);
		}
	}

	if (PlayerHurt == uNameHash)
	{
		TFireBulletInfo* pShotInfo = stkFireBullets.last();

		if (pShotInfo) {
			CBaseEntity* pAttacker = TInterfaces::ClientEntityList->Get<CBaseEntity>(
				TInterfaces::EngineClient->GetPlayerForUserID(pEvent->GetInt(TSTRING("attacker"))));

			CBaseEntity* pVictim = TInterfaces::ClientEntityList->Get<CBaseEntity>(
				TInterfaces::EngineClient->GetPlayerForUserID(pEvent->GetInt(TSTRING("userid"))));

			if (pShotInfo->pOwnerEntity == pAttacker && pAttacker->GetClientClass()->nClassID == EClassIndex::CCSPlayer
				&& pVictim && pVictim->GetClientClass()->nClassID == EClassIndex::CCSPlayer)
			{

				if (TVariables::TVisuals::TOthers::HitSound.bEnabled && TGlobalVars::pLocalEntity == pAttacker)
				{
					TInterfaces::Surface->PlaySoundSurface(TSTRING("buttons\\arena_switch_press_02.wav"));
				}

				pShotInfo->PlayerHurtInfo.iHurtHitgroup = pEvent->GetInt(TSTRING("hitgroup"));
				pShotInfo->PlayerHurtInfo.iDamageHurt = pEvent->GetInt(TSTRING("dmg_health"));
				pShotInfo->PlayerHurtInfo.iHealth = pEvent->GetInt(TSTRING("health"));
				pShotInfo->PlayerHurtInfo.iHitbox = TBaseEntity::GetHitboxForHitGroup(pShotInfo->PlayerHurtInfo.iHurtHitgroup);
				pShotInfo->PlayerHurtInfo.vecEndHurtPos = TBaseEntity::GetHitboxPosition(TEntityListener::GetEntityObject(pVictim), pShotInfo->PlayerHurtInfo.iHitbox);
				pShotInfo->PlayerHurtInfo.bHasHurtInfo = true;

			}
		}

	}
}

void TFireBullet::RenderRecord(TEntityObject* Object, TModelShotRecord* Record) {

	auto pRenderContext = TInterfaces::MaterialSystem->GetRenderContext();

	if (!pRenderContext)
		return;

	CBaseEntity* pPlayer = TInterfaces::ClientEntityList->Get<CBaseEntity>(Record->iPlayerIndex);

	if (!pPlayer || !pPlayer->IsPlayer() || !Record->state.pRenderable || !Record->state.pModelToWorld || !Record->state.pRenderable || !Record->state.pStudioHdr
		|| !Record->state.pStudioHdr->nID || !Record->state.pStudioHWData || !Record->info.pRenderable || !Record->info.pModelToWorld || !Record->info.pModel)
		return;


	IMaterial* pMaterialMain = nullptr;
	IMaterial* pMaterialExtra = TModelChams::matMaterialsDME[MAT_GLOW];

	if (TVariables::TModels::TEnemy::BacktrackHidden.Value != 0) {

		pMaterialMain = TModelChams::matMaterialsDME[TVariables::TModels::TEnemy::BacktrackHidden.Value - 1];

		pMaterialMain->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
		pMaterialMain->SetColorModulation(TVariables::TModels::TEnemy::BacktrackHidden.Color.flColors);
		TInterfaces::RenderView->SetBlend(TVariables::TModels::TEnemy::BacktrackHidden.Color.flColors[3]);
		TInterfaces::ModelRender->ForcedMaterialOverride(pMaterialMain);

		if (TVariables::TModels::TEnemy::BacktrackOverlayHidden.bEnabled)
		{
			TGlobalVars::bCallingDrawModelExecute = true;
			TInterfaces::ModelRender->DrawModelExecute(pRenderContext, Record->state, Record->info, Record->matModel);
			TGlobalVars::bCallingDrawModelExecute = false;
			
			pMaterialExtra->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
			TInterfaces::ModelRender->ForcedMaterialOverride(pMaterialExtra);

			IMaterialVar* Overlay = pMaterialExtra->FindVar(("$envmaptint"), nullptr);
			Overlay->SetVector(TVariables::TModels::TEnemy::BacktrackOverlayHidden.Color.flColors);

		}

		TGlobalVars::bCallingDrawModelExecute = true;
		TInterfaces::ModelRender->DrawModelExecute(pRenderContext, Record->state, Record->info, Record->matModel);
		TGlobalVars::bCallingDrawModelExecute = false;
	}

	if (TVariables::TModels::TEnemy::BacktrackVisible.Value != 0) {

		pMaterialMain = TModelChams::matMaterialsDME[TVariables::TModels::TEnemy::BacktrackVisible.Value - 1];

		pMaterialMain->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
		pMaterialMain->SetColorModulation(TVariables::TModels::TEnemy::BacktrackVisible.Color.flColors);
		TInterfaces::RenderView->SetBlend(TVariables::TModels::TEnemy::BacktrackVisible.Color.flColors[3]);
		TInterfaces::ModelRender->ForcedMaterialOverride(pMaterialMain);

		if (TVariables::TModels::TEnemy::BacktrackOverlayVisible.bEnabled)
		{
			TGlobalVars::bCallingDrawModelExecute = true;
			TInterfaces::ModelRender->DrawModelExecute(pRenderContext, Record->state, Record->info, Record->matModel);
			TGlobalVars::bCallingDrawModelExecute = false;

			pMaterialExtra->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			TInterfaces::ModelRender->ForcedMaterialOverride(pMaterialExtra);

			IMaterialVar* Overlay = pMaterialExtra->FindVar(("$envmaptint"), nullptr);
			Overlay->SetVector(TVariables::TModels::TEnemy::BacktrackOverlayVisible.Color.flColors);

		}

		TGlobalVars::bCallingDrawModelExecute = true;
		TInterfaces::ModelRender->DrawModelExecute(pRenderContext, Record->state, Record->info, Record->matModel);
		TGlobalVars::bCallingDrawModelExecute = false;

	}

	TInterfaces::RenderView->SetBlend(1.f);
	TInterfaces::ModelRender->ForcedMaterialOverride(nullptr);

}

void TFireBullet::RenderRecords() {

	if (TEntityListener::vecEntities.empty())
		return;

	for (auto& CurrentObject : TEntityListener::vecEntities)
	{
		if (CurrentObject->nClass != (int)EClassIndex::CCSPlayer)
			continue;

		while (CurrentObject->stkShotModel.first() && CurrentObject->stkShotModel.first()->iTickcount + 3000 < GetTickCount64())
			CurrentObject->stkShotModel.pop_front();

		if (CurrentObject->stkShotModel.empty())
			continue;

		for (int iCurrent =0;iCurrent < CurrentObject->stkShotModel.size();iCurrent++)
		{
			auto Object = CurrentObject->stkShotModel[iCurrent];

			RenderRecord(CurrentObject,Object);
		}
	}
}

void TFireBullet::AddModelRecord(TEntityObject* Object, TAnimationRecord* pAnimationRecord) {

	if (!Object)
		return;

	auto pRecord = Object->stkShotModel.add_back(TModelShotRecord());

	pRecord->iTickcount = GetTickCount64();
	pRecord->iPlayerIndex = Object->pEntity->GetIndex();

	std::memcpy(pRecord->matModel, pAnimationRecord->matModel, sizeof(matrix3x4_t) * 128);

	pRecord->info.vecOrigin = pAnimationRecord->vecOrigin;
	pRecord->info.angAngles = pAnimationRecord->vecAbsAngles;

	auto pClientRenderable = Object->pEntity->GetClientRenderable();

	if (!pClientRenderable)
		return;

	auto model = Object->pEntity->GetModel();

	if (!model)
		return;

	studiohdr_t* pStudioHdr = TInterfaces::ModelInfo->GetStudioModel(model);

	if (!pStudioHdr)
		return;

	pRecord->state.pStudioHdr = pStudioHdr;
	pRecord->state.pStudioHWData = TInterfaces::MDLCache->GetHardwareData(model->pStudioModel);
	pRecord->state.pRenderable = pClientRenderable;
	pRecord->state.iDrawFlags = 0;
	pRecord->info.pRenderable = pClientRenderable;
	pRecord->info.pModel = model;
	pRecord->info.pLightingOffset = nullptr;
	pRecord->info.pLightingOrigin = nullptr;
	pRecord->info.iHitboxSet = Object->pEntity->GetHitboxSet();
	pRecord->info.iSkin = Object->pEntity->GetSkinFromMap();
	pRecord->info.iBody = Object->pEntity->GetBodyFromMap();
	pRecord->info.nEntityIndex = Object->pEntity->GetIndex();
	pRecord->info.hInstance = pClientRenderable->GetModelInstance();
	pRecord->info.iFlags = 0x1;
	pRecord->info.pModelToWorld = pRecord->matModelToWorld;
	pRecord->state.pModelToWorld = pRecord->matModelToWorld;
	TMath::AngleMatrix(pRecord->info.angAngles, pRecord->info.vecOrigin, *pRecord->matModelToWorld);
}