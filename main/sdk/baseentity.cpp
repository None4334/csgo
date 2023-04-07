#include "../../includes.h"
#include "animation.h"
#include "baseentity.h"
#include "../handlers/entityhandler.h"
#include "../math.h"
#include "../handlers/animationsmanager.h"
#include "../handlers/convarmanager.h"

Vector3D TBaseEntity::GetBonePosition(TEntityObject* EntityObject,int nIndex) {
	
	//if (EntityObject->iSetupBonesUpdateTick + 75 < TIME_TO_TICKS(EntityObject->pEntity->GetSimulationTime()))
		//return Vector3D();

	if (!EntityObject)
		return Vector3D();

	if (!EntityObject->AnimationInstance || EntityObject->AnimationInstance->iSetupBonesTick == 0)
		return Vector3D();

	if (nIndex <= BONE_INVALID) 
		return Vector3D();

	return EntityObject->AnimationInstance->matCached[nIndex].at(3);
}

Vector3D TBaseEntity::GetHitboxPosition(TEntityObject* EntityObject,int nIndex) {

	//if (EntityObject->iSetupBonesUpdateTick + 75 < TIME_TO_TICKS(EntityObject->pEntity->GetSimulationTime()))
		//return Vector3D();

	if (!EntityObject)
		return Vector3D();

	if (nIndex <= HITBOX_INVALID || nIndex >= HITBOX_MAX)
		return Vector3D();

	if (!EntityObject->AnimationInstance || EntityObject->AnimationInstance->iSetupBonesTick == 0)
		return Vector3D();

	if (const auto pModel = EntityObject->pEntity->GetModel(); pModel != nullptr)
	{
		if (const auto pStudioHdr = TInterfaces::ModelInfo->GetStudioModel(pModel); pStudioHdr != nullptr)
		{
			if (const auto pHitbox = pStudioHdr->GetHitbox(nIndex, 0); pHitbox != nullptr)
			{
				const Vector3D vecMin = TMath::VectorTransform(pHitbox->vecBBMin, EntityObject->AnimationInstance->matCached[pHitbox->iBone]);
				const Vector3D vecMax = TMath::VectorTransform(pHitbox->vecBBMax, EntityObject->AnimationInstance->matCached[pHitbox->iBone]);
				return (vecMin + vecMax) * 0.5f;
			}
		}
	}

	return Vector3D();
}

CBaseCombatWeapon* TBaseEntity::GetWeapon(CBaseEntity* pEntity) {
	return TInterfaces::ClientEntityList->Get<CBaseCombatWeapon>(pEntity->GetActiveWeaponHandle());
}

CCSWeaponData* TBaseEntity::GetWeaponData(CBaseCombatWeapon* BaseWeapon) {
	return TInterfaces::WeaponSystem->GetWeaponData(BaseWeapon->GetItemDefinitionIndex());
}

mstudioposeparamdesc_t* CBoneMergeCache::UpdatePoseParamters(CStudioHdr* Hdr, int Value) {

	static auto oPoseParameter = (mstudioposeparamdesc_t * (__thiscall*)(CStudioHdr*, int))TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 8B 45 08 57 8B F9 8B 4F 04 85 C9 75 15 8B"));
	return oPoseParameter(Hdr, Value);
}

void CBoneMergeCache::ClearCache() {
	Init(NULL);
}

