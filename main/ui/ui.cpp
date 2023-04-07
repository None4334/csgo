#include "../../includes.h"
#include "ui.h"
#include "vars.h"

#include "../../dependencies/imgui/imgui.h"
#include "../../dependencies/imgui/backends/imgui_impl_dx9.h"
#include "../../dependencies/imgui/backends/imgui_impl_win32.h"
#include "../../dependencies/imgui/imgui_internal.h"
#include "../renderer.h"
#include "../features/skins/skins.h"

void TUserInterface::Setup(IDirect3DDevice9* pDevice) {

	if (TVariables::bMenuInitialized)
		return;

	ImGui::CreateContext();

	ImGui_ImplWin32_Init(IPT::hWindow);
	ImGui_ImplDX9_Init(pDevice);
	
	ImGuiIO& io = ImGui::GetIO();

	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha = 1.0f;
	style.WindowPadding = ImVec2(8, 8);
	style.WindowRounding = 0.0f;
	style.WindowBorderSize = 1.0f;
	style.WindowMinSize = ImVec2(32, 32);
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	style.ChildRounding = 0.0f;
	style.ChildBorderSize = 1.0f;
	style.PopupRounding = 0.0f;
	style.PopupBorderSize = 1.0f;
	style.FramePadding = ImVec2(4, 2);
	style.FrameRounding = 0.0f;
	style.FrameBorderSize = 1.0f;
	style.ItemSpacing = ImVec2(8, 4);
	style.ItemInnerSpacing = ImVec2(4, 4);
	style.IndentSpacing = 6.0f;
	style.ColumnsMinSpacing = 6.0f;
	style.ScrollbarSize = 6.0f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 0.0f;
	style.GrabRounding = 0.0f;
	style.AntiAliasedLines = false;
	style.AntiAliasedFill = false;
	style.AntiAliasedLinesUseTex = false;
	style.TabRounding = 0.0f;
	style.TabBorderSize = 1.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.SelectableTextAlign = ImVec2(0.0f, 0.5f);
	style.MouseCursorScale = 0.75f;


	TDirectXRender::TDirectFonts::InitFonts();

	TVariables::bMenuInitialized = true;

}

void TUserInterface::Shutdown() {
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
}


