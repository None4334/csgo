
#include "../../includes.h"
#include "../handlers/localengine.h"
#include "../handlers/entityhandler.h"
#include "../gvars.h"
#include "../sdk.h"
#include "../math.h"
#include "../handlers/convarmanager.h"
#include "../ui/vars.h"
#include "../features/visuals.h"
#include "../renderer.h"
#include "nadepred.h"

void TNadePrediction::Reset() {
	m_vecStart = Vector3D{ };
	m_vecMove = Vector3D{ };
	m_vecVelocity = Vector3D{ };
	m_flVelocity = 0.f;
	m_flPower = 0.f;
	m_pPath.clear();
	m_pBounces.clear();
	pPredictionEntity = nullptr;
	pPredictionBaseWeapon = nullptr;
	pPredictionBaseWeaponData = nullptr;
}

void TNadePrediction::SetupAndRunPrediction(CBaseEntity* pPredEntity) {

	if (!TVariables::TVisuals::TLocalPlayer::NadePrediction.bEnabled)
		return;

    Reset();

	pPredictionEntity = pPredEntity;

	if (pPredictionEntity && pPredictionEntity->IsPlayer() && pPredictionEntity->IsAlive())
	{
		pPredictionEntity = pPredEntity;
		pPredictionBaseWeapon = TBaseEntity::GetWeapon(pPredictionEntity);

		if (pPredictionBaseWeapon)
			pPredictionBaseWeaponData = TBaseEntity::GetWeaponData(pPredictionBaseWeapon);

		if (pPredictionBaseWeaponData && pPredictionBaseWeapon)
		{
			RunPrediction();
		}
	}
}

void TNadePrediction::RunPrediction() {

	if (pPredictionEntity == TGlobalVars::pLocalEntity)
	{

		if (!TBaseEntity::IsGrenade(pPredictionBaseWeaponData))
			return;

		auto pBaseGrenade = (CBaseCSGrenade*)pPredictionBaseWeapon;

		m_iId = pPredictionBaseWeapon->GetItemDefinitionIndex();
		m_flPower = pBaseGrenade->GetThrowStrength();
		m_flVelocity = pPredictionBaseWeaponData->flThrowVelocity;
	}

	Simulate();
}

void TNadePrediction::Render(CBaseEntity* pBasePlayer) {
	
	if (!TVariables::TVisuals::TLocalPlayer::NadePrediction.bEnabled)
		return;

	pPredictionEntity = pBasePlayer;

	if (!pPredictionEntity || !pPredictionEntity->IsAlive())
		return;

	static CTraceFilterSimple filter{ };
	Trace_t	                   trace;

	if (!pPredictionEntity->IsAlive())
		return;

	if (m_pPath.size() < 3)
		return;

	filter.SetPassEntity(pPredictionEntity);


	auto vecPreviousPath = Vector3D();

	for (auto i = 2u; i < m_pPath.size(); ++i) {
		Vector3D vecCurrPath = m_pPath.at(i);

		if (!vecPreviousPath.IsZero() && !vecCurrPath.IsZero())
			TDirectXRender::BoxLine3D(vecPreviousPath, vecCurrPath, TVariables::ColorToVector(TVariables::TVisuals::TLocalPlayer::NadePredictionSecondColor.Color.flColors), 4.F);

		vecPreviousPath = vecCurrPath;
	}

	if (!TEntityListener::vecEntities.empty())
	{
		std::pair< float, CBaseEntity* >    target{ 0.f, nullptr };
		Vector4D vecColorDamage;

		for (auto CurrentEntity : TEntityListener::vecEntities) {

			if (CurrentEntity->nClass != (int)EClassIndex::CCSPlayer)
				continue;

			if (!CurrentEntity->pEntity->IsAlive())
				continue;

			Vector3D center = CurrentEntity->pEntity->WorldSpaceCenter();

			Vector3D delta = center - vecPreviousPath;

			if (m_iId == WEAPON_HEGRENADE) {

				if (delta.Length() > 350.f)
					continue;

				TInterfaces::EngineTrace->TraceRay(Ray_t(vecPreviousPath, center), MASK_SHOT, (CTraceFilter*)&filter, &trace);

				if (!trace.pHitEntity || trace.pHitEntity != CurrentEntity->pEntity)
					continue;

				float d = (delta.Length() - 25.f) / 140.f;
				float damage = 105.f * std::exp(-d * d);

				damage = TLocalEngine::TTraceSystem::ScaleDamage(CurrentEntity->pEntity, damage, 1.f, HITGROUP_CHEST);

				damage = min(damage, (CurrentEntity->pEntity->GetArmor() > 0) ? 57.f : 98.f);

				if (CurrentEntity->pEntity->GetHealth() < damage)
					vecColorDamage = Vector4D(50, 255, 50, 0xb4);
				else
					vecColorDamage = Vector4D(255, 255, 255, 0xb4);

				if (damage > target.first) {
					target.first = damage;
					target.second = CurrentEntity->pEntity;
				}
			}
		}

		if (target.second) {

			Vector2D vecScreen;

			if (TVisuals::TUtils::WorldToScreen(vecPreviousPath, vecScreen)) {
				std::string szData = std::to_string((int)target.first) + TSTRING(" dmg");
				TDirectXRender::Text(TDirectXRender::TDirectFonts::SmallestPixel, szData, Vector2D(vecScreen.x, vecScreen.y + 5), Vector4D(200, 200, 200, 200), false, true, Vector4D(24, 24, 24, 225));
			}
		}
	}



	if (!m_pBounces.empty() && pPredictionBaseWeapon) {

		Vector2D vecScreen;


		if (TVisuals::TUtils::WorldToScreen(m_pBounces.back(), vecScreen))
		{
			if (pPredictionBaseWeapon->GetItemDefinitionIndex() == WEAPON_HEGRENADE )
			    TDirectXRender::Circle3DFilled(m_pBounces.back(), 290, 80, TVariables::ColorToVector(TVariables::TVisuals::TLocalPlayer::NadePrediction.Color.flColors));
			else if (pPredictionBaseWeapon->GetItemDefinitionIndex() == WEAPON_MOLOTOV || pPredictionBaseWeapon->GetItemDefinitionIndex() == WEAPON_INCGRENADE)
				TDirectXRender::Circle3DFilled(m_pBounces.back(), 200, 80, TVariables::ColorToVector(TVariables::TVisuals::TLocalPlayer::NadePrediction.Color.flColors));
			else if (pPredictionBaseWeapon->GetItemDefinitionIndex() == WEAPON_SMOKEGRENADE)
				 TDirectXRender::Circle3DFilled(m_pBounces.back(), 180, 80, TVariables::ColorToVector(TVariables::TVisuals::TLocalPlayer::NadePrediction.Color.flColors));
		}

	}


}


