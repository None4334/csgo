#include "../../includes.h"
#include "vars.h"
#include "../features/skins/skins.h"

void TConfigManager::Initialize() {

	TVariableStack::SetSection(TSTRING("main"));

	TVariableStack::SetSection(TSTRING("s_1"));
	TVariableStack::PushVar(&TVariables::TVisuals::bEnabled);

	TVarUtil::PushConfigVar(&TVariables::TVisuals::TPlayer::BoundingBox);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TPlayer::Name);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TPlayer::HealthBar);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TPlayer::HealthText);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TPlayer::AmmoBar);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TPlayer::Skeleton);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TPlayer::WeaponText);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TPlayer::WeaponIcon);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TPlayer::Dormant);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TPlayer::Glow);

	TVariableStack::SetSection(TSTRING("s_2"));
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TLocalPlayer::NadePrediction);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TLocalPlayer::NadeWarning);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TLocalPlayer::Glow);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TLocalPlayer::ThirdPerson);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TLocalPlayer::ThirdPersonWhileSpecating);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TLocalPlayer::ViewModelFov);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TLocalPlayer::AspectRatio);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TLocalPlayer::ThirdPersonDistance);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TLocalPlayer::NadePredictionSecondColor);

	TVariableStack::SetSection(TSTRING("s_3"));
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::ServerImpacts);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::ClientImpacts);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::HitMarker);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::DamageMarker);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::DroppedWeaponText);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::DroppedWeaponIcon);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::DroppedWeaponAmmo);

	TVariableStack::SetSection(TSTRING("s_4"));
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::Removals,REMOVAL_MAX);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TLocalPlayer::ViewFov);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TLocalPlayer::NadeWarningSecondColor);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TPlayer::Flags);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::BulletTracers);

	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::NadeInfo);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::BombInfo);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::BarColor);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::NadeInfoRadius);

	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::Nightmode);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::Fullbright);

	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::Propscol);
	TVarUtil::PushConfigVar(&TVariables::TVisuals::TOthers::HitSound);

	TVariableStack::SetSection(TSTRING("s_5"));
	TVarUtil::PushConfigVar(&TVariables::TMisc::MenuColor);
	TVarUtil::PushConfigVar(&TVariables::TMisc::ElementsColor);
	TVarUtil::PushConfigVar(&TVariables::TMisc::UnlockInventory);
	TVarUtil::PushConfigVar(&TVariables::TMisc::ServerPureBypass);
	TVarUtil::PushConfigVar(&TVariables::TMisc::PreserveDeathNotice);
	TVarUtil::PushConfigVar(&TVariables::TMisc::AutoAcceptMatchMaking);
	TVarUtil::PushConfigVar(&TVariables::TMisc::BunnyHop);
	TVarUtil::PushConfigVar(&TVariables::TMisc::ShotLogger);
	TVarUtil::PushConfigVar(&TVariables::TMisc::BuyLogger);
	TVarUtil::PushConfigVar(&TVariables::TMisc::ItemPurchaseLog);
	TVarUtil::PushConfigVar(&TVariables::TMisc::QuickStop);
	TVarUtil::PushConfigVar(&TVariables::TMisc::AutomaticKnife);
	TVarUtil::PushConfigVar(&TVariables::TMisc::AutomaticZeus);
	TVarUtil::PushConfigVar(&TVariables::TMisc::InfiniteDuck);
	TVarUtil::PushConfigVar(&TVariables::TMisc::AutoStrafer);
	TVarUtil::PushConfigVar(&TVariables::TMisc::AutomaticPeek);
	TVarUtil::PushConfigVar(&TVariables::TMisc::AntiUntrusted);

	TVariableStack::SetSection(TSTRING("s_6"));
	TVarUtil::PushConfigVar(&TVariables::TMisc::BuyBot);
	TVarUtil::PushConfigVar(&TVariables::TMisc::PrimaryWeapon);
	TVarUtil::PushConfigVar(&TVariables::TMisc::SecondaryWeapon);
	TVarUtil::PushConfigVar(&TVariables::TMisc::ArmorType);
	TVarUtil::PushConfigVar(&TVariables::TMisc::MapGrenades,6);
	TVarUtil::PushConfigVar(&TVariables::TMisc::Defuser);
	TVarUtil::PushConfigVar(&TVariables::TMisc::Taser);


	TVariableStack::SetSection(TSTRING("s_7"));
	TVariableStack::PushVar(&TVariables::TModels::TGeneral::bEnabled);
	TVarUtil::PushConfigVar(&TVariables::TModels::TLocal::Model);
	TVarUtil::PushConfigVar(&TVariables::TModels::TLocal::Overlay);
	TVarUtil::PushConfigVar(&TVariables::TModels::TLocal::FakeModel);
	TVarUtil::PushConfigVar(&TVariables::TModels::TLocal::FakeOverlay);
	TVarUtil::PushConfigVar(&TVariables::TModels::TLocal::FakelagModel);
	TVarUtil::PushConfigVar(&TVariables::TModels::TLocal::FakelagFakeOverlay);
	TVarUtil::PushConfigVar(&TVariables::TModels::TLocal::Weapon);
	TVarUtil::PushConfigVar(&TVariables::TModels::TLocal::WeaponOverlay);
	TVarUtil::PushConfigVar(&TVariables::TModels::TLocal::Arms);
	TVarUtil::PushConfigVar(&TVariables::TModels::TLocal::ArmsOverlay);
	TVarUtil::PushConfigVar(&TVariables::TModels::TLocal::Attachment);
	TVarUtil::PushConfigVar(&TVariables::TModels::TLocal::AttachmentOverlay);

	TVariableStack::SetSection(TSTRING("s_8"));
	TVarUtil::PushConfigVar(&TVariables::TModels::TEnemy::ModelVisible);
	TVarUtil::PushConfigVar(&TVariables::TModels::TEnemy::ModelHidden);
	TVarUtil::PushConfigVar(&TVariables::TModels::TEnemy::ModelOverlayVisible);
	TVarUtil::PushConfigVar(&TVariables::TModels::TEnemy::ModelOverlayHidden);

	TVariableStack::SetSection(TSTRING("s_9"));
	TVarUtil::PushConfigVar(&TVariables::TModels::TEnemy::BacktrackVisible);
	TVarUtil::PushConfigVar(&TVariables::TModels::TEnemy::BacktrackHidden);
	TVarUtil::PushConfigVar(&TVariables::TModels::TEnemy::BacktrackOverlayVisible);
	TVarUtil::PushConfigVar(&TVariables::TModels::TEnemy::BacktrackOverlayHidden);


	TVariableStack::SetSection(TSTRING("s_10"));
	TVarUtil::PushConfigVar(&TVariables::TModels::TTeam::ModelVisible);
	TVarUtil::PushConfigVar(&TVariables::TModels::TTeam::ModelHidden);
	TVarUtil::PushConfigVar(&TVariables::TModels::TTeam::ModelOverlayVisible);
	TVarUtil::PushConfigVar(&TVariables::TModels::TTeam::ModelOverlayHidden);

	TVariableStack::SetSection(TSTRING("s_11"));
	TSkinChanger::SetupConfig();

	TVariableStack::SetSection(TSTRING("sa_1"));
	TVariableStack::PushVar(&TVariables::TAntiAim::TFakeAngles::bEnabled);
	TVariableStack::PushVar(&TVariables::TAntiAim::TFakeAngles::bEnabled);
	TVariableStack::PushVar(&TVariables::TAntiAim::TFakeAngles::iPitchMode);
	TVariableStack::PushVar(&TVariables::TAntiAim::TFakeAngles::iYawMode);
	TVariableStack::PushVar(&TVariables::TAntiAim::TFakeAngles::iYawValue);
	TVariableStack::PushVar(&TVariables::TAntiAim::TFakeAngles::iYawJitterMode);
	TVariableStack::PushVar(&TVariables::TAntiAim::TFakeAngles::iYawJitterValue);
	TVarUtil::PushConfigVar(&TVariables::TAntiAim::TFakeAngles::Fake);
	TVariableStack::PushVar(&TVariables::TAntiAim::TFakeAngles::iFakeMode);
	TVariableStack::PushVar(&TVariables::TAntiAim::TFakeAngles::iFakeValue);
	TVariableStack::PushVar(&TVariables::TAntiAim::TFakeAngles::iRollValue);
	TVariableStack::SetSection(TSTRING("sa_2"));
	TVariableStack::PushVar(&TVariables::TAntiAim::TFakelag::bFakelagEnabled);
	TVariableStack::PushVar(&TVariables::TAntiAim::TFakelag::iFakelagVariance);
	TVariableStack::PushVar(&TVariables::TAntiAim::TFakelag::iFakelagValue);
	TVariableStack::SetSection(TSTRING("sa_3"));
	TVarUtil::PushConfigVar(&TVariables::TAntiAim::TOthers::Slowmotion);
	TVarUtil::PushConfigVar(&TVariables::TAntiAim::TOthers::Fakeduck);
	TVariableStack::PushVar(&TVariables::TAntiAim::TOthers::iSlowmotionSpeed);

	TVariableStack::SetSection(TSTRING("sab_1"));

	TVariableStack::PushVar(&TVariables::TRagebot::bEnabled);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::HitboxList,CFG_HITBOXMAX);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::MultipointList, CFG_HITBOXMAX);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::ForceSafePointList, CFG_HITBOXMAX);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::Headscale);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::Bodyscale);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::PreferBaim);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::Hitchance);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::MinimumDamage);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::AutoFire);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::AutoPenetration);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::SilentAim);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::Fov);

	TVariableStack::SetSection(TSTRING("sab_2"));

	TVarUtil::PushConfigVar(&TVariables::TRagebot::Autostop, ASC_MAX);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::ForceSafePointConditions, SPC_MAX);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::AccuracyBoost);

	TVariableStack::SetSection(TSTRING("sab_3"));
	TVarUtil::PushConfigVar(&TVariables::TRagebot::Doubletap);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::Hideshots);
	TVarUtil::PushConfigVar(&TVariables::TRagebot::DoubletapAutostop, ASC_MAX);

	for (int Current = 0; Current < 7; Current++)
	{
		if (!TUtil::FileDoesExist(TSTRING("config_")+std::to_string(Current)+TSTRING(".cfg")))
			TConfigManager::SaveConfig(TSTRING("config_") + std::to_string(Current));
	}

}