void TUserInterface::Render(IDirect3DDevice9* pDevice) {

	TDirectXRender::OnDrawList(ImGui::GetBackgroundDrawList());
	
	static ImColor defMenu = ImColor(176, 240, 38, 200);
	static ImColor defElementAccent = ImColor(155, 155, 155, 255);

	if (TVariables::TMisc::MenuColor.bEnabled)
	{
		ImGui::MenuAccent = ImColor(TVariables::TMisc::MenuColor.Color.flColors[0],
			TVariables::TMisc::MenuColor.Color.flColors[1],
			TVariables::TMisc::MenuColor.Color.flColors[2], 
			TVariables::TMisc::MenuColor.Color.flColors[3]);
	}
	else
		ImGui::MenuAccent = defMenu;


	if (TVariables::TMisc::ElementsColor.bEnabled)
	{
		ImGui::ElementAccent = ImColor(TVariables::TMisc::ElementsColor.Color.flColors[0],
			TVariables::TMisc::ElementsColor.Color.flColors[1],
			TVariables::TMisc::ElementsColor.Color.flColors[2],
			TVariables::TMisc::ElementsColor.Color.flColors[3]);
	}
	else
		ImGui::ElementAccent = defElementAccent;


	if (!TVariables::bMenuOpen)
		return;

	ImVec2 vecScreenSize = ImVec2(1920, 1080);

	static int CurrentTab = 1;
	ImGui::SetNextWindowPos(ImVec2(vecScreenSize.x * 0.5f, vecScreenSize.y * 0.5f), ImGuiCond_Once, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(730, 527), ImGuiCond_Always);

	ImGui::Begin(TSTRING("Ke32"),nullptr, ImGuiWindowFlags_NoTitleBar| ImGuiWindowFlags_NoResize | ImGuiWindowFlags_MenuRenderStart | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse);
	{
		ImGui::SetCursorPos(ImVec2(-60, 60));
		ImGui::CheckboxSetValueOnClick(TSTRING("RB"), &CurrentTab, 0);

		ImGui::SetCursorPos(ImVec2(-60, 60 + 35));
		ImGui::CheckboxSetValueOnClick(TSTRING("AA"), &CurrentTab, 1);

		ImGui::SetCursorPos(ImVec2(-60, 60 + 35 + 35));
		ImGui::CheckboxSetValueOnClick(TSTRING("VS"), &CurrentTab, 2);

		ImGui::SetCursorPos(ImVec2(-60, 60 + 35 + 35 + 35));
		ImGui::CheckboxSetValueOnClick(TSTRING("MD"), &CurrentTab, 3);

		ImGui::SetCursorPos(ImVec2(-60, 60 + 35 + 35 + 35 + 35));
		ImGui::CheckboxSetValueOnClick(TSTRING("SK"), &CurrentTab, 4);

		ImGui::SetCursorPos(ImVec2(-60, 60 + 35 + 35 + 35 + 35 + 35));
		ImGui::CheckboxSetValueOnClick(TSTRING("MS"), &CurrentTab, 5);

		ImGui::SetCursorPos(ImVec2(-60, 60 + 35 + 35 + 35 + 35 + 35 + 35));
		ImGui::CheckboxSetValueOnClick(TSTRING("CG"), &CurrentTab, 6);

		static int iSelectedConfig = 0;

		ImGui::SetCursorPos(ImVec2(260, -4));
		if (ImGui::Button(TSTRING("Load "))) {
			TConfigManager::ReadConfig(TSTRING("config_") + std::to_string(iSelectedConfig));
		}

		ImGui::SetCursorPos(ImVec2(350, -4));
		if (ImGui::Button(TSTRING("Save "))) {
			TConfigManager::SaveConfig(TSTRING("config_") + std::to_string(iSelectedConfig));
		}

		ImGui::SetCursorPos(ImVec2(442, -4));
		if (ImGui::Button(TSTRING("Reset "))) {
			TConfigManager::ResetConfig();
		}

		ImGui::SetCursorPos(ImVec2(160, 29));
		ImGui::SetNextWindowSize(ImVec2(120, 180), ImGuiCond_Always);
		ImGui::SingleSelect(TSTRING("##Config"), &iSelectedConfig, { TSTRING("rage"),TSTRING("legit"),TSTRING("semi-rage"),TSTRING("alt rage"),TSTRING("alt legit"),TSTRING("alt semi-rage"),TSTRING("other") }, 120);
		
		static const std::vector<const char*> vecHitboxNameList = { TSTRING("head"),TSTRING("chest"),TSTRING("stomach"),TSTRING("arms"),TSTRING("legs"),TSTRING("feet") };
		static const std::vector<const char*> vecMultipointHitboxNameList = { TSTRING("head"),TSTRING("chest"),TSTRING("stomach") };
		static const std::vector<const char*> vecAutostopConditions = { TSTRING("early"),TSTRING("between shots"),TSTRING("in air"),TSTRING("slowmotion") };
		static const std::vector<const char*> vecSafepointConditions = { TSTRING("standing"),TSTRING("moving"),TSTRING("in air") ,TSTRING("crouching") };

		if (CurrentTab == 0)
		{
			ImGui::SetCursorPos(ImVec2(80, 100));
			ImGui::BeginChild(TSTRING("Target"), ImVec2(300, 400), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);

			ImGui::Checkbox(TSTRING("Enabled"), &TVariables::TRagebot::bEnabled);
			ImGui::Spacing();
			ImGui::MultiSelect(TSTRING("Hitbox"), &TVariables::TRagebot::HitboxList.mapValues, vecHitboxNameList);
			ImGui::MultiSelect(TSTRING("Multipoint"), &TVariables::TRagebot::MultipointList.mapValues, vecMultipointHitboxNameList);
			ImGui::MultiSelect(TSTRING("Safepoint hitbox"), &TVariables::TRagebot::ForceSafePointList.mapValues, vecHitboxNameList);
			ImGui::SliderInt(TSTRING("Head scale"), &TVariables::TRagebot::Headscale.Value, 0, 100);
			ImGui::SliderInt(TSTRING("Body scale"), &TVariables::TRagebot::Bodyscale.Value, 0, 100);
			ImGui::Checkbox(TSTRING("Prefer baim"), &TVariables::TRagebot::PreferBaim.bEnabled);
			ImGui::SliderInt(TSTRING("Hitchance"), &TVariables::TRagebot::Hitchance.Value, 0, 100);
			ImGui::SliderInt(TSTRING("Minimum damage"), &TVariables::TRagebot::MinimumDamage.Value, 1, 130);
			ImGui::Checkbox(TSTRING("Autofire"), &TVariables::TRagebot::AutoFire.bEnabled);
			ImGui::Checkbox(TSTRING("Auto penetration"), &TVariables::TRagebot::AutoPenetration.bEnabled);
			ImGui::Checkbox(TSTRING("Silent aim"), &TVariables::TRagebot::SilentAim.bEnabled);
			ImGui::SliderInt(TSTRING("Field of view"), &TVariables::TRagebot::Fov.Value, 1, 180);


			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(80 + 300 + 20, 100));
			ImGui::BeginChild(TSTRING("Accuracy"), ImVec2(300, 180), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);

			ImGui::Checkbox(TSTRING("Autostop"), &TVariables::TRagebot::Autostop.bEnabled);
			ImGui::MultiSelect(TSTRING("##AutostopCond"), &TVariables::TRagebot::Autostop.mapValues, vecAutostopConditions);
			ImGui::MultiSelect(TSTRING("Force safepoint"), &TVariables::TRagebot::ForceSafePointConditions.mapValues, vecSafepointConditions);
			ImGui::Checkbox(TSTRING("Accuracy boost"), &TVariables::TRagebot::AccuracyBoost.bEnabled);

			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(80 + 300 + 20, 100 + 180 + 20 + 20));
			ImGui::BeginChild(TSTRING("Exploits"), ImVec2(300, 180), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);

			ImGui::Checkbox(TSTRING("Hideshots"), &TVariables::TRagebot::Hideshots.bEnabled);
			ImGui::Keybind(TSTRING("##HideshotsKey"), &TVariables::TRagebot::Hideshots.iValue, &TVariables::TRagebot::Hideshots.iStyle);
			
			ImGui::Checkbox(TSTRING("Doubletap"), &TVariables::TRagebot::Doubletap.bEnabled);
			ImGui::Keybind(TSTRING("##DoubletapKey"), &TVariables::TRagebot::Doubletap.iValue, &TVariables::TRagebot::Doubletap.iStyle);

			ImGui::MultiSelect(TSTRING("##DoubletapAutostop"), &TVariables::TRagebot::DoubletapAutostop.mapValues, vecAutostopConditions);
			ImGui::EndChild();


		}

		if (CurrentTab == 1)
		{
			ImGui::SetCursorPos(ImVec2(80, 100));
			ImGui::BeginChild(TSTRING("Fake angles"), ImVec2(300, 400), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);
			
			ImGui::Checkbox(TSTRING("Enabled"), &TVariables::TAntiAim::TFakeAngles::bEnabled);
			ImGui::Spacing();
			ImGui::SingleSelect(TSTRING("Pitch mode"), &TVariables::TAntiAim::TFakeAngles::iPitchMode, { TSTRING("off"),TSTRING("down"),TSTRING("zero"),TSTRING("up") });
			ImGui::SingleSelect(TSTRING("Yaw mode"), &TVariables::TAntiAim::TFakeAngles::iYawMode, { TSTRING("off"),TSTRING("default"),TSTRING("default at targets"),TSTRING("180"),TSTRING("at targets") });
			ImGui::SliderInt(TSTRING("##Yawvalue"), &TVariables::TAntiAim::TFakeAngles::iYawValue, -180, 180);
			ImGui::SingleSelect(TSTRING("Yaw jitter"), &TVariables::TAntiAim::TFakeAngles::iYawJitterMode, { TSTRING("off"),TSTRING("offset"),TSTRING("center") });
			ImGui::SliderInt(TSTRING("##Yawjittervalue"), &TVariables::TAntiAim::TFakeAngles::iYawJitterValue, -180, 180);
			ImGui::Checkbox(TSTRING("Fake mode"), &TVariables::TAntiAim::TFakeAngles::Fake.bEnabled);
			ImGui::Keybind(TSTRING("##FakeInverter"), &TVariables::TAntiAim::TFakeAngles::Fake.iValue, &TVariables::TAntiAim::TFakeAngles::Fake.iStyle);

			ImGui::SingleSelect(TSTRING("##Fakemode"), &TVariables::TAntiAim::TFakeAngles::iFakeMode, { TSTRING("off"),TSTRING("movement"),TSTRING("no movement") });
			ImGui::SliderInt(TSTRING("##Fakevalue"), &TVariables::TAntiAim::TFakeAngles::iFakeValue, 0, 60);
			ImGui::SliderInt(TSTRING("Roll"), &TVariables::TAntiAim::TFakeAngles::iRollValue, -50.f, 50.f);
			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(80 + 300 + 20, 100));
			ImGui::BeginChild(TSTRING("Fakelag"), ImVec2(300, 180), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);

			ImGui::Checkbox(TSTRING("Enabled"), &TVariables::TAntiAim::TFakelag::bFakelagEnabled);
			//ImGui::SliderInt(TSTRING("Variation"), &TVariables::TAntiAim::TFakelag::iFakelagVariance, 0, 180);
			ImGui::SliderInt(TSTRING("Maximum lag"), &TVariables::TAntiAim::TFakelag::iFakelagValue, 0, 14);

			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(80 + 300 + 20, 100 + 180 + 20 + 20));
			ImGui::BeginChild(TSTRING("Other"), ImVec2(300, 180), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);

			ImGui::Checkbox(TSTRING("Fakeduck"), &TVariables::TAntiAim::TOthers::Fakeduck.bEnabled);
			ImGui::Keybind(TSTRING("##Fakeduckkey"), &TVariables::TAntiAim::TOthers::Fakeduck.iValue, &TVariables::TAntiAim::TOthers::Fakeduck.iStyle);
			ImGui::Checkbox(TSTRING("Slowmotion"), &TVariables::TAntiAim::TOthers::Slowmotion.bEnabled);
			ImGui::Keybind(TSTRING("##SlowmoKey"), &TVariables::TAntiAim::TOthers::Slowmotion.iValue, &TVariables::TAntiAim::TOthers::Slowmotion.iStyle);

			ImGui::SliderInt(TSTRING("##SlowmoSpeed"), &TVariables::TAntiAim::TOthers::iSlowmotionSpeed, 0, 130);

			ImGui::EndChild();


		}

		if (CurrentTab == 2) {
			ImGui::SetCursorPos(ImVec2(80, 100));
			ImGui::BeginChild(TSTRING("Player"), ImVec2(300, 180), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);
			
			ImGui::Checkbox(TSTRING("Enabled"), &TVariables::TVisuals::bEnabled);

			ImGui::Checkbox(TSTRING("Bounding box"), &TVariables::TVisuals::TPlayer::BoundingBox.bEnabled);
			ImGui::ColorPicker(TSTRING("##BBoxCol"), TVariables::TVisuals::TPlayer::BoundingBox.Color.flColors,true);
			
			ImGui::Checkbox(TSTRING("Name"), &TVariables::TVisuals::TPlayer::Name.bEnabled);
			ImGui::ColorPicker(TSTRING("##NameCol"), TVariables::TVisuals::TPlayer::Name.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Health bar"), &TVariables::TVisuals::TPlayer::HealthBar.bEnabled);
			ImGui::ColorPicker(TSTRING("##HlthBarCol"), TVariables::TVisuals::TPlayer::HealthBar.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Health text"), &TVariables::TVisuals::TPlayer::HealthText.bEnabled);
			ImGui::ColorPicker(TSTRING("##HlthTxtCol"), TVariables::TVisuals::TPlayer::HealthText.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Ammo bar"), &TVariables::TVisuals::TPlayer::AmmoBar.bEnabled);
			ImGui::ColorPicker(TSTRING("##AmmoBarCol"), TVariables::TVisuals::TPlayer::AmmoBar.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Skeleton"), &TVariables::TVisuals::TPlayer::Skeleton.bEnabled);
			ImGui::ColorPicker(TSTRING("##SklCol"), TVariables::TVisuals::TPlayer::Skeleton.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Weapon text"), &TVariables::TVisuals::TPlayer::WeaponText.bEnabled);
			ImGui::ColorPicker(TSTRING("##WpnTxtCol"), TVariables::TVisuals::TPlayer::WeaponText.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Weapon icon"), &TVariables::TVisuals::TPlayer::WeaponIcon.bEnabled);
			ImGui::ColorPicker(TSTRING("##WpnIconCol"), TVariables::TVisuals::TPlayer::WeaponIcon.Color.flColors, true);
			
			ImGui::Checkbox(TSTRING("Flags"), &TVariables::TVisuals::TPlayer::Flags.bEnabled);
			ImGui::ColorPicker(TSTRING("##FlagsCol"), TVariables::TVisuals::TPlayer::Flags.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Dormant"), &TVariables::TVisuals::TPlayer::Dormant.bEnabled);

			ImGui::Checkbox(TSTRING("Player glow"), &TVariables::TVisuals::TPlayer::Glow.bEnabled);
			ImGui::ColorPicker(TSTRING("##PlayerglowC"), TVariables::TVisuals::TPlayer::Glow.Color.flColors, true);
			ImGui::SingleSelect(TSTRING("##PlayerglowMode"), &TVariables::TVisuals::TPlayer::Glow.iSelectable, { TSTRING("Default"),TSTRING("Overlay"),TSTRING("Outline"),TSTRING("Outline Pulse") });

			ImGui::EndChild();


			ImGui::SetCursorPos(ImVec2(80, 100 + 180 + 20 + 20));
			ImGui::BeginChild(TSTRING("World"), ImVec2(300, 180), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);

			ImGui::Checkbox(TSTRING("Nightmode"), &TVariables::TVisuals::TOthers::Nightmode.bEnabled);
			ImGui::DoubleColPicker(TSTRING("##NightmodeCol"), TVariables::TVisuals::TOthers::Nightmode.Color.flColors, TVariables::TVisuals::TOthers::Propscol.Color.flColors);

			ImGui::Checkbox(TSTRING("Fullbright"), &TVariables::TVisuals::TOthers::Fullbright.bEnabled);

			ImGui::Checkbox(TSTRING("Server impacts"), &TVariables::TVisuals::TOthers::ServerImpacts.bEnabled);
			ImGui::ColorPicker(TSTRING("##ServerImpactsCol"), TVariables::TVisuals::TOthers::ServerImpacts.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Client impacts"), &TVariables::TVisuals::TOthers::ClientImpacts.bEnabled);
			ImGui::ColorPicker(TSTRING("##ClientImpactsCol"), TVariables::TVisuals::TOthers::ClientImpacts.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Bullet tracers"), &TVariables::TVisuals::TOthers::BulletTracers.bEnabled);
			ImGui::ColorPicker(TSTRING("##TracersCol"), TVariables::TVisuals::TOthers::BulletTracers.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Hitmarker"), &TVariables::TVisuals::TOthers::HitMarker.bEnabled);
			ImGui::ColorPicker(TSTRING("##Hitmarkerx"), TVariables::TVisuals::TOthers::HitMarker.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Damage marker"), &TVariables::TVisuals::TOthers::DamageMarker.bEnabled);
			ImGui::ColorPicker(TSTRING("##Dmgmarkerx"), TVariables::TVisuals::TOthers::DamageMarker.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Hit sound"), &TVariables::TVisuals::TOthers::HitSound.bEnabled);

			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(80+ 300 + 20, 100));
			ImGui::BeginChild(TSTRING("Local"), ImVec2(300, 180), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);

			ImGui::Checkbox(TSTRING("Thirdperson"), &TVariables::TVisuals::TLocalPlayer::ThirdPerson.bEnabled);
			ImGui::Keybind(TSTRING("##ThirdpersonK"), &TVariables::TVisuals::TLocalPlayer::ThirdPerson.iValue, &TVariables::TVisuals::TLocalPlayer::ThirdPerson.iStyle);
			
			if (TVariables::TVisuals::TLocalPlayer::ThirdPerson.bEnabled)
				ImGui::SliderInt(TSTRING("##ThirdpersonDist"), &TVariables::TVisuals::TLocalPlayer::ThirdPersonDistance.Value, 0, 180);

			ImGui::Checkbox(TSTRING("While spectating"), &TVariables::TVisuals::TLocalPlayer::ThirdPersonWhileSpecating.bEnabled);

			ImGui::Checkbox(TSTRING("Local glow"), &TVariables::TVisuals::TLocalPlayer::Glow.bEnabled);
			ImGui::ColorPicker(TSTRING("##Locglow"), TVariables::TVisuals::TLocalPlayer::Glow.Color.flColors, true);
			ImGui::SingleSelect(TSTRING("##Locglowmode"), &TVariables::TVisuals::TLocalPlayer::Glow.iSelectable, { TSTRING("Default"),TSTRING("Overlay"),TSTRING("Outline"),TSTRING("Outline Pulse") });

			ImGui::Checkbox(TSTRING("Nade Prediction"), &TVariables::TVisuals::TLocalPlayer::NadePrediction.bEnabled);
			ImGui::DoubleColPicker(TSTRING("##GrenadePredSec"), TVariables::TVisuals::TLocalPlayer::NadePrediction.Color.flColors,TVariables::TVisuals::TLocalPlayer::NadePredictionSecondColor.Color.flColors);

			ImGui::Checkbox(TSTRING("Nade Warning"), &TVariables::TVisuals::TLocalPlayer::NadeWarning.bEnabled);
			ImGui::DoubleColPicker(TSTRING("##GrenadeWarnCol"), TVariables::TVisuals::TLocalPlayer::NadeWarning.Color.flColors, TVariables::TVisuals::TLocalPlayer::NadeWarningSecondColor.Color.flColors);
			
			ImGui::Checkbox(TSTRING("View fov"), &TVariables::TVisuals::TLocalPlayer::ViewFov.bEnabled);
			if (TVariables::TVisuals::TLocalPlayer::ViewFov.bEnabled)
				ImGui::SliderInt(TSTRING("##ViewFovS"), &TVariables::TVisuals::TLocalPlayer::ViewFov.Value, -88, 88);

			ImGui::Checkbox(TSTRING("Viewmodel fov"), &TVariables::TVisuals::TLocalPlayer::ViewModelFov.bEnabled);
			if (TVariables::TVisuals::TLocalPlayer::ViewModelFov.bEnabled)
				ImGui::SliderInt(TSTRING("##ViewmodelFovS"), &TVariables::TVisuals::TLocalPlayer::ViewModelFov.Value, -88, 88);

			ImGui::Checkbox(TSTRING("Aspect ratio"), &TVariables::TVisuals::TLocalPlayer::AspectRatio.bEnabled);
			if (TVariables::TVisuals::TLocalPlayer::AspectRatio.bEnabled)
				ImGui::SliderInt(TSTRING("##AspectRatS"), &TVariables::TVisuals::TLocalPlayer::AspectRatio.Value, 1, 200);

			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(80 + 300 + 20, 100 + 180 + 20 + 20));
			ImGui::BeginChild(TSTRING("Other"), ImVec2(300, 180), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);

			ImGui::MultiSelect(TSTRING("Removals"), &TVariables::TVisuals::TOthers::Removals.mapValues, { TSTRING("scope"),TSTRING("scope zoom")
	         ,TSTRING("smoke"),TSTRING("flash effects"),TSTRING("visual recoil"),TSTRING("fog"),
	         TSTRING("shadows"),TSTRING("landing bob"),TSTRING("post processing"),TSTRING("scope sens modifier")});

			ImGui::Checkbox(TSTRING("Nade info"), &TVariables::TVisuals::TOthers::NadeInfo.bEnabled);
			ImGui::DoubleColPicker(TSTRING("##NadeInfCol"), TVariables::TVisuals::TOthers::NadeInfo.Color.flColors, TVariables::TVisuals::TOthers::NadeInfoRadius.Color.flColors);

			ImGui::Checkbox(TSTRING("Bomb info"), &TVariables::TVisuals::TOthers::BombInfo.bEnabled);
			ImGui::DoubleColPicker(TSTRING("##BombInfCol"), TVariables::TVisuals::TOthers::BombInfo.Color.flColors, TVariables::TVisuals::TOthers::BarColor.Color.flColors);

			ImGui::Checkbox(TSTRING("Dropped text"), &TVariables::TVisuals::TOthers::DroppedWeaponText.bEnabled);
			ImGui::ColorPicker(TSTRING("##Droppedweaptext"), TVariables::TVisuals::TOthers::DroppedWeaponText.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Dropped icon"), &TVariables::TVisuals::TOthers::DroppedWeaponIcon.bEnabled);
			ImGui::ColorPicker(TSTRING("##Droppedweapicon"), TVariables::TVisuals::TOthers::DroppedWeaponIcon.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Dropped ammo"), &TVariables::TVisuals::TOthers::DroppedWeaponAmmo.bEnabled);
			ImGui::ColorPicker(TSTRING("##Droppedweapammo"), TVariables::TVisuals::TOthers::DroppedWeaponAmmo.Color.flColors, true);

			ImGui::EndChild();
			
		}

		if (CurrentTab == 3) {

			static const std::vector<const char*> vecChams = { TSTRING("none"), TSTRING("default"), TSTRING("flat"), TSTRING("pulse"), TSTRING("metallic") };

			ImGui::SetCursorPos(ImVec2(80, 100));
			ImGui::BeginChild(TSTRING("General"), ImVec2(300, 180), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);

			ImGui::Checkbox(TSTRING("Enabled"), &TVariables::TModels::TGeneral::bEnabled);


			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(80, 100 + 180 + 20 + 20));
			ImGui::BeginChild(TSTRING("Team"), ImVec2(300, 180), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);

			ImGui::Spacing();

			ImGui::SingleSelect(TSTRING("Visible"), &TVariables::TModels::TTeam::ModelVisible.Value, vecChams, 0, 4);
			ImGui::ColorPicker(TSTRING("##mdlvis"), TVariables::TModels::TTeam::ModelVisible.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Visible overlay"), &TVariables::TModels::TTeam::ModelOverlayVisible.bEnabled);
			ImGui::ColorPicker(TSTRING("##mdloverlay"), TVariables::TModels::TTeam::ModelOverlayVisible.Color.flColors, true);
			ImGui::Spacing();

			ImGui::SingleSelect(TSTRING("Invisible"), &TVariables::TModels::TTeam::ModelHidden.Value, vecChams, 0, 4);
			ImGui::ColorPicker(TSTRING("##mdlinvvis"), TVariables::TModels::TTeam::ModelHidden.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Invisible overlay"), &TVariables::TModels::TTeam::ModelOverlayHidden.bEnabled);
			ImGui::ColorPicker(TSTRING("##mdlinvoverlay"), TVariables::TModels::TTeam::ModelOverlayHidden.Color.flColors, true);
			ImGui::Spacing();

			ImGui::EndChild();


			ImGui::SetCursorPos(ImVec2(80 + 300 + 20, 100));
			ImGui::BeginChild(TSTRING("Local"), ImVec2(300, 180), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);
			{
				ImGui::Spacing();

				static const std::vector<const char*> vecStatements = { TSTRING("model"), TSTRING("fake"), TSTRING("fakelag"), TSTRING("weapon") , TSTRING("arms"), TSTRING("attachment") };

				static int iSelectedModel = 0;
				ImGui::SingleSelect(TSTRING("Selected type"), &iSelectedModel, vecStatements, 0, 4);
				ImGui::Spacing();

				switch (iSelectedModel)
				{
				case 0: {
					ImGui::SingleSelect(TSTRING("Model"), &TVariables::TModels::TLocal::Model.Value, vecChams, 0, 4);
					ImGui::ColorPicker(TSTRING("##ModelCol"), TVariables::TModels::TLocal::Model.Color.flColors, true);

					ImGui::Checkbox(TSTRING("Model overlay"), &TVariables::TModels::TLocal::Overlay.bEnabled);
					ImGui::ColorPicker(TSTRING("##ModelOverCol"), TVariables::TModels::TLocal::Overlay.Color.flColors, true);
					ImGui::Spacing();
					break;
				}
				case 1: {
					ImGui::SingleSelect(TSTRING("Fake"), &TVariables::TModels::TLocal::FakeModel.Value, vecChams, 0, 4);
					ImGui::ColorPicker(TSTRING("##FakeCol"), TVariables::TModels::TLocal::FakeModel.Color.flColors, true);

					ImGui::Checkbox(TSTRING("Fake overlay"), &TVariables::TModels::TLocal::FakeOverlay.bEnabled);
					ImGui::ColorPicker(TSTRING("##FakeOverCol"), TVariables::TModels::TLocal::FakeOverlay.Color.flColors, true);
					ImGui::Spacing();
					break;
				}
				case 2: {
					ImGui::SingleSelect(TSTRING("Fakelag"), &TVariables::TModels::TLocal::FakelagModel.Value, vecChams, 0, 4);
					ImGui::ColorPicker(TSTRING("##FakelagCol"), TVariables::TModels::TLocal::FakelagModel.Color.flColors, true);

					ImGui::Checkbox(TSTRING("Fakelag overlay"), &TVariables::TModels::TLocal::FakelagFakeOverlay.bEnabled);
					ImGui::ColorPicker(TSTRING("##FakelagOverCol"), TVariables::TModels::TLocal::FakelagFakeOverlay.Color.flColors, true);
					ImGui::Spacing();
					break;
				}
				case 3: {
					ImGui::SingleSelect(TSTRING("Weapon"), &TVariables::TModels::TLocal::Weapon.Value, vecChams, 0, 4);
					ImGui::ColorPicker(TSTRING("##WeapCol"), TVariables::TModels::TLocal::Weapon.Color.flColors, true);

					ImGui::Checkbox(TSTRING("Weapon overlay"), &TVariables::TModels::TLocal::WeaponOverlay.bEnabled);
					ImGui::ColorPicker(TSTRING("##WeapOverCol"), TVariables::TModels::TLocal::WeaponOverlay.Color.flColors, true);
					ImGui::Spacing();
					break;
				}
				case 4: {
					ImGui::SingleSelect(TSTRING("Arms Model"), &TVariables::TModels::TLocal::Arms.Value, vecChams, 0, 4);
					ImGui::ColorPicker(TSTRING("##ArmsCol"), TVariables::TModels::TLocal::Arms.Color.flColors, true);

					ImGui::Checkbox(TSTRING("Arms overlay"), &TVariables::TModels::TLocal::ArmsOverlay.bEnabled);
					ImGui::ColorPicker(TSTRING("##ArmsOverCol"), TVariables::TModels::TLocal::ArmsOverlay.Color.flColors, true);
					ImGui::Spacing();
					break;
				}
				case 5: {
					ImGui::SingleSelect(TSTRING("Attachment Model"), &TVariables::TModels::TLocal::Attachment.Value, vecChams, 0, 4);
					ImGui::ColorPicker(TSTRING("##AttachCol"), TVariables::TModels::TLocal::Attachment.Color.flColors, true);

					ImGui::Checkbox(TSTRING("Attachment overlay"), &TVariables::TModels::TLocal::AttachmentOverlay.bEnabled);
					ImGui::ColorPicker(TSTRING("##AttachmentOverCol"), TVariables::TModels::TLocal::AttachmentOverlay.Color.flColors, true);
					ImGui::Spacing();
					break;
				}
				default:
					break;
				}
			}
			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(80 + 300 + 20, 100 + 180 + 20 + 20));
			ImGui::BeginChild(TSTRING("Enemy"), ImVec2(300, 180), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);
			{
				ImGui::Spacing();

				static const std::vector<const char*> vecStatements = { TSTRING("model"), TSTRING("backtrack") };

				static int iSelectedModel = 0;
				ImGui::SingleSelect(TSTRING("Selected type"), &iSelectedModel, vecStatements, 0, 4);
				ImGui::Spacing();

				switch (iSelectedModel)
				{
				case 0: {

					ImGui::SingleSelect(TSTRING("Visible"), &TVariables::TModels::TEnemy::ModelVisible.Value, vecChams, 0, 4);
					ImGui::ColorPicker(TSTRING("##mdlvis"), TVariables::TModels::TEnemy::ModelVisible.Color.flColors, true);

					ImGui::Checkbox(TSTRING("Visible overlay"), &TVariables::TModels::TEnemy::ModelOverlayVisible.bEnabled);
					ImGui::ColorPicker(TSTRING("##mdloverlay"), TVariables::TModels::TEnemy::ModelOverlayVisible.Color.flColors, true);
					ImGui::Spacing();

					ImGui::SingleSelect(TSTRING("Invisible"), &TVariables::TModels::TEnemy::ModelHidden.Value, vecChams, 0, 4);
					ImGui::ColorPicker(TSTRING("##mdlinvvis"), TVariables::TModels::TEnemy::ModelHidden.Color.flColors, true);

					ImGui::Checkbox(TSTRING("Invisible overlay"), &TVariables::TModels::TEnemy::ModelOverlayHidden.bEnabled);
					ImGui::ColorPicker(TSTRING("##mdlinvoverlay"), TVariables::TModels::TEnemy::ModelOverlayHidden.Color.flColors, true);
					ImGui::Spacing();
					break;
				}
				case 1: {

					ImGui::SingleSelect(TSTRING("Visible"), &TVariables::TModels::TEnemy::BacktrackVisible.Value, vecChams, 0, 4);
					ImGui::ColorPicker(TSTRING("##mdlvis"), TVariables::TModels::TEnemy::BacktrackVisible.Color.flColors, true);

					ImGui::Checkbox(TSTRING("Visible overlay"), &TVariables::TModels::TEnemy::BacktrackOverlayVisible.bEnabled);
					ImGui::ColorPicker(TSTRING("##mdloverlay"), TVariables::TModels::TEnemy::BacktrackOverlayVisible.Color.flColors, true);
					ImGui::Spacing();

					ImGui::SingleSelect(TSTRING("Invisible"), &TVariables::TModels::TEnemy::BacktrackHidden.Value, vecChams, 0, 4);
					ImGui::ColorPicker(TSTRING("##mdlinvvis"), TVariables::TModels::TEnemy::BacktrackHidden.Color.flColors, true);

					ImGui::Checkbox(TSTRING("Invisible overlay"), &TVariables::TModels::TEnemy::BacktrackOverlayHidden.bEnabled);
					ImGui::ColorPicker(TSTRING("##mdlinvoverlay"), TVariables::TModels::TEnemy::BacktrackOverlayHidden.Color.flColors, true);
					ImGui::Spacing();
					break;
				}
				default:
					break;
				}
			}
			ImGui::EndChild();

		}


		if (CurrentTab == 4) {

			TSkinChanger::RenderMenu();

			/*
			ImGui::SetCursorPos(ImVec2(80, 100));
			ImGui::BeginChild(TSTRING("List"), ImVec2(300, 400), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);
			static bool bValue = false;
			ImGui::Checkbox("Enabled", &bValue);

			//ImGui::SingleSelect("Config", &iSelectedConfig, { "rage","legit","semi-rage","alt rage","alt legit","alt semi-rage","other" });

			//ImGui::SliderInt("Hitchance2", &Value, 0, 100);

			//ImGui::Button(TSTRING("Visuals"));

			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(80 + 300 + 20, 100));
			ImGui::BeginChild(TSTRING("Browser"), ImVec2(300, 400), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);



			//ImGui::SliderInt("Hitchance2", &Value, 0, 100);

			//ImGui::Button(TSTRING("Visuals"));

			ImGui::EndChild();
			*/
		}
	
		if (CurrentTab == 5) {

			ImGui::SetCursorPos(ImVec2(80, 100));
			ImGui::BeginChild(TSTRING("Misc"), ImVec2(300, 400), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);

			ImGui::Checkbox(TSTRING("Menu color"), &TVariables::TMisc::MenuColor.bEnabled);
			ImGui::ColorPicker(TSTRING("##Menucol"), TVariables::TMisc::MenuColor.Color.flColors, true);///
	
			ImGui::Checkbox(TSTRING("Element color"), &TVariables::TMisc::ElementsColor.bEnabled);
			ImGui::ColorPicker(TSTRING("##Elemcol"), TVariables::TMisc::ElementsColor.Color.flColors, true);//

			ImGui::Checkbox(TSTRING("Log shots"), &TVariables::TMisc::ShotLogger.bEnabled); 
			ImGui::Checkbox(TSTRING("Log purchase"), &TVariables::TMisc::BuyLogger.bEnabled);  //
			ImGui::Checkbox(TSTRING("Bunny hop"), &TVariables::TMisc::BunnyHop.bEnabled); //
			ImGui::Checkbox(TSTRING("Quick stop"), &TVariables::TMisc::QuickStop.bEnabled); //

			ImGui::Checkbox(TSTRING("Knife bot"), &TVariables::TMisc::AutomaticKnife.bEnabled); 
			ImGui::Checkbox(TSTRING("Zeus bot"), &TVariables::TMisc::AutomaticZeus.bEnabled); 
			ImGui::Checkbox(TSTRING("Infinite duck"), &TVariables::TMisc::InfiniteDuck.bEnabled); //

			ImGui::Checkbox(TSTRING("Autostrafer"), &TVariables::TMisc::AutoStrafer.bEnabled);
			ImGui::SingleSelect(TSTRING("##Strafemode"), &TVariables::TMisc::AutoStrafer.Value, {TSTRING("default"), TSTRING("wasd")}, 0, 4);

			ImGui::Checkbox(TSTRING("Auto peek"), &TVariables::TMisc::AutomaticPeek.bEnabled); 
			ImGui::ColorPicker(TSTRING("##Autopeekcol"), TVariables::TMisc::ElementsColor.Color.flColors, true);

			ImGui::Checkbox(TSTRING("Preserve killfeed"), &TVariables::TMisc::PreserveDeathNotice.bEnabled); //
			ImGui::Checkbox(TSTRING("Unlock inventory"), &TVariables::TMisc::UnlockInventory.bEnabled); //
			ImGui::Checkbox(TSTRING("Bypass sv_pure"), &TVariables::TMisc::ServerPureBypass.bEnabled); //
			ImGui::Checkbox(TSTRING("Auto accept"), &TVariables::TMisc::AutoAcceptMatchMaking.bEnabled); //
			ImGui::Checkbox(TSTRING("Anti untrusted"), &TVariables::TMisc::AntiUntrusted.bEnabled); //

			ImGui::EndChild();

			ImGui::SetCursorPos(ImVec2(80 + 300 + 20, 100));
			ImGui::BeginChild(TSTRING("Buy"), ImVec2(300, 400), true, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_ChildRenderStart);

			ImGui::Checkbox(TSTRING("Enabled"), &TVariables::TMisc::BuyBot.bEnabled);
			ImGui::Checkbox(TSTRING("Defuse kit"), &TVariables::TMisc::Defuser.bEnabled);
			ImGui::Checkbox(TSTRING("Taser"), &TVariables::TMisc::Taser.bEnabled);
			ImGui::Spacing();
			ImGui::SingleSelect(TSTRING("Primary weapon"), &TVariables::TMisc::PrimaryWeapon.Value, { TSTRING("none"),TSTRING("galilar"),TSTRING("famas"),TSTRING("ak47"),TSTRING("m4a1"),TSTRING("m4a1 silencer")
				,TSTRING("ssg08"),TSTRING("aug"),TSTRING("sg556"),TSTRING("awp"),TSTRING("scar20"),TSTRING("g3sg1"),TSTRING("nova"),TSTRING("xm1014"),TSTRING("mag7"),TSTRING("m249"),
				TSTRING("negev"),TSTRING("mac10"),TSTRING("mp9"),TSTRING("mp7"),TSTRING("ump45"),TSTRING("p90"),TSTRING("bizon")});
			ImGui::SingleSelect(TSTRING("Secondary weapon"), &TVariables::TMisc::SecondaryWeapon.Value, { TSTRING("none"), TSTRING("glock"),TSTRING("hkp2000"),TSTRING("usp"),TSTRING("elite"),TSTRING("p250"),TSTRING("tec9"),TSTRING("fn57"),TSTRING("deagle") });
			ImGui::SingleSelect(TSTRING("Armor type"), &TVariables::TMisc::ArmorType.Value, { TSTRING("none"),TSTRING("vest"),TSTRING("vest and helmet") });
			ImGui::MultiSelect(TSTRING("Throwables"), &TVariables::TMisc::MapGrenades.mapValues, { TSTRING("molotov"),TSTRING("incendiary grenade"),TSTRING("he grenade") ,TSTRING("smoke grenade") ,TSTRING("flashbang") ,TSTRING("decoy") });

			ImGui::EndChild();
		}
	}
	ImGui::End();



}