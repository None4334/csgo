
#define MAX_MULTIPOINTS 10

enum EMultipoint : int {
	MULTIPOINT_CENTER = 0,
	MULTIPOINT_LEFT,
	MULTIPOINT_RIGHT,
	MULTIPOINT_TOP,
	MULTIPOINT_LOWCENTER,
	MULTIPOINT_MAX
};

struct TPointInfo {

	TPointInfo(Vector3D _vecHigher, Vector3D _vecLower, int _Multipoint)
	{
		vecHigher = _vecHigher;
		vecLower = _vecLower;
		Multipoint = _Multipoint;
	}

	TPointInfo(Vector3D _vecHigher, EMultipoint _Multipoint)
	{
		vecHigher = _vecHigher;
		vecLower = Vector3D();
		Multipoint = _Multipoint;
	}

	TPointInfo(EMultipoint _Multipoint) {
		Multipoint = _Multipoint;
	}

	TPointInfo()
	{
		vecHigher= Vector3D();
		vecLower = Vector3D();
		Multipoint = MULTIPOINT_CENTER;
	}
	Vector3D vecHigher;
	Vector3D vecLower;
	int Multipoint = MULTIPOINT_CENTER;
};

struct THitboxInfo {

	void Reset() {
		Index = 0;
		stkMultipoints.clear();
		flDamage = 0.0f;
		pStudioBox = nullptr;
	}

	THitboxInfo() {
		Reset();
	}

	int Index = 0;
	TDynamicStack<TPointInfo, MAX_MULTIPOINTS> stkMultipoints;
	float flDamage = 0.0f;
	mstudiobbox_t* pStudioBox = nullptr;
};

struct TResolvedObject {
	TResolvedObject()
	{
		 LatestInfo = THitboxInfo();
		 OldestInfo = THitboxInfo();
	}

	TResolvedObject(THitboxInfo* Object)
	{
		LatestInfo.Index = Object->Index;
		LatestInfo.pStudioBox = Object->pStudioBox;

		OldestInfo.Index = Object->Index;
		OldestInfo.pStudioBox = Object->pStudioBox;

	}

	THitboxInfo LatestInfo;
	THitboxInfo OldestInfo;
};

struct TTraceInfo {
	Vector3D vecPoint;
	float flDamage;
	int Index = HITBOX_INVALID;
	bool bSafePoint = false;
	TAnimationRecord* AnimationRecord;
	mstudiohitboxset_t* pStudioSet;
	TEntityObject* EntityObject;
};

struct TTracePoint {

	TTracePoint() {
		 flDamage = 0.0f;
		 bSafePoint = false;
		 PointInfo = TPointInfo();
		 Index = HITBOX_INVALID;
	}

	TTracePoint(float _flDamage ,
	bool _bSafePoint,
	TPointInfo _PointInfo,
	int _Index) {
		 flDamage = _flDamage;
		 bSafePoint = _bSafePoint;
		 PointInfo = _PointInfo;
		 Index = _Index;
	}
	float flDamage = 0.0f;
	bool bSafePoint = false;
	TPointInfo PointInfo;
	int Index = HITBOX_INVALID;
};


namespace TRageBot {
	 void Run(CUserCmd* pCmd,CBaseEntity* pBaseEntity);
	 void Clear();

	 bool CanFire(CBaseEntity* pLocalPlayer, CBaseCombatWeapon* pWeapon);

	 namespace THitboxSafety {
		 bool IsSafePoint(Vector3D vecPoint, mstudiohitboxset_t* pStudioSet, int Index, Vector3D vecEyePosition, TAnimationRecord* recAnimation, TEntityObject* EntityObject);
		 bool DoesIntersectHitbox(Vector3D vecPoint, mstudiobbox_t* pStudioBox, matrix3x4_t* matModex, Vector3D vecEyePosition);
		 bool HasMaximumAccuracy(CBaseCombatWeapon* pWeapon, CCSWeaponData* pWeaponData, float flWeaponInAccuracy);
		 float GetHitchance(TAnimationRecord* AnimationRecord, mstudiohitboxset_t* pStudioSet, Vector3D vecEyePosition, Vector3D vecEndPosition, Vector3D vecAimAngle, int iHitbox);
		 bool EvaluateForceSafepointCondition(CBaseEntity* pBaseEntity);
		 bool SafepointEnabledOnHitbox(int Index);
		 void ComputeSeeds() ;
		 inline bool bSeedsComputed = false;
		 inline  std::array<std::tuple<float, float, float, float, float>, 150> arrPrecomputedSeeds;
		 inline std::array<std::tuple<float, float, float, float, float>, 64> arrPrecomputedSmallSeeds;
		 inline bool bShouldForceSafePoint = false;
	 }
	 namespace TMultipointSystem {
		 void AddMultipoints(TEntityObject* EntityObject, mstudiohitboxset_t* pStudioSet , int iHitbox, bool bAdjustForSingleScan);
		 void CopyAndAdjustMultipoints(TEntityObject* EntityObject, mstudiohitboxset_t* pStudioSet,TAnimationRecord*recLatest,TAnimationRecord* recOldest, bool bAdjustForSingleScan);
		 void TransformVector(TEntityObject* EntityObject,TAnimationRecord* recLatest, TAnimationRecord* recOldest);
		 TTraceInfo TraceList(TAnimationRecord* recAnimation, mstudiohitboxset_t* pStudioSet, TDynamicStack<THitboxInfo, HITBOX_MAX>& stkList, TEntityObject* EntityObject, Vector3D vecEyePosition, TTempAnimationRecord* TempRecord, bool bAdjustForSingleScan, float flMinimumDamage = 1);
		 void TraceMultipoints(TEntityObject* EntityObject, mstudiohitboxset_t* pStudioSet, Vector3D vecEyePosition, TTraceInfo& TraceInfo, TAnimationRecord* recLatest, TAnimationRecord* recOldest ,TTempAnimationRecord* TempRecord, bool bAdjustForSingleScan , float flMinimumDamage = 1);
		 TTracePoint TracePoint(TPointInfo Point,int Index, TEntityObject* EntityObject, Vector3D vecEyePosition, TTempAnimationRecord* TempRecord);

	 }
	 inline	TDynamicStack<TTraceInfo, 128> stkTraceInfo;
     inline	TDynamicStack<THitboxInfo,HITBOX_MAX> stkHitboxInfo;
	 inline TDynamicStack<THitboxInfo, HITBOX_MAX> stkLatestAdjustedInfo;
	 inline TDynamicStack<THitboxInfo, HITBOX_MAX> stkOldAdjustedInfo;

	 inline CBaseCombatWeapon* pWeapon;
	 inline CCSWeaponData* pWeaponData;

	 inline bool bEarlyTargetFound = false;
	 inline bool bTargetFound = false;
	 inline bool bRequireAutoStop = false;

	 void EvaluateAutostopCondition();
	 void RunAutostop(CUserCmd* pCmd);

}
