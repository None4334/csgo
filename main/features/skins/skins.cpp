#include "../../../includes.h"
#include "skins.h"
#include "../../sdk.h"
#include "../../ui/vars.h"
#include "../../gvars.h"

#include "../../../dependencies/imgui/imgui.h"
#include "../../../dependencies/imgui/backends/imgui_impl_dx9.h"
#include "../../../dependencies/imgui/backends/imgui_impl_win32.h"
#include "../../../dependencies/imgui/imgui_internal.h"
#include "../../handlers/eventlogger.h"

/*
cl_bob_lower_amt default
cl_bobamt_lat default
cl_bobamt_vert default
cl_bobcycle default
cl_bobup 5
*/

CreateClientClassFn TSkinChanger::CreateWearable() {
	auto arrClasses = TInterfaces::BaseClient->GetAllClasses();

	while (strcmp(arrClasses->szNetworkName, TSTRING("CEconWearable")))
		arrClasses = arrClasses->pNext;

	return arrClasses->pCreateFn;
}

bool should_update_hud = false;

CBaseAttributableItem* TSkinChanger::MakeGlove(int entry, int serial) {
	static auto create_wearable_fn = CreateWearable();
	create_wearable_fn(entry, serial);

	using SetAbsOriginFn = void(__thiscall*)(void*, const Vector3D&);
	static auto oSetAbsOrigin = reinterpret_cast<SetAbsOriginFn>(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8")));

	const auto glove = reinterpret_cast<CBaseAttributableItem*>(TInterfaces::ClientEntityList->GetClientEntity(entry));
	assert(glove); {
		Vector3D new_pos = { 10000.f, 10000.f, 10000.f };
		oSetAbsOrigin(glove, new_pos);
	}
	return glove;
}


void TSkinChanger::UpdateSkins()
{
	TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("record x;stop"));
	TInterfaces::EngineClient->ClientCmdUnrestricted(TSTRING("clear"));
	should_update_hud = true;
}

void TSkinChanger::PostDataUpdate(CBaseEntity* pLocalPlayer) {
	
	PlayerInfo_t pPlayerInfo;

	if (!TInterfaces::EngineClient->GetPlayerInfo(pLocalPlayer->GetIndex(), &pPlayerInfo))
		return;

	auto weapons = pLocalPlayer->GetWeaponsHandle();
	for (int i = 0; weapons[i].IsValid(); i++)
	{
		CBaseAttributableItem* weapon = (CBaseAttributableItem*)TInterfaces::ClientEntityList->GetClientEntityFromHandle(weapons[i]);
		if (!weapon)
			continue;
		auto& definition_index = weapon->GetItemDefinitionIndex();
		const auto active_conf = &options.Items[IsKnife(definition_index) ? WEAPON_KNIFE : definition_index];
		ApplyConfigOnAttributableItem(pLocalPlayer, weapon, active_conf, pPlayerInfo.nXuidLow);
	}

	const auto view_model_handle = pLocalPlayer->GetViewModelHandle();

	if (!view_model_handle.IsValid())
		return;

	const auto view_model = reinterpret_cast<CBaseViewModel*>(TInterfaces::ClientEntityList->GetClientEntityFromHandle(view_model_handle));
	if (!view_model)
		return;

	const auto view_model_weapon_handle = view_model->GetWeaponHandle();
	if (!view_model_weapon_handle.IsValid())
		return;

	const auto view_model_weapon = reinterpret_cast <CBaseCombatWeapon*>(TInterfaces::ClientEntityList->GetClientEntityFromHandle(view_model_weapon_handle));
	if (!view_model_weapon)
		return;

	if (WeaponInfo.count(view_model_weapon->GetItemDefinitionIndex()))
	{
		const auto override_model = WeaponInfo.at(view_model_weapon->GetItemDefinitionIndex()).charModel;
		auto override_model_index = TInterfaces::ModelInfo->GetModelIndex(override_model);
		view_model->GetVModelIndex() = override_model_index;
		charIndex = override_model;
		auto world_model_handle = view_model_weapon->GetWorldModelHandle();

		if (!world_model_handle.IsValid())
			return;

		const auto world_model = reinterpret_cast<CBaseWeaponWorldModel*>(TInterfaces::ClientEntityList->GetClientEntityFromHandle(world_model_handle));

		if (!world_model)
			return;

		*world_model->GetModelIndex() = override_model_index + 1;
	}
}