void TNadePrediction::Simulate() {

	if (pPredictionEntity == TGlobalVars::pLocalEntity)
		m_vecEyeAngles = TLocalEngine::vecViewAngles;

	Setup();

	size_t iStep = (size_t)TIME_TO_TICKS(0.05f), iTimer{ 0u };

	for (size_t iCurrent{ 0u }; iCurrent < 4096u; ++iCurrent) {

		if (!iTimer)
			m_pPath.push_back(m_vecStart);

		size_t flags = Advance(iCurrent);


		if ((flags & DETONATE))
			break;

		if ((flags & BOUNCE) || iTimer >= iStep)
			iTimer = 0;

		else
			++iTimer;

		if (m_vecVelocity == Vector3D())
			break;
	}

	if (m_iId == WEAPON_MOLOTOV || m_iId == WEAPON_INCGRENADE) {
		Trace_t trace;
		PhysicsPushEntity(m_vecStart, { 0.f, 0.f, -131.f }, trace, pPredictionEntity);

		if (trace.flFraction < 0.9f)
			m_vecStart = trace.vecEnd;
	}

	m_pPath.push_back(m_vecStart);
	m_pBounces.push_back(m_vecStart);
}

void TNadePrediction::Setup() {

	Vector3D angle = m_vecEyeAngles;

	float flPitch = angle.x;

	if (flPitch < -90.f)
		flPitch += 360.f;

	else if (flPitch > 90.f)
		flPitch -= 360.f;

	angle.x = flPitch - (90.f - abs(flPitch)) * 10.f / 90.f;

	float flVelocity = m_flVelocity * 0.9f;

	flVelocity = std::clamp(flVelocity, 15.f, 750.f);

	flVelocity *= ((m_flPower * 0.7f) + 0.3f);

	Vector3D vecForward;
	TMath::AngleVectors(angle, &vecForward);

	m_vecStart = pPredictionEntity->GetEyePosition();

	m_vecStart.z += (m_flPower * 12.f) - 12.f;

	Vector3D m_vecEnd = m_vecStart + (vecForward * 22.f);

	Trace_t pEngineTrace;
	TLocalEngine::TTraceSystem::TraceHull(m_vecStart, m_vecEnd, pEngineTrace, pPredictionEntity);

	m_vecStart = pEngineTrace.vecEnd - (vecForward * 6.f);

	if (pPredictionEntity == TGlobalVars::pLocalEntity)
		m_vecVelocity = pPredictionEntity->GetVelocity();

	m_vecVelocity *= 1.25f;
	m_vecVelocity += (vecForward * flVelocity);
}

size_t TNadePrediction::Advance(size_t iTick) {

	size_t     iFlags{ NONE };
	Trace_t pEngineTrace;

	PhysicsAddGravityMove(m_vecMove, m_vecVelocity);
	PhysicsPushEntity(m_vecStart, m_vecMove, pEngineTrace, pPredictionEntity);

	if (Detonate(iTick, pEngineTrace))
		iFlags |= DETONATE;

	if (pEngineTrace.flFraction != 1.f) {

		iFlags |= BOUNCE;

		ResolveFlyCollisionBounce(pEngineTrace, m_vecStart, m_vecMove, m_vecVelocity, m_pBounces);
	}

	m_vecStart = pEngineTrace.vecEnd;

	return iFlags;
}

