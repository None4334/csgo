#include "../../includes.h"
#include "../handlers/localengine.h"
#include "../handlers/entityhandler.h"
#include "../gvars.h"
#include "../sdk.h"
#include "../math.h"
#include "../handlers/convarmanager.h"
#include "../ui/vars.h"
#include "../handlers/eventlogger.h"
#include "autowall.h"


void TEngineFireBullet::CacheWeaponData()
{

	m_Data.m_Weapon = TBaseEntity::GetWeapon(TGlobalVars::pLocalEntity);
	if (!m_Data.m_Weapon)
		return;

	m_Data.m_WeaponData = TBaseEntity::GetWeaponData(m_Data.m_Weapon);
	if (!m_Data.m_WeaponData)
		return;

	m_Data.m_flMaxRange = m_Data.m_WeaponData->flRange;
	m_Data.m_flWeaponDamage = m_Data.m_WeaponData->iDamage;
	m_Data.m_flPenetrationPower = m_Data.m_WeaponData->flPenetration;
	m_Data.m_flArmorRatio = m_Data.m_WeaponData->flArmorRatio;
	m_Data.m_flPenetrationDistance = 3000.0f;
	m_Data.m_flRangeModifier = m_Data.m_WeaponData->flRangeModifier;
}
bool TEngineFireBullet::HandleBulletPenetration(PenetrationData_t* m_PenetrationData)
{
	bool bHitGrate = m_PenetrationData->EnterTrace.iContents & CONTENTS_GRATE;
	bool bIsNoDraw = m_PenetrationData->EnterTrace.surface.uFlags & SURF_NODRAW;

	int iEnterMaterial = TInterfaces::PhysicsProps->GetSurfaceData(m_PenetrationData->EnterTrace.surface.nSurfaceProps)->game.hMaterial;
	if (m_PenetrationData->PenetrationCount == 0 && !bHitGrate && !bIsNoDraw
		&& iEnterMaterial != CHAR_TEX_GLASS && iEnterMaterial != CHAR_TEX_GRATE)
		return false;

	if (m_PenetrationData->PenetrationCount <= 0 || m_Data.m_flPenetrationPower <= 0.0f)
		return false;

	if (!TraceToExit(m_PenetrationData, m_PenetrationData->EnterTrace.vecEnd, &m_PenetrationData->EnterTrace, &m_PenetrationData->ExitTrace))
		if (!( TInterfaces::EngineTrace->GetPointContents_WorldOnly(m_PenetrationData->EnterTrace.vecEnd, MASK_SHOT_HULL) & MASK_SHOT_HULL))
			return false;

	float flDamLostPercent = 0.16f;
	float flDamageModifier = m_PenetrationData->flDamageModifier;
	float flPenetrationModifier = m_PenetrationData->flPenetrationModifier;

	const float flDamageReductionBullet = TConvarManager::varDamageReductionBullets.pBase->GetFloat();
	if (bHitGrate || bIsNoDraw || iEnterMaterial == CHAR_TEX_GLASS || iEnterMaterial == CHAR_TEX_GRATE)
	{
		if (iEnterMaterial == CHAR_TEX_GLASS || iEnterMaterial == CHAR_TEX_GRATE)
		{
			flPenetrationModifier = 3.0f;
			flDamLostPercent = 0.05;
		}
		else
			flPenetrationModifier = 1.0f;

		flDamageModifier = 0.99f;
	}
	else if (iEnterMaterial == CHAR_TEX_FLESH && flDamageReductionBullet == 0
		&& m_PenetrationData->EnterTrace.pHitEntity && m_PenetrationData->EnterTrace.pHitEntity->IsPlayer() && !TGlobalVars::pLocalEntity->IsEnemy(m_PenetrationData->EnterTrace.pHitEntity))
	{
		if (flDamageReductionBullet == 0)
			return true;

		flPenetrationModifier = flDamageReductionBullet;
		flDamageModifier = flDamageReductionBullet;
	}
	else
	{
		float flExitPenetrationModifier = TInterfaces::PhysicsProps->GetSurfaceData(m_PenetrationData->ExitTrace.surface.nSurfaceProps)->game.flPenetrationModifier;
		float flExitDamageModifier = TInterfaces::PhysicsProps->GetSurfaceData(m_PenetrationData->ExitTrace.surface.nSurfaceProps)->game.flDamageModifier;
		flPenetrationModifier = (flPenetrationModifier + flExitPenetrationModifier) * 0.5f;
		flDamageModifier = (flDamageModifier + flExitDamageModifier) * 0.5f;
	}

	int iExitMaterial = TInterfaces::PhysicsProps->GetSurfaceData(m_PenetrationData->ExitTrace.surface.nSurfaceProps)->game.hMaterial;
	if (iEnterMaterial == iExitMaterial)
	{
		if (iExitMaterial == CHAR_TEX_WOOD || iExitMaterial == CHAR_TEX_CARDBOARD)
			flPenetrationModifier = 3.0f;
		else if (iExitMaterial == CHAR_TEX_PLASTIC)
			flPenetrationModifier = 2.0f;
	}

	float flTraceDistance = (m_PenetrationData->ExitTrace.vecEnd - m_PenetrationData->EnterTrace.vecEnd).Length();
	float flPenMod = fmax(0.f, (1.f / flPenetrationModifier));
	float flPercentDamageChunk = m_PenetrationData->flCurrentDamage * flDamLostPercent;
	float flPenWepMod = flPercentDamageChunk + fmax(0.0f, (3.0f / m_Data.m_flPenetrationPower) * 1.25f) * (flPenMod * 3.0f);
	float flLostDamageObject = ((flPenMod * (flTraceDistance * flTraceDistance)) / 24.0f);
	float flTotalLostDamage = flPenWepMod + flLostDamageObject;

	m_PenetrationData->flCurrentDamage -= fmax(flTotalLostDamage, 0.0f);
	if (m_PenetrationData->flCurrentDamage <= 0.0f)
		return false;

	m_PenetrationData->vecShootPosition = m_PenetrationData->ExitTrace.vecEnd;
	m_PenetrationData->flCurrentDistance += flTraceDistance;
	m_PenetrationData->PenetrationCount--;

	return true;
}
bool TEngineFireBullet::TraceToExit(PenetrationData_t* m_PenetrationData, const Vector3D& vecStart, CGameTrace* pEnterTrace, CGameTrace* pExitTrace)
{
	CTraceFilterSimple SimpleTraceFilter(TGlobalVars::pLocalEntity,NULL);

	int	nFirstContents = 0;
	for (int nIteration = 1; nIteration <= 22; nIteration++)
	{
		// increase distance
		float flDistance = (float)(nIteration) * 4.0f;

		// calc new end
		Vector3D vecEnd = vecStart + (m_PenetrationData->vecDirection * flDistance);

		// cache contents
		int nContents = TInterfaces::EngineTrace->GetPointContents_WorldOnly(vecEnd, MASK_SHOT_HULL | CONTENTS_HITBOX) & (MASK_SHOT_HULL | CONTENTS_HITBOX);
		if (!nFirstContents)
			nFirstContents = nContents;

		Vector3D vecTraceEnd = vecEnd - (m_PenetrationData->vecDirection * 4.0f);
		if (!(nContents & MASK_SHOT_HULL) || ((nContents & CONTENTS_HITBOX) && nFirstContents != nContents))
		{
	
		
			TLocalEngine::TTraceSystem::TraceLine(vecEnd, vecTraceEnd, MASK_SHOT_HULL | CONTENTS_HITBOX, NULL, COLLISION_GROUP_NONE, &m_PenetrationData->ExitTrace);

			ClipTraceToPlayers(vecTraceEnd, vecEnd, &m_PenetrationData->ExitTrace, (CTraceFilter*)&SimpleTraceFilter, MASK_SHOT_HULL | CONTENTS_HITBOX);

			// we exited the wall into a player's hitbox
			if (m_PenetrationData->ExitTrace.bStartSolid && (m_PenetrationData->ExitTrace.surface.uFlags & SURF_HITBOX))
			{
				 TLocalEngine::TTraceSystem::TraceLine(vecEnd, vecStart, MASK_SHOT_HULL, m_PenetrationData->ExitTrace.pHitEntity, COLLISION_GROUP_NONE, &m_PenetrationData->ExitTrace);
				if (m_PenetrationData->ExitTrace.DidHit() && !m_PenetrationData->ExitTrace.bStartSolid)
					return true;
			}
			else if (m_PenetrationData->ExitTrace.DidHit() && !m_PenetrationData->ExitTrace.bStartSolid)
			{
				bool bStartIsNodraw = m_PenetrationData->EnterTrace.surface.uFlags & SURF_NODRAW;
				bool bExitIsNodraw = m_PenetrationData->ExitTrace.surface.uFlags & SURF_NODRAW;

				if (bExitIsNodraw && m_PenetrationData->ExitTrace.pHitEntity->IsBreakable() && m_PenetrationData->EnterTrace.pHitEntity->IsBreakable())
					return true;
				else if (!bExitIsNodraw || (bStartIsNodraw && bExitIsNodraw))
				{
					if (m_PenetrationData->vecDirection.DotProduct(m_PenetrationData->ExitTrace.plane.vecNormal) <= 1.0f)
						return true;
				}
			}
			else if (TLocalEngine::TTraceSystem::TraceDidHitNonWorldEntity(&m_PenetrationData->EnterTrace) && m_PenetrationData->EnterTrace.pHitEntity->IsBreakable())
			{
				m_PenetrationData->ExitTrace = m_PenetrationData->EnterTrace;
				m_PenetrationData->ExitTrace.vecEnd = vecStart + m_PenetrationData->vecDirection;

				return true;
			}
		}
	}

	return false;
}
bool TEngineFireBullet::IsPenetrablePoint(const Vector3D& vecShootPosition, const Vector3D& vecTargetPosition)
{
	m_Data.m_Weapon = TBaseEntity::GetWeapon(TGlobalVars::pLocalEntity);
	if (!m_Data.m_Weapon)
		return false;

	m_Data.m_WeaponData = TBaseEntity::GetWeaponData(m_Data.m_Weapon);
	if (!m_Data.m_WeaponData)
		return false;

	Vector3D angLocalAngles;
	TInterfaces::EngineClient->GetViewAngles(angLocalAngles);

	Vector3D vecDirection;
	TMath::AngleVectors(angLocalAngles, &vecDirection);

	CGameTrace Trace;
	 TLocalEngine::TTraceSystem::TraceLine(vecShootPosition, vecTargetPosition, MASK_SHOT_HULL | CONTENTS_HITBOX,TGlobalVars::pLocalEntity, NULL, &Trace);

	if ((int)(Trace.flFraction))
		return false;

	PenetrationData_t m_PenetrationData;
	m_PenetrationData.EnterTrace = Trace;
	m_PenetrationData.vecDirection = vecDirection;
	m_PenetrationData.flCurrentDistance = 0.0f;
	m_PenetrationData.flCurrentDamage = (float)(m_Data.m_WeaponData->iDamage);
	m_PenetrationData.PenetrationCount = 1;
	m_PenetrationData.vecShootPosition = vecShootPosition;
	m_PenetrationData.vecTargetPosition = vecTargetPosition;

	if (!HandleBulletPenetration(&m_PenetrationData))
		return false;

	return true;
}
bool TEngineFireBullet::SimulateFireBullet(PenetrationData_t* m_PenetrationData)
{
	static const auto uTraceFilterSkipTwoEntities = (void*)((DWORD)(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 81 EC BC 00 00 00 56 8B F1 8B 86"))) + 0x226);
	std::array < uintptr_t, 5 > aSkipTwoEntities
		=
	{
		*(uintptr_t*)(uTraceFilterSkipTwoEntities),
		(uintptr_t)(TGlobalVars::pLocalEntity),
		NULL,
		NULL,
		NULL
	};

	while (m_PenetrationData->flCurrentDamage > 0.0f)
	{
		m_PenetrationData->EnterTrace = CGameTrace();
		m_PenetrationData->ExitTrace = CGameTrace();

		Vector3D vecEnd = m_PenetrationData->vecShootPosition + m_PenetrationData->vecDirection * (m_Data.m_flMaxRange - m_PenetrationData->flCurrentDistance);

		Ray_t Ray(m_PenetrationData->vecShootPosition, vecEnd);

		// run trace
		 TInterfaces::EngineTrace->TraceRay(Ray, MASK_SHOT_HULL | CONTENTS_HITBOX, (CTraceFilter*)(aSkipTwoEntities.data()), &m_PenetrationData->EnterTrace);
		{
			ClipTraceToPlayers
			(
				m_PenetrationData->vecShootPosition,
				vecEnd + m_PenetrationData->vecDirection * 40.0f,
				&m_PenetrationData->EnterTrace,
				(CTraceFilter*)(aSkipTwoEntities.data()),
				MASK_SHOT_HULL | CONTENTS_HITBOX
			);
		}

		aSkipTwoEntities[4] = (uintptr_t)(m_PenetrationData->EnterTrace.pHitEntity);

		if (m_PenetrationData->EnterTrace.flFraction == 1.0f)
			break;

		surfacedata_t* pSurfaceData = TInterfaces::PhysicsProps->GetSurfaceData(m_PenetrationData->EnterTrace.surface.nSurfaceProps);

		m_PenetrationData->flPenetrationModifier = pSurfaceData->game.flPenetrationModifier;
		m_PenetrationData->flDamageModifier = pSurfaceData->game.flDamageModifier;

		m_PenetrationData->flCurrentDistance += m_PenetrationData->EnterTrace.flFraction * (m_Data.m_flMaxRange - m_PenetrationData->flCurrentDistance);
		m_PenetrationData->flCurrentDamage *= std::powf(m_Data.m_flRangeModifier, (m_PenetrationData->flCurrentDistance / 500.0f));

		if ((m_PenetrationData->flCurrentDistance > m_Data.m_flPenetrationDistance && m_Data.m_flPenetrationPower > 0) ||
			m_PenetrationData->flPenetrationModifier < 0.1f)
			m_PenetrationData->PenetrationCount = 0;

		if (m_PenetrationData->EnterTrace.pHitEntity)
		{
			if (m_PenetrationData->EnterTrace.pHitEntity->IsPlayer() && m_PenetrationData->EnterTrace.pHitEntity->IsAlive()
				&& TGlobalVars::pLocalEntity->IsEnemy(m_PenetrationData->EnterTrace.pHitEntity) && !m_PenetrationData->EnterTrace.pHitEntity->IsDormant())
			{
				ScaleDamage(&m_PenetrationData->EnterTrace, m_PenetrationData->flCurrentDamage);
				return (int)(std::floorf(m_PenetrationData->flCurrentDamage)) >= (int)(m_PenetrationData->flMinDamage);
			}
		}

		bool bIsBulletStopped = !HandleBulletPenetration(m_PenetrationData);
		if (bIsBulletStopped)
			break;
	}

	return false;
}
void TEngineFireBullet::ScaleDamage(CGameTrace* Trace, float& flDamage)
{
	if (!Trace->pHitEntity || !Trace->pHitEntity->IsPlayer() || !Trace->pHitEntity->IsAlive())
		return;

	float flHeadScale = TConvarManager::varDamageScaleCtHead.pBase->GetFloat();
	if (Trace->pHitEntity->GetTeam() == TEAM_TT)
		flHeadScale = TConvarManager::varDamageScaleTHead.pBase->GetFloat();

	float flBodyScale = TConvarManager::varDamageScaleCTBody.pBase->GetFloat();
	if (Trace->pHitEntity->GetTeam() == TEAM_TT)
		flBodyScale = TConvarManager::varDamageScaleTBody.pBase->GetFloat();

	if (Trace->pHitEntity->HasHeavyArmor())
		flHeadScale *= 0.5f;

	int nHitGroup = Trace->iHitGroup;
	switch (nHitGroup)
	{
	case HITGROUP_HEAD:
		flDamage = (flDamage * 4.0f) * flHeadScale;
		break;

	case HITGROUP_STOMACH:
		flDamage = (flDamage * 1.25f) * flBodyScale;
		break;

	case HITGROUP_NECK:
	case HITGROUP_CHEST:
	case HITGROUP_LEFTARM:
	case HITGROUP_RIGHTARM:
		flDamage *= flBodyScale;
		break;

	case HITGROUP_LEFTLEG:
	case HITGROUP_RIGHTLEG:
		flDamage = (flDamage * 0.75f) * flBodyScale;
		break;

	default:
		break;
	}

	if (!IsArmored(Trace->pHitEntity, nHitGroup))
	{
		flDamage = std::floorf(flDamage);
		return;
	}

	float fDamageToHealth = flDamage;
	float fDamageToArmor = 0;
	float flArmorBonus = 0.5f;
	float fHeavyArmorBonus = 1.0f;
	float flArmorRatio = m_Data.m_flArmorRatio;

	if (Trace->pHitEntity->HasHeavyArmor())
	{
		flArmorRatio *= 0.5f;
		flArmorBonus = 0.33f;
		fHeavyArmorBonus = 0.33f;
	}

	fDamageToHealth *= flArmorRatio;
	fDamageToArmor = (flDamage - fDamageToHealth) * (flArmorBonus * fHeavyArmorBonus);
	if (fDamageToArmor > Trace->pHitEntity->GetArmor())
		fDamageToHealth = flDamage - (float)(Trace->pHitEntity->GetArmor()) / flArmorBonus;

	flDamage = std::floorf(fDamageToHealth);
}
bool TEngineFireBullet::IsArmored(CBaseEntity* pPlayer, const int& nHitGroup)
{
	bool bIsArmored = false;
	switch (nHitGroup)
	{
	case HITGROUP_HEAD:
		bIsArmored = pPlayer->HasHelmet();
		break;

	default:
		bIsArmored = pPlayer->HasHeavyArmor();
		break;
	}

	if (pPlayer->GetArmor() <= 0)
		return false;

	return bIsArmored;
}
void TEngineFireBullet::ClipTraceToPlayers(const Vector3D& vecStart, const Vector3D& vecEnd, CGameTrace* Trace, CTraceFilter* pTraceFilter, uint32_t nMask)
{
	CGameTrace NewTrace;
	Ray_t Ray(vecStart, vecEnd);

	float flSmallestFraction = Trace->flFraction;

	for (auto& CurrentObject : TEntityListener::vecEntities)
	{
		if (CurrentObject->nClass != (int)EClassIndex::CBaseEntity)
			continue;

		if (!CurrentObject->pEntity || !CurrentObject->pEntity->IsAlive() || CurrentObject->pEntity->IsDormant() || !TGlobalVars::pLocalEntity->IsEnemy(CurrentObject->pEntity))
			continue;

		if (pTraceFilter && !pTraceFilter->ShouldHitEntity(CurrentObject->pEntity, nMask))
			continue;

		float flRange = TMath::DistanceToRay(CurrentObject->pEntity->WorldSpaceCenter(), vecStart, vecEnd);
		if (flRange < 0.0f || flRange > 60.0f)
			continue;

		 TInterfaces::EngineTrace->ClipRayToEntity(Ray, nMask | CONTENTS_HITBOX, CurrentObject->pEntity, &NewTrace);
		if (NewTrace.flFraction < flSmallestFraction)
		{
			*Trace = NewTrace;
			flSmallestFraction = NewTrace.flFraction;
		}
	}
}

void TEngineFireBullet::ScanPoint(PenetrationData_t* m_PenetrationData)
{
	m_PenetrationData->vecDirection = GetPointDirection(m_PenetrationData->vecShootPosition, m_PenetrationData->vecTargetPosition);
	m_PenetrationData->flCurrentDistance = 0.0f;
	m_PenetrationData->PenetrationCount = 4;
	m_PenetrationData->flDamageModifier = 0.5f;
	m_PenetrationData->flPenetrationModifier = 1.0f;
	m_PenetrationData->flCurrentDamage = m_Data.m_flWeaponDamage;
	m_PenetrationData->bSuccess = SimulateFireBullet(m_PenetrationData);
	if (!m_PenetrationData->bSuccess)
		m_PenetrationData->flCurrentDamage = -1.0f;
}
Vector3D TEngineFireBullet::GetPointDirection(const Vector3D& vecShootPosition, const Vector3D& vecTargetPosition)
{
	Vector3D vecDirection;
	Vector3D angDirection;

	TMath::VectorAngles(vecTargetPosition - vecShootPosition, angDirection);
	TMath::AngleVectors(angDirection, &vecDirection);

	vecDirection.NormalizeInPlaceRetNil();
	return vecDirection;
}