void CBoneMergeCache::UpdateCache() {
	
	static auto oUpdateCache = (void(__thiscall*)(void*))TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 EC 14 53 56 57 8B F9 8B 37"));
	oUpdateCache(this);
	
/*
	auto Studio_BoneIndexByName = (int(__fastcall*)(const CStudioHdr*, const char*))TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 EC 0C 8B 01 53"));

	if (!m_pOwner)
		return;

	CStudioHdr* pOwnerHdr = m_pOwner->GetModelPtr();
	if (!pOwnerHdr)
		return;

	const studiohdr_t* pOwnerRenderHdr = pOwnerHdr->GetRenderHdr();

	CBaseEntity* pFollow = m_pOwner->FindFollowedEntity();
	CStudioHdr* pFollowHdr = (pFollow ? pFollow->GetModelPtr() : NULL);
	const studiohdr_t* pFollowRenderHdr = (pFollowHdr ? pFollowHdr->GetRenderHdr() : NULL);

	if (pFollow != m_pFollow || pFollowRenderHdr != m_pFollowRenderHdr || pOwnerRenderHdr != m_pOwnerRenderHdr || m_bForceCacheClear)
	{
		m_MergedBones.Purge();

		m_bForceCacheClear = false;

		if (pFollow && pFollowHdr && pOwnerHdr)
		{
			m_pFollow = pFollow;
			m_pFollowHdr = pFollowHdr;
			m_pFollowRenderHdr = pFollowRenderHdr;
			m_pOwnerHdr = pOwnerHdr;
			m_pOwnerRenderHdr = pOwnerRenderHdr;

			m_BoneMergeBits.Resize(pOwnerHdr->numbones());
			m_BoneMergeBits.ClearAll();

			mstudiobone_t* pOwnerBones = m_pOwnerHdr->pBone(0);

			m_nFollowBoneSetupMask = BONE_USED_BY_BONE_MERGE;
			for (int i = 0; i < m_pOwnerHdr->numbones(); i++)
			{
				int parentBoneIndex = Studio_BoneIndexByName(m_pFollowHdr, pOwnerBones[i].GetName());
				if (parentBoneIndex < 0)
					continue;

				m_iRawIndexMapping[i] = parentBoneIndex;

				CMergedBone mergedBone;
				mergedBone.m_iMyBone = i;
				mergedBone.m_iParentBone = parentBoneIndex;
				m_MergedBones.AddToTail(mergedBone);
				m_BoneMergeBits.Set(i);

				if ((m_pFollowHdr->boneFlags(parentBoneIndex) & BONE_USED_BY_BONE_MERGE) == 0)
				{
					int n = parentBoneIndex;
					while (n != -1)
					{
						m_pFollowHdr->setBoneFlags(n, BONE_USED_BY_BONE_MERGE);
						n = m_pFollowHdr->boneParent(n);
					}
				}

				if ((m_pOwnerHdr->boneFlags(i) & BONE_USED_BY_BONE_MERGE) == 0)
				{
					int n = i;
					while (n != -1)
					{
						m_pOwnerHdr->setBoneFlags(n, BONE_USED_BY_BONE_MERGE);
						n = m_pOwnerHdr->boneParent(n);
					}
				}
			}

			if (!m_MergedBones.Count())
			{
				m_nFollowBoneSetupMask = 0;
			}

			for (int i = 0; i < MAXSTUDIOPOSEPARAM; i++)
			{
				// init mapping as invalid
				m_nOwnerToFollowPoseParamMapping[i] = -1;

				if (i < m_pFollowHdr->GetNumPoseParameters())
				{
					// get the follower's pose param name for this index
					const char* szFollowerPoseParamName = m_pFollowHdr->pPoseParameter(i).GetName();

					// find it on the owner
					for (int n = 0; n < MAXSTUDIOPOSEPARAM && n < m_pOwnerHdr->GetNumPoseParameters(); n++)
					{
						const char* szOwnerPoseParamName = m_pOwnerHdr->pPoseParameter(n).GetName();
						if (!strcmp(szFollowerPoseParamName, szOwnerPoseParamName))
						{
							//match
							m_nOwnerToFollowPoseParamMapping[i] = n;
							break;
						}
					}
				}
			}

		}
		else
		{
			Init(m_pOwner);
		}
	}
	*/
}


float CBoneMergeCache::GetPoseParamValue(CStudioHdr* hdr, int index, float flValue)
{
	if (index < 0 || index > 24)
		return 0.0f;

	auto pose_param = UpdatePoseParamters(hdr, index);

	if (!pose_param)
		return 0.0f;

	auto PoseParam = *pose_param;

	if (PoseParam.flLoop)
	{
		auto wrap = (PoseParam.flStart + PoseParam.flEnd) / 2.0f + PoseParam.flLoop / 2.0f;
		auto shift = PoseParam.flLoop - wrap;

		flValue = flValue - PoseParam.flLoop * floor((flValue + shift) / PoseParam.flLoop);
	}

	return (flValue - PoseParam.flStart) / (PoseParam.flEnd - PoseParam.flStart);
}

void CBoneMergeCache::CopyToFollow(Vector3D* vecOut, Quaternion* qtOut, int Value, Vector3D* vecIn, Quaternion* qtIn) {
	static auto oCopyToFollow = (void(__thiscall*)(void*, Vector3D*, Quaternion*, int, Vector3D*, Quaternion*))(TUtil::Rel32Fix(TUtil::FindPattern(CLIENT_STR, TSTRING("E8 ?? ?? ?? ?? 8B 87 ?? ?? ?? ?? 8D 8C 24 ?? ?? ?? ?? 8B 7C 24 18"))));
	return oCopyToFollow(this, vecOut, qtOut, Value, vecIn, qtIn);
}

