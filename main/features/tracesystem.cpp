#include "../../includes.h"
#include "../handlers/localengine.h"
#include "../handlers/animationsmanager.h"
#include "../handlers/entityhandler.h"
#include "../gvars.h"
#include "../sdk.h"
#include "../math.h"
#include "../handlers/convarmanager.h"
#include "../ui/vars.h"


void TLocalEngine::TTraceSystem::TraceHull(Vector3D& vecStart, Vector3D& vecEnd, Trace_t& pEngineTrace, CBaseEntity* pPlayer) {
	bEngineTraceRunning = true;
	CTraceFilterSimple filter;
	filter.SetPassEntity(pPlayer);
	TInterfaces::EngineTrace->TraceRay(Ray_t(vecStart, vecEnd, { -2.f, -2.f, -2.f }, { 2.f, 2.f, 2.f }), MASK_SOLID, (CTraceFilter*)&filter, &pEngineTrace);
	bEngineTraceRunning = false;
}

void TLocalEngine::TTraceSystem::TraceHull(const Vector3D& src, const Vector3D& dst, const Vector3D& mins, const Vector3D& maxs, int mask, CBaseEntity* entity, int collision_group, CGameTrace* trace) {
	bEngineTraceRunning = true;
	CTraceFilterSimple filter(entity, collision_group);
	auto ray = Ray_t(src, dst, mins, maxs);
	TInterfaces::EngineTrace->TraceRay(ray, mask, (CTraceFilter*)&filter, trace);
	bEngineTraceRunning = false;
}

void  TLocalEngine::TTraceSystem::TraceLine(const Vector3D& src, const Vector3D& dst, int mask, CBaseEntity* entity, int collision_group, CGameTrace* trace) {
	bEngineTraceRunning = true;
	CTraceFilterSimple filter(entity, collision_group);
	auto ray = Ray_t(src, dst);
	TInterfaces::EngineTrace->TraceRay(ray, mask, (CTraceFilter*)&filter, trace);
	bEngineTraceRunning = false;
}

bool TLocalEngine::TTraceSystem::TraceDidHitWorld(Trace_t* Trace)
{
	return Trace->pHitEntity == TInterfaces::ClientEntityList->GetClientEntity(NULL);
}

bool TLocalEngine::TTraceSystem::TraceDidHitNonWorldEntity(Trace_t* Trace)
{
	return Trace->pHitEntity != nullptr && !TraceDidHitWorld(Trace);
}

bool  TLocalEngine::TTraceSystem::DoesHitHeadClipTrace(Vector3D vecStart, Vector3D vecEnd, CBaseEntity* pBaseEntity) {
	Trace_t EngineTrace;
	TInterfaces::EngineTrace->ClipRayToEntity(Ray_t(vecStart, vecEnd), MASK_SHOT, pBaseEntity, &EngineTrace);
	return EngineTrace.iHitGroup == HITGROUP_HEAD || EngineTrace.iHitbox == HITBOX_HEAD;
}
float TLocalEngine::TTraceSystem::ScaleDamage(CBaseEntity* pPlayer, float flDamage, float flArmorRatio, int iHitGroup) {

	bool  bHasHeavyArmor;
	int   iArmor;
	float flHeavyRatio, flBonusRatio, flRatio, flNewDamage;

	static auto IsArmored = [](CBaseEntity* player, int armor, int hitgroup) {
		if (armor <= 0)
			return false;

		if (hitgroup == HITGROUP_HEAD && player->HasHelmet())
			return true;

		else if (hitgroup >= HITGROUP_CHEST && hitgroup <= HITGROUP_RIGHTARM)
			return true;

		return false;
	};

	bHasHeavyArmor = pPlayer->HasHeavyArmor();

	switch (iHitGroup) {
	case HITGROUP_HEAD:
		if (bHasHeavyArmor)
			flDamage = (flDamage * 4.f) * 0.5f;
		else
			flDamage *= 4.f;
		break;

	case HITGROUP_STOMACH:
		flDamage *= 1.25f;
		break;

	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		flDamage *= 0.75f;
		break;

	default:
		break;
	}

	iArmor = pPlayer->GetArmor();

	if (IsArmored(pPlayer, iArmor, iHitGroup)) {
		flHeavyRatio = 1.f;
		flBonusRatio = 0.5f;
		flRatio = flArmorRatio * 0.5f;

		if (bHasHeavyArmor) {
			flBonusRatio = 0.33f;
			flRatio = flArmorRatio * 0.25f;
			flHeavyRatio = 0.33f;
			flNewDamage = (flDamage * flRatio) * 0.85f;
		}

		else
			flNewDamage = flDamage * flRatio;

		if (((flDamage - flNewDamage) * (flHeavyRatio * flBonusRatio)) > iArmor)
			flNewDamage = flDamage - (iArmor / flBonusRatio);

		flDamage = flNewDamage;
	}

	return std::floor(flDamage);
}