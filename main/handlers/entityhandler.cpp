#include "../../includes.h"
#include "../sdk.h"
#include "entityhandler.h"
#include "../gvars.h"
#include "animationsmanager.h"
#include "../handlers/localengine.h"
#include "../features/nadewarning.h"

void CEntityListener::Setup()
{
	TInterfaces::ClientEntityList->AddListenerEntity(this);
}

void CEntityListener::Destroy()
{
	TInterfaces::ClientEntityList->RemoveListenerEntity(this);
}

void CEntityListener::OnEntityCreated(CBaseEntity* pEntity)
{
	if (pEntity == nullptr)
		return;
	
	const int nIndex = pEntity->GetIndex();

	if (nIndex < 0)
		return;

	CBaseClient* pClientClass = pEntity->GetClientClass();

	if (pClientClass == nullptr)
		return;

	bool IsPlayer = pClientClass->nClassID == EClassIndex::CCSPlayer && pEntity->IsPlayer();
	bool IsNadeProjectile = pClientClass->nClassID == EClassIndex::CMolotovProjectile || pClientClass->nClassID == EClassIndex::CBaseCSGrenadeProjectile;

	if (TEntityListener::VerifyEntityPlayerAndIntegrity(pEntity) && !TEntityListener::CheckForEntityInStack(pEntity, nIndex))  
		TEntityListener::vecEntities.emplace_back(new TEntityObject{ pEntity, nIndex, (int)pClientClass->nClassID , IsPlayer ? new TAnimationInstance(pEntity,nIndex) : nullptr, IsNadeProjectile ? (TNadeInfo*)(new TNadeWarning::TNadeInfo()) : nullptr});
	
	TGlobalVars::LocalIndex = TInterfaces::EngineClient->GetLocalPlayer();
	TGlobalVars::pLocalEntity = TInterfaces::ClientEntityList->Get<CBaseEntity>(TGlobalVars::LocalIndex);
}

void CEntityListener::OnEntityDeleted(CBaseEntity* pEntity)
{
	if (pEntity == nullptr)
		return;

	const int nIndex = pEntity->GetIndex();

	if (nIndex < 0)
		return;

	if (nIndex == TGlobalVars::LocalIndex) {
		TGlobalVars::LocalIndex = -1;
		TGlobalVars::pLocalEntity = nullptr;
	}

	TEntityListener::RemoveFromEntityList(nIndex);
}

void TEntityListener::TOnUpdate::FillEntityList() {

	TGlobalVars::LocalIndex = TInterfaces::EngineClient->GetLocalPlayer();
	TGlobalVars::pLocalEntity = TInterfaces::ClientEntityList->Get<CBaseEntity>(TGlobalVars::LocalIndex);
	if (TGlobalVars::pLocalEntity)
    	TGlobalVars::nLocalTeamIndex = TGlobalVars::pLocalEntity->GetTeam();

	int iHighestEntityIndex = TInterfaces::ClientEntityList->GetHighestEntityIndex();

	if (iHighestEntityIndex)
	{
		for (int iCurrent = 0; iCurrent < iHighestEntityIndex; iCurrent++)
		{
			CBaseEntity* pBaseEntity = TInterfaces::ClientEntityList->Get<CBaseEntity>(iCurrent);

			if (!VerifyEntityPlayerAndIntegrity(pBaseEntity))
				continue;

			if (CheckForEntityInStack(pBaseEntity, iCurrent))
				continue;

			bool IsPlayer = pBaseEntity->GetClientClass()->nClassID == EClassIndex::CCSPlayer && pBaseEntity->IsPlayer();

			bool IsNadeProjectile = pBaseEntity->GetClientClass()->nClassID == EClassIndex::CMolotovProjectile || pBaseEntity->GetClientClass()->nClassID == EClassIndex::CBaseCSGrenadeProjectile;

			TEntityListener::vecEntities.emplace_back(new TEntityObject{ pBaseEntity, iCurrent, (int)pBaseEntity->GetClientClass()->nClassID, IsPlayer ? new TAnimationInstance(pBaseEntity,iCurrent) : nullptr,  IsNadeProjectile ? (TNadeInfo*)(new TNadeWarning::TNadeInfo()) : nullptr });
		}
	}

}

