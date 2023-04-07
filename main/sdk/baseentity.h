

class IHandleEntity
{
public:
	virtual							~IHandleEntity() { }
	virtual void					SetRefEHandle(const CBaseHandle& hRef) = 0;
	virtual const CBaseHandle2& GetRefEHandle() const = 0;
};

class IClientUnknown;
class ICollideable
{
public:
	virtual IHandleEntity* GetEntityHandle() = 0;
	virtual const Vector3D& OBBMins() const = 0;
	virtual const Vector3D& OBBMaxs() const = 0;
	virtual void					WorldSpaceTriggerBounds(Vector3D* pVecWorldMins, Vector3D* pVecWorldMaxs) const = 0;
	virtual bool					TestCollision(const Ray_t& ray, unsigned int fContentsMask, Trace_t& tr) = 0;
	virtual bool					TestHitboxes(const Ray_t& ray, unsigned int fContentsMask, Trace_t& tr) = 0;
	virtual int						GetCollisionModelIndex() = 0;
	virtual const Model_t* GetCollisionModel() = 0;
	virtual Vector3D& GetCollisionOrigin() const = 0;
	virtual Vector3D& GetCollisionAngles() const = 0;
	virtual const matrix3x4_t& CollisionToWorldTransform() const = 0;
	virtual ESolidType				GetSolid() const = 0;
	virtual int						GetSolidFlags() const = 0;
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual int						GetCollisionGroup() const = 0;


	virtual void					WorldSpaceSurroundingBounds(Vector3D* pVecMins, Vector3D* pVecMaxs) = 0;
	virtual unsigned int			GetRequiredTriggerFlags() const = 0;
	virtual const matrix3x4_t* GetRootParentToWorldTransform() const = 0;
	virtual void* GetVPhysicsObject() const = 0;
};

class IClientAlphaProperty
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual void SetAlphaModulation(std::uint8_t uAlpha) = 0;
	virtual void SetRenderFX(int nRenderFx, int nRenderMode, float flStartTime = FLT_MAX, float flDuration = 0.0f) = 0;
	virtual void SetFade(float flGlobalFadeScale, float flDistFadeStart, float flDistFadeEnd) = 0;
	virtual void SetDesyncOffset(int nOffset) = 0;
	virtual void EnableAlphaModulationOverride(bool bEnable) = 0;
	virtual void EnableShadowAlphaModulationOverride(bool bEnable) = 0;
	virtual void SetDistanceFadeMode(int nFadeMode) = 0;
};

class IClientNetworkable;
class IClientRenderable;
class IClientEntity;
class CBaseEntity;
class IClientThinkable;
class IClientUnknown : public IHandleEntity
{
public:
	virtual ICollideable* GetCollideable() = 0;
	virtual IClientNetworkable* GetClientNetworkable() = 0;
	virtual IClientRenderable* GetClientRenderable() = 0;
	virtual IClientEntity* GetIClientEntity() = 0;
	virtual CBaseEntity* GetBaseEntity() = 0;
	virtual IClientThinkable* GetClientThinkable() = 0;
	virtual IClientAlphaProperty* GetClientAlphaProperty() = 0;
};

struct RenderableInstance_t
{
	std::uint8_t uAlpha;
};

using ClientShadowHandle_t = std::uint16_t;
using ClientRenderHandle_t = std::uint16_t;
using ModelInstanceHandle_t = std::uint16_t;
class IClientRenderable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual Vector3D& GetRenderOrigin() = 0;
	virtual Vector3D& GetRenderAngles() = 0;
	virtual bool					ShouldDraw() = 0;
	virtual int						GetRenderFlags() = 0;
	virtual bool					IsTransparent() = 0;
	virtual ClientShadowHandle_t	GetShadowHandle() const = 0;
	virtual ClientRenderHandle_t& RenderHandle() = 0;
	virtual const Model_t* GetModel() const = 0;
	virtual int						DrawModel(int nFlags, const RenderableInstance_t& uInstance) = 0;
	virtual int						GetBody() = 0;
	virtual void					GetColorModulation(float* pColor) = 0;
	virtual bool					LODTest() = 0;
	virtual bool					SetupBones(matrix3x4_t* matBoneToWorldOut, int nMaxBones, int fBoneMask, float flCurrentTime) = 0;
	virtual void					SetupWeights(const matrix3x4_t* matBoneToWorld, int nFlexWeightCount, float* flFlexWeights, float* flFlexDelayedWeights) = 0;
	virtual void					DoAnimationEvents() = 0;
	virtual void* GetPVSNotifyInterface() = 0;
	virtual void					GetRenderBounds(Vector3D& vecMins, Vector3D& vecMaxs) = 0;
	virtual void					GetRenderBoundsWorldspace(Vector3D& vecMins, Vector3D& vecMaxs) = 0;
	virtual void					GetShadowRenderBounds(Vector3D& vecMins, Vector3D& vecMaxs, int iShadowType) = 0;
	virtual bool					ShouldReceiveProjectedTextures(int nFlags) = 0;
	virtual bool					GetShadowCastDistance(float* pDistance, int iShadowType) const = 0;
	virtual bool					GetShadowCastDirection(Vector3D* vecDirection, int iShadowType) const = 0;
	virtual bool					IsShadowDirty() = 0;
	virtual void					MarkShadowDirty(bool bDirty) = 0;
	virtual IClientRenderable* GetShadowParent() = 0;
	virtual IClientRenderable* FirstShadowChild() = 0;
	virtual IClientRenderable* NextShadowPeer() = 0;
	virtual int						ShadowCastType() = 0;
	virtual void					unused2() {}
	virtual void					CreateModelInstance() = 0;
	virtual ModelInstanceHandle_t	GetModelInstance() = 0;
	virtual const matrix3x4_t& RenderableToWorldTransform() = 0;
	virtual int						LookupAttachment(const char* pAttachmentName) = 0;
	virtual bool					GetAttachment(int nIndex, Vector3D& vecOrigin, Vector3D& angView) = 0;
	virtual bool					GetAttachment(int nIndex, matrix3x4_t& matAttachment) = 0;
	virtual bool					ComputeLightingOrigin(int nAttachmentIndex, Vector3D vecModelLightingCenter, const matrix3x4_t& matrix, Vector3D& vecTransformedLightingCenter) = 0;
	virtual float* GetRenderClipPlane() = 0;
	virtual int						GetSkin() = 0;
	virtual void					OnThreadedDrawSetup() = 0;
	virtual bool					UsesFlexDelayedWeights() = 0;
	virtual void					RecordToolMessage() = 0;
	virtual bool					ShouldDrawForSplitScreenUser(int nSlot) = 0;
	virtual std::uint8_t			OverrideAlphaModulation(std::uint8_t uAlpha) = 0;
	virtual std::uint8_t			OverrideShadowAlphaModulation(std::uint8_t uAlpha) = 0;
	virtual void* GetClientModelRenderable() = 0;
};

