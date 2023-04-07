#include "../../includes.h"
#include "visuals.h"
#include "../renderer.h"
#include "../handlers/entityhandler.h"
#include "../gvars.h"
#include "../sdk.h"
#include "../ui/vars.h"
#include "../handlers/animationsmanager.h"
#include "../handlers/convarmanager.h"
#include "../handlers/localengine.h"

#include "../../dependencies/imgui/imgui.h"
#include "../../dependencies/imgui/imgui_internal.h"
#include "../../dependencies/fonts/weaponicons.h"
#include "../../dependencies/imgui/imgui_freetype.h"
#include "../handlers/firebulletmanager.h"
#include "../handlers/eventlogger.h"
#include "nadepred.h"
#include "nadewarning.h"

void TVisuals::RunVisuals(ISurface* SurfaceHandle, unsigned int uPanel, bool bForceRepaint, bool bForce) {

	if (!TDirectXRender::TDirectFonts::bInitialized)
		return;

	if (TEntityListener::vecEntities.empty()  || !TGlobalVars::pLocalEntity)
		return;

	static int iWidth = 0, iHeight = 0;
	TInterfaces::EngineClient->GetScreenSize(iWidth, iHeight);

	if (!TGlobalVars::vecScreenSize.IsZero())
	{
		if (TGlobalVars::vecScreenSize.x != iWidth || TGlobalVars::vecScreenSize.y != iHeight) 
			TSurfaceRender::Initialize();
	}
	
	TGlobalVars::vecScreenSize = Vector2D(iWidth, iHeight);

	PerTick();

	TVisuals::TLocal::FireBulletRoutine();

	if (!TVariables::TVisuals::bEnabled)
		return;


	TLocal::ScopeLines();

	if (TGlobalVars::pLocalEntity->IsAlive())
	    TNadePrediction::Render(TGlobalVars::pLocalEntity);

	matCurrentWorldMatrix = TInterfaces::EngineClient->WorldToScreenMatrix();
	
	float flServerTime = TICKS_TO_TIME(TGlobalVars::pLocalEntity->GetTickBase());

	for (auto CurrentObject : TEntityListener::vecEntities)
	{
		if (CurrentObject->pEntity == TGlobalVars::pLocalEntity || CurrentObject->nIndex == TGlobalVars::LocalIndex)
			continue;

		if (CurrentObject->nClass == (int)EClassIndex::CCSPlayer)
			Player(CurrentObject);
		else
		{

			if (CurrentObject->nClass == (int)EClassIndex::CPlantedC4)
			{
				if (TVariables::TVisuals::TOthers::BombInfo.bEnabled) {

					CPlantedC4* pBomb = reinterpret_cast<CPlantedC4*>(CurrentObject->pEntity);

					if (!pBomb->IsPlanted())
						return;

					const float flCurrentTime = pBomb->GetTimer(flServerTime);
					const float flMaxTime = pBomb->GetTimerLength();

					float flTimeRounded = std::clamp(flCurrentTime, 0.0f, flMaxTime);
					std::string szTime = std::to_string(flTimeRounded).substr(0, 2);
					szTime.append(TSTRING("s"));
					const float flFactor = flCurrentTime / flMaxTime;

					TDirectXRender::Rectangle(Vector2D(0, 0), Vector2D(TGlobalVars::vecScreenSize.x * flFactor, 7), TVariables::ColorToVector(TVariables::TVisuals::TOthers::BarColor.Color.flColors), true);

					std::string szIcon = std::string(reinterpret_cast<const char*>(TBaseEntity::GetWeaponIcon(WEAPON_C4)));
			
					Vector2D vecScreen;

					if (TUtils::WorldToScreen(CurrentObject->pEntity->GetAbsOrigin(), vecScreen))
					{

						TDirectXRender::Circle(Vector2D(vecScreen.x + 10, vecScreen.y - 5), 20, 40, TVariables::ColorToVector(TVariables::TVisuals::TOthers::BombInfo.Color.flColors), false, 2);
						TDirectXRender::Circle(Vector2D(vecScreen.x + 10, vecScreen.y - 5), 20, 40, Vector4D(24, 24, 24, 220), true);
						TDirectXRender::Text(TDirectXRender::TDirectFonts::WeaponIcons, szIcon, Vector2D(vecScreen.x +2, vecScreen.y - 20), Vector4D(200, 200, 200, 200), false, false, Vector4D(24, 24, 24, 255));
						TDirectXRender::Text(TDirectXRender::TDirectFonts::VerdanaSmall, szTime, Vector2D(vecScreen.x, vecScreen.y - 5), Vector4D(200, 200, 200, 200), false, false, Vector4D(24, 24, 24, 255));

					}

				}
			}
			else if (CurrentObject->nClass == (int)EClassIndex::CInferno) {
				if (TVariables::TVisuals::TOthers::NadeInfo.bEnabled) {

					Vector2D vecScreen;
					if (TGlobalVars::pLocalEntity->GetOrigin().DistTo(CurrentObject->pEntity->GetAbsOrigin()) < 2000.f)
					{
						if (TUtils::WorldToScreen(CurrentObject->pEntity->GetAbsOrigin(), vecScreen))
						{
							std::string szIcon = std::string(reinterpret_cast<const char*>(TBaseEntity::GetWeaponIcon(WEAPON_MOLOTOV)));

							CInferno* pInferno = reinterpret_cast<CInferno*>(CurrentObject->pEntity);
							float flTimeRounded = std::clamp(((TICKS_TO_TIME(pInferno->GetEffectTickBegin()) + pInferno->GetMaxTime()) - flServerTime), 0.0f, pInferno->GetMaxTime());

							std::string szTime = std::to_string((int)flTimeRounded).substr(0, 2);
							szTime.append(TSTRING("s"));


							if (TGlobalVars::pLocalEntity->GetOrigin().DistTo(CurrentObject->pEntity->GetAbsOrigin()) < 1500.f)
								TDirectXRender::Circle3DFilled(CurrentObject->pEntity->GetAbsOrigin(), 200, 80, TVariables::ColorToVector(TVariables::TVisuals::TOthers::NadeInfoRadius.Color.flColors));

							TDirectXRender::Circle(Vector2D(vecScreen.x + 10, vecScreen.y - 5), 20, 40, TVariables::ColorToVector(TVariables::TVisuals::TOthers::NadeInfo.Color.flColors), false, 2);
							TDirectXRender::Circle(Vector2D(vecScreen.x + 10, vecScreen.y - 5), 20, 40, Vector4D(24, 24, 24, 220), true);
							TDirectXRender::Text(TDirectXRender::TDirectFonts::WeaponIcons, szIcon, Vector2D(vecScreen.x + 5, vecScreen.y - 20), Vector4D(200, 200, 200, 200), false, false, Vector4D(24, 24, 24, 255));

							TDirectXRender::Text(TDirectXRender::TDirectFonts::VerdanaSmall, szTime, Vector2D(vecScreen.x, vecScreen.y - 5), Vector4D(200, 200, 200, 200), false, false, Vector4D(24, 24, 24, 255));
						}
				    }
				}
			}
			else if (CurrentObject->nClass == (int)EClassIndex::CSmokeGrenadeProjectile)
			{
				if (TVariables::TVisuals::TOthers::NadeInfo.bEnabled) {

					CSmokeGrenade* pSmoke = reinterpret_cast<CSmokeGrenade*>(CurrentObject->pEntity);

					float flTime = ((TICKS_TO_TIME(pSmoke->GetEffectTickBegin()) + pSmoke->GetMaxTime()) - flServerTime);
					float flFactor = flTime / pSmoke->GetMaxTime();

					Vector2D vecScreen;
					if (TGlobalVars::pLocalEntity->GetOrigin().DistTo(CurrentObject->pEntity->GetAbsOrigin()) < 2000.f)
					{
						if (TUtils::WorldToScreen(CurrentObject->pEntity->GetAbsOrigin(), vecScreen) && flFactor > 0.0f)
						{
							std::string szIcon = std::string(reinterpret_cast<const char*>(TBaseEntity::GetWeaponIcon(WEAPON_SMOKEGRENADE)));

							float flTimeRounded = std::clamp(((TICKS_TO_TIME(pSmoke->GetEffectTickBegin()) + pSmoke->GetMaxTime()) - flServerTime), 0.0f, pSmoke->GetMaxTime());

							std::string szTime = std::to_string((int)flTimeRounded).substr(0, 2);
							szTime.append(TSTRING("s"));

							TDirectXRender::Circle(Vector2D(vecScreen.x + 10, vecScreen.y - 5), 20, 40, TVariables::ColorToVector(TVariables::TVisuals::TOthers::NadeInfo.Color.flColors), false, 2);
							TDirectXRender::Circle(Vector2D(vecScreen.x + 10, vecScreen.y - 5), 20, 40, Vector4D(24, 24, 24, 220), true);
							TDirectXRender::Text(TDirectXRender::TDirectFonts::WeaponIcons, szIcon, Vector2D(vecScreen.x + 5, vecScreen.y - 20), Vector4D(200, 200, 200, 200), false, false, Vector4D(24, 24, 24, 255));
							TDirectXRender::Text(TDirectXRender::TDirectFonts::VerdanaSmall, szTime, Vector2D(vecScreen.x, vecScreen.y - 5), Vector4D(200, 200, 200, 200), false, false, Vector4D(24, 24, 24, 255));
						}
					}
				}

			}
			else {
				TVisuals::DroppedWeapons(CurrentObject);

				if (CurrentObject->NadeInfo && TVariables::TVisuals::TLocalPlayer::NadeWarning.bEnabled)
				{
					CBaseCSGrenade* pGrenade = (CBaseCSGrenade*)CurrentObject->pEntity;

					TNadeWarning::TNadeInfo* pNadeInfo = (TNadeWarning::TNadeInfo*)CurrentObject->NadeInfo;

					if (pGrenade->GetEffectTickBegin())
					{

					}
					else
					{
						CBaseEntity* pBaseOwner = (CBaseEntity*)TInterfaces::ClientEntityList->GetClientEntityFromHandle(pGrenade->GetThrower());

						if (!pNadeInfo->bInitialized)
						{
							pNadeInfo->bInitialized = true;
							pNadeInfo->Init(CurrentObject->pEntity,
								pBaseOwner,
								(unsigned long)CurrentObject->pEntity->GetRefEHandle().Get(),
								CurrentObject->nClass == (int)EClassIndex::CMolotovProjectile ? WEAPON_MOLOTOV : WEAPON_HEGRENADE,
								CurrentObject->pEntity->GetOrigin(),
								CurrentObject->pEntity->GetVelocity(),
								0,
								0);
						}

						pNadeInfo->vecCurrentOrigin = CurrentObject->pEntity->GetAbsOrigin();
						pNadeInfo->Draw();
						pNadeInfo->UpdatePath();
						pNadeInfo->vecPrevOrigin = CurrentObject->pEntity->GetAbsOrigin();

					}
				}
			}
		}
	}

}