void TEntityListener::TOnUpdate::ValidateEntityList() {

	if (vecEntities.empty())
		return;

	for (auto it = vecEntities.begin(); it != TEntityListener::vecEntities.end();) {

		TEntityObject* EntityObject = *it;

		if (!VerifyEntityIntegrity(EntityObject)) {

			if (EntityObject->AnimationInstance)
			   delete EntityObject->AnimationInstance;

			if (EntityObject->NadeInfo)
				delete EntityObject->NadeInfo;

			delete EntityObject;
			it = vecEntities.erase(it);
		}
		else
			it = next(it);

	}


}

void TEntityListener::RemoveFromEntityList(int Index) {

	if (vecEntities.empty())
		return;

	for (auto it = vecEntities.begin(); it != TEntityListener::vecEntities.end();) {

		TEntityObject* EntityObject = *it;

		if (EntityObject->nIndex == Index) {

			if (EntityObject->AnimationInstance)
			    delete EntityObject->AnimationInstance;

			if (EntityObject->NadeInfo)
				delete EntityObject->NadeInfo;

			delete EntityObject;
			it = vecEntities.erase(it);
		}
		else
			it = next(it);

	}

	return;
}

void TEntityListener::ClearAndDeallocateMemory() {

	if (vecEntities.empty())
		return;

	for (auto it = vecEntities.begin(); it != TEntityListener::vecEntities.end();) {

		TEntityObject* EntityObject = *it;

		if (EntityObject->AnimationInstance)
			delete EntityObject->AnimationInstance;

		if (EntityObject->NadeInfo)
			delete EntityObject->NadeInfo;

		delete EntityObject;

		it = vecEntities.erase(it);
	}
}

void TEntityListener::RemoveFromEntityList(CBaseEntity* Entity) {
	
	if (vecEntities.empty())
		return;

	for (auto it = vecEntities.begin(); it != TEntityListener::vecEntities.end();) {

		TEntityObject* EntityObject = *it;

		if (EntityObject->pEntity == Entity)
		{
			if (EntityObject->AnimationInstance)
			    delete EntityObject->AnimationInstance;

			if (EntityObject->NadeInfo)
				delete EntityObject->NadeInfo;

			delete EntityObject;
			it = vecEntities.erase(it);
		}
		else
		   it = next(it);

	}

	return;
}

bool TEntityListener::VerifyEntityIntegrity(TEntityObject* Object) {
	
	if (!Object)
		return false;

	CBaseEntity* pBaseEntity = TInterfaces::ClientEntityList->Get<CBaseEntity>(Object->nIndex);

	if (!pBaseEntity || !Object->pEntity)
		return false;

	if (pBaseEntity != Object->pEntity)
		return false;

	if ((int)pBaseEntity->GetClientClass()->nClassID != Object->nClass)
		return false;

	return VerifyEntityPlayerAndIntegrity(pBaseEntity);
}

bool TEntityListener::VerifyEntityPlayerAndIntegrity(CBaseEntity* Entity) {
	
	if (!Entity)
		return false;

	if (Entity->GetClientClass()->nClassID == EClassIndex::CCSPlayer) 
		return Entity->IsPlayer();

	EClassIndex eClass = Entity->GetClientClass()->nClassID;

	if (eClass == EClassIndex::CEnvTonemapController
		|| eClass == EClassIndex::CPlantedC4
		|| eClass == EClassIndex::CInferno
		|| eClass == EClassIndex::CBaseCSGrenadeProjectile
		|| eClass == EClassIndex::CDecoyProjectile
		|| eClass == EClassIndex::CSmokeGrenadeProjectile
		|| eClass == EClassIndex::CMolotovProjectile
		|| eClass == EClassIndex::CC4
		|| eClass == EClassIndex::CFlashbang
		|| eClass == EClassIndex::CBaseWeaponWorldModel
		|| eClass == EClassIndex::CDEagle 
		|| eClass == EClassIndex::CAK47
		|| strstr(Entity->GetClientClass()->szNetworkName, TSTRING("CWeapon")) != nullptr
		)
		return true;

	return false;
}

TEntityObject* TEntityListener::GetEntityObject(CBaseEntity* pBaseEntity) {

	if (vecEntities.empty())
		return nullptr;

	for (auto CurrentObject : vecEntities) 
		if (CurrentObject->pEntity == pBaseEntity)
			return CurrentObject;

	return nullptr;
}

bool TEntityListener::CheckForEntityInStack(CBaseEntity* Entity, int Index) {

	if (vecEntities.empty())
		return false;

	for (auto CurrentObject : vecEntities) {
		if (CurrentObject->pEntity == Entity)
			return true;

		if (Index != -1)
		{
			if (CurrentObject->nIndex == Index)
				return true;
		}
	}

	return false;

}