class CBaseClient;
class IClientNetworkable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual void					Release() = 0;
	virtual CBaseClient* GetClientClass() = 0;
	virtual void					NotifyShouldTransmit(int iState) = 0;
	virtual void					OnPreDataChanged(EDataUpdateType updateType) = 0;
	virtual void					OnDataChanged(EDataUpdateType updateType) = 0;
	virtual void					PreDataUpdate(EDataUpdateType updateType) = 0;
	virtual void					PostDataUpdate(EDataUpdateType updateType) = 0;
	virtual void					OnDataUnchangedInPVS() = 0;
	virtual bool					IsDormant() const = 0;
	virtual int						GetIndex() const = 0;
	virtual void					ReceiveMessage(EClassIndex classIndex, bf_read& msg) = 0;
	virtual void* GetDataTableBasePtr() = 0;
	virtual void					SetDestroyedOnRecreateEntities() = 0;
};

class CClientThinkHandle;
using ClientThinkHandle_t = CClientThinkHandle*;
class IClientThinkable
{
public:
	virtual IClientUnknown* GetIClientUnknown() = 0;
	virtual void					ClientThink() = 0;
	virtual ClientThinkHandle_t		GetThinkHandle() = 0;
	virtual void					SetThinkHandle(ClientThinkHandle_t hThink) = 0;
	virtual void					Release() = 0;
};

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	virtual const Vector3D& GetAbsOrigin() const = 0;
	virtual const Vector3D& GetAbsAngles() const = 0;
	virtual void* GetMouth() = 0;
	virtual bool					GetSoundSpatialization(struct SpatializationInfo_t& info) = 0;
	virtual bool					IsBlurred() = 0;

	void SetAbsOrigin(const Vector3D& vecOrigin)
	{
		using SetAbsOriginFn = void(__thiscall*)(void*, const Vector3D&);
		static auto oSetAbsOrigin = reinterpret_cast<SetAbsOriginFn>(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8")));
		oSetAbsOrigin(this, vecOrigin);
	}

	void SetAbsAngles(const Vector3D& angView)
	{
		using SetAbsAngleFn = void(__thiscall*)(void*, const Vector3D&);
		static auto oSetAbsAngles = reinterpret_cast<SetAbsAngleFn>(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1 E8")));
		oSetAbsAngles(this, angView);
	}

	N_ADD_VARIABLE(unsigned, GetModelIndex, "CBaseEntity->m_nModelIndex");

	DataMap_t* GetDataDescMap()
	{
		return TUtil::CallVFunc<DataMap_t*>(this, 15);
	}

	DataMap_t* GetPredictionDescMap()
	{
		return TUtil::CallVFunc<DataMap_t*>(this, 17);
	}

	void SetModelIndex(int nModelIndex)
	{
		TUtil::CallVFunc<void>(this, 75, nModelIndex);
	}
};

class CCSGOPlayerAnimState;
class CBaseCombatWeapon;
class CWeaponCSBase;
class CAnimationLayer;
class CBoneAccessor;
class CIKContext;

class CBoneMergeCache {
public:

	void Init(CBaseEntity* BaseEntity);
	void UpdateCache();
	void ClearCache();
	mstudioposeparamdesc_t* UpdatePoseParamters(CStudioHdr* Hdr, int Value);
	float GetPoseParamValue(CStudioHdr* hdr, int index, float flValue);
	void MergeMatchingPoseParams(float* poses, float* target_poses);

	void CopyToFollow(Vector3D* vecOut, Quaternion* qtOut, int Value, Vector3D* vecIn, Quaternion* qtIn);
	void CopyFromFollow(Vector3D* vecOut, Quaternion* qtOut, int Value, Vector3D* vecIn, Quaternion* qtIn);

	CBaseEntity* m_pOwner;
	CBaseEntity* m_pFollow;
	CStudioHdr* m_pFollowHdr;
	const studiohdr_t* m_pFollowRenderHdr;
	CStudioHdr* m_pOwnerHdr;
	const studiohdr_t* m_pOwnerRenderHdr;
	int				m_nCopiedFramecount;
	int				m_nFollowBoneSetupMask;

	class CMergedBone
	{
	public:
		unsigned short m_iMyBone;		
		unsigned short m_iParentBone;	
	};
	int				m_nOwnerToFollowPoseParamMapping[MAXSTUDIOPOSEPARAM];
	CUtlVector<CMergedBone> m_MergedBones;
	CVarBitVec m_BoneMergeBits;	
	unsigned short m_iRawIndexMapping[256];
	bool m_bForceCacheClear;

	BYTE pad[0x1000];
};


class CBaseViewModel;
class CBaseEntity : public IClientEntity {
public:
	N_ADD_PVARIABLE(float, GetFallVelocity, "CBasePlayer->m_flFallVelocity");
	N_ADD_VARIABLE(Vector3D, GetViewPunch, "CBasePlayer->m_viewPunchAngle");
	N_ADD_VARIABLE(Vector3D, GetPunch, "CBasePlayer->m_aimPunchAngle");
	N_ADD_VARIABLE(Vector3D, GetPunchAngleVelocity, "CBasePlayer->m_aimPunchAngleVel");
	N_ADD_VARIABLE(Vector3D, GetViewOffset, "CBasePlayer->m_vecViewOffset[0]");
	N_ADD_VARIABLE(int, GetTickBase, "CBasePlayer->m_nTickBase");
	N_ADD_PVARIABLE(int, GetNextThinkTick, "CBasePlayer->m_nNextThinkTick");
	N_ADD_VARIABLE(Vector3D, GetVelocity, "CBasePlayer->m_vecVelocity[0]");
	N_ADD_PVARIABLE_OFFSET(Vector3D, GetViewAngles, "CBasePlayer->deadflag", 0x4);
	N_ADD_VARIABLE(CBaseHandle, GetGroundEntityHandle, "CBasePlayer->m_hGroundEntity");
	N_ADD_VARIABLE(int, GetHealth, "CBasePlayer->m_iHealth");
	N_ADD_VARIABLE(int, GetLifeState, "CBasePlayer->m_lifeState");
	N_ADD_VARIABLE(float, GetMaxSpeed, "CBasePlayer->m_flMaxspeed");
	N_ADD_VARIABLE(int, GetFlags, "CBasePlayer->m_fFlags");
	N_ADD_VARIABLE(bool, IsDucking, "CBasePlayer->m_bDucking");
	N_ADD_VARIABLE(bool, IsDucked, "CBasePlayer->m_bDucked");
	N_ADD_PVARIABLE(int, GetObserverMode, "CBasePlayer->m_iObserverMode");
	N_ADD_VARIABLE(CBaseHandle, GetObserverTargetHandle, "CBasePlayer->m_hObserverTarget");
	N_ADD_VARIABLE(CHandle<CBaseViewModel>, GetViewModelHandle, "CBasePlayer->m_hViewModel[0]");
	N_ADD_VARIABLE_OFFSET(int, GetButtonDisabled, "CBasePlayer->m_hViewEntity", -0xC);
	N_ADD_VARIABLE_OFFSET(int, GetButtonForced, "CBasePlayer->m_hViewEntity", -0x8);
	N_ADD_PVARIABLE_OFFSET(CUserCmd*, GetCurrentCommand, "CBasePlayer->m_hViewEntity", -0x4); 

	N_ADD_DATAFIELD(int, GetEFlags, this->GetPredictionDescMap(), "m_iEFlags");
	N_ADD_PDATAFIELD(int, GetButtons, this->GetPredictionDescMap(), "m_nButtons");
	N_ADD_DATAFIELD(int, GetButtonLast, this->GetPredictionDescMap(), "m_afButtonLast");
	N_ADD_DATAFIELD(int, GetButtonPressed, this->GetPredictionDescMap(), "m_afButtonPressed");
	N_ADD_DATAFIELD(int, GetButtonReleased, this->GetPredictionDescMap(), "m_afButtonReleased");
	N_ADD_PDATAFIELD(int, GetImpulse, this->GetPredictionDescMap(), "m_nImpulse");
	N_ADD_DATAFIELD(float, GetSurfaceFriction, this->GetPredictionDescMap(), "m_surfaceFriction");

	N_ADD_DATAFIELD(Vector3D, GetAbsVelocity, this->GetPredictionDescMap(), "m_vecAbsVelocity");

	N_ADD_DATAFIELD(int, GetSkinFromMap, this->GetPredictionDescMap(), "m_nSkin");
	N_ADD_DATAFIELD(int, GetBodyFromMap, this->GetPredictionDescMap(), "m_nBody");

	inline bool IsAlive()
	{
		return (this->GetLifeState() == LIFE_ALIVE);
	}

	bool DuckUntilOnGround()
	{
		return *(bool*)((DWORD)(this) + 0x10478);
	}

	int& GetTakeDamage()
	{
		static const std::uintptr_t uTakeDamageOffset = *reinterpret_cast<std::uintptr_t*>(TUtil::FindPattern(CLIENT_STR, TSTRING("80 BE ? ? ? ? ? 75 46 8B 86")) + 0x2);
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + uTakeDamageOffset);
	}

	CUserCmd& GetLastCommand()
	{
		static const std::uintptr_t uLastCommandOffset = *reinterpret_cast<std::uintptr_t*>(TUtil::FindPattern(CLIENT_STR, TSTRING("8D 8E ? ? ? ? 89 5C 24 3C")) + 0x2);
		return *reinterpret_cast<CUserCmd*>(reinterpret_cast<std::uintptr_t>(this) + uLastCommandOffset);
	}

	CBoneMergeCache** GetBoneMerge() {
		static const size_t uOffset = *reinterpret_cast<size_t*>(TUtil::FindPattern(CLIENT_STR, TSTRING("89 86 ?? ?? ?? ?? E8 ?? ?? ?? ?? FF 75 08")) + 0x2);
		return (CBoneMergeCache**)((uintptr_t)this + uOffset);
	}

	CIKContext** GetIKContext() {
		static const size_t uOffset = *reinterpret_cast<size_t*>(TUtil::FindPattern(TSTRING("client.dll"), TSTRING("8B 8F ?? ?? ?? ?? 89 4C 24 1C")) + 0x2);
		return (CIKContext**)((uintptr_t)this + uOffset);
	}
	N_ADD_VARIABLE(bool, IsWalking, "CCSPlayer->m_bIsWalking");
	N_ADD_VARIABLE(float, GetDuckSpeed, "CCSPlayer->m_flDuckSpeed");
	N_ADD_VARIABLE(float, GetDuckAmount, "CCSPlayer->m_flDuckAmount");
	N_ADD_VARIABLE(float, GetVelocityModifier, "CCSPlayer->m_flVelocityModifier");
	N_ADD_VARIABLE(float, GetThirdPersonRecoil, "CCSPlayer->m_flThirdpersonRecoil");
	N_ADD_VARIABLE(int, GetShotsFired, "CCSPlayer->m_iShotsFired");
	N_ADD_VARIABLE_OFFSET(float, GetSpawnTime, "CCSPlayer->m_iAddonBits", -0x4); 
	N_ADD_VARIABLE(int, GetMoney, "CCSPlayer->m_iAccount");
	N_ADD_VARIABLE(int, GetArmor, "CCSPlayer->m_ArmorValue");
	N_ADD_VARIABLE(Vector3D, GetEyeAngles, "CCSPlayer->m_angEyeAngles");
	N_ADD_VARIABLE(bool, IsDefusing, "CCSPlayer->m_bIsDefusing");
	N_ADD_VARIABLE(bool, IsScoped, "CCSPlayer->m_bIsScoped");
	N_ADD_VARIABLE_OFFSET(CCSGOPlayerAnimState*, GetAnimationState, "CCSPlayer->m_bIsScoped", -0x14); 
	N_ADD_VARIABLE(bool, IsGrabbingHostage, "CCSPlayer->m_bIsGrabbingHostage");
	N_ADD_VARIABLE(bool, IsRescuing, "CCSPlayer->m_bIsRescuing");
	N_ADD_VARIABLE(bool, HasHelmet, "CCSPlayer->m_bHasHelmet");
	N_ADD_VARIABLE(bool, HasHeavyArmor, "CCSPlayer->m_bHasHeavyArmor");
	N_ADD_VARIABLE(bool, HasDefuser, "CCSPlayer->m_bHasDefuser");
	N_ADD_VARIABLE(bool, HasImmunity, "CCSPlayer->m_bGunGameImmunity");
	N_ADD_VARIABLE(bool, IsInBuyZone, "CCSPlayer->m_bInBuyZone");
	N_ADD_PVARIABLE(float, GetFlashMaxAlpha, "CCSPlayer->m_flFlashMaxAlpha");
	N_ADD_VARIABLE_OFFSET(float, GetFlashAlpha, "CCSPlayer->m_flFlashMaxAlpha", -0x8);
	N_ADD_VARIABLE(float, GetFlashDuration, "CCSPlayer->m_flFlashDuration");
	N_ADD_VARIABLE(float, GetLowerBodyYaw, "CCSPlayer->m_flLowerBodyYawTarget");
	N_ADD_VARIABLE(int, GetSurvivalTeam, "CCSPlayer->m_nSurvivalTeam");
	N_ADD_VARIABLE_OFFSET(int, IsUsedNewAnimState, "CCSPlayer->m_flLastExoJumpTime", 0x8);


	float GetMaxAnimationDelta();

	inline bool IsArmored(const int iHitGroup)
	{

		bool bIsArmored = false;

		if (this->GetArmor() > 0)
		{
			switch (iHitGroup)
			{
			case HITGROUP_GENERIC:
			case HITGROUP_CHEST:
			case HITGROUP_STOMACH:
			case HITGROUP_LEFTARM:
			case HITGROUP_RIGHTARM:
			case HITGROUP_NECK:
				bIsArmored = true;
				break;
			case HITGROUP_HEAD:
				if (this->HasHelmet())
					bIsArmored = true;
				[[fallthrough]];
			case HITGROUP_LEFTLEG:
			case HITGROUP_RIGHTLEG:
				if (this->HasHeavyArmor())
					bIsArmored = true;
				break;
			default:
				break;
			}
		}

		return bIsArmored;
	}

	N_ADD_VARIABLE(float, GetAnimationTime, "CBaseEntity->m_flAnimTime");
	N_ADD_VARIABLE(float, GetSimulationTime, "CBaseEntity->m_flSimulationTime");
	N_ADD_VARIABLE_OFFSET(float, GetOldSimulationTime, "CBaseEntity->m_flSimulationTime", 0x4);
	N_ADD_VARIABLE(Vector3D, GetOrigin, "CBaseEntity->m_vecOrigin");
	N_ADD_VARIABLE(Vector3D, GetRotation, "CBaseEntity->m_angRotation");
	N_ADD_VARIABLE(int, GetEffects, "CBaseEntity->m_fEffects");
	N_ADD_VARIABLE(int, GetTeam, "CBaseEntity->m_iTeamNum");
	N_ADD_VARIABLE(CBaseHandle, GetOwnerEntityHandle, "CBaseEntity->m_hOwnerEntity");
	N_ADD_PVARIABLE(ICollideable, GetCollision, "CBaseEntity->m_Collision");
	N_ADD_VARIABLE(int, GetCollisionGroup, "CBaseEntity->m_CollisionGroup");
	N_ADD_PVARIABLE(bool, IsSpotted, "CBaseEntity->m_bSpotted");

	N_ADD_DATAFIELD(Vector3D, GetAbsRotation, this->GetDataDescMap(), "m_angAbsRotation");
	N_ADD_DATAFIELD(const matrix3x4_t, GetCoordinateFrame, this->GetDataDescMap(), "m_rgflCoordinateFrame");
	N_ADD_DATAFIELD(int, GetMoveType, this->GetPredictionDescMap(), "m_MoveType");

	N_ADD_VARIABLE(float, GetNextAttack, "CBaseCombatCharacter->m_flNextAttack");
	N_ADD_VARIABLE(CBaseHandle, GetActiveWeaponHandle, "CBaseCombatCharacter->m_hActiveWeapon");
	N_ADD_PVARIABLE(CBaseHandle2, GetWeaponsHandle, "CBaseCombatCharacter->m_hMyWeapons");
	N_ADD_PVARIABLE(CBaseHandle, GetWearablesHandle, "CBaseCombatCharacter->m_hMyWearables");

	N_ADD_VARIABLE(int, GetSequence, "CBaseAnimating->m_nSequence");

	N_ADD_PVARIABLE_OFFSET(CBoneAccessor, GetBoneAccessor, "CBaseAnimating->m_nForceBone", 0x1C);
	N_ADD_VARIABLE(int, GetHitboxSet, "CBaseAnimating->m_nHitboxSet");
	N_ADD_VARIABLE(bool, IsClientSideAnimation, "CBaseAnimating->m_bClientSideAnimation");
	N_ADD_VARIABLE(float, GetCycle, "CBaseAnimating->m_flCycle");

	[[nodiscard]] std::array<float, MAXSTUDIOPOSEPARAM>& GetPoseParameter()
	{
		static std::uintptr_t m_flPoseParameter = TNetvarManager::mapProps[FNV1A::HashConst("CBaseAnimating->m_flPoseParameter")].uOffset;
		return *reinterpret_cast<std::array<float, MAXSTUDIOPOSEPARAM>*>(reinterpret_cast<std::uintptr_t>(this) + m_flPoseParameter);
	}

	[[nodiscard]] std::array<float, MAXSTUDIOBONECTRLS>& GetEncodedController()
	{
		static std::uintptr_t m_flEncodedController = TNetvarManager::mapProps[FNV1A::HashConst("CBaseAnimating->m_flEncodedController")].uOffset;
		return *reinterpret_cast<std::array<float, MAXSTUDIOBONECTRLS>*>(reinterpret_cast<std::uintptr_t>(this) + m_flEncodedController);
	}

	int& GetFinalPredictedTick() {
		static std::uintptr_t uOffset = TNetvarManager::mapProps[FNV1A::HashConst("CBasePlayer->m_nTickBase")].uOffset + 0x4;
		return *reinterpret_cast<int*>(reinterpret_cast<std::uintptr_t>(this) + uOffset);
	}
	

	inline void SetPoseAngles(float flYaw, float flPitch)
	{
		auto& arrPose = this->GetPoseParameter();
		arrPose.at(11U) = (flPitch + 90.f) / 180.f;
		arrPose.at(2U) = (flYaw + 180.f) / 360.f;
	}

	[[nodiscard]] CUtlVector<CAnimationLayer>& GetAnimationOverlays()
	{
		static const std::uintptr_t uAnimationOverlaysOffset = *reinterpret_cast<std::uintptr_t*>(TUtil::FindPattern(CLIENT_STR, TSTRING("8B 89 ? ? ? ? 8D 0C D1")) + 0x2);
		return *reinterpret_cast<CUtlVector<CAnimationLayer>*>(reinterpret_cast<std::uintptr_t>(this) + uAnimationOverlaysOffset);
	}

	[[nodiscard]] inline CAnimationLayer* GetAnimationLayer(const int nLayer)
	{
		if (nLayer >= 0 && nLayer < 13)
			return &GetAnimationOverlays()[nLayer];

		return nullptr;
	}


	int IsMaxHealth()
	{
		return TUtil::CallVFunc<int>(this, 123);
	}

	void Think()
	{
		TUtil::CallVFunc<void>(this, 139);
	}

	const char* GetClassname()
	{
		return TUtil::CallVFunc<const char*>(this, 143);
	}

	unsigned int PhysicsSolidMaskForEntity()
	{
		return TUtil::CallVFunc<unsigned int>(this, 152);
	}

	bool IsPlayer()
	{
		return TUtil::CallVFunc<bool>(this, 158);
	}

	[[nodiscard]] Vector3D GetEyePosition(bool bShouldCorrect = true)
	{
		Vector3D vecPosition = { };

		TUtil::CallVFunc<void>(this, 169, std::ref(vecPosition));

		if (IsUsedNewAnimState() && bShouldCorrect)
		{
			if (CCSGOPlayerAnimState* pAnimState = this->GetAnimationState(); pAnimState != nullptr)
				ModifyEyePosition(pAnimState, &vecPosition);
		}

		return vecPosition;
	}

	void SetSequence(int iSequence)
	{
		TUtil::CallVFunc<void>(this, 219, iSequence);
	}

	void StudioFrameAdvance()
	{
		TUtil::CallVFunc<void>(this, 220);
	}

	void UpdateClientSideAnimations()
	{
		TUtil::CallVFunc<void>(this, 224);
	}

	void PreThink()
	{
		TUtil::CallVFunc<void>(this, 318);
	}

	void UpdateCollisionBounds()
	{
		TUtil::CallVFunc<void>(this, 340);
	}

	void CalculateIKLocks(float flTime) 
	{
		TUtil::CallVFunc<void>(this, 193, flTime);
	}

	void UpdateDispatchLayer(CAnimationLayer* pOverlay, CStudioHdr* pStudioHdr, int  iSequence) 
	{
		TUtil::CallVFunc<void>(this, 247, pOverlay, pStudioHdr, iSequence);
	}

	bool PhysicsRunThink(int nThinkMethod)
	{
		using PhysicsRunThinkFn = bool(__thiscall*)(void*, int);
		static auto oPhysicsRunThink = reinterpret_cast<PhysicsRunThinkFn>(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 83 EC 10 53 56 57 8B F9 8B 87")));
		return oPhysicsRunThink(this, nThinkMethod);
	}

	float GetMaxPlayerSpeed();

	bool& MaintainSequenceTransitions()
	{
		return *(bool*)(reinterpret_cast<std::uintptr_t>(this) + 0x9F0);
	}

	int& GetClientSideEffects()
	{
		return *(int*)(reinterpret_cast<std::uintptr_t>(this) + 0x68);
	}

	int& GetLastSkippedFrameCount()
	{
		return *(int*)(reinterpret_cast<std::uintptr_t>(this) + 0xA68);
	}

	uint32_t& GetOcclusionFrame()
	{
		return *(uint32_t*)(reinterpret_cast<std::uintptr_t>(this) + 0xA30);
	}

	uint32_t& GetOcclusionFlags()
	{
		return *(uint32_t*)(reinterpret_cast<std::uintptr_t>(this) + 0xA28);
	}

	CUtlVector<matrix3x4_t>& GetCachedBoneData()
	{
		return *(CUtlVector<matrix3x4_t>*)((std::uintptr_t)this + 0x2914);
	}

	const Vector3D& WorldSpaceCenter()
	{
		return TUtil::CallVFunc<const Vector3D&>(this, 79);
	}

	float& GetLastBoneSetupTime();
	uint32_t& GetMostRecentModelBoneCounter();

	void InvalidateBoneCache()
	{
		GetLastBoneSetupTime() = -FLT_MAX;
		GetMostRecentModelBoneCounter() = UINT_MAX;
	}

	CStudioHdr* GetStudioHdr();
	CStudioHdr* GetModelPtr();

	void AttachmentHelper();
	int GetSequenceActivity(int iSequence);


	void InvalidatePhysicsRecursive(int uflags)
	{
		static const auto oInvalidatePhysicsRecursive = reinterpret_cast<void(__thiscall*)(CBaseEntity*, int)>(TUtil::FindPattern(CLIENT_STR,TSTRING("55 8B EC 83 E4 F8 83 EC 0C 53 8B 5D 08 8B C3 56 83 E0 04")));
		oInvalidatePhysicsRecursive(this, uflags);
	}

	N_ADD_VARIABLE(CBaseHandle, GetMoveParent, "CBaseEntity->moveparent");

	/*
	CBaseEntity* GetMoveParent()
	{
		static auto uOffset = TNetvarManager::mapProps[FNV1A::HashConst("CBasePlayer->m_hOwnerEntity")].uOffset;  
		return (CBaseEntity*)TInterfaces::ClientEntityList->GetClientEntityFromHandle(*(CBaseHandle*)(uintptr_t(this) + uOffset - sizeof(CBaseHandle)));
	}
	*/

	bool HasC4();
	bool IsFollowingEntity();

	CBaseEntity* GetFollowedEntity();
	CBaseEntity* FindFollowedEntity();
	bool IsBreakable();

	int GetBoneByHash(const FNV1A_t uBoneHash) const;
	void ModifyEyePosition(const CCSGOPlayerAnimState* pAnimState, Vector3D* vecPosition) const;
	void PostThink();
	bool IsEnemy(CBaseEntity* pEntity);

	bool CanShoot(bool bCheckRevolver = false);

};

class CCSWeaponData
{
public:
	std::byte pad0[0x14];			// 0x0000
	int iMaxClip1;					// 0x0014
	int iMaxClip2;					// 0x0018
	int iDefaultClip1;				// 0x001C
	int iDefaultClip2;				// 0x0020
	int iPrimaryMaxReserveAmmo;		// 0x0024
	int iSecondaryMaxReserveAmmo;	// 0x0028
	const char* szWorldModel;		// 0x002C
	const char* szViewModel;		// 0x0030
	const char* szDroppedModel;		// 0x0034
	std::byte pad1[0x50];			// 0x0038
	const char* szHudName;			// 0x0088
	const char* szWeaponName;		// 0x008C
	std::byte pad2[0x2];			// 0x0090
	bool bIsMeleeWeapon;			// 0x0092
	std::byte pad3[0x9];			// 0x0093
	float flWeaponWeight;			// 0x009C
	std::byte pad4[0x4];			// 0x00A0
	int iSlot;						// 0x00A4
	int iPosition;					// 0x00A8
	std::byte pad5[0x1C];			// 0x00AC
	int nWeaponType;				// 0x00C8
	std::byte pad6[0x4];			// 0x00CC
	int iWeaponPrice;				// 0x00D0
	int iKillAward;					// 0x00D4
	const char* szAnimationPrefix;	// 0x00D8
	float flCycleTime;				// 0x00DC
	float flCycleTimeAlt;			// 0x00E0
	std::byte pad8[0x8];			// 0x00E4
	bool bFullAuto;					// 0x00EC
	std::byte pad9[0x3];			// 0x00ED
	int iDamage;					// 0x00F0
	float flHeadShotMultiplier;		// 0x00F4
	float flArmorRatio;				// 0x00F8
	int iBullets;					// 0x00FC
	float flPenetration;			// 0x0100
	std::byte pad10[0x8];			// 0x0104
	float flRange;					// 0x010C
	float flRangeModifier;			// 0x0110
	float flThrowVelocity;			// 0x0114
	std::byte pad11[0xC];			// 0x0118
	bool bHasSilencer;				// 0x0124
	std::byte pad12[0xF];			// 0x0125
	float flMaxSpeed[2];			// 0x0134
	std::byte pad13[0x4];			// 0x013C
	float flSpread[2];				// 0x0140
	float flInaccuracyCrouch[2];	// 0x0148
	float flInaccuracyStand[2];		// 0x0150
	std::byte pad14[0x8];			// 0x0158
	float flInaccuracyJump[2];		// 0x0160
	float flInaccuracyLand[2];		// 0x0168
	float flInaccuracyLadder[2];	// 0x0170
	float flInaccuracyFire[2];		// 0x0178
	float flInaccuracyMove[2];		// 0x0180
	float flInaccuracyReload;		// 0x0188
	int iRecoilSeed;				// 0x018C
	float flRecoilAngle[2];			// 0x0190
	float flRecoilAngleVariance[2];	// 0x0198
	float flRecoilMagnitude[2];		// 0x01A0
	float flRecoilMagnitudeVariance[2]; // 0x01A8
	int iSpreadSeed;				// 0x01B0

	bool IsGun() const
	{
		switch (this->nWeaponType)
		{
		case WEAPONTYPE_PISTOL:
		case WEAPONTYPE_SUBMACHINEGUN:
		case WEAPONTYPE_RIFLE:
		case WEAPONTYPE_SHOTGUN:
		case WEAPONTYPE_SNIPER:
		case WEAPONTYPE_MACHINEGUN:
			return true;
		}

		return false;
	}
};


class CEconItemView
{
public:
	N_ADD_OFFSET(CUtlVector<IRefCounted*>, GetCustomMaterials, 0x14);

	CUtlVector<CRefCounted*>& GetVisualsDataProcessors()
	{
		static const std::uintptr_t uVisualsDataProcessorsOffset = *reinterpret_cast<std::uintptr_t*>(TUtil::FindPattern(CLIENT_STR, TSTRING("81 C7 ? ? ? ? 8B 4F 0C 8B 57 04 89 4C")) + 0x2);
		return *reinterpret_cast<CUtlVector<CRefCounted*>*>(reinterpret_cast<std::uintptr_t>(this) + uVisualsDataProcessorsOffset);
	}
};

class CBaseCombatWeapon : public IClientEntity
{
public:

	N_ADD_VARIABLE(float, GetNextPrimaryAttack, "CBaseCombatWeapon->m_flNextPrimaryAttack");
	N_ADD_VARIABLE(float, GetNextSecondaryAttack, "CBaseCombatWeapon->m_flNextSecondaryAttack");
	N_ADD_VARIABLE(int, GetAmmo, "CBaseCombatWeapon->m_iClip1");
	N_ADD_VARIABLE(int, GetAmmoReserve, "CBaseCombatWeapon->m_iPrimaryReserveAmmoCount");
	N_ADD_VARIABLE(int, GetViewModelIndex, "CBaseCombatWeapon->m_iViewModelIndex");
	N_ADD_VARIABLE(int, GetWorldModelIndex, "CBaseCombatWeapon->m_iWorldModelIndex");
	N_ADD_VARIABLE(CBaseHandle2, GetWorldModelHandle, "CBaseCombatWeapon->m_hWeaponWorldModel");
	N_ADD_DATAFIELD(bool, IsReloading, this->GetPredictionDescMap(), "m_bInReload");
	N_ADD_VARIABLE(short, GetItemDefinitionIndex, "CBaseAttributableItem->m_iItemDefinitionIndex");
	N_ADD_VARIABLE(int, GetItemIDHigh, "CBaseAttributableItem->m_iItemIDHigh");
	N_ADD_VARIABLE(int, GetItemIDLow, "CBaseAttributableItem->m_iItemIDLow");
	N_ADD_VARIABLE(int, GetAccountID, "CBaseAttributableItem->m_iAccountID");
	N_ADD_VARIABLE(int, GetEntityQuality, "CBaseAttributableItem->m_iEntityQuality");
	N_ADD_PVARIABLE(char, GetCustomName, "CBaseAttributableItem->m_szCustomName");
	N_ADD_VARIABLE(int, GetOwnerXuidLow, "CBaseAttributableItem->m_OriginalOwnerXuidLow");
	N_ADD_VARIABLE(int, GetOwnerXuidHigh, "CBaseAttributableItem->m_OriginalOwnerXuidHigh");
	N_ADD_VARIABLE(int, GetFallbackPaintKit, "CBaseAttributableItem->m_nFallbackPaintKit");
	N_ADD_VARIABLE(int, GetFallbackSeed, "CBaseAttributableItem->m_nFallbackSeed");
	N_ADD_VARIABLE(float, GetFallbackWear, "CBaseAttributableItem->m_flFallbackWear");
	N_ADD_VARIABLE(int, GetFallbackStatTrak, "CBaseAttributableItem->m_nFallbackStatTrak");
	N_ADD_PVARIABLE(CEconItemView, GetEconItemView, "CBaseAttributableItem->m_Item");


	bool IsZoomable(bool bExtraCheck = true)
	{
		return GetItemDefinitionIndex() == WEAPON_SSG08
			|| GetItemDefinitionIndex() == WEAPON_SCAR20
			|| GetItemDefinitionIndex() == WEAPON_AWP
			|| GetItemDefinitionIndex() == WEAPON_G3SG1
			|| (bExtraCheck && (GetItemDefinitionIndex() == WEAPON_SG556 || GetItemDefinitionIndex() == WEAPON_AUG));
	}

	bool IsWeapon()
	{
		return TUtil::CallVFunc<bool>(this, 166);
	}

	void UpdateAccuracyPenalty()
	{
		 TUtil::CallVFunc<void>(this, 484);
	}

	[[nodiscard]] float GetSpread()
	{
		return TUtil::CallVFunc<float>(this, 453);
	}

	[[nodiscard]] float GetInaccuracy()
	{
		return TUtil::CallVFunc<float>(this, 483);
	}

	inline int& GetWeaponMode()
	{
		return *(int*)((DWORD)(this) + 0x3328);
	}

};

class CTEFireBullets
{
public:
	std::byte	pad0[0x10];				
	int			nPlayer;				
	int			nItemDefinitionIndex;	
	Vector3D	vecOrigin;				
	Vector3D	vecAngles;				
	int			iWeapon;			
	int			nWeaponID;				
	int			iMode;					
	int			nSeed;					
	float		flInaccuracy;			
	float		flRecoilIndex;			
	float		flSpread;				
	int			nSoundType;				
}; 

class CWeaponCSBase : public CBaseCombatWeapon
{
public:
	N_ADD_VARIABLE(int, GetZoomLevel, "CWeaponCSBaseGun->m_zoomLevel");
	N_ADD_VARIABLE(int, GetBurstShotsRemaining, "CWeaponCSBaseGun->m_iBurstShotsRemaining");
	N_ADD_VARIABLE(bool, IsBurstMode, "CWeaponCSBase->m_bBurstMode");
	N_ADD_VARIABLE(float, GetAccuracyPenalty, "CWeaponCSBase->m_fAccuracyPenalty");
	N_ADD_VARIABLE(float, GetFireReadyTime, "CWeaponCSBase->m_flPostponeFireReadyTime");
	N_ADD_VARIABLE(float, GetRecoilIndex, "CWeaponCSBase->m_flRecoilIndex");

	CUtlVector<IRefCounted*>& GetCustomMaterials()
	{
		static auto uAddress = *reinterpret_cast<std::uintptr_t*>(TUtil::FindPattern(CLIENT_STR, TSTRING("83 BE ? ? ? ? ? 7F 67")) + 0x2) - 0xC;
		return *reinterpret_cast<CUtlVector<IRefCounted*>*>(reinterpret_cast<std::uintptr_t>(this) + uAddress);
	}

	bool& IsCustomMaterialInitialized()
	{
		static auto uAddress = *reinterpret_cast<std::uintptr_t*>(TUtil::FindPattern(CLIENT_STR, TSTRING("C6 86 ? ? ? ? ? FF 50 04")) + 0x2);
		return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + uAddress);
	}
};