void CBoneMergeCache::CopyFromFollow(Vector3D* vecOut, Quaternion* qtOut, int Value, Vector3D* vecIn, Quaternion* qtIn) {
	static auto oCopyFromFollow = (void(__thiscall*)(void*, Vector3D*, Quaternion*, int, Vector3D*, Quaternion*))(TUtil::Rel32Fix(TUtil::FindPattern(CLIENT_STR, TSTRING("E8 ?? ?? ?? ?? F3 0F 10 45 ?? 8D 84 24 ?? ?? ?? ??"))));
	return oCopyFromFollow(this, vecOut, qtOut, Value, vecIn, qtIn);
}

void CBoneMergeCache::MergeMatchingPoseParams(float* poses, float* target_poses)
{
	UpdateCache();

	if (*(DWORD*)((uintptr_t)this + 0x10) && *(DWORD*)((uintptr_t)this + 0x8C))
	{
		auto index = (int*)((uintptr_t)this + 0x20);
		auto counter = 0;

		do
		{
			if (*index != -1)
			{
				auto target = *(CBaseEntity**)((uintptr_t)this + 0x4);
				auto hdr = target->GetStudioHdr();
				auto pose_param_value = 0.0f;

				if (hdr && *(studiohdr_t**)hdr && counter >= 0)
				{
					auto pose = target_poses[counter];
					auto pose_param = UpdatePoseParamters(hdr, counter);

					pose_param_value = pose * (pose_param->flEnd - pose_param->flStart) + pose_param->flStart;
				}

				auto target2 = *(CBaseEntity**)(this);
				auto hdr2 = target2->GetStudioHdr();

				poses[*index] = GetPoseParamValue(hdr2, *index, pose_param_value);
			}

			++counter;
			++index;
		} while (counter < 24);
	}
}


void CBoneMergeCache::Init(CBaseEntity* BaseEntity) {

	auto oInit = (void(__thiscall*)(void*, CBaseEntity*))TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 8B 45 08 56 8B F1 89 06 C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C7 ? ? ? ? ? ? C6"));
	oInit(this, BaseEntity);
	/*
	m_pOwner = BaseEntity;
	m_pFollow = NULL;
	m_pFollowHdr = NULL;
	m_pFollowRenderHdr = NULL;
	m_pOwnerHdr = NULL;
	m_pFollowRenderHdr = NULL;
	m_nFollowBoneSetupMask = 0;
	m_bForceCacheClear = false;
	m_MergedBones.Purge();
	m_BoneMergeBits.FreeAllocation();
	memset(m_iRawIndexMapping, 0xFF, sizeof(m_iRawIndexMapping));	
	*/
	/*
	((uintptr_t*)this)[0] = (uintptr_t)BaseEntity;
	((uintptr_t*)this)[1] = NULL;
	((uintptr_t*)this)[2] = NULL;
	((uintptr_t*)this)[3] = NULL;
	((uintptr_t*)this)[4] = NULL;
	((uintptr_t*)this)[7] = NULL;
	*((BYTE*)this + 0x2A0) = NULL;
	((uintptr_t*)this)[0x23] = NULL;


	if (((int*)this)[0x22] >= NULL)
	{
		if (((uintptr_t*)this)[0x20])
		{
			TInterfaces::MemAlloc->Free((void*)((uintptr_t*)this)[0x20]);
			((uintptr_t*)this)[0x20] = NULL;
		}
		((uintptr_t*)this)[0x21] = NULL;
	}
	
	((uintptr_t*)this)[0x24] = ((uintptr_t*)this)[0x20];

	memset((void*)((uintptr_t)this + 0x28), 0xFF, 0x200u);
	*/
}