bool TNadePrediction::Detonate(size_t iTick, Trace_t& pEngineTrace) {

	float flTime = TICKS_TO_TIME(iTick);

	switch (m_iId) {
	case WEAPON_FLASHBANG:
	case WEAPON_HEGRENADE:
		return flTime >= 1.5f && !(iTick % TIME_TO_TICKS(0.2f));

	case WEAPON_SMOKEGRENADE:
		return m_vecVelocity.Length() <= 0.1f && !(iTick % TIME_TO_TICKS(0.2f));

	case WEAPON_DECOY:
		return m_vecVelocity.Length() <= 0.2f && !(iTick % TIME_TO_TICKS(0.2f));

	case WEAPON_MOLOTOV:
	case WEAPON_FIREBOMB:
		if (pEngineTrace.flFraction != 1.f && (std::cos(M_DEG2RAD(TConvarManager::varMolotovMaxDetonationSlope.pBase->GetFloat())) <= pEngineTrace.plane.vecNormal.z))
			return true;
		return flTime >= TConvarManager::varMolotovMaxDetonationSlope.pBase->GetFloat() && !(iTick % TIME_TO_TICKS(0.1f));

	default:
		return false;
	}

	return false;
}

void TNadePrediction::ResolveFlyCollisionBounce(Trace_t& pEngineTrace, Vector3D& mvStart, Vector3D& mvMove, Vector3D& mvVelocity, std::vector< Vector3D >& mvBounces) {

	float flSurface = 1.f;
	auto iEntityClassIndex = pEngineTrace.pHitEntity->GetClientClass()->nClassID;

	if (pEngineTrace.pHitEntity) {
		if (pEngineTrace.pHitEntity->IsBreakable()) {
			if (iEntityClassIndex != EClassIndex::CFuncBrush &&
				iEntityClassIndex != EClassIndex::CBaseDoor &&
				iEntityClassIndex != EClassIndex::CCSPlayer &&
				iEntityClassIndex != EClassIndex::CBaseEntity) {
				PhysicsPushEntity(mvStart, mvMove, pEngineTrace, pEngineTrace.pHitEntity);
				mvVelocity *= 0.4f;
				return;
			}
		}
	}

	float flElasticity = 0.45f * flSurface;

	flElasticity = std::clamp(flElasticity, 0.f, 0.9f);

	Vector3D vecVelocity;
	PhysicsClipVelocity(mvVelocity, pEngineTrace.plane.vecNormal, vecVelocity, 2.f);
	vecVelocity *= flElasticity;

	if (pEngineTrace.plane.vecNormal.z > 0.7f) {
		float speed = vecVelocity.LengthSqr();

		if (speed > 96000.f) {

			float flLength = vecVelocity.Normalized().DotProduct(pEngineTrace.plane.vecNormal);
			if (flLength > 0.5f)
				vecVelocity *= 1.5f - flLength;
		}

		if (speed < 400.f)
			mvVelocity = Vector3D();

		else {
			mvVelocity = vecVelocity;

			float left = 1.f - pEngineTrace.flFraction;

			PhysicsPushEntity(pEngineTrace.vecEnd, vecVelocity * (left * TICK_INTERVAL), pEngineTrace, pPredictionEntity);
		}
	}

	else {
		mvVelocity = vecVelocity;
		float left = 1.f - pEngineTrace.flFraction;
		PhysicsPushEntity(pEngineTrace.vecEnd, vecVelocity * (left * TICK_INTERVAL), pEngineTrace, pPredictionEntity);
	}

	mvBounces.push_back(pEngineTrace.vecEnd);
}

void TNadePrediction::PhysicsPushEntity(Vector3D& vecStart, const Vector3D& vecMove, Trace_t& pEngineTrace, CBaseEntity* pPlayer) {
	Vector3D end = vecStart + vecMove;
	TLocalEngine::TTraceSystem::TraceHull(vecStart, end, pEngineTrace, pPlayer);
}

void TNadePrediction::PhysicsAddGravityMove(Vector3D& vecMove, Vector3D& vecVelocity) {
	float gravity = 800.f * 0.4f;

	vecMove.x = vecVelocity.x * TICK_INTERVAL;
	vecMove.y = vecVelocity.y * TICK_INTERVAL;

	float z = vecVelocity.z - (gravity * TICK_INTERVAL);
	vecMove.z = ((vecVelocity.z + z) / 2.f) * TICK_INTERVAL;
	vecVelocity.z = z;
}

void TNadePrediction::PhysicsClipVelocity(const Vector3D& vecIn, const Vector3D& vecNormal, Vector3D& vecOut, float flOverBounce) {

	float flBackOff = vecIn.DotProduct(vecNormal) * flOverBounce;

	for (int i{ }; i < 3; ++i) {
		vecOut[i] = vecIn[i] - (vecNormal[i] * flBackOff);

		if (vecOut[i] > -0.1f && vecOut[i] < 0.1f)
			vecOut[i] = 0.f;
	}
}