void TVisuals::DroppedWeapons(TEntityObject* CurrentObject) {

	if (strstr(CurrentObject->pEntity->GetClientClass()->szNetworkName, TSTRING("CWeapon")) != nullptr || CurrentObject->nClass == (int)EClassIndex::CDEagle || CurrentObject->nClass == (int)EClassIndex::CAK47)
	{
		CBaseCombatWeapon* pWeapon = reinterpret_cast<CBaseCombatWeapon*>(CurrentObject->pEntity);

		if (pWeapon == nullptr)
			return;

		const short nDefinitionIndex = pWeapon->GetItemDefinitionIndex();

		const CBaseEntity* pOwner = TInterfaces::ClientEntityList->Get<CBaseEntity>(CurrentObject->pEntity->GetOwnerEntityHandle());

		if (pOwner != nullptr)
			return;

		CCSWeaponData* pWeaponData = TInterfaces::WeaponSystem->GetWeaponData(nDefinitionIndex);

		if (pWeaponData == nullptr || !pWeaponData->IsGun())
			return;

		TBoundingBox Box;

		if (!TUtils::GetBoundingBox(CurrentObject->pEntity, Box))
			return;

		float flBottomOffset = 0.0f;

		if (TVariables::TVisuals::TOthers::DroppedWeaponAmmo.bEnabled)
		{
			float flScale = (float)std::clamp(pWeapon->GetAmmo(), 0, pWeaponData->iMaxClip1) / pWeaponData->iMaxClip1;

			if (pWeaponData->iMaxClip1 != -1)
			{
				flScale = std::clamp(flScale, 0.01f, 100.f);

				int iBarValue = (int)(Box.w - 2) * flScale;

				if (iBarValue < 0)
					iBarValue = 0;

				TDirectXRender::Rectangle(Vector2D(Box.x + 1, Box.y + Box.h + 3), Vector2D(iBarValue + 1, 2), TVariables::ColorToVector(TVariables::TVisuals::TOthers::DroppedWeaponAmmo.Color.flColors), true);
				TDirectXRender::Rectangle(Vector2D(Box.x, Box.y + Box.h + 2), Vector2D(Box.w + 1, 4), Vector4D(0.f, 0.f, 0.f, 255.f ), true);


				flBottomOffset += 7.f;
			}

		}

		if (TVariables::TVisuals::TOthers::DroppedWeaponText.bEnabled)
		{
			std::string szName = TBaseEntity::GetLocalizedName(pWeapon);
			std::transform(szName.begin(), szName.end(), szName.begin(), ::toupper);

			const ImVec2 vecSize = TDirectXRender::TDirectFonts::SmallestPixel->CalcTextSizeA(TDirectXRender::TDirectFonts::SmallestPixel->FontSize, FLT_MAX, 0.f, szName.c_str());
			TDirectXRender::Text(TDirectXRender::TDirectFonts::SmallestPixel, szName, Vector2D((Box.x + Box.w / 2) - (vecSize.x / 2) - 1, Box.y + Box.h + flBottomOffset + 2.f), TVariables::ColorToVector(TVariables::TVisuals::TOthers::DroppedWeaponText.Color.flColors), false, true, Vector4D(24, 24, 24, 225));

			flBottomOffset += 10.f;
		}


		
		if (TVariables::TVisuals::TOthers::DroppedWeaponIcon.bEnabled)
		{
			std::string szIcon = std::string(reinterpret_cast<const char*>(TBaseEntity::GetWeaponIcon(nDefinitionIndex)));
			const ImVec2 vecIconSize = TDirectXRender::TDirectFonts::WeaponIconsMedium->CalcTextSizeA(TDirectXRender::TDirectFonts::WeaponIconsMedium->FontSize, FLT_MAX, 0.f, szIcon.c_str());

			TDirectXRender::Text(TDirectXRender::TDirectFonts::WeaponIconsMedium, szIcon, Vector2D((Box.x + Box.w / 2) - vecIconSize.x / 2, Box.y + Box.h + 2.f + flBottomOffset), TVariables::ColorToVector(TVariables::TVisuals::TOthers::DroppedWeaponIcon.Color.flColors),false, false, Vector4D(24, 24, 24, 255));
		}
		

	}
}