void CBaseEntity::AttachmentHelper()
{
	static auto oAttachmentHelper = reinterpret_cast<void(__thiscall*)(CBaseEntity*, void*)>(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 EC 48 53 8B 5D 08 89 4D F4")));
	oAttachmentHelper(this, GetStudioHdr());
};


CStudioHdr* CBaseEntity::GetStudioHdr() {
	static auto uStudioHdr = *(size_t*)(TUtil::FindPattern(CLIENT_STR, TSTRING("8B 86 ? ? ? ? 89 44 24 10 85 C0")) + 2);
	return *(CStudioHdr**)((uintptr_t)this + uStudioHdr);
}

CStudioHdr* CBaseEntity::GetModelPtr() {
	
	static auto oLockStudioHdr = (void(__thiscall*)(decltype(this)))TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 51 53 8B D9 56 57 8D ? ? ? ? ? FF"));

	if (!GetStudioHdr())
	   oLockStudioHdr(this);

	return GetStudioHdr();
}

bool CBaseEntity::HasC4() {

	static auto oHasC4 = reinterpret_cast<bool(__thiscall*)(decltype(this))>(TUtil::FindPattern(CLIENT_STR, TSTRING("56 8B F1 85 F6 74 31")));
	return oHasC4(this);
}

bool CBaseEntity::IsFollowingEntity()
{
	return ((GetEffects() & EF_BONEMERGE) != 0) && (GetMoveType() == MOVETYPE_NONE) && GetMoveParent();
}

CBaseEntity* CBaseEntity::GetFollowedEntity()
{
	if (!IsFollowingEntity())
		return NULL;
	return TInterfaces::ClientEntityList->Get<CBaseEntity>(GetMoveParent());
}

int CBaseEntity::GetBoneByHash(const FNV1A_t uBoneHash) const
{
	if (const auto pModel = this->GetModel(); pModel != nullptr)
	{
		if (const auto pStudioHdr = TInterfaces::ModelInfo->GetStudioModel(pModel); pStudioHdr != nullptr)
		{
			for (int i = 0; i < pStudioHdr->nBones; i++)
			{
				if (const auto pBone = pStudioHdr->GetBone(i); pBone != nullptr && FNV1A::Hash(pBone->GetName()) == uBoneHash)
					return i;
			}
		}
	}

	return BONE_INVALID;
}

void CBaseEntity::ModifyEyePosition(const CCSGOPlayerAnimState* pAnimState, Vector3D* vecPosition) const
{

	if (TInterfaces::EngineClient->IsHLTV() || TInterfaces::EngineClient->IsPlayingDemo())
		return;

	CBaseEntity* pBaseEntity = pAnimState->m_pBasePlayer;

	if (pBaseEntity == nullptr)
		return;

	IClientEntity* pGroundEntity = TInterfaces::ClientEntityList->GetClientEntityFromHandle(pBaseEntity->GetGroundEntityHandle());

	if (!pAnimState->m_bHitGroundAnimation && pAnimState->m_flAnimDuckAmount == 0.f && pGroundEntity != nullptr)
		return;

	TEntityObject* EntObject = TEntityListener::GetEntityObject(pBaseEntity);

	if (!EntObject)
		return;

	if (const auto headPosition = TBaseEntity::GetBonePosition(EntObject, pBaseEntity->GetBoneByHash(FNV1A::HashConst("head_0"))); !headPosition.IsZero())
	{
		Vector3D vecHead = headPosition;
		vecHead.z += 1.7f;

		if (vecHead.z < vecPosition->z)
		{
			float flFactor = 0.f;
			const float flDelta = std::fabsf(vecPosition->z - vecHead.z);
			const float flOffset = (flDelta - 4.0f) / 6.0f;

			if (flOffset >= 0.f)
				flFactor = min(flOffset, 1.0f);

			const float flFactorSquared = (flFactor * flFactor);
			vecPosition->z += ((vecHead.z - vecPosition->z) * ((flFactorSquared * 3.0f) - ((flFactorSquared * 2.0f) * flFactor)));
		}
	}
}

bool CBaseEntity::CanShoot(bool bCheckRevolver )
{
	CBaseCombatWeapon* pWeapon = TBaseEntity::GetWeapon(this);

	if (!pWeapon)
		return false;

	auto pWeaponData = TBaseEntity::GetWeaponData(pWeapon);

	if (!pWeaponData)
		return false;

	if (pWeaponData->IsGun() && pWeapon->GetAmmo() <= 0)
		return false;

	if (IsDefusing())
		return false;

	CWeaponCSBase* pBaseWeapon = static_cast<CWeaponCSBase*>(pWeapon);

	const float flServerTime = TICKS_TO_TIME(this->GetTickBase());

	if (this->GetNextAttack() > flServerTime)
		return false;

	const short nDefinitionIndex = pBaseWeapon->GetItemDefinitionIndex();

	if ((nDefinitionIndex == WEAPON_FAMAS || nDefinitionIndex == WEAPON_GLOCK) &&
		pBaseWeapon->IsBurstMode() && pBaseWeapon->GetBurstShotsRemaining() > 0)
		return true;

	if (pBaseWeapon->GetNextPrimaryAttack() > flServerTime)
		return false;

	if (bCheckRevolver && pWeapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
	{
		if (pBaseWeapon->GetFireReadyTime() > flServerTime)
			return false;
	}

	return true;
}

bool CBaseEntity::IsEnemy(CBaseEntity* pEntity)
{
	if (TInterfaces::GameTypes->GetCurrentGameType() == GAMETYPE_FREEFORALL)
		return (this->GetSurvivalTeam() != pEntity->GetSurvivalTeam());

	if (TConvarManager::varTeammatesAreEnemies.pBase != nullptr && TConvarManager::varTeammatesAreEnemies.pBase->GetBool() && this->GetTeam() == pEntity->GetTeam() && this != pEntity)
		return true;

	if (this->GetTeam() != pEntity->GetTeam())
		return true;

	return false;
}

void CBaseEntity::PostThink()
{

	using PostThinkVPhysicsFn = bool(__thiscall*)(CBaseEntity*);
	static auto oPostThinkVPhysics = reinterpret_cast<PostThinkVPhysicsFn>(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 E4 F8 81 EC ? ? ? ? 53 8B D9 56 57 83 BB")));

	using SimulatePlayerSimulatedEntitiesFn = void(__thiscall*)(CBaseEntity*);
	static auto oSimulatePlayerSimulatedEntities = reinterpret_cast<SimulatePlayerSimulatedEntitiesFn>(TUtil::FindPattern(CLIENT_STR, TSTRING("56 8B F1 57 8B BE ? ? ? ? 83 EF 01 78 74")));

	TInterfaces::MDLCache->BeginLock();

	if (this->IsAlive())
	{
		this->UpdateCollisionBounds();

		if (this->GetFlags() & FL_ONGROUND)
			*this->GetFallVelocity() = 0.f;

		if (this->GetSequence() == -1)
			this->SetSequence(0);

		this->StudioFrameAdvance();
		oPostThinkVPhysics(this);
	}

	oSimulatePlayerSimulatedEntities(this);

	TInterfaces::MDLCache->EndLock();
}

float CBaseEntity::GetMaxAnimationDelta() {

	CCSGOPlayerAnimState* pAnimState = (CCSGOPlayerAnimState*)GetAnimationState();

	if (!pAnimState)
		return 0.0f;

	auto flSpeedFraction = max(0.f, min(pAnimState->m_flSpeedAsPortionOfWalkTopSpeed, 1.f));
	auto flSpeedFactor = max(0.f, max(1.f, pAnimState->m_flSpeedAsPortionOfCrouchTopSpeed));
	auto flYawModifier = (((pAnimState->m_flWalkToRunTransition * -0.30000001f) - 0.19999999f) * flSpeedFraction) + 1.f;

	if (pAnimState->m_flAnimDuckAmount > 0.f)
		flYawModifier = flYawModifier + ((pAnimState->m_flAnimDuckAmount * flSpeedFactor) * (0.5f - flYawModifier));

	return (pAnimState->m_flAimYawMax) * flYawModifier;
}


bool CBaseEntity::IsBreakable() {

	const int iHealth = this->GetHealth();

	if (iHealth < 0 && this->IsMaxHealth() > 0)
		return true;

	if (this->GetTakeDamage() != DAMAGE_YES)
	{
		const EClassIndex nClassIndex = this->GetClientClass()->nClassID;

		if (nClassIndex != EClassIndex::CFuncBrush)
			return false;
	}

	if (const int nCollisionGroup = this->GetCollisionGroup(); nCollisionGroup != COLLISION_GROUP_PUSHAWAY && nCollisionGroup != COLLISION_GROUP_BREAKABLE_GLASS && nCollisionGroup != COLLISION_GROUP_NONE)
		return false;

	if (iHealth > 200)
		return false;

	if (IMultiplayerPhysics* pPhysicsInterface = dynamic_cast<IMultiplayerPhysics*>(this); pPhysicsInterface != nullptr)
	{
		if (pPhysicsInterface->GetMultiplayerPhysicsMode() != PHYSICS_MULTIPLAYER_SOLID)
			return false;
	}
	else
	{
		if (const char* szClassName = this->GetClassname(); !strcmp(szClassName, TSTRING("func_breakable")) || !strcmp(szClassName, TSTRING("func_breakable_surf")))
		{
			if (!strcmp(szClassName, TSTRING("func_breakable_surf")))
			{
				CBreakableSurface* pSurface = static_cast<CBreakableSurface*>(this);

				if (pSurface->IsBroken())
					return false;
			}
		}
		else if (this->PhysicsSolidMaskForEntity() & CONTENTS_PLAYERCLIP)
			return false;
	}

	if (IBreakableWithPropData* pBreakableInterface = dynamic_cast<IBreakableWithPropData*>(this); pBreakableInterface != nullptr)
	{
		if (pBreakableInterface->GetDmgModBullet() <= 0.0f)
			return false;
	}

	return true;
}

CBaseEntity* CBaseEntity::FindFollowedEntity()
{
	CBaseEntity* follow = GetFollowedEntity();

	if (!follow)
		return NULL;

	if (follow->IsDormant())
		return NULL;

	if (!follow->GetModel())
		return NULL;

	if (TInterfaces::ModelInfo->GetModelType(follow->GetModel()) != 3)
	{
		return NULL;
	}

	return follow;
}

int CBaseEntity::GetSequenceActivity(int iSequence)
{
	studiohdr_t* pStudioHdr = TInterfaces::ModelInfo->GetStudioModel(this->GetModel());

	if (pStudioHdr == nullptr)
		return -1;

	using GetSequenceActivityFn = int(__fastcall*)(void*, void*, int);
	static auto oGetSequenceActivity = reinterpret_cast<GetSequenceActivityFn>(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 53 8B 5D 08 56 8B F1 83"))); 
	return oGetSequenceActivity(this, pStudioHdr, iSequence);
}

int TBaseEntity::GetHitboxForHitGroup(int iHitgroup)
{
	if (iHitgroup == HITGROUP_GENERIC)
		return HITBOX_CHEST;
	else if (iHitgroup == HITGROUP_HEAD)
		return HITBOX_HEAD;
	else if (iHitgroup == HITGROUP_CHEST)
		return HITBOX_CHEST;
	else if (iHitgroup == HITGROUP_STOMACH)
		return HITBOX_STOMACH;
	else if (iHitgroup == HITGROUP_LEFTARM)
		return HITBOX_LEFT_HAND;
	else if (iHitgroup == HITGROUP_RIGHTARM)
		return HITBOX_RIGHT_HAND;
	else if (iHitgroup == HITGROUP_LEFTLEG)
		return HITBOX_LEFT_THIGH;
	else if (iHitgroup == HITGROUP_RIGHTLEG)
		return HITBOX_RIGHT_THIGH;
	else
		return HITBOX_MAX;

	return -2;
};

bool TBaseEntity::IsGrenade(CCSWeaponData* WeaponData) {
	return WeaponData->nWeaponType == WEAPONTYPE_GRENADE;
}

const char8_t* TBaseEntity::GetWeaponIcon(short nItemDefinitionIndex)
{
	/*
	 * @note: icon code = weapon item definition index in hex
	 * list of other icons:
	 *	"E210" - kevlar
	 *	"E20E" - helmet
	 *	"E20F" - defuser kit
	 *	"E211" - banner
	 *	"E212" - target
	 */
	switch (nItemDefinitionIndex)
	{
	case WEAPON_DEAGLE:
		return u8"\uE001";
	case WEAPON_ELITE:
		return u8"\uE002";
	case WEAPON_FIVESEVEN:
		return u8"\uE003";
	case WEAPON_GLOCK:
		return u8"\uE004";
	case WEAPON_AK47:
		return u8"\uE007";
	case WEAPON_AUG:
		return u8"\uE008";
	case WEAPON_AWP:
		return u8"\uE009";
	case WEAPON_FAMAS:
		return u8"\uE00A";
	case WEAPON_G3SG1:
		return u8"\uE00B";
	case WEAPON_GALILAR:
		return u8"\uE00D";
	case WEAPON_M249:
		return u8"\uE00E";
	case WEAPON_M4A1:
		return u8"\uE010";
	case WEAPON_MAC10:
		return u8"\uE011";
	case WEAPON_P90:
		return u8"\uE013";
	case WEAPON_MP5SD:
		return u8"\uE017";
	case WEAPON_UMP45:
		return u8"\uE018";
	case WEAPON_XM1014:
		return u8"\uE019";
	case WEAPON_BIZON:
		return u8"\uE01A";
	case WEAPON_MAG7:
		return u8"\uE01B";
	case WEAPON_NEGEV:
		return u8"\uE01C";
	case WEAPON_SAWEDOFF:
		return u8"\uE01D";
	case WEAPON_TEC9:
		return u8"\uE01E";
	case WEAPON_TASER:
		return u8"\uE01F";
	case WEAPON_HKP2000:
		return u8"\uE020";
	case WEAPON_MP7:
		return u8"\uE021";
	case WEAPON_MP9:
		return u8"\uE022";
	case WEAPON_NOVA:
		return u8"\uE023";
	case WEAPON_P250:
		return u8"\uE024";
	case WEAPON_SCAR20:
		return u8"\uE026";
	case WEAPON_SG556:
		return u8"\uE027";
	case WEAPON_SSG08:
		return u8"\uE028";
	case WEAPON_KNIFE:
		return u8"\uE02A";
	case WEAPON_FLASHBANG:
		return u8"\uE02B";
	case WEAPON_HEGRENADE:
		return u8"\uE02C";
	case WEAPON_SMOKEGRENADE:
		return u8"\uE02D";
	case WEAPON_MOLOTOV:
		[[fallthrough]];
	case WEAPON_FIREBOMB:
		return u8"\uE02E";
	case WEAPON_DECOY:
		[[fallthrough]];
	case WEAPON_DIVERSION:
		return u8"\uE02F";
	case WEAPON_INCGRENADE:
		return u8"\uE030";
	case WEAPON_C4:
		return u8"\uE031";
	case WEAPON_HEALTHSHOT:
		return u8"\uE039";
	case WEAPON_KNIFE_GG:
		[[fallthrough]];
	case WEAPON_KNIFE_T:
		return u8"\uE03B";
	case WEAPON_M4A1_SILENCER:
		return u8"\uE03C";
	case WEAPON_USP_SILENCER:
		return u8"\uE03D";
	case WEAPON_CZ75A:
		return u8"\uE03F";
	case WEAPON_REVOLVER:
		return u8"\uE040";
	case WEAPON_TAGRENADE:
		return u8"\uE044";
	case WEAPON_FISTS:
		return u8"\uE045";
	case WEAPON_TABLET:
		return u8"\uE048";
	case WEAPON_MELEE:
		return u8"\uE04A";
	case WEAPON_AXE:
		return u8"\uE04B";
	case WEAPON_HAMMER:
		return u8"\uE04C";
	case WEAPON_SPANNER:
		return u8"\uE04E";
	case WEAPON_KNIFE_BAYONET:
		return u8"\uE1F4";
	case WEAPON_KNIFE_CSS:
		return u8"\uE1F7";
	case WEAPON_KNIFE_FLIP:
		return u8"\uE1F9";
	case WEAPON_KNIFE_GUT:
		return u8"\uE1FA";
	case WEAPON_KNIFE_KARAMBIT:
		return u8"\uE1FB";
	case WEAPON_KNIFE_M9_BAYONET:
		return u8"\uE1FC";
	case WEAPON_KNIFE_TACTICAL:
		return u8"\uE1FD";
	case WEAPON_KNIFE_FALCHION:
		return u8"\uE200";
	case WEAPON_KNIFE_SURVIVAL_BOWIE:
		return u8"\uE202";
	case WEAPON_KNIFE_BUTTERFLY:
		return u8"\uE203";
	case WEAPON_KNIFE_PUSH:
		return u8"\uE204";
	case WEAPON_KNIFE_CORD:
		return u8"\uE205";
	case WEAPON_KNIFE_CANIS:
		return u8"\uE206";
	case WEAPON_KNIFE_URSUS:
		return u8"\uE207";
	case WEAPON_KNIFE_GYPSY_JACKKNIFE:
		return u8"\uE208";
	case WEAPON_KNIFE_OUTDOOR:
		return u8"\uE209";
	case WEAPON_KNIFE_STILETTO:
		return u8"\uE20A";
	case WEAPON_KNIFE_WIDOWMAKER:
		return u8"\uE20B";
	case WEAPON_KNIFE_SKELETON:
		return u8"\uE20D";
	default:
		return u8"\u003F";
	}
}

float TBaseEntity::GetMaxPlayerSpeed(CBaseEntity* pBaseEntity, CBaseCombatWeapon* BaseWeapon, CCSWeaponData* WeaponData)
{
	float flMaxSpeed = 260.0f;

	switch (BaseWeapon->GetWeaponMode())
	{
	case 0:
	{
		flMaxSpeed = pBaseEntity->IsScoped() ? WeaponData->flMaxSpeed[1] : WeaponData->flMaxSpeed[0];
	}
	break;

	case 1:
	{
		flMaxSpeed = WeaponData->flMaxSpeed[1];
	}
	break;
	}

	return flMaxSpeed;
}
const char* TBaseEntity::GetWeaponNameById(int id)
{
	switch (id)
	{
	case 1:
		return TSTRING("deagle");
	case 2:
		return TSTRING("elite");
	case 3:
		return TSTRING("fiveseven");
	case 4:
		return TSTRING("glock");
	case 7:
		return TSTRING("ak47");
	case 8:
		return TSTRING("aug");
	case 9:
		return TSTRING("awp");
	case 10:
		return TSTRING("famas");
	case 11:
		return TSTRING("g3sg1");
	case 13:
		return TSTRING("galilar");
	case 14:
		return TSTRING("m249");
	case 60:
		return TSTRING("m4a1_silencer");
	case 16:
		return TSTRING("m4a1");
	case 17:
		return TSTRING("mac10");
	case 19:
		return TSTRING("p90");
	case 23:
		return TSTRING("mp5sd");
	case 24:
		return TSTRING("ump45");
	case 25:
		return TSTRING("xm1014");
	case 26:
		return TSTRING("bizon");
	case 27:
		return TSTRING("mag7");
	case 28:
		return TSTRING("negev");
	case 29:
		return TSTRING("sawedoff");
	case 30:
		return TSTRING("tec9");
	case 32:
		return TSTRING("hkp2000");
	case 33:
		return TSTRING("mp7");
	case 34:
		return TSTRING("mp9");
	case 35:
		return TSTRING("nova");
	case 36:
		return TSTRING("p250");
	case 38:
		return TSTRING("scar20");
	case 39:
		return TSTRING("sg556");
	case 40:
		return TSTRING("ssg08");
	case 61:
		return TSTRING("usp_silencer");
	case 63:
		return TSTRING("cz75a");
	case 64:
		return TSTRING("revolver");
	case 508:
		return TSTRING("knife_m9_bayonet");
	case 500:
		return TSTRING("bayonet");
	case 505:
		return TSTRING("knife_flip");
	case 506:
		return TSTRING("knife_gut");
	case 507:
		return TSTRING("knife_karambit");
	case 509:
		return TSTRING("knife_tactical");
	case 512:
		return TSTRING("knife_falchion");
	case 514:
		return TSTRING("knife_survival_bowie");
	case 515:
		return TSTRING("knife_butterfly");
	case 516:
		return TSTRING("knife_push");
	case 519:
		return TSTRING("knife_ursus");
	case 520:
		return TSTRING("knife_gypsy_jackknife");
	case 522:
		return TSTRING("knife_stiletto");
	case 523:
		return TSTRING("knife_widowmaker");
	case WEAPON_KNIFE_CSS:
		return TSTRING("knife_css");
	case WEAPON_KNIFE_CORD:
		return TSTRING("knife_cord");
	case WEAPON_KNIFE_CANIS:
		return TSTRING("knife_canis");
	case WEAPON_KNIFE_OUTDOOR:
		return TSTRING("knife_outdoor");
	case WEAPON_KNIFE_SKELETON:
		return TSTRING("knife_skeleton");
	case 5027:
		return TSTRING("studded_bloodhound_gloves");
	case 5028:
		return TSTRING("t_gloves");
	case 5029:
		return TSTRING("ct_gloves");
	case 5030:
		return TSTRING("sporty_gloves");
	case 5031:
		return TSTRING("slick_gloves");
	case 5032:
		return TSTRING("leather_handwraps");
	case 5033:
		return TSTRING("motorcycle_gloves");
	case 5034:
		return TSTRING("specialist_gloves");
	case 5035:
		return TSTRING("studded_hydra_gloves");

	default:
		return TSTRING("");
	}
}

std::string TBaseEntity::GetLocalizedName(CBaseCombatWeapon* BaseWeapon)
{
	auto pWeaponData = GetWeaponData(BaseWeapon);

	if (!pWeaponData)
		return TSTRING("ERROR");

	return TUtil::WideToMultiByte(TInterfaces::Localize->Find(pWeaponData->szHudName));
}

float CBaseEntity::GetMaxPlayerSpeed()
{
	CBaseCombatWeapon* pWeapon = TBaseEntity::GetWeapon(this);

	if (pWeapon)
	{
		auto pWeaponData = TBaseEntity::GetWeaponData(pWeapon);

		if (pWeaponData)
			return this->IsScoped() ? pWeaponData->flMaxSpeed[1] : pWeaponData->flMaxSpeed[0];
	}

	return 260.0f;
}

float& CBaseEntity::GetLastBoneSetupTime()
{
	static auto uInvalidateBoneCache = TUtil::FindPattern(CLIENT_STR, TSTRING("80 3D ?? ?? ?? ?? ?? 74 16 A1 ?? ?? ?? ?? 48 C7 81"));
	static auto uLastBoneSetupTime = *(uintptr_t*)(uInvalidateBoneCache + 0x11);
	return *(float*)((uintptr_t)this + uLastBoneSetupTime);
}

uint32_t& CBaseEntity::GetMostRecentModelBoneCounter()
{
	static auto uInvalidateBoneCache = TUtil::FindPattern(CLIENT_STR, TSTRING("80 3D ?? ?? ?? ?? ?? 74 16 A1 ?? ?? ?? ?? 48 C7 81"));
	static auto uMostRecentModelBoneCounter = *(uintptr_t*)(uInvalidateBoneCache + 0x1B);
	return *(uint32_t*)((uintptr_t)this + uMostRecentModelBoneCounter);
}