void TVariables::UpdateKeybindStates() {
	TVariables::UpdateKeyState(&TVariables::TVisuals::TLocalPlayer::ThirdPerson);
	TVariables::UpdateKeyState(&TVariables::TAntiAim::TFakeAngles::Fake);
	TVariables::UpdateKeyState(&TVariables::TAntiAim::TOthers::Slowmotion);
	TVariables::UpdateKeyState(&TVariables::TAntiAim::TOthers::Fakeduck);

}

void TVariables::UpdateKeyState(TConfigVarKeybind* Keybind) 
{
	static auto IsButtonDown = [](int ButtonCode) -> bool
	{

		if (HIWORD(GetKeyState(ButtonCode)))
			return true;

		return TInterfaces::InputSystem->IsButtonDown((EButtonCode)ButtonCode);
	};

	bool bIsDown = IsButtonDown(Keybind->iValue);

	if (!Keybind->bEnabled)
	{
		Keybind->KeyState.bState = false;

	}
	else {
		switch (Keybind->iStyle)
		{
		case 0:
		{
			Keybind->KeyState.bState = true;
			break;
		}
		case 1:
		{
			if (bIsDown)
				Keybind->KeyState.bState = true;
			else
				Keybind->KeyState.bState = false;
			break;
		}
		case 2:
		{
			if (!Keybind->KeyState.bHolding && bIsDown)
			{
				Keybind->KeyState.bState = !Keybind->KeyState.bState;
				Keybind->KeyState.bHolding = true;
			}
			else if (Keybind->KeyState.bHolding && !bIsDown)
				Keybind->KeyState.bHolding = false;

			break;
		}
		case 3:
		{
			if (bIsDown)
				Keybind->KeyState.bState = false;
			else
				Keybind->KeyState.bState = true;

			break;
		}
		default:
			Keybind->KeyState.bState = false;
		}
	}


}

Vector4D TVariables::ColorToVector(float* flColor,bool bDormant, float flCustomAlpha) {
	return Vector4D(flColor[0] * 255.f, flColor[1] * 255.f, flColor[2] * 255.f, flCustomAlpha >= 0 ? ((flColor[3] * flCustomAlpha) * 255.f) : ((flColor[3] * 255.f) / (bDormant ? 2.f : 1.f)));
}

Color TVariables::ConvertToColor(float* flColor)
{
	return Color((int)(flColor[0] * 255.f), (int)(flColor[1] * 255.f), (int)(flColor[2] * 255.f), (int)(flColor[3] * 255.f));
}

void TConfigManager::TVariableStack::SetSection(std::string szSection) {
	szCurrentSection = szSection;
	
}

void TConfigManager::TVarUtil::PushConfigVar(TConfigVarBool* Var) {
	TConfigManager::TVariableStack::PushVar(&Var->bEnabled);
	TVariableStack::PushVar(&Var->Color.flColors[0]);
	TVariableStack::PushVar(&Var->Color.flColors[1]);
	TVariableStack::PushVar(&Var->Color.flColors[2]);
	TVariableStack::PushVar(&Var->Color.flColors[3]);
}

void TConfigManager::TVarUtil::PushConfigVar(TConfigVarFloat* Var) {
	TConfigManager::TVariableStack::PushVar(&Var->bEnabled);
	TConfigManager::TVariableStack::PushVar(&Var->flValue);
	TVariableStack::PushVar(&Var->Color.flColors[0]);
	TVariableStack::PushVar(&Var->Color.flColors[1]);
	TVariableStack::PushVar(&Var->Color.flColors[2]);
	TVariableStack::PushVar(&Var->Color.flColors[3]);
}

void TConfigManager::TVarUtil::PushConfigVar(TConfigVarKeybind* Var) {
	TConfigManager::TVariableStack::PushVar(&Var->bEnabled);
	TConfigManager::TVariableStack::PushVar(&Var->iValue);
	TConfigManager::TVariableStack::PushVar(&Var->iStyle);
}

void TConfigManager::TVarUtil::PushConfigVar(TConfigVarInt* Var) {
	TConfigManager::TVariableStack::PushVar(&Var->bEnabled);
	TConfigManager::TVariableStack::PushVar(&Var->Value);
	TVariableStack::PushVar(&Var->Color.flColors[0]);
	TVariableStack::PushVar(&Var->Color.flColors[1]);
	TVariableStack::PushVar(&Var->Color.flColors[2]);
	TVariableStack::PushVar(&Var->Color.flColors[3]);
}

void TConfigManager::TVarUtil::PushConfigVar(TConfigVarMap* Var, int SizeOfMap) {
	TConfigManager::TVariableStack::PushVar(&Var->bEnabled);
	TConfigManager::TVariableStack::PushMap(Var->mapValues, SizeOfMap);
}

void TConfigManager::TVarUtil::PushConfigVar(TConfigVarSelectable* Var) {
	TConfigManager::TVariableStack::PushVar(&Var->bEnabled);
	TVariableStack::PushVar(&Var->Color.flColors[0]);
	TVariableStack::PushVar(&Var->Color.flColors[1]);
	TVariableStack::PushVar(&Var->Color.flColors[2]);
	TVariableStack::PushVar(&Var->Color.flColors[3]);
	TConfigManager::TVariableStack::PushVar(&Var->iSelectable);
}


void TConfigManager::ResetConfig() {

	for (auto& b : TVariableStack::vecBooleans) 
		*b.pPointer = b.pDefaultValue;
	

	for (auto& i : TVariableStack::vecIntegers)
		*i.pPointer = i.pDefaultValue;

	for (auto& f : TVariableStack::vecFloats) 
		*f.pPointer = f.pDefaultValue;
	
}

void TConfigManager::SaveConfig(std::string path) {

	std::string szFileName = path;

	path = path + ".cfg";

	json retData;

	auto sanityChecks = [&](std::string category) -> void {
		if (retData.find(category) == retData.end())
			retData[category] = json();
	};

	for (auto& b : TVariableStack::vecBooleans) {

		sanityChecks(b.szCategory);
		retData[b.szCategory][b.szName] = *b.pPointer;
	}
	for (auto& i : TVariableStack::vecIntegers) {

		sanityChecks(i.szCategory);
		retData[i.szCategory][i.szName] = *i.pPointer;
	}
	for (auto& f : TVariableStack::vecFloats) {

		sanityChecks(f.szCategory);
		retData[f.szCategory][f.szName] = *f.pPointer;
	}

	std::ofstream reader(path);
	reader.clear();
	reader << std::setw(4) << retData << std::endl;
	reader.close();
}

void TConfigManager::ReadConfig(std::string path) {

	std::string szFileName = path;

	path = path + TSTRING(".cfg");


	if (!TUtil::FileDoesExist(path))
		return;

	json data;

	std::ifstream reader(path);

	reader >> data;
	reader.close();

	auto sanityChecks = [](json& d, std::string& c, std::string& n) -> bool {
		return d.find(c) != d.end() && d[c].find(n) != d[c].end();
	};

	for (auto& b : TConfigManager::TVariableStack::vecBooleans) {

		if (sanityChecks(data, b.szCategory, b.szName))
			*b.pPointer = data[b.szCategory][b.szName].get<bool>();
	}

	for (auto& i : TConfigManager::TVariableStack::vecIntegers) {

		if (sanityChecks(data, i.szCategory, i.szName))
			*i.pPointer = data[i.szCategory][i.szName].get<int>();
	}

	for (auto& f : TConfigManager::TVariableStack::vecFloats) {

		if (sanityChecks(data, f.szCategory, f.szName))
			*f.pPointer = data[f.szCategory][f.szName].get<float>();
	}

}

void TConfigManager::TVariableStack::PushVar(bool* bPointer) {
	
	static int Counter = 0;
	std::string name = TSTRING("be_") + std::to_string(Counter);
	Counter++;

	for (auto& pItem : vecBooleans)
		if (pItem.szCategory == szCurrentSection && pItem.szName == name)
			return;

	vecBooleans.push_back({ bPointer, szCurrentSection, name, *bPointer });
}

void TConfigManager::TVariableStack::PushVar(int* pointer) {

	static int Counter = 0;
	std::string name = TSTRING("ie_") + std::to_string(Counter);
	Counter++;

	for (auto& pItem : vecIntegers)
		if (pItem.szCategory == szCurrentSection && pItem.szName == name)
			return;

	vecIntegers.push_back({ pointer, szCurrentSection, name, *pointer });
}

void TConfigManager::TVariableStack::PushVar(float* pointer) {

	static int Counter = 0;
	std::string name = TSTRING("fe_") + std::to_string(Counter);
	Counter++;

	for (auto& pItem : vecFloats)
		if (pItem.szCategory == szCurrentSection && pItem.szName == name)
			return;

	vecFloats.push_back({ pointer, szCurrentSection, name,*pointer });
}

void TConfigManager::TVariableStack::PushMap(std::unordered_map<int, bool>& pBuffer, int iSize) {

	for (int Current = 0; Current < iSize; Current++)
		PushVar(&pBuffer[Current]);

}