void TVisuals::Player(TEntityObject* EntityObject) {

	if (!EntityObject->pEntity->IsAlive())
		return;

	if (!TGlobalVars::pLocalEntity->IsEnemy(EntityObject->pEntity))
		return;

	PlayerInfo_t pPlayerInfo;

	if (!TInterfaces::EngineClient->GetPlayerInfo(EntityObject->nIndex, &pPlayerInfo))
		return;

	TBoundingBox Box;
	if (!TUtils::GetBoundingBox(EntityObject->pEntity, Box))
		return;

	bool bDormant = EntityObject->pEntity->IsDormant();

	if (bDormant && !TVariables::TVisuals::TPlayer::Dormant.bEnabled)
		return;

	if (TVariables::TVisuals::TPlayer::Name.bEnabled) {
		std::string szName{ std::string(pPlayerInfo.szName).substr(0, 24) };

		const ImVec2 vecSize = TDirectXRender::TDirectFonts::Tahoma->CalcTextSizeA(TDirectXRender::TDirectFonts::Tahoma->FontSize, FLT_MAX, 0.f, szName.c_str());
		TDirectXRender::Text(TDirectXRender::TDirectFonts::Tahoma, szName, Vector2D((Box.x + Box.w / 2 - 1) - (vecSize.x / 2) + 2, Box.y - 16), TVariables::ColorToVector(TVariables::TVisuals::TPlayer::Name.Color.flColors, bDormant), false, true, Vector4D(24, 24, 24, 225 / (bDormant ? 2.f : 1.f)));
	}

	if (TVariables::TVisuals::TPlayer::BoundingBox.bEnabled) {
		TDirectXRender::Rectangle(Vector2D(Box.x, Box.y), Vector2D(Box.w, Box.h), TVariables::ColorToVector(TVariables::TVisuals::TPlayer::BoundingBox.Color.flColors, bDormant), false);
		TDirectXRender::Rectangle(Vector2D(Box.x - 1, Box.y - 1), Vector2D(Box.w + 2, Box.h + 2), Vector4D(0.f, 0.f, 0.f, 255.f / (bDormant ? 2.f : 1.f)), false);
		TDirectXRender::Rectangle(Vector2D(Box.x + 1, Box.y + 1), Vector2D(Box.w - 2, Box.h - 2), Vector4D(0.f, 0.f, 0.f, 255.f / (bDormant ? 2.f : 1.f)), false);
	}

	int iHealth = min(100, EntityObject->pEntity->GetHealth());
	float flHealthFraction = 1.f - float(iHealth) / 100.f;
	int iAmountToDeduct = (int)floor((Box.h * flHealthFraction));

	if (TVariables::TVisuals::TPlayer::HealthBar.bEnabled) {

		TDirectXRender::Rectangle(Vector2D(Box.x - 6-1, Box.y + iAmountToDeduct - 1), Vector2D(3, Box.h - iAmountToDeduct+ 1), TVariables::ColorToVector(TVariables::TVisuals::TPlayer::HealthBar.Color.flColors, bDormant), true);
		
		if (!bDormant)
			TDirectXRender::Rectangle(Vector2D(Box.x - 7, Box.y - 1), Vector2D(3, Box.h + 1), Vector4D(0, 0, 0, 255 / (bDormant ? 2.f : 1.f)), true);
	}

	if (bDormant)
		return;

	if (TVariables::TVisuals::TPlayer::HealthText.bEnabled) {
		float flModifier = 22.f;

		if (iHealth >= 100)
			flModifier = 32.f;
		else if (iHealth >= 10)
			flModifier = 26.f;


		TDirectXRender::Text(TDirectXRender::TDirectFonts::SmallestPixel, std::to_string(iHealth), Vector2D(6 + Box.x - flModifier, Box.y + iAmountToDeduct - 7.f), TVariables::ColorToVector(TVariables::TVisuals::TPlayer::HealthText.Color.flColors), false, true, Vector4D(24, 24, 24, 225));
	}

	float flBottomOffset = 0.0f;

	CBaseCombatWeapon* pBaseWeapon = TBaseEntity::GetWeapon(EntityObject->pEntity);

	if (pBaseWeapon) {

		CCSWeaponData* pWeaponData = TBaseEntity::GetWeaponData(pBaseWeapon);

		if (pWeaponData) {

			float flScale = (float)std::clamp(pBaseWeapon->GetAmmo(), 0, pWeaponData->iMaxClip1) / pWeaponData->iMaxClip1;

			CAnimationLayer* pAnimOverlay = EntityObject->pEntity->GetAnimationLayer(1);

			if (TVariables::TVisuals::TPlayer::AmmoBar.bEnabled) {
				if (pWeaponData->iMaxClip1 != -1)
				{
					int iBarValue;

					flScale = std::clamp(flScale, 0.01f, 100.f);

					if (pBaseWeapon->IsReloading())
						iBarValue = (std::clamp(pAnimOverlay->flCycle, 0.0f, 1.f) * Box.w) / 1.f;
					else
						iBarValue = (int)(Box.w - 2) * flScale;

					if (iBarValue < 0)
						iBarValue = 0;

					TDirectXRender::Rectangle(Vector2D(Box.x + 1, Box.y + Box.h + 3), Vector2D(iBarValue + 1, 2), TVariables::ColorToVector(TVariables::TVisuals::TPlayer::AmmoBar.Color.flColors), true);
					TDirectXRender::Rectangle(Vector2D(Box.x, Box.y + Box.h + 2), Vector2D(Box.w + 1, 4), Vector4D(0.f, 0.f, 0.f, 255.f), true);

					flBottomOffset += 6.f;
				}
			}

			if (TVariables::TVisuals::TPlayer::WeaponText.bEnabled) {
				std::string szName = TBaseEntity::GetLocalizedName(pBaseWeapon);
				std::transform(szName.begin(), szName.end(), szName.begin(), ::toupper);

				const ImVec2 vecSize = TDirectXRender::TDirectFonts::SmallestPixel->CalcTextSizeA(TDirectXRender::TDirectFonts::SmallestPixel->FontSize, FLT_MAX, 0.f, szName.c_str());
				TDirectXRender::Text(TDirectXRender::TDirectFonts::SmallestPixel, szName, Vector2D((Box.x + Box.w / 2) - (vecSize.x / 2) + 1, Box.y + Box.h + flBottomOffset + 2.f), TVariables::ColorToVector(TVariables::TVisuals::TPlayer::WeaponText.Color.flColors), false, true, Vector4D(24, 24, 24, 225));

				flBottomOffset += 10.f;
			}

			if (TVariables::TVisuals::TPlayer::WeaponIcon.bEnabled)
			{
				std::string szIcon = std::string(reinterpret_cast<const char*>(TBaseEntity::GetWeaponIcon(pBaseWeapon->GetItemDefinitionIndex())));
				const ImVec2 vecIconSize = TDirectXRender::TDirectFonts::WeaponIconsMedium->CalcTextSizeA(TDirectXRender::TDirectFonts::WeaponIconsMedium->FontSize, FLT_MAX, 0.f, szIcon.c_str());
				TDirectXRender::Text(TDirectXRender::TDirectFonts::WeaponIconsMedium, szIcon, Vector2D((Box.x + Box.w / 2) - vecIconSize.x / 2, Box.y + Box.h + flBottomOffset + 2.f), TVariables::ColorToVector(TVariables::TVisuals::TPlayer::WeaponIcon.Color.flColors), false, false, Vector4D(24, 24, 24, 255));
			}



		}

	}

	if (TVariables::TVisuals::TPlayer::Skeleton.bEnabled) {


		studiohdr_t* pStudioModel =TInterfaces::ModelInfo->GetStudioModel(EntityObject->pEntity->GetModel());

		if (pStudioModel) {

			Vector3D vecBreastBone = TBaseEntity::GetBonePosition(EntityObject, 6) + (TBaseEntity::GetBonePosition(EntityObject, 7) - TBaseEntity::GetBonePosition(EntityObject, 6)) * 0.5f;

			for (auto i = 0; i < pStudioModel->nBones; i++)
			{
				auto bone = pStudioModel->GetBone(i);

				if (!bone)
					continue;

				if (bone->iParent == -1)
					continue;

				if (!(bone->iFlags & BONE_USED_BY_HITBOX))
					continue;

				auto vecChild = TBaseEntity::GetBonePosition(EntityObject, i);
				auto vecParent = TBaseEntity::GetBonePosition(EntityObject, bone->iParent);

				Vector3D vecChildDelta = vecChild - vecBreastBone;
				Vector3D vecDeltaParent = vecParent - vecBreastBone;

				if (vecDeltaParent.Length() < 9.0f && vecChildDelta.Length() < 9.0f)
					vecParent = vecBreastBone;

				if (i == 5)
					vecChild = vecBreastBone;

				if (fabs(vecChildDelta.z) < 5.0f && vecDeltaParent.Length() < 5.0f && vecChildDelta.Length() < 5.0f || i == 6)
					continue;

				Vector2D vecScreenChild;
				Vector2D vecScreenParent;

				if (TUtils::WorldToScreen(vecChild, vecScreenChild) && TUtils::WorldToScreen(vecParent, vecScreenParent))
					TDirectXRender::Line(Vector2D(vecScreenChild.x, vecScreenChild.y), Vector2D(vecScreenParent.x, vecScreenParent.y), TVariables::ColorToVector(TVariables::TVisuals::TPlayer::Skeleton.Color.flColors), 1);

			}
		}

	}


	if (TVariables::TVisuals::TPlayer::Flags.bEnabled)
	{
		float flFlagOffset = 0.0f;

		if (EntityObject->pEntity->HasC4())
		{
			TDirectXRender::Text(TDirectXRender::TDirectFonts::SmallestPixel, TSTRING("C4"), Vector2D(Box.x + Box.w + 4, Box.y + flFlagOffset), Vector4D(60, 180, 225, 225.f * TVariables::TVisuals::TPlayer::Flags.Color.flColors[3]), false, true, Vector4D(24, 24, 24, 225));
			flFlagOffset += 8.f;
		}

		if (EntityObject->pEntity->GetArmor() > 0)
		{
			TDirectXRender::Text(TDirectXRender::TDirectFonts::SmallestPixel, EntityObject->pEntity->HasHelmet() ? TSTRING("HK") : TSTRING("K"), Vector2D(Box.x + Box.w + 4, Box.y + flFlagOffset), Vector4D(255, 255, 255, 225.f * TVariables::TVisuals::TPlayer::Flags.Color.flColors[3]), false, true, Vector4D(24, 24, 24, 225));
			flFlagOffset += 8.f;
		}

		if (pBaseWeapon) {

			if (pBaseWeapon->IsReloading())
			{
				TDirectXRender::Text(TDirectXRender::TDirectFonts::SmallestPixel, TSTRING("RELOAD"), Vector2D(Box.x + Box.w + 4, Box.y + flFlagOffset), Vector4D(60, 180, 225, 225.f * TVariables::TVisuals::TPlayer::Flags.Color.flColors[3]), false, true, Vector4D(24, 24, 24, 225));
				flFlagOffset += 8.f;
			}
		}

		if (EntityObject->pEntity->IsScoped()) {
			TDirectXRender::Text(TDirectXRender::TDirectFonts::SmallestPixel, TSTRING("ZOOM"), Vector2D(Box.x + Box.w + 4, Box.y + flFlagOffset), Vector4D(60, 180, 225, 225.f * TVariables::TVisuals::TPlayer::Flags.Color.flColors[3]), false, true, Vector4D(24, 24, 24, 225));
			flFlagOffset += 8.f;
		}


		if (!EntityObject->AnimationInstance->bCanLagCompensate)
		{
			TDirectXRender::Text(TDirectXRender::TDirectFonts::SmallestPixel, TSTRING("LC"), Vector2D(Box.x + Box.w + 4, Box.y + flFlagOffset), Vector4D(168, 2, 2, 225.f * TVariables::TVisuals::TPlayer::Flags.Color.flColors[3]), false, true, Vector4D(24, 24, 24, 225));
			flFlagOffset += 8.f;
		}

		if (!EntityObject->AnimationInstance->bActiveExploits)
		{
			TDirectXRender::Text(TDirectXRender::TDirectFonts::SmallestPixel, TSTRING("X"), Vector2D(Box.x + Box.w + 4, Box.y + flFlagOffset), Vector4D(255, 255, 255, 225.f * TVariables::TVisuals::TPlayer::Flags.Color.flColors[3]), false, true, Vector4D(24, 24, 24, 225));
			flFlagOffset += 8.f;
		}
	}
	


}