void TSkinChanger::SetupConfig() {

	static int iCounter = 0;

	for (auto& [Key, Val] : WeaponNamesFull) {
		auto& Option = options.Items[Key];

		TConfigManager::TVariableStack::SetSection(TSTRING("s_sk")+std::to_string(iCounter));
		TConfigManager::TVariableStack::PushVar(&Option.iDefinitionVectorIndex);
		TConfigManager::TVariableStack::PushVar(&Option.iDefinitionIndex);
		TConfigManager::TVariableStack::PushVar(&Option.iPaintKitVectorIndex);
		TConfigManager::TVariableStack::PushVar(&Option.iPaintKitIndex);
		TConfigManager::TVariableStack::PushVar(&Option.iDefinitionOverrideIndex);
		TConfigManager::TVariableStack::PushVar(&Option.iDefinitionOverrideVectorIndex);
		TConfigManager::TVariableStack::PushVar(&Option.iSeed);
		TConfigManager::TVariableStack::PushVar(&Option.bEnableStatTrack);
		TConfigManager::TVariableStack::PushVar(&Option.iStatTrack);
		TConfigManager::TVariableStack::PushVar(&Option.flWear);
		iCounter++;
	}

	iCounter++;
	TConfigManager::TVariableStack::SetSection(TSTRING("s_sk") + std::to_string(iCounter));
	TConfigManager::TVarUtil::PushConfigVar(&TVariables::TSkins::ModelChanger);
	TConfigManager::TVarUtil::PushConfigVar(&TVariables::TSkins::KnifeModel);
	TConfigManager::TVarUtil::PushConfigVar(&TVariables::TSkins::GloveModel);
	TConfigManager::TVarUtil::PushConfigVar(&TVariables::TSkins::GloveWear);
	TConfigManager::TVarUtil::PushConfigVar(&TVariables::TSkins::GloveSkinBlood);
	TConfigManager::TVarUtil::PushConfigVar(&TVariables::TSkins::GloveSkinSport);
	TConfigManager::TVarUtil::PushConfigVar(&TVariables::TSkins::GloveSkinDriver);
	TConfigManager::TVarUtil::PushConfigVar(&TVariables::TSkins::GloveSkinHandWrap);
	TConfigManager::TVarUtil::PushConfigVar(&TVariables::TSkins::GloveSkinMotor);
	TConfigManager::TVarUtil::PushConfigVar(&TVariables::TSkins::GloveSkinSpecialist);
	TConfigManager::TVarUtil::PushConfigVar(&TVariables::TSkins::GloveSkinHydra);
	TConfigManager::TVarUtil::PushConfigVar(&TVariables::TSkins::GloveSkinFang);
}

void TSkinChanger::Start(bool m_bEnd)
{
	if (!TGlobalVars::pLocalEntity)
		return;

	PlayerInfo_t pPlayerInfo;

	if (!TInterfaces::EngineClient->GetPlayerInfo(TGlobalVars::pLocalEntity->GetIndex(), &pPlayerInfo))
		return;

	if (!TGlobalVars::pLocalEntity->IsAlive())
		return;

	if (m_bEnd) {

		if (TGlobalVars::pLocalEntity)
		{
			if (should_update_hud)
			{
				using ClearHudWeaponIconFn = int(__thiscall*)(void*, int);
				static auto oClearHudWeaponIcon = reinterpret_cast<ClearHudWeaponIconFn>(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 51 53 56 8B 75 08 8B D9 57 6B")));

				if (const auto pHudWeapons = TUtil::FindHudElement(TSTRING("CCSGO_HudWeaponSelection")) - 0x28; pHudWeapons != nullptr)
				{
					for (std::size_t i = 0; i < *(pHudWeapons + 0x20); i++)
						i = oClearHudWeaponIcon(pHudWeapons, i);
				}



				should_update_hud = false;
			}
		}

		UpdateGloves(TGlobalVars::pLocalEntity);
	}
	else
		PostDataUpdate(TGlobalVars::pLocalEntity);

}


void TSkinChanger::RenderMenu()
{
	static char search_bar[62];

	if (TInterfaces::EngineClient->IsInGame() && TInterfaces::EngineClient->IsConnected())
		if (TGlobalVars::pLocalEntity && TGlobalVars::pLocalEntity->IsAlive())
			PickCurrentWeapon(TGlobalVars::pLocalEntity, &iWeaponIndexSkins, &iWeaponVectorIndexSkins, WeaponNamesFull);

	auto& pSelectedEntry = options.Items[WeaponNamesFull[iWeaponVectorIndexSkins].iDefinitionIndex];
	pSelectedEntry.iDefinitionIndex = iWeaponIndexSkins;
	pSelectedEntry.iDefinitionOverrideVectorIndex = iWeaponVectorIndexSkins;

	ImGui::SetCursorPos(ImVec2(80, 100));
	ImGui::BeginChild(TSTRING("Models"), ImVec2(300, 400), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);

	static std::vector<const char*> vecMenuItems;

	static bool bFilled = false;

	if (!bFilled) {
		for (auto& vecCurrent : vecModels)
			vecMenuItems.emplace_back(vecCurrent.szMenuName.c_str());
		bFilled = true;
	}

	ImGui::SingleSelect(TSTRING("Player Model"), &TVariables::TSkins::ModelChanger.Value, vecMenuItems);

	if (pSelectedEntry.iDefinitionIndex == WEAPON_KNIFE)
	{
		ImGui::SingleSelect(TSTRING("Knife model"), &TVariables::TSkins::KnifeModel.Value, { TSTRING("default"), TSTRING("bayonet"), TSTRING("classic"), TSTRING("cord"), TSTRING("canis"), TSTRING("outdoor"), TSTRING("skeleton"),TSTRING("flip"), TSTRING("gut"), TSTRING("karambit"), TSTRING("m9 bayonet"), TSTRING("huntsman"), TSTRING("falchion"), TSTRING("bowie"), TSTRING("butterfly"), TSTRING("daggers"),TSTRING("ursus"), TSTRING("navaja"), TSTRING("stiletto"), TSTRING("talon")});
		pSelectedEntry.iDefinitionOverrideVectorIndex = TVariables::TSkins::KnifeModel.Value;
		pSelectedEntry.iDefinitionOverrideIndex = KnifeNames.at(pSelectedEntry.iDefinitionOverrideVectorIndex).iDefinitionIndex;
	}
	else
	{
		ImGui::SingleSelect(TSTRING("Knife model"), &TVariables::TSkins::KnifeModel.Value, { TSTRING("default"), TSTRING("bayonet"), TSTRING("classic"), TSTRING("cord"),TSTRING("canis"), TSTRING("outdoor"), TSTRING("skeleton"), TSTRING("flip"), TSTRING("gut"), TSTRING("karambit"), TSTRING("m9 bayonet"), TSTRING("huntsman"), TSTRING("falchion"), TSTRING("bowie"), TSTRING("butterfly"), TSTRING("daggers"), TSTRING("ursus"), TSTRING("navaja"), TSTRING("stiletto"), TSTRING("talon")});
	}

	ImGui::SingleSelect(TSTRING("Glove model"), &TVariables::TSkins::GloveModel.Value, { TSTRING("default"),TSTRING("bloodhound"),TSTRING("sport"),TSTRING("driver"),TSTRING("hand wraps"),TSTRING("motorcycle"),TSTRING("specialist"),TSTRING("hydra"), TSTRING("broken fang")});

	switch (TVariables::TSkins::GloveModel.Value)
	{
	case 1:
		ImGui::SingleSelect(TSTRING("Glove skin"), &TVariables::TSkins::GloveSkinBlood.Value, {glove_skinz_blood});
		break;
	case 2:
		ImGui::SingleSelect(TSTRING("Glove skin"), &TVariables::TSkins::GloveSkinSport.Value, { glove_skinz_sport });
		break;
	case 3:
		ImGui::SingleSelect(TSTRING("Glove skin"), &TVariables::TSkins::GloveSkinDriver.Value, { glove_skinz_driver });
		break;
	case 4:
		ImGui::SingleSelect(TSTRING("Glove skin"), &TVariables::TSkins::GloveSkinHandWrap.Value, { glove_skinz_handwar });
		break;
	case 5:
		ImGui::SingleSelect(TSTRING("Glove skin"), &TVariables::TSkins::GloveSkinMotor.Value, { glove_skinz_motor });
		break;
	case 6:
		ImGui::SingleSelect(TSTRING("Glove skin"), &TVariables::TSkins::GloveSkinSpecialist.Value, { glove_skinz_specialist });
		break;
	case 7:
		ImGui::SingleSelect(TSTRING("Glove skin"), &TVariables::TSkins::GloveSkinHydra.Value, { glove_skinz_hydra });
		break;
	case 8:
		ImGui::SingleSelect(TSTRING("Glove skin"), &TVariables::TSkins::GloveSkinFang.Value, { glove_skinz_fang });
		break;
	}

	if (ImGui::SliderFloat(TSTRING("Wear"), &TVariables::TSkins::GloveWear.flValue, 0.f, 1.f, TSTRING("%.1f")))
		TVariables::TSkins::GloveWear.flValue = std::clamp(TVariables::TSkins::GloveWear.flValue, 0.f, 1.f);

	ImGui::EndChild();

	ImGui::SetCursorPos(ImVec2(80 + 300 + 20, 100));
	ImGui::BeginChild(TSTRING("Skins"), ImVec2(300, 400), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);


	ImGui::Checkbox(TSTRING("Stattrak"), &pSelectedEntry.bEnableStatTrack);
	ImGui::SliderInt(TSTRING("##stattrakVal"), &pSelectedEntry.iStatTrack, 0, 1000);
	ImGui::SliderInt(TSTRING("Weapon Seed"), &pSelectedEntry.iSeed, 1, 5555, TSTRING("%.1f"));

	if (ImGui::SliderFloat(TSTRING("Weapon Wear"), &pSelectedEntry.flWear, 0.f, 1.f, TSTRING("%.1f")))
		pSelectedEntry.flWear = std::clamp(pSelectedEntry.flWear, 0.f, 1.f);

	ImGui::InputText(TSTRING("##Searchbyname"), search_bar, sizeof(search_bar));

	ImGui::ListBoxHeader(TSTRING("##paintkits"), ImVec2(0, 200));

	if (ImGui::Selectable(TSTRING("Default"), pSelectedEntry.iPaintKitIndex == 0))
		pSelectedEntry.iPaintKitIndex = 0;

	static auto GetColRar = [&](int rar) -> ImVec4 {
		switch (rar)
		{
		case 1:
			return ImColor(150, 150, 150, 255);
			break;
		case 2:
			return ImColor(100, 100, 255, 255);
			break;
		case 3:
			return ImColor(50, 50, 255, 255);
			break;
		case 4:
			return ImColor(255, 64, 242, 255);
			break;
		case 5:
			return ImColor(255, 50, 50, 255);
			break;
		case 6:
			return ImColor(255, 50, 50, 255);
			break;
		case 7:
			return ImColor(255, 196, 46, 255);
			break;
		default:
			return ImColor(150, 150, 150, 255);
			break;
		}
	};

	bool is_knife = pSelectedEntry.iDefinitionIndex == WEAPON_KNIFE || pSelectedEntry.iDefinitionIndex == WEAPON_KNIFE_T;
	int defindex = (is_knife || pSelectedEntry.iDefinitionIndex == GLOVE_T || pSelectedEntry.iDefinitionIndex == GLOVE_CT) ? pSelectedEntry.iDefinitionOverrideIndex : pSelectedEntry.iDefinitionIndex;
	std::string skinName = TBaseEntity::GetWeaponNameById(defindex);

	if (skinName.compare("") != 0)
	{

		std::string filter = std::string(search_bar);

		int curItem = -1;
		int s = 0;

		auto set = TPaintKits::weaponSkins[skinName];
		std::vector<std::string> list(set.size());
		std::vector<std::string> anti_repeat;
		static std::vector<std::string> pLdlist;

		std::copy(set.begin(), set.end(), list.begin());

		list = {};
		for (auto [key, value] : TPaintKits::weaponSkins) {
			for (auto skin : value) {
				list.push_back(skin);
			}
		}

		auto fnAntiRepeat = [&](std::string tx) -> bool {
			auto ret = std::find(anti_repeat.begin(), anti_repeat.end(), tx) == anti_repeat.end();

			if (ret)
				anti_repeat.push_back(tx);
			return ret;
		};

		static bool bListLoaded = false;

		if (!bListLoaded) {
			for (auto& skin : list) {

				if (fnAntiRepeat(TPaintKits::skinNames[TPaintKits::skinMap[skin].tagName]))
					pLdlist.emplace_back(skin);
			}
			bListLoaded = true;
		}

		for (auto skin : pLdlist) {
			int pk = TPaintKits::skinMap[skin].iPaintKit;

			if (pk == pSelectedEntry.iPaintKitIndex)
				curItem = s;

			bool passed_filter = true;

			if (!filter.empty() && !TPaintKits::skinNames[TPaintKits::skinMap[skin].tagName].empty() && filter != " ") {
				std::string tempName = TPaintKits::skinNames[TPaintKits::skinMap[skin].tagName];
				std::string tempQuery = filter;
				std::transform(tempName.begin(), tempName.end(), tempName.begin(), ::tolower);
				std::transform(tempQuery.begin(), tempQuery.end(), tempQuery.begin(), ::tolower);
				if (tempName.find(tempQuery) == std::string::npos)
					passed_filter = false;
			}


			if (passed_filter && !TPaintKits::skinNames[TPaintKits::skinMap[skin].tagName].empty()) {
				if (ImGui::Selectable((TPaintKits::skinNames[TPaintKits::skinMap[skin].tagName]).c_str(), pk == pSelectedEntry.iPaintKitIndex)) {
					pSelectedEntry.iPaintKitIndex = pk;
				}
			}

			s++;
		}
	}

	ImGui::ListBoxFooter();
	ImGui::EndChild();
}

void TSkinChanger::UpdateGloves(CBaseEntity* pLocalPlayer) {

	if (TVariables::TSkins::GloveModel.Value != 0) {
		auto model_blood = TSTRING("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl");
		auto model_sport = TSTRING("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl");
		auto model_slick = TSTRING("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl");
		auto model_leath = TSTRING("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl");
		auto model_moto = TSTRING("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl");
		auto model_speci = TSTRING("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl");
		auto model_hydra = TSTRING("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl");
		auto model_fang = TSTRING("models/weapons/v_models/arms/anarchist/v_glove_anarchist.mdl");

		auto index_blood = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound.mdl"));
		auto index_sport = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_models/arms/glove_sporty/v_glove_sporty.mdl"));
		auto index_slick = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_models/arms/glove_slick/v_glove_slick.mdl"));
		auto index_leath = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_models/arms/glove_handwrap_leathery/v_glove_handwrap_leathery.mdl"));
		auto index_moto = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_models/arms/glove_motorcycle/v_glove_motorcycle.mdl"));
		auto index_speci = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_models/arms/glove_specialist/v_glove_specialist.mdl"));
		auto index_hydra = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_models/arms/glove_bloodhound/v_glove_bloodhound_hydra.mdl"));
		auto index_fang = TInterfaces::ModelInfo->GetModelIndex(TSTRING("models/weapons/v_models/arms/anarchist/v_glove_anarchist.mdl"));

		const auto wearables = pLocalPlayer->GetWearablesHandle();

		if (!wearables)
			return;

		static UINT glove_handle = UINT(0);

		auto glove = reinterpret_cast<CBaseAttributableItem*>(TInterfaces::ClientEntityList->GetClientEntityFromHandle(wearables[0]));

		if (!glove) {
			const auto our_glove = reinterpret_cast<CBaseAttributableItem*>(TInterfaces::ClientEntityList->GetClientEntityFromHandle(glove_handle));

			if (our_glove) {
				wearables[0] = glove_handle;
				glove = our_glove;
			}
		}
		if (!pLocalPlayer->IsAlive()) {
			if (glove) {
				glove->GetClientNetworkable()->SetDestroyedOnRecreateEntities();
				glove->GetClientNetworkable()->Release();
			}
			return;
		}
		if (!glove) {
			const auto entry = TInterfaces::ClientEntityList->GetHighestEntityIndex() + 1;
			const auto serial = rand() % 0x1000;
			glove = MakeGlove(entry, serial);
			wearables[0] = entry | serial << 16;
			glove_handle = wearables[0];
		}
		if (glove) {

			float gloves_wear = TVariables::TSkins::GloveWear.flValue;
			switch (TVariables::TSkins::GloveModel.Value) {
			case 0:
				gloves_wear = 0.0000001f;
				break;
			case 1:
				gloves_wear = 0.07f;
				break;
			case 2:
				gloves_wear = 0.15f;
				break;
			case 3:
				gloves_wear = 0.38f;
				break;
			case 4:
				gloves_wear = 0.45f;
				break;
			}

			switch (TVariables::TSkins::GloveModel.Value) {
			case 0:
				break;
			case 1:
				ApplyGloveModel(glove, model_blood);
				break;
			case 2:
				ApplyGloveModel(glove, model_sport);
				break;
			case 3:
				ApplyGloveModel(glove, model_slick);
				break;
			case 4:
				ApplyGloveModel(glove, model_leath);
				break;
			case 5:
				ApplyGloveModel(glove, model_moto);
				break;
			case 6:
				ApplyGloveModel(glove, model_speci);
				break;
			case 7:
				ApplyGloveModel(glove, model_hydra);
				break;
			case 8:
				ApplyGloveModel(glove, model_fang);
				break;
			}

			switch (TVariables::TSkins::GloveModel.Value) {
			case 0:
				break;
			case 1:
				ApplyGloveSkins(glove, GLOVE_STUDDED_BLOODHOUND, GetGloveSkinByMenu(0, TVariables::TSkins::GloveSkinBlood.Value), index_blood, 0, 3, gloves_wear);
				break;
			case 2:
				ApplyGloveSkins(glove, GLOVE_SPORTY, GetGloveSkinByMenu(1, TVariables::TSkins::GloveSkinSport.Value), index_sport, 0, 3, gloves_wear);
				break;
			case 3:
				ApplyGloveSkins(glove, GLOVE_SLICK, GetGloveSkinByMenu(2, TVariables::TSkins::GloveSkinDriver.Value), index_slick, 0, 3, gloves_wear);
				break;
			case 4:
				ApplyGloveSkins(glove, GLOVE_LEATHER_HANDWRAPS, GetGloveSkinByMenu(3, TVariables::TSkins::GloveSkinHandWrap.Value), index_leath, 0, 3, gloves_wear);
				break;
			case 5:
				ApplyGloveSkins(glove, GLOVE_MOTORCYCLE, GetGloveSkinByMenu(4, TVariables::TSkins::GloveSkinMotor.Value), index_moto, 0, 3, gloves_wear);
				break;
			case 6:
				ApplyGloveSkins(glove, GLOVE_SPECIALIST, GetGloveSkinByMenu(5, TVariables::TSkins::GloveSkinSpecialist.Value), index_speci, 0, 3, gloves_wear);
				break;
			case 7:
				ApplyGloveSkins(glove, GLOVE_STUDDED_HYDRA, GetGloveSkinByMenu(6, TVariables::TSkins::GloveSkinHandWrap.Value), index_hydra, 0, 3, gloves_wear);
				break;
			case 8:
				ApplyGloveSkins(glove, GLOVE_STUDDED_BROKENFANG, GetGloveSkinByMenu(7, TVariables::TSkins::GloveSkinFang.Value), index_fang, 0, 3, gloves_wear);
				break;
			}

			glove->GetItemIDHigh() = -1;
			glove->GetFallbackSeed() = 0;
			glove->GetFallbackStatTrak() = -1;
			glove->PreDataUpdate(DATA_UPDATE_CREATED);
		}
	}
}

void TSkinChanger::CheckSkinUpdate()
{
	if (!TInterfaces::EngineClient->IsInGame())
		return;

	if (!TGlobalVars::pLocalEntity)
		return;

	if (!TGlobalVars::pLocalEntity->IsAlive() || !TBaseEntity::GetWeapon(TGlobalVars::pLocalEntity))
		return;

	static int stored_knife = 0;
	static int stored_glovemodel_index = 0;
	static int stored_glove_blood = 0;
	static int stored_glove_sport = 0;
	static int stored_glove_driver = 0;
	static int stored_glove_handwar = 0;
	static int stored_glove_motor = 0;
	static int stored_glove_specialist = 0;
	static int stored_glove_hydra = 0;
	static int stored_glove_fang = 0;

	if (stored_glove_hydra != TVariables::TSkins::GloveSkinHydra.Value
		|| stored_glove_fang != TVariables::TSkins::GloveSkinFang.Value
		|| stored_glove_specialist != TVariables::TSkins::GloveSkinSpecialist.Value
		|| stored_glove_motor != TVariables::TSkins::GloveSkinMotor.Value
		|| stored_glove_handwar != TVariables::TSkins::GloveSkinHandWrap.Value
		|| stored_glove_driver != TVariables::TSkins::GloveSkinDriver.Value
		|| stored_glove_sport != TVariables::TSkins::GloveSkinSport.Value
		|| stored_glove_blood != TVariables::TSkins::GloveSkinBlood.Value
		|| stored_glovemodel_index != TVariables::TSkins::GloveModel.Value
		|| stored_knife != TVariables::TSkins::KnifeModel.Value)
	{
		UpdateSkins();
		
		stored_glovemodel_index = TVariables::TSkins::GloveModel.Value;
		stored_glove_blood = TVariables::TSkins::GloveSkinBlood.Value;
		stored_glove_sport = TVariables::TSkins::GloveSkinSport.Value;
		stored_glove_driver = TVariables::TSkins::GloveSkinDriver.Value;
		stored_glove_handwar = TVariables::TSkins::GloveSkinHandWrap.Value;
		stored_glove_motor = TVariables::TSkins::GloveSkinMotor.Value;
		stored_glove_specialist = TVariables::TSkins::GloveSkinSpecialist.Value;
		stored_glove_hydra = TVariables::TSkins::GloveSkinHydra.Value;
		stored_glove_fang = TVariables::TSkins::GloveSkinFang.Value;
		stored_knife = TVariables::TSkins::KnifeModel.Value;
	}

	auto& selected_entry = options.Items[WeaponNamesFull[iWeaponVectorIndexSkins].iDefinitionIndex];

	static int stored_paint_kint = 0;
	static bool stored_statrak_bool = false;
	static int stored_statrak = 0;
	static int stored_seed = 0;
	static float stored_wear = 0.0;
	static float stored_glove_wear = 0.0;

	if (stored_paint_kint != selected_entry.iPaintKitIndex
		|| stored_seed != selected_entry.iSeed
		|| stored_wear != selected_entry.flWear
		|| stored_glove_wear != TVariables::TSkins::GloveWear.flValue
		|| stored_statrak != selected_entry.iStatTrack
		|| stored_statrak_bool != selected_entry.bEnableStatTrack)
	{

		UpdateSkins();
		
		stored_paint_kint = selected_entry.iPaintKitIndex;
		stored_seed = selected_entry.iSeed;
		stored_statrak_bool = selected_entry.bEnableStatTrack;
		stored_statrak = selected_entry.iStatTrack;
		stored_wear = selected_entry.flWear;
		stored_glove_wear = TVariables::TSkins::GloveWear.flValue;
	}
}


void TSkinChanger::EraseOverrideIfExsistsByIndex(const int iDefinitionIndex)
{
	if (WeaponInfo.count(iDefinitionIndex)) {
		auto& mapIconOverride = options.IconOverrides;
		const auto& pOriginalItem = WeaponInfo.at(iDefinitionIndex);
		if (pOriginalItem.charIcon && mapIconOverride.count(pOriginalItem.charIcon))
			mapIconOverride.erase(mapIconOverride.at(pOriginalItem.charIcon));
	}

}


std::string GetModelAddress(int iIndex) {

	if (vecModels.size() > iIndex)
		return vecModels.at(iIndex).szAddress;

	return TSTRING("?");
}

void TSkinChanger::ModelChanger(int Stage, bool bForced) {

	static bool bRun = false;

	if (bForced)
	{
		bRun = true;
	}

	if (Stage != FRAME_NET_UPDATE_POSTDATAUPDATE_START && Stage != FRAME_RENDER_END)
		return;

	if (!TGlobalVars::pLocalEntity || !TGlobalVars::pLocalEntity->IsAlive())
		return;

	static float flSpawnTime = 0.0f;

	if (flSpawnTime != TGlobalVars::pLocalEntity->GetSpawnTime())
	{
		bRun = true;
		flSpawnTime = TGlobalVars::pLocalEntity->GetSpawnTime();
	}

	static int iStoredIndex = 0;
	static int iBackupIndex = -1;

	static bool bShouldReset = false;

	static bool bStorePlayerModel = true;

	if (!bRun) {

		if (iStoredIndex != TVariables::TSkins::ModelChanger.Value)
		{

			if (TVariables::TSkins::ModelChanger.Value == 0)
				bShouldReset = true;

			if (iStoredIndex == 0 && TVariables::TSkins::ModelChanger.Value != TVariables::TSkins::ModelChanger.Value)
				bStorePlayerModel = true;

			bRun = true;
			iStoredIndex = TVariables::TSkins::ModelChanger.Value;
		}

	}

	if (bStorePlayerModel) {
		iBackupIndex = TGlobalVars::pLocalEntity->GetModelIndex();
		bStorePlayerModel = false;
		return;
	}

	if (bRun && Stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {

		static auto szAddress = (uintptr_t)(TUtil::FindPattern(CLIENT_STR,TSTRING("E8 ? ? ? ? 89 87 ? ? ? ? 6A")) + 1);

		static std::add_pointer_t<const char** __fastcall(const char* playerModelName)> getPlayerViewmodelArmConfigForPlayerModel =
			TUtil::RelativeToAbsolute<decltype(getPlayerViewmodelArmConfigForPlayerModel)>(szAddress);

		if (bShouldReset) {

			if (iBackupIndex != -1)
				TGlobalVars::pLocalEntity->SetModelIndex(iBackupIndex);

			bShouldReset = false;
		}
		else {
			if (TVariables::TSkins::ModelChanger.Value != 0) {

				std::string szModelAddress = GetModelAddress(TVariables::TSkins::ModelChanger.Value);

				if (Stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
					if (const auto modelprecache =TInterfaces::StringTableContainer->findTable(("modelprecache"))) {
						modelprecache->addString(false, szModelAddress.c_str());
						const auto viewmodelArmConfig = getPlayerViewmodelArmConfigForPlayerModel(szModelAddress.c_str());
						modelprecache->addString(false, viewmodelArmConfig[2]);
						modelprecache->addString(false, viewmodelArmConfig[3]);
					}
				}

				TGlobalVars::pLocalEntity->SetModelIndex(TInterfaces::ModelInfo->GetModelIndex(szModelAddress.c_str()));
			}

		}
	}

}

void TSkinChanger::ApplyConfigOnAttributableItem(CBaseEntity* pLocalPlayer, CBaseAttributableItem* pItem, const ItemSetting_t* pConfig, const unsigned xUidLow) {

	if (!pLocalPlayer)
		return;

	const auto pWeapon = TBaseEntity::GetWeapon(TGlobalVars::pLocalEntity);

	if (!pWeapon)
		return;

	pItem->GetItemIDHigh() = -1;
	pItem->GetAccountID() = xUidLow;

	if (pConfig->charName[0] != TSTRING('\0'))
		strcpy_s(pItem->GetCustomName(), pConfig->charName);

	if (pConfig->iPaintKitIndex) 
		pItem->GetFallbackPaintKit() = pConfig->iPaintKitIndex;
	

	if (pConfig->iSeed)
		pItem->GetFallbackSeed() = pConfig->iSeed;

	if (pConfig->iStatTrack + 1 && pConfig->bEnableStatTrack)
		pItem->GetFallbackStatTrak() = pConfig->iStatTrack;

	if (pConfig->flWear > 0.1)
		pItem->GetFallbackWear() = pConfig->flWear;
	else
		pItem->GetFallbackWear() = 0;



	auto& iDefIndex = pItem->GetItemDefinitionIndex();
	auto& mapIconOverride = options.IconOverrides;

	if (pConfig->iDefinitionOverrideIndex && pConfig->iDefinitionOverrideIndex != iDefIndex && WeaponInfo.count(pConfig->iDefinitionOverrideIndex))
	{
		const auto iOldDefIndex = iDefIndex;
		iDefIndex = pConfig->iDefinitionOverrideIndex;
		const auto& pReplacementItem = WeaponInfo.at(pConfig->iDefinitionOverrideIndex);
		pItem->GetModelIndex() = TInterfaces::ModelInfo->GetModelIndex(pReplacementItem.charModel);
		pItem->SetModelIndex(TInterfaces::ModelInfo->GetModelIndex(pReplacementItem.charModel));
		pItem->PreDataUpdate(EDataUpdateType::DATA_UPDATE_CREATED);

		if (iOldDefIndex && WeaponInfo.count(iOldDefIndex))
		{
			const auto& pOriginalItem = WeaponInfo.at(iOldDefIndex);
			if (pOriginalItem.charIcon && pReplacementItem.charIcon)
			{
				pItem->GetEntityQuality() = 3;
				mapIconOverride[pOriginalItem.charIcon] = pReplacementItem.charIcon;
			}
		}
	}
	else
	{
		EraseOverrideIfExsistsByIndex(iDefIndex);
	}
}


template <typename T>
void TSkinChanger::PickCurrentWeapon(CBaseEntity* pLocalPlayer, int* idx, int* vec_idx, std::vector<T> arr) {

	if (!pLocalPlayer || !TInterfaces::EngineClient->IsInGame())
		return;

	auto pWeapon = TBaseEntity::GetWeapon(TGlobalVars::pLocalEntity);

	if (!pWeapon)
		return;

	short iWeaponIndex = pWeapon->GetItemDefinitionIndex();

	if (IsKnife(iWeaponIndex)) {
		*idx = WeaponNamesFull.at(0).iDefinitionIndex;
		*vec_idx = 0;
		return;
	}
	auto iWeaponIterator = std::find_if(arr.begin(), arr.end(), [iWeaponIndex](const T& a) {
		return a.iDefinitionIndex == iWeaponIndex;
		});

	if (iWeaponIterator != arr.end()) {
		*idx = iWeaponIndex;
		*vec_idx = std::abs(std::distance(arr.begin(), iWeaponIterator));
	}
}

bool TSkinChanger::IsKnife(const int i) {
	return (i >= WEAPON_KNIFE_BAYONET && i < GLOVE_STUDDED_BLOODHOUND) || i == WEAPON_KNIFE_T || i == WEAPON_KNIFE;
}

const char* TSkinChanger::GetIconOverride(const std::string szOriginal) {
	return options.IconOverrides.count(szOriginal) ? options.IconOverrides.at(szOriginal).data() : nullptr;
};

bool TSkinChanger::ApplyGloveModel(CBaseAttributableItem* glove, const char* model) noexcept {
	*reinterpret_cast<int*>(uintptr_t(glove) + 0x64) = -1;
	return true;
}

bool TSkinChanger::ApplyGloveSkins(CBaseAttributableItem* glove, int item_definition_index, int paint_kit, int model_index, int world_model_index, int entity_quality, float fallback_wear) noexcept {
	glove->GetItemDefinitionIndex() = item_definition_index;
	glove->GetFallbackPaintKit() = paint_kit;
	glove->SetModelIndex(model_index);
	glove->GetEntityQuality() = entity_quality;
	glove->GetFallbackWear() = fallback_wear;
	return true;
}

int TSkinChanger::GetGloveSkinByMenu(int gloveIndex, int skinIndex)
{
	if (gloveIndex == 0)
	{
		switch (skinIndex)
		{
		case 0:
			return 10006;
		case 1:
			return 10007;
		case 2:
			return 10008;
		case 3:
			return 10039;
		default:
			return 0;
		}
	}
	else if (gloveIndex == 1)
	{
		switch (skinIndex)
		{
		case 0:
			return 10038;
		case 1:
			return 10037;
		case 2:
			return 10018;
		case 3:
			return 10019;
		case 4:
			return 10048;
		case 5:
			return 10047;
		case 6:
			return 10045;
		case 7:
			return 10046;
		default:
			return 0;
		}
	}
	else if (gloveIndex == 2)
	{
		switch (skinIndex)
		{
		case 0:
			return 10013;
		case 1:
			return 10015;
		case 2:
			return 10016;
		case 3:
			return 10040;
		case 4:
			return 10043;
		case 5:
			return 10044;
		case 6:
			return 10041;
		case 7:
			return 10042;
		default:
			return 0;
		}
	}
	else if (gloveIndex == 3)
	{
		switch (skinIndex)
		{
		case 0:
			return 10009;
		case 1:
			return 10010;
		case 2:
			return 10021;
		case 3:
			return 10036;
		case 4:
			return 10053;
		case 5:
			return 10054;
		case 6:
			return 10055;
		case 7:
			return 10056;
		default:
			return 0;
		}
	}
	else if (gloveIndex == 4)
	{
		switch (skinIndex)
		{
		case 0:
			return 10024;
		case 1:
			return 10026;
		case 2:
			return 10027;
		case 3:
			return 10028;
		case 4:
			return 10050;
		case 5:
			return 10051;
		case 6:
			return 10052;
		case 7:
			return 10049;

		default:
			return 0;
		}
	}
	else if (gloveIndex == 5)
	{
		switch (skinIndex)
		{
		case 0:
			return 10030;
		case 1:
			return 10033;
		case 2:
			return 10034;
		case 3:
			return 10035;
		case 4:
			return 10061;
		case 5:
			return 10062;
		case 6:
			return 10063;
		case 7:
			return 10064;
		default:
			return 0;
		}
	}
	else if (gloveIndex == 6)
	{
		switch (skinIndex)
		{
		case 0:
			return 10057;
		case 1:
			return 10058;
		case 2:
			return 10059;
		case 3:
			return 10060;
		}
	}
	else if (gloveIndex == 7)
	{
		switch (skinIndex)
		{
		case 0:
			return 10085;
		case 1:
			return 10087;
		case 2:
			return 10088;
		case 3:
			return 10086;
		}
	}
	else
		return 0;
	return 0;
}