class CBaseCSGrenade : public CWeaponCSBase
{
public:

	N_ADD_VARIABLE(bool, IsPinPulled, "CBaseCSGrenade->m_bPinPulled");
	N_ADD_VARIABLE(float, GetThrowTime, "CBaseCSGrenade->m_fThrowTime");
	N_ADD_VARIABLE(float, GetThrowStrength, "CBaseCSGrenade->m_flThrowStrength");
	N_ADD_VARIABLE(int, GetEffectTickBegin, "CBaseCSGrenadeProjectile->m_nExplodeEffectTickBegin");
	N_ADD_VARIABLE(float, GetBaseGrenadeAnimTime, "CBaseCSGrenadeProjectile->m_flAnimTime");
	N_ADD_VARIABLE(Vector3D, GetVelocity, "CBaseGrenade->m_vecVelocity");
	N_ADD_VARIABLE(CBaseHandle, GetThrower, "CBaseGrenade->m_hThrower");
};

class CBaseViewModel
{
public:

	N_ADD_VARIABLE(int, GetVModelIndex, "CBaseViewModel->m_nModelIndex");
	N_ADD_VARIABLE(CBaseHandle, GetOwnerHandle, "CBaseViewModel->m_hOwner");
	N_ADD_VARIABLE(CHandle<CBaseCombatWeapon>, GetWeaponHandle, "CBaseViewModel->m_hWeapon");
	N_ADD_VARIABLE(int, GetAnimationParity, "CBaseViewModel->m_nAnimationParity");
	N_ADD_VARIABLE(int, GetSequence, "CBaseViewModel->m_nSequence");

	void SendViewModelMatchingSequence(int nSequence)
	{
		TUtil::CallVFunc<void>(this, 247, nSequence);
	}

	void SetWeaponModel(const char* szFileName, CBaseCombatWeapon* pWeapon)
	{
		TUtil::CallVFunc<void>(this, 248, szFileName, pWeapon);
	}
};

class CBaseAttributableItem : public CBaseEntity
{
public:
	N_ADD_VARIABLE(short, GetItemDefinitionIndex, "CBaseAttributableItem->m_iItemDefinitionIndex");
	N_ADD_VARIABLE(int, GetItemIDHigh, "CBaseAttributableItem->m_iItemIDHigh");
	N_ADD_VARIABLE(int, GetItemIDLow, "CBaseAttributableItem->m_iItemIDLow");
	N_ADD_VARIABLE(int, GetAccountID, "CBaseAttributableItem->m_iAccountID");
	N_ADD_VARIABLE(int, GetEntityQuality, "CBaseAttributableItem->m_iEntityQuality");
	N_ADD_VARIABLE(char[32], GetCustomName, "CBaseAttributableItem->m_szCustomName");
	N_ADD_VARIABLE(int, GetOwnerXuidLow, "CBaseAttributableItem->m_OriginalOwnerXuidLow");
	N_ADD_VARIABLE(int, GetOwnerXuidHigh, "CBaseAttributableItem->m_OriginalOwnerXuidHigh");
	N_ADD_VARIABLE(int, GetFallbackPaintKit, "CBaseAttributableItem->m_nFallbackPaintKit");
	N_ADD_VARIABLE(int, GetFallbackSeed, "CBaseAttributableItem->m_nFallbackSeed");
	N_ADD_VARIABLE(float, GetFallbackWear, "CBaseAttributableItem->m_flFallbackWear");
	N_ADD_VARIABLE(int, GetFallbackStatTrak, "CBaseAttributableItem->m_nFallbackStatTrak");
	N_ADD_PVARIABLE(CEconItemView, GetEconItemView, "CBaseAttributableItem->m_Item");

};

class CBreakableSurface : public CBaseEntity, public IBreakableWithPropData
{
public:
	N_ADD_VARIABLE(bool, IsBroken, "CBreakableSurface->m_bIsBroken");
};

class CPlantedC4
{
public:

	N_ADD_VARIABLE(float, GetBlowTime, "CPlantedC4->m_flC4Blow");
	N_ADD_VARIABLE(float, GetTimerLength, "CPlantedC4->m_flTimerLength");
	N_ADD_VARIABLE(float, GetDefuseLength, "CPlantedC4->m_flDefuseLength");
	N_ADD_VARIABLE(float, GetDefuseCountDown, "CPlantedC4->m_flDefuseCountDown");
	N_ADD_VARIABLE(bool, IsPlanted, "CPlantedC4->m_bBombTicking");
	N_ADD_VARIABLE(CBaseHandle, GetDefuserHandle, "CPlantedC4->m_hBombDefuser");
	N_ADD_VARIABLE(bool, IsDefused, "CPlantedC4->m_bBombDefused");

	inline float GetTimer(const float flServerTime)
	{
		return std::clamp(this->GetBlowTime() - flServerTime, 0.0f, this->GetTimerLength());
	}

	inline float GetDefuseTimer(const float flServerTime)
	{
		return std::clamp(this->GetDefuseCountDown() - flServerTime, 0.0f, this->GetDefuseLength());
	}
};

class CInferno
{
public:
	N_ADD_VARIABLE(int, GetEffectTickBegin, "CInferno->m_nFireEffectTickBegin");

	inline float GetMaxTime()
	{
		return 7.f;
	}
};

class CSmokeGrenade
{
public:
	N_ADD_VARIABLE(int, GetEffectTickBegin, "CSmokeGrenadeProjectile->m_nSmokeEffectTickBegin");

	inline float GetMaxTime()
	{
		return 18.f;
	}
};

class CBaseWeaponWorldModel : public CBaseEntity
{
public:
	inline int* GetModelIndex()
	{
		return (int*)((DWORD)this + 0x258);
	}

	CBaseHandle2 GetWorldModel()
	{
		return *(CBaseHandle2*)((uintptr_t)this + 0x3254);
	}
};

struct TEntityObject;
namespace TBaseEntity {
	Vector3D GetBonePosition(TEntityObject* EntityObject,int nIndex);
	Vector3D GetHitboxPosition(TEntityObject* EntityObject,int nIndex);
	CBaseCombatWeapon* GetWeapon(CBaseEntity* pEntity);
	CCSWeaponData* GetWeaponData(CBaseCombatWeapon* BaseWeapon);
	const char8_t* GetWeaponIcon(short nItemDefinitionIndex);

	std::string GetLocalizedName(CBaseCombatWeapon* BaseWeapon);
	bool IsGrenade(CCSWeaponData* WeaponData);
	int GetHitboxForHitGroup(int iHitgroup);

	const char* GetWeaponNameById(int id);

	float GetMaxPlayerSpeed(CBaseEntity* pBaseEntity, CBaseCombatWeapon* BaseWeapon, CCSWeaponData* WeaponData);

}