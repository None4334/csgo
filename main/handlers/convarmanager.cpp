#include "../../includes.h"
#include "convarmanager.h"


void TConvarManager::Restore() {
	varInterpolation.Restore();
	varViewModelX.Restore();
	varViewModelY.Restore();
	varViewModelZ.Restore();
	varCsmShadows.Restore();
	varFootContactShadows.Restore();
	varCsmStaticPropShadows.Restore();
	varCsmWorldShadows.Restore();
	varCsmViewmodelShadows.Restore();
	varCsmSpriteShadows.Restore();
	varCsmRopeShadows.Restore();
	varShadows.Restore();
	varFogMaxDensity.Restore();
	varFogOverride.Restore();
	varDrawSpecificStaticProp.Restore();
	varFogStart.Restore();
	varFogEnd.Restore();
}

float TConvarManager::GetLerp() {

	float flUpdateRate = std::clamp(varUpdateRate.pBase->GetFloat(), varMinUpdateRate.pBase->GetFloat(), varMaxUpdateRate.pBase->GetFloat());
	float flLerpRatio = std::clamp(varInterpRatio.pBase->GetFloat(), varClientMinInterpRatio.pBase->GetFloat(), varClientMaxInterpRatio.pBase->GetFloat());

	return std::clamp(flLerpRatio / flUpdateRate, varInterp.pBase->GetFloat(), 1.0f);
}

void TConvarManager::Initialize() {

	varInterp = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_interp")));
	varInterpRatio = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_interp_ratio")));
	varClientMinInterpRatio = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("sv_client_min_interp_ratio")));
	varClientMaxInterpRatio = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("sv_client_max_interp_ratio")));
	varUpdateRate = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_updaterate")));
    varMinUpdateRate = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("sv_minupdaterate")));
	varMaxUpdateRate = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("sv_maxupdaterate")));

	varWeaponNoSpread = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("weapon_accuracy_nospread")));
	varDamageScaleCtHead = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("mp_damage_scale_ct_head")));
	varDamageScaleTHead = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("mp_damage_scale_t_head")));
	varDamageScaleCTBody = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("mp_damage_scale_ct_body")));
	varDamageScaleTBody = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("mp_damage_scale_t_body")));

	varDamageReductionBullets = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("ff_damage_reduction_bullets")));
	varPredDoResetLatch = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_pred_doresetlatch")));
	varNetShowFragments = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("net_showfragments")));
	varServerCheats = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("sv_cheats")));
	varLagCompensation = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_lagcompensation")));
	varInterpolation = TFloatConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_interp")));
	varDrawSpecificStaticProp = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("r_drawspecificstaticprop")));
	var3Dsky = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("r_3dsky")));

	varGravity = TFloatConsoleVar(TInterfaces::Convars->FindVar(TSTRING("sv_gravity")));

	varFogOverride = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("fog_override")));
	varFogStart = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("fog_start")));
	varFogEnd = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("fog_end")));;
	varFogMaxDensity = TFloatConsoleVar(TInterfaces::Convars->FindVar(TSTRING("fog_maxdensity")));
	varFogColor = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("fog_color")));

	varViewModelX = TFloatConsoleVar(TInterfaces::Convars->FindVar(TSTRING("viewmodel_offset_x")));
	varViewModelY = TFloatConsoleVar(TInterfaces::Convars->FindVar(TSTRING("viewmodel_offset_y")));
	varViewModelZ = TFloatConsoleVar(TInterfaces::Convars->FindVar(TSTRING("viewmodel_offset_z")));
	
	varCompMinSpec = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("sv_competitive_minspec")));
	varMolotovMaxDetonationSlope = TFloatConsoleVar(TInterfaces::Convars->FindVar(TSTRING("weapon_molotov_maxdetonateslope")));

	varCsmShadows = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_csm_shadows")));
	varFootContactShadows = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_foot_contact_shadows")));
	varCsmStaticPropShadows = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_csm_static_prop_shadows")));
	varCsmWorldShadows = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_csm_world_shadows")));
	varCsmViewmodelShadows = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_csm_viewmodel_shadows")));
	varCsmSpriteShadows = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_csm_sprite_shadows")));
	varCsmRopeShadows = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_csm_rope_shadows")));
	varShadows = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("r_shadows")));
	varZoomSensitivityRatioMouse = TFloatConsoleVar(TInterfaces::Convars->FindVar(TSTRING("zoom_sensitivity_ratio_mouse")));  
	varTeammatesAreEnemies = TIntConsoleVar(TInterfaces::Convars->FindVar(TSTRING("mp_teammates_are_enemies")));
	varForwardSpeed = TFloatConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_forwardspeed"))); 
	varSideSpeed = TFloatConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_sidespeed")));  
	varUpSpeed = TFloatConsoleVar(TInterfaces::Convars->FindVar(TSTRING("cl_upspeed")));



}