void TVisuals::TLocal::RemoveSmoke() {

		static IMaterial* pMaterialFire;
		static IMaterial* pMaterialSmokeGrenade;
		static IMaterial* pMaterialemods;
		static IMaterial* pMaterialImpactDust;

		if (!pMaterialFire)
			pMaterialFire = TInterfaces::MaterialSystem->FindMaterial(TSTRING("particle/vistasmokev1/vistasmokev1_fire"), TSTRING("Other textures"));

		if (!pMaterialSmokeGrenade)
			pMaterialSmokeGrenade = TInterfaces::MaterialSystem->FindMaterial(TSTRING("particle/vistasmokev1/vistasmokev1_smokegrenade"), TSTRING("Other textures"));

		if (!pMaterialemods)
			pMaterialemods = TInterfaces::MaterialSystem->FindMaterial(TSTRING("particle/vistasmokev1/vistasmokev1_emods"), TSTRING("Other textures"));

		if (!pMaterialImpactDust)
			pMaterialImpactDust = TInterfaces::MaterialSystem->FindMaterial(TSTRING("particle/vistasmokev1/vistasmokev1_emods_impactdust"), TSTRING("Other textures"));

		static auto SmokeCount = TUtil::FindAddress(CLIENT_STR, TSTRING("A3 ? ? ? ? 57 8B CB")).add(0x1).to();

		if (TVariables::TVisuals::TOthers::Removals.mapValues[REMOVAL_NO_SMOKE]) {

			if (!pMaterialFire->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
				pMaterialFire->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

			if (!pMaterialSmokeGrenade->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
				pMaterialSmokeGrenade->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

			if (!pMaterialemods->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
				pMaterialemods->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

			if (!pMaterialImpactDust->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
				pMaterialImpactDust->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, true);

			if (SmokeCount.get() != 0)
				SmokeCount.set(0);
		}
		else {
			if (pMaterialFire->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
				pMaterialFire->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);

			if (pMaterialSmokeGrenade->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
				pMaterialSmokeGrenade->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);

			if (pMaterialemods->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
				pMaterialemods->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);

			if (pMaterialImpactDust->GetMaterialVarFlag(MATERIAL_VAR_NO_DRAW))
				pMaterialImpactDust->SetMaterialVarFlag(MATERIAL_VAR_NO_DRAW, false);
		}
	
}

void TVisuals::TLocal::Removals() {

	static bool bPrevShadowRemoval = false;

	if (bPrevShadowRemoval != TVariables::TVisuals::TOthers::Removals.mapValues[REMOVAL_SHADOW]) {
		
		if (TVariables::TVisuals::TOthers::Removals.mapValues[REMOVAL_SHADOW]) {
			TConvarManager::varCsmShadows.pBase->SetValue(0);
			TConvarManager::varFootContactShadows.pBase->SetValue(0);
			TConvarManager::varCsmStaticPropShadows.pBase->SetValue(0);
			TConvarManager::varCsmWorldShadows.pBase->SetValue(0);
			TConvarManager::varCsmViewmodelShadows.pBase->SetValue(0);
			TConvarManager::varCsmSpriteShadows.pBase->SetValue(0);
			TConvarManager::varCsmRopeShadows.pBase->SetValue(0);
			TConvarManager::varShadows.pBase->SetValue(0);
		}
		else {
			TConvarManager::varCsmShadows.Restore();
			TConvarManager::varFootContactShadows.Restore();
			TConvarManager::varCsmStaticPropShadows.Restore();
			TConvarManager::varCsmWorldShadows.Restore();
			TConvarManager::varCsmViewmodelShadows.Restore();
			TConvarManager::varCsmSpriteShadows.Restore();
			TConvarManager::varCsmRopeShadows.Restore();
			TConvarManager::varShadows.Restore();
		}

		bPrevShadowRemoval = TVariables::TVisuals::TOthers::Removals.mapValues[REMOVAL_SHADOW];
	}

}

void TVisuals::TLocal::ScopeLines() {

	static bool bRequireReset = false;

	if (TGlobalVars::pLocalEntity->IsAlive())
	{

		CBaseCombatWeapon* pBaseWeapon = TBaseEntity::GetWeapon(TGlobalVars::pLocalEntity);

		if (pBaseWeapon && pBaseWeapon->IsZoomable(false)
			&& TGlobalVars::pLocalEntity->IsScoped())
		{
			if (TVariables::TVisuals::TOthers::Removals.mapValues[REMOVAL_SCOPE_SENSITIVITY_MODIFIER])
			{
				TConvarManager::varZoomSensitivityRatioMouse.pBase->SetValue(0.0f);
				bRequireReset = true;
			}

			if (TVariables::TVisuals::TOthers::Removals.mapValues[REMOVAL_NOSCOPE])
			{
				int w = TGlobalVars::vecScreenSize.x;
				int h = TGlobalVars::vecScreenSize.y;

				TDirectXRender::Rectangle(Vector2D(0, TGlobalVars::vecScreenSize.y / 2.f),
					Vector2D(TGlobalVars::vecScreenSize.x, TGlobalVars::vecScreenSize.y / 2.f), Vector4D(0, 0, 0, 255), false);

				TDirectXRender::Line(Vector2D(TGlobalVars::vecScreenSize.x / 2.f, 0.f),
					Vector2D(TGlobalVars::vecScreenSize.x / 2.f, TGlobalVars::vecScreenSize.y), Vector4D(0, 0, 0, 255),1);

			}

		}
		else
		{
			if (bRequireReset)
			{
				TConvarManager::varZoomSensitivityRatioMouse.Restore();
				bRequireReset = false;
			}

			TConvarManager::varZoomSensitivityRatioMouse.flBackup = TConvarManager::varZoomSensitivityRatioMouse.pBase->GetFloat();
		}
	}
}

void TVisuals::PerTick() {

	static int Tick = 0;

	if (Tick + 14 <= GetTickCount64())
	{
		TLocal::View();
		TLocal::Removals();
		Tick = GetTickCount64();
	}
}

void  TVisuals::TLocal::FireBulletRoutine() {

	if (TFireBullet::stkPlayerHurt.size())
	{
		while (TFireBullet::stkPlayerHurt.first() && TFireBullet::stkPlayerHurt.first()->bUsed)
		{
			TFireBullet::stkPlayerHurt.pop_front();
		}
	}

	if (!TVariables::TVisuals::bEnabled)
		return;

	if (!TFireBullet::stkPlayerHurt.size())
		return;

	for (int iCurrent = TFireBullet::stkPlayerHurt.size() - 1; iCurrent >= 0; iCurrent--)
	{
		TPlayerHurtInfo* Object = TFireBullet::stkPlayerHurt[iCurrent];

			if (!Object->bUsed)
			{
				TVisuals::TLocal::Hitmarker(Object);
					
			}
		
	}
}

void TVisuals::TLocal::Hitmarker(TPlayerHurtInfo* Object) {

	if (Object->iSystemTickCount != GetTickCount64()) {
		Object->iTickCount++;
		Object->iSystemTickCount = GetTickCount64();
	}

	float flAlphaMultipler = 1.f;
	if (Object->iTickCount >= 100)
	{
		flAlphaMultipler = 1.f - std::clamp(((float)Object->iTickCount - 100) * 2.f, 0.f,255.f) / 255.f;
	}

	if (Object->iTickCount >= 100 + (int(255.f / 2.f)))
	{
		Object->bUsed = true;
		return;
	}

	static constexpr int iHitMarkerGap = 2;
	static constexpr int iHitMarkerLength = 4;

	Vector2D vecScreen = { };
	static constexpr std::array<std::array<float, 2U>, 4U> arrSides = { { { -1.0f, -1.0f }, { 1.0f, 1.0f }, { -1.0f, 1.0f }, { 1.0f, -1.0f } } };


	if (TUtils::WorldToScreen(Object->vecEndHurtPos, vecScreen)) {

		constexpr float flDistance = 40.f;

		if (TVariables::TVisuals::TOthers::HitMarker.bEnabled)
			for (const auto& arrSide : arrSides)
				TDirectXRender::Line(Vector2D(floor(vecScreen.x + iHitMarkerGap * arrSide[0]), floor(vecScreen.y + iHitMarkerGap * arrSide[1])),
					Vector2D(floor(vecScreen.x + iHitMarkerLength * arrSide[0]), floor(vecScreen.y + iHitMarkerLength * arrSide[1])),
					TVariables::ColorToVector(TVariables::TVisuals::TOthers::HitMarker.Color.flColors,false, flAlphaMultipler),1.f);

		if (TVariables::TVisuals::TOthers::DamageMarker.bEnabled)
			TDirectXRender::Text(TDirectXRender::TDirectFonts::SmallestPixelMedium, std::to_string((int)Object->iDamageHurt), Vector2D(vecScreen.x - 20, vecScreen.y- 10 - Object->iTickCount / 2), TVariables::ColorToVector(TVariables::TVisuals::TOthers::DamageMarker.Color.flColors,false, flAlphaMultipler), false, true, Vector4D(24, 24, 24, 225));

	}
	
}

void TVisuals::TLocal::View() {

	static bool ResetRequired[4];


}

void TVisuals::DoPostScreenEffects(IClientModeShared* ClientModeShared, CViewSetup* pSetup) {

	if (TInterfaces::GlowObjectManager->vecGlowObjectDefinitions.Size() <= NULL)
		return;

	for (int i = 0; i < TInterfaces::GlowObjectManager->vecGlowObjectDefinitions.Count(); i++)
	{
		IGlowObjectManager::GlowObject_t& hGlowObject = TInterfaces::GlowObjectManager->vecGlowObjectDefinitions[i];

		if (hGlowObject.IsEmpty())
			continue;

		CBaseEntity* pEntity = hGlowObject.pEntity;

		if (pEntity == nullptr)
			continue;

		CBaseClient* ClientClass = pEntity->GetClientClass();

		if (ClientClass->nClassID == EClassIndex::CCSPlayer)
		{
			if (!pEntity->IsPlayer())
				continue;

			if (!pEntity->IsAlive())
				continue;

			if (pEntity == TGlobalVars::pLocalEntity)
			{
				if (TVariables::TVisuals::TLocalPlayer::Glow.bEnabled) {
					hGlowObject.bRenderWhenOccluded = true;
					hGlowObject.bRenderWhenUnoccluded = false;
					hGlowObject.bFullBloomRender = false;
					hGlowObject.arrColor = { TVariables::TVisuals::TLocalPlayer::Glow.Color.flColors[0],
						TVariables::TVisuals::TLocalPlayer::Glow.Color.flColors[1],
					TVariables::TVisuals::TLocalPlayer::Glow.Color.flColors[2],
					TVariables::TVisuals::TLocalPlayer::Glow.Color.flColors[3] };
					hGlowObject.nRenderStyle = TVariables::TVisuals::TLocalPlayer::Glow.iSelectable;
				}
			}
			else 
			{
				if (TVariables::TVisuals::TPlayer::Glow.bEnabled) {
					hGlowObject.bRenderWhenOccluded = true;
					hGlowObject.bRenderWhenUnoccluded = false;
					hGlowObject.bFullBloomRender = false;
					hGlowObject.arrColor = { TVariables::TVisuals::TPlayer::Glow.Color.flColors[0],
						TVariables::TVisuals::TPlayer::Glow.Color.flColors[1],
					TVariables::TVisuals::TPlayer::Glow.Color.flColors[2],
					TVariables::TVisuals::TPlayer::Glow.Color.flColors[3] };
					hGlowObject.nRenderStyle = TVariables::TVisuals::TPlayer::Glow.iSelectable;
				}
			}
		}
		else if (strstr(ClientClass->szNetworkName, TSTRING("CWeapon")) != nullptr || ClientClass->nClassID == EClassIndex::CDEagle || ClientClass->nClassID == EClassIndex::CAK47)
		{
				CBaseCombatWeapon* pWeapon = reinterpret_cast<CBaseCombatWeapon*>(pEntity);

				if (pWeapon == nullptr)
					continue;

				const short nDefinitionIndex = pWeapon->GetItemDefinitionIndex();

				const CBaseEntity* pOwner = TInterfaces::ClientEntityList->Get<CBaseEntity>(pEntity->GetOwnerEntityHandle());

				if (pOwner != nullptr)
					continue;

				CCSWeaponData* pWeaponData = TInterfaces::WeaponSystem->GetWeaponData(nDefinitionIndex);

				if (pWeaponData == nullptr || !pWeaponData->IsGun())
					continue;


		}

	}
	
}


bool TVisuals::TUtils::WorldToScreen(const Vector3D& vecOrigin, Vector2D& vecScreen)
{
	const float flWidth = matCurrentWorldMatrix[3][0] * vecOrigin.x + matCurrentWorldMatrix[3][1] * vecOrigin.y + matCurrentWorldMatrix[3][2] * vecOrigin.z + matCurrentWorldMatrix[3][3];

	if (flWidth < 0.001f)
		return false;

	const float flInverse = 1.0f / flWidth;
	vecScreen.x = (matCurrentWorldMatrix[0][0] * vecOrigin.x + matCurrentWorldMatrix[0][1] * vecOrigin.y + matCurrentWorldMatrix[0][2] * vecOrigin.z + matCurrentWorldMatrix[0][3]) * flInverse;
	vecScreen.y = (matCurrentWorldMatrix[1][0] * vecOrigin.x + matCurrentWorldMatrix[1][1] * vecOrigin.y + matCurrentWorldMatrix[1][2] * vecOrigin.z + matCurrentWorldMatrix[1][3]) * flInverse;

	const Vector2D vecDisplaySize = TGlobalVars::vecScreenSize;
	vecScreen.x = (vecDisplaySize.x * 0.5f) + (vecScreen.x * vecDisplaySize.x) * 0.5f;
	vecScreen.y = (vecDisplaySize.y * 0.5f) - (vecScreen.y * vecDisplaySize.y) * 0.5f;

	vecScreen.x = floor(vecScreen.x);
	vecScreen.y = floor(vecScreen.y);

	return true;
}

bool TVisuals::TUtils::GetBoundingBox(CBaseEntity* pPlayer, TBoundingBox& BBox)
{
	Vector3D vecTop, vecDown;
	Vector2D vecScreen[2];

	vecDown = pPlayer->GetAbsOrigin();
	vecTop = vecDown + Vector3D(0, 0, 72);

	vecDown  += Vector3D(0, 0, -8);

	if (WorldToScreen(vecTop, vecScreen[1]) && WorldToScreen(vecDown, vecScreen[0]))
	{
		Vector2D delta = vecScreen[1] - vecScreen[0];

		BBox.h = fabsf(delta.y) ;
		BBox.w = BBox.h / 2 ;

		BBox.x = vecScreen[1].x - (BBox.w / 2) + 2;
		BBox.y = vecScreen[1].y - 1;

		return true;
	}


	return false;
}