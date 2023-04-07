#include "../../includes.h"
#include "../handlers/localengine.h"
#include "../handlers/entityhandler.h"
#include "../gvars.h"
#include "../sdk.h"
#include "../math.h"
#include "../handlers/convarmanager.h"
#include "../handlers/animationsmanager.h"
#include "../ui/vars.h"
#include "../features/visuals.h"
#include "../renderer.h"
#include "nadepred.h"
#include "chams.h"
#include "../hooks.h"
#include "../handlers/eventlogger.h"
#include "../handlers/localanimmanager.h"

IMaterial* TModelChams::CreateMaterial(std::string szName, std::string szShader, std::string szBaseTexture, std::string szEnvMap, bool bIgnorez, bool bWireframe, std::string_view szProxies)
{
	const std::string szMaterial = fmt::vformat((R"#("{0}"
	{{
	"$basetexture"		"{1}"
		"$envmap"			"{2}"
		"$envmapfresnel"	"0"
		"$model"			"1"
		"$translucent"		"0"
		"$ignorez"			"{3}"
		"$selfillum"		"1"
		"$halflambert"		"1"
		"$wireframe"		"{4}"
		"$nofog"			"1"
		"proxies"
		{{
			{5}
		}}
	}})#"), fmt::make_format_args(szShader, szBaseTexture, szEnvMap, bIgnorez ? 1 : 0, bWireframe ? 1 : 0, szProxies));

	CKeyValues* pKeyValues = new CKeyValues(szShader.data());
	pKeyValues->LoadFromBuffer(szName.data(), szMaterial.c_str());

	return TInterfaces::MaterialSystem->CreateMaterial(szName.data(), pKeyValues);
}

IMaterial* TModelChams::CreateOverlay(int num)
{
	if (num == 1)
	{
		static const std::string material_name = "game_material.vmt";
		std::string shade_type;
		shade_type = "VertexLitGeneric";
		std::string material_data;
		material_data += "\"" + shade_type + "\"\n{\n";
		material_data += "\t\"$additive\" \"1\"\n";
		material_data += "\t\"$envmap\" \"models/effects/cube_white\"\n";
		material_data += "\t\"$envmaptint\" \"[1 1 1]\"\n";
		material_data += "\t\"$envmapfresnel\" \"1\"\n";
		material_data += "\t\"$envmapfresnelminmaxexp\" \"[0 1 2]\"\n";
		material_data += "\t\"$alpha\" \"0.8\"\n";
		material_data += "}\n";

		CKeyValues* pKeyValues = new CKeyValues(shade_type.data());
		pKeyValues->LoadFromBuffer(material_name.c_str(), material_data.c_str());

		return  TInterfaces::MaterialSystem->CreateMaterial(material_name.c_str(), pKeyValues);

	}
	else if (num == 2)
	{
		return  TInterfaces::MaterialSystem->FindMaterial("dev/glow_armsrace", nullptr, true);
	}
}
void TModelChams::Initialize() {

	if (bMaterialInitialized)
		return;

	constexpr std::string_view szScrollProxies = R"#("texturescroll"
		{
			"texturescrollvar"		"$basetexturetransform"
			"texturescrollrate"		"0.2"
			"texturescrollangle"	"90"
		})#";
	
	if (!matMaterials[MAT_DEFAULT])
	   matMaterials[MAT_DEFAULT] = CreateMaterial((TSTRING("custom")), (TSTRING("VertexLitGeneric")));
	
	if (!matMaterials[MAT_FLAT])
	   matMaterials[MAT_FLAT] = TInterfaces::MaterialSystem->FindMaterial("debug/debugdrawflat", "Model textures", true);
	
	if (!matMaterials[MAT_PULSE])
	   matMaterials[MAT_PULSE] = TInterfaces::MaterialSystem->FindMaterial("models/inventory_items/dogtags/dogtags_outline", "Model textures", true);
	
	if (!matMaterials[MAT_METALLIC])
	   matMaterials[MAT_METALLIC] = CreateMaterial(TSTRING("custom1"), TSTRING("VertexLitGeneric"), TSTRING("vgui/white"), TSTRING("env_cubemap"));
	
	if (!matMaterials[MAT_GLOW])
	   matMaterials[MAT_GLOW] = CreateOverlay(1);

	if (!matMaterialsDME[MAT_DEFAULT])
		matMaterialsDME[MAT_DEFAULT] = CreateMaterial((TSTRING("custom")), (TSTRING("VertexLitGeneric")));

	if (!matMaterialsDME[MAT_FLAT])
		matMaterialsDME[MAT_FLAT] = TInterfaces::MaterialSystem->FindMaterial("debug/debugdrawflat", "Model textures", true);

	if (!matMaterialsDME[MAT_PULSE])
		matMaterialsDME[MAT_PULSE] = TInterfaces::MaterialSystem->FindMaterial("models/inventory_items/dogtags/dogtags_outline", "Model textures", true);

	if (!matMaterialsDME[MAT_METALLIC])
		matMaterialsDME[MAT_METALLIC] = CreateMaterial(TSTRING("custom1"), TSTRING("VertexLitGeneric"), TSTRING("vgui/white"), TSTRING("env_cubemap"));

	if (!matMaterialsDME[MAT_GLOW])
		matMaterialsDME[MAT_GLOW] = CreateOverlay(1);


	bMaterialInitialized = matMaterials[MAT_DEFAULT] && matMaterials[MAT_FLAT] && matMaterials[MAT_PULSE] && matMaterials[MAT_METALLIC] && matMaterials[MAT_GLOW] &&
		matMaterialsDME[MAT_DEFAULT] && matMaterialsDME[MAT_FLAT] && matMaterialsDME[MAT_PULSE] && matMaterialsDME[MAT_METALLIC] && matMaterialsDME[MAT_GLOW];
	
	/*
	if (matMaterials[MAT_DEFAULT] != nullptr && !matMaterials[MAT_DEFAULT]->IsErrorMaterial())
		matMaterials[MAT_DEFAULT]->IncrementReferenceCount();

	if (matMaterials[MAT_FLAT] != nullptr && !matMaterials[MAT_FLAT]->IsErrorMaterial())
		matMaterials[MAT_FLAT]->IncrementReferenceCount();

	if (matMaterials[MAT_PULSE] != nullptr && !matMaterials[MAT_PULSE]->IsErrorMaterial())
		matMaterials[MAT_PULSE]->IncrementReferenceCount();

	if (matMaterials[MAT_GLOW] != nullptr && !matMaterials[MAT_GLOW]->IsErrorMaterial())
		matMaterials[MAT_GLOW]->IncrementReferenceCount();
	*/

}


bool TModelChams::OnRenderModel(DrawModelResults_t* pResults, const DrawModelInfo_t& info, matrix3x4_t* pBoneToWorld, float* flFlexWeights, float* flFlexDelayedWeights, const Vector3D& vecModelOrigin, int nFlags)
{
	static auto oDrawModel = THooks::dtrDrawModel.GetOriginal<decltype(&THooks::hkDrawModel)>();

	float flChamAlpha = 1.f;

	if (!bMaterialInitialized)
		return false;

	if (!TVariables::TModels::TGeneral::bEnabled)
		return false;

	if (!TGlobalVars::pLocalEntity)
		return false;

	if (!TInterfaces::EngineClient->IsInGame())
		return false;

	auto* pRenderable = (IClientRenderable*)info.pClientEntity;

	if (pRenderable == nullptr)
		return false;

	CBaseEntity* pEntity = (CBaseEntity*)pRenderable->GetIClientUnknown()->GetBaseEntity();

	if (pEntity == nullptr)
		return false;

	std::string_view szModelName = info.pStudioHdr->szName;

	if (nFlags & (STUDIO_RENDER | STUDIO_SKIP_FLEXES | STUDIO_DONOTMODIFYSTENCILSTATE | STUDIO_NOLIGHTING_OR_CUBEMAP | STUDIO_SKIP_DECALS))
		return false;

	IMaterial* pMaterialMain = nullptr;
	IMaterial* pMaterialExtra = matMaterials[MAT_GLOW];

	if (szModelName.find((TSTRING("weapons\\w_"))) != std::string_view::npos)
	{
		auto pOwnerEntity = TInterfaces::ClientEntityList->Get<CBaseEntity>(pEntity->GetMoveParent());

		if (pOwnerEntity && pOwnerEntity->GetIndex() == TGlobalVars::LocalIndex) {

			if (TVariables::TModels::TLocal::Attachment.Value != 0) {
				
				pMaterialMain = matMaterials[TVariables::TModels::TLocal::Attachment.Value - 1];

				pMaterialMain->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				TInterfaces::StudioRender->SetColorModulation(TVariables::TModels::TLocal::Attachment.Color.flColors);
				TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialMain);

				if (TVariables::TModels::TLocal::AttachmentOverlay.bEnabled)
				{
					oDrawModel(TInterfaces::StudioRender, 0, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

					pMaterialExtra->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
					TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialExtra);

					IMaterialVar* Overlay = pMaterialExtra->FindVar(("$envmaptint"), nullptr);
					Overlay->SetVector(TVariables::TModels::TLocal::AttachmentOverlay.Color.flColors);

				}

				return true;
			}
		}
		return false;
	}

	static float flDefaultCol[] = {1.f,1.f,1.f,1.f};

	if (szModelName.find((TSTRING("arms"))) != std::string_view::npos)
	{
		auto pOwnerEntity = TInterfaces::ClientEntityList->Get<CBaseEntity>(pEntity->GetMoveParent());

		if (TVariables::TModels::TLocal::Arms.Value != 0) {

			pMaterialMain = matMaterials[TVariables::TModels::TLocal::Arms.Value - 1];

			pMaterialMain->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
			TInterfaces::StudioRender->SetColorModulation(TVariables::TModels::TLocal::Arms.Color.flColors);
			TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialMain);

			if (TVariables::TModels::TLocal::ArmsOverlay.bEnabled)
			{
				oDrawModel(TInterfaces::StudioRender, 0, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

				pMaterialExtra->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialExtra);

				IMaterialVar* Overlay = pMaterialExtra->FindVar(("$envmaptint"), nullptr);
				Overlay->SetVector(TVariables::TModels::TLocal::ArmsOverlay.Color.flColors);

			}

			return true;
		}
	}
	else if (szModelName.find((TSTRING("weapons\\v_"))) != std::string_view::npos && szModelName.find((TSTRING("arms"))) == std::string_view::npos)
	{
		auto pOwnerEntity = TInterfaces::ClientEntityList->Get<CBaseEntity>(pEntity->GetMoveParent());

			if (TVariables::TModels::TLocal::Weapon.Value != 0) {

				pMaterialMain = matMaterials[TVariables::TModels::TLocal::Weapon.Value - 1];

				pMaterialMain->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				TInterfaces::StudioRender->SetColorModulation(TVariables::TModels::TLocal::Weapon.Color.flColors);
				TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialMain);

				if (TVariables::TModels::TLocal::WeaponOverlay.bEnabled)
				{
					oDrawModel(TInterfaces::StudioRender, 0, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

					pMaterialExtra->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
					TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialExtra);

					IMaterialVar* Overlay = pMaterialExtra->FindVar(("$envmaptint"), nullptr);
					Overlay->SetVector(TVariables::TModels::TLocal::WeaponOverlay.Color.flColors);

				}

				return true;
			}
	}
	else if (szModelName.find((TSTRING("player"))) != std::string_view::npos && szModelName.find((TSTRING("shadow"))) == std::string_view::npos)
	{
		if (!pEntity->IsAlive() || pEntity->IsDormant())
			return false;

		if (pEntity == TGlobalVars::pLocalEntity) {

			if (TVariables::TModels::TLocal::FakelagModel.Value != 0 && TGlobalVars::pLocalEntity->GetVelocity().Length2D() > 1.5f) {

				pMaterialMain = matMaterials[TVariables::TModels::TLocal::FakelagModel.Value - 1];

				pMaterialMain->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				TInterfaces::StudioRender->SetColorModulation(TVariables::TModels::TLocal::FakelagModel.Color.flColors);
				TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialMain);

				if (TVariables::TModels::TLocal::FakelagFakeOverlay.bEnabled)
				{
					oDrawModel(TInterfaces::StudioRender, 0, pResults, info, TLocalAnimation::matLag, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

					pMaterialExtra->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
					TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialExtra);

					IMaterialVar* Overlay = pMaterialExtra->FindVar(("$envmaptint"), nullptr);
					Overlay->SetVector(TVariables::TModels::TLocal::FakelagFakeOverlay.Color.flColors);
				}


				oDrawModel(TInterfaces::StudioRender, 0, pResults, info, TLocalAnimation::matLag, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

				TInterfaces::StudioRender->SetColorModulation(flDefaultCol);
				TInterfaces::StudioRender->ForcedMaterialOverride(nullptr);
			}

			if (TVariables::TModels::TLocal::FakeModel.Value != 0) {

				pMaterialMain = matMaterials[TVariables::TModels::TLocal::FakeModel.Value - 1];

				pMaterialMain->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				TInterfaces::StudioRender->SetColorModulation(TVariables::TModels::TLocal::FakeModel.Color.flColors);
				TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialMain);

				if (TVariables::TModels::TLocal::FakeOverlay.bEnabled)
				{
					oDrawModel(TInterfaces::StudioRender, 0, pResults, info, TLocalAnimation::matDesync, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

					pMaterialExtra->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
					TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialExtra);

					IMaterialVar* Overlay = pMaterialExtra->FindVar(("$envmaptint"), nullptr);
					Overlay->SetVector(TVariables::TModels::TLocal::FakeOverlay.Color.flColors);
				}


				oDrawModel(TInterfaces::StudioRender, 0, pResults, info, TLocalAnimation::matDesync, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

				TInterfaces::StudioRender->SetColorModulation(flDefaultCol);
				TInterfaces::StudioRender->ForcedMaterialOverride(nullptr);
			}

			if (TVariables::TModels::TLocal::Model.Value != 0) {

				pMaterialMain = matMaterials[TVariables::TModels::TLocal::Model.Value - 1];

				pMaterialMain->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				TInterfaces::StudioRender->SetColorModulation(TVariables::TModels::TLocal::Model.Color.flColors);
				TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialMain);

				if (TVariables::TModels::TLocal::Overlay.bEnabled)
				{
					oDrawModel(TInterfaces::StudioRender, 0, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

					pMaterialExtra->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
					TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialExtra);

					IMaterialVar* Overlay = pMaterialExtra->FindVar(("$envmaptint"), nullptr);
					Overlay->SetVector(TVariables::TModels::TLocal::Overlay.Color.flColors);

				}
				

				return true;
			}

		}
		else if (!TGlobalVars::pLocalEntity->IsEnemy(pEntity))
		{
			bool bReturn = false;

			TInterfaces::StudioRender->SetColorModulation(flDefaultCol);
			TInterfaces::StudioRender->ForcedMaterialOverride(nullptr);

			if (TVariables::TModels::TTeam::ModelHidden.Value != 0) {

				pMaterialMain = matMaterials[TVariables::TModels::TTeam::ModelHidden.Value - 1];

				pMaterialMain->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				TInterfaces::StudioRender->SetColorModulation(TVariables::TModels::TTeam::ModelHidden.Color.flColors);
				TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialMain);

				if (TVariables::TModels::TTeam::ModelOverlayHidden.bEnabled)
				{
					oDrawModel(TInterfaces::StudioRender, 0, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

					pMaterialExtra->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
					TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialExtra);

					IMaterialVar* Overlay = pMaterialExtra->FindVar(("$envmaptint"), nullptr);
					Overlay->SetVector(TVariables::TModels::TTeam::ModelOverlayHidden.Color.flColors);

				}

				oDrawModel(TInterfaces::StudioRender, 0, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

				bReturn = true;
			}

			TInterfaces::StudioRender->SetColorModulation(flDefaultCol);
			TInterfaces::StudioRender->ForcedMaterialOverride(nullptr);

			if (TVariables::TModels::TTeam::ModelVisible.Value != 0) {

				pMaterialMain = matMaterials[TVariables::TModels::TTeam::ModelVisible.Value - 1];

				pMaterialMain->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				TInterfaces::StudioRender->SetColorModulation(TVariables::TModels::TTeam::ModelVisible.Color.flColors);
				TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialMain);

				if (TVariables::TModels::TTeam::ModelOverlayVisible.bEnabled)
				{
					oDrawModel(TInterfaces::StudioRender, 0, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

					pMaterialExtra->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
					TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialExtra);
					IMaterialVar* Overlay = pMaterialExtra->FindVar(("$envmaptint"), nullptr);
					Overlay->SetVector(TVariables::TModels::TTeam::ModelOverlayVisible.Color.flColors);

				}

				bReturn = true;
			}

			return bReturn;
		}
		else
		{

			bool bReturn = false;

			if (TVariables::TModels::TEnemy::BacktrackHidden.Value != 0 || TVariables::TModels::TEnemy::BacktrackVisible.Value != 0)
			{

				TEntityObject* Object = TEntityListener::GetEntityObject(pEntity);

				if (Object && pEntity->GetVelocity().Length2D()> 4.f && Object->AnimationInstance) {

					TAnimationRecord* OldestValidRecord = TAnimationUpdate::GetLatestValidRecord(Object->AnimationInstance);

					if (OldestValidRecord) {

						if (TVariables::TModels::TEnemy::BacktrackHidden.Value != 0) {

							pMaterialMain = matMaterials[TVariables::TModels::TEnemy::BacktrackHidden.Value - 1];

							pMaterialMain->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
							TInterfaces::StudioRender->SetColorModulation(TVariables::TModels::TEnemy::BacktrackHidden.Color.flColors);
							TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialMain);

							if (TVariables::TModels::TEnemy::BacktrackOverlayHidden.bEnabled)
							{
								oDrawModel(TInterfaces::StudioRender, 0, pResults, info, OldestValidRecord->matModel, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

								pMaterialExtra->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
								TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialExtra);

								IMaterialVar* Overlay = pMaterialExtra->FindVar(("$envmaptint"), nullptr);
								Overlay->SetVector(TVariables::TModels::TEnemy::BacktrackOverlayHidden.Color.flColors);

							}

							oDrawModel(TInterfaces::StudioRender, 0, pResults, info, OldestValidRecord->matModel, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

							bReturn = true;
						}

						if (TVariables::TModels::TEnemy::BacktrackVisible.Value != 0) {

							pMaterialMain = matMaterials[TVariables::TModels::TEnemy::BacktrackVisible.Value - 1];

							pMaterialMain->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
							TInterfaces::StudioRender->SetColorModulation(TVariables::TModels::TEnemy::BacktrackVisible.Color.flColors);
							TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialMain);

							if (TVariables::TModels::TEnemy::BacktrackOverlayVisible.bEnabled)
							{
								oDrawModel(TInterfaces::StudioRender, 0, pResults, info, OldestValidRecord->matModel, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

								pMaterialExtra->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
								TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialExtra);

								IMaterialVar* Overlay = pMaterialExtra->FindVar(("$envmaptint"), nullptr);
								Overlay->SetVector(TVariables::TModels::TEnemy::BacktrackOverlayVisible.Color.flColors);

							}

							oDrawModel(TInterfaces::StudioRender, 0, pResults, info, OldestValidRecord->matModel, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

							bReturn = true;
						}
					}
				}
			}

			TInterfaces::StudioRender->SetColorModulation(flDefaultCol);
			TInterfaces::StudioRender->ForcedMaterialOverride(nullptr);

			if (TVariables::TModels::TEnemy::ModelHidden.Value != 0) {

				pMaterialMain = matMaterials[TVariables::TModels::TEnemy::ModelHidden.Value - 1];

				pMaterialMain->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
				TInterfaces::StudioRender->SetColorModulation(TVariables::TModels::TEnemy::ModelHidden.Color.flColors);
				TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialMain);

				if (TVariables::TModels::TEnemy::ModelOverlayHidden.bEnabled)
				{
					oDrawModel(TInterfaces::StudioRender, 0, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

					pMaterialExtra->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
					TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialExtra);

					IMaterialVar* Overlay = pMaterialExtra->FindVar(("$envmaptint"), nullptr);
					Overlay->SetVector(TVariables::TModels::TEnemy::ModelOverlayHidden.Color.flColors);

				}

				oDrawModel(TInterfaces::StudioRender, 0, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

				bReturn = true;
			}

			TInterfaces::StudioRender->SetColorModulation(flDefaultCol);
			TInterfaces::StudioRender->ForcedMaterialOverride(nullptr);

			if (TVariables::TModels::TEnemy::ModelVisible.Value != 0) {

				pMaterialMain = matMaterials[TVariables::TModels::TEnemy::ModelVisible.Value - 1];

				pMaterialMain->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
				TInterfaces::StudioRender->SetColorModulation(TVariables::TModels::TEnemy::ModelVisible.Color.flColors);
				TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialMain);

				if (TVariables::TModels::TEnemy::ModelOverlayVisible.bEnabled)
				{
					oDrawModel(TInterfaces::StudioRender, 0, pResults, info, pBoneToWorld, flFlexWeights, flFlexDelayedWeights, vecModelOrigin, nFlags);

					pMaterialExtra->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, false);
					TInterfaces::StudioRender->ForcedMaterialOverride(pMaterialExtra);

					IMaterialVar* Overlay = pMaterialExtra->FindVar(("$envmaptint"), nullptr);
					Overlay->SetVector(TVariables::TModels::TEnemy::ModelOverlayVisible.Color.flColors);

				}

				bReturn = true;
			}

			return bReturn;
		}

	}


	return false;

}
