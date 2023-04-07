#include "../../selfprotect/strencrypt.h"

enum EDispSurfFlags : int
{
	DISPSURF_FLAG_SURFACE = (1 << 0),
	DISPSURF_FLAG_WALKABLE = (1 << 1),
	DISPSURF_FLAG_BUILDABLE = (1 << 2),
	DISPSURF_FLAG_SURFPROP1 = (1 << 3),
	DISPSURF_FLAG_SURFPROP2 = (1 << 4)
};

enum ETraceType : int
{
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
	TRACE_FILTERSKY
};

enum EDebugTraceCounterBehavior : int
{
	TRACE_COUNTER_SET = 0,
	TRACE_COUNTER_INC,
};
#pragma endregion

struct BrushSideInfo_t
{
	VectorAligned	vecPlane;	
	std::uint16_t	uBevel;		
	std::uint16_t	uThin;		
};

struct cplane_t
{
	Vector3D		vecNormal;
	float		flDistance;
	std::byte	dType;
	std::byte	dSignBits;
	std::byte	pad[0x2];
};

struct csurface_t
{
	const char* szName;
	short			nSurfaceProps;
	std::uint16_t	uFlags;
};

class CBaseTrace
{
public:
	CBaseTrace() { }

	Vector3D			vecStart;		// start position
	Vector3D			vecEnd;			// final position
	cplane_t		plane;			// surface normal at impact
	float			flFraction;		// time completed, 1.0 = didn't hit anything
	int				iContents;		// contents on other side of surface hit
	std::uint16_t	fDispFlags;		// displacement flags for marking surfaces with data
	bool			bAllSolid;		// if true, plane is not valid
	bool			bStartSolid;	// if true, the initial point was in a solid area
};

class CBaseEntity;
class CGameTrace : public CBaseTrace
{
public:
	CGameTrace() : pHitEntity(nullptr) { }

	float				flFractionLeftSolid;	// time we left a solid, only valid if we started in solid
	csurface_t			surface;				// surface hit (impact surface)
	int					iHitGroup;				// 0 == generic, non-zero is specific body part
	short				sPhysicsBone;			// physics bone hit by trace in studio
	std::uint16_t		uWorldSurfaceIndex;		// index of the msurface2_t, if applicable
	CBaseEntity* pHitEntity;				// entity hit by trace
	int					iHitbox;				// box hit by trace in studio

	inline bool DidHit() const
	{
		return (flFraction < 1.0f || bAllSolid || bStartSolid);
	}

	inline bool IsVisible() const
	{
		return (flFraction > 0.97f);
	}

private:
	CGameTrace(const CGameTrace& other)
	{
		this->vecStart = other.vecStart;
		this->vecEnd = other.vecEnd;
		this->plane = other.plane;
		this->flFraction = other.flFraction;
		this->iContents = other.iContents;
		this->fDispFlags = other.fDispFlags;
		this->bAllSolid = other.bAllSolid;
		this->bStartSolid = other.bStartSolid;
		this->flFractionLeftSolid = other.flFractionLeftSolid;
		this->surface = other.surface;
		this->iHitGroup = other.iHitGroup;
		this->sPhysicsBone = other.sPhysicsBone;
		this->uWorldSurfaceIndex = other.uWorldSurfaceIndex;
		this->pHitEntity = other.pHitEntity;
		this->iHitbox = other.iHitbox;
	}
public:
	inline void ClearOut()
	{
		std::memset(this, 0, sizeof(CGameTrace));

		flFraction = 1.f;
		surface.szName = "**empty**";
	}
};

using Trace_t = CGameTrace;

struct Ray_t
{
	Ray_t(const Vector3D& vecStart, const Vector3D& vecEnd) :
		vecStart(vecStart), vecDelta(vecEnd - vecStart), matWorldAxisTransform(nullptr), bIsRay(true)
	{
		this->bIsSwept = (this->vecDelta.LengthSqr() != 0.f);
	}

	Ray_t(const Vector3D& vecStart, const Vector3D& vecEnd, const Vector3D& vecMins, const Vector3D& vecMaxs)
	{
		this->vecDelta = vecEnd - vecStart;

		this->matWorldAxisTransform = nullptr;
		this->bIsSwept = (this->vecDelta.LengthSqr() != 0.f);

		this->vecExtents = vecMaxs - vecMins;
		this->vecExtents *= 0.5f;
		this->bIsRay = (this->vecExtents.LengthSqr() < 1e-6f);

		this->vecStartOffset = vecMins + vecMaxs;
		this->vecStartOffset *= 0.5f;
		this->vecStart = vecStart + this->vecStartOffset;
		this->vecStartOffset *= -1.0f;
	}

	VectorAligned		vecStart;
	VectorAligned		vecDelta;
	VectorAligned		vecStartOffset;
	VectorAligned		vecExtents;
	const matrix3x4_t* matWorldAxisTransform;
	bool				bIsRay;
	bool				bIsSwept;
};

class IHandleEntity;
class ITraceFilter
{
public:
	virtual bool ShouldHitEntity(IHandleEntity* pEntity, int fContentsMask) = 0;
	virtual ETraceType GetTraceType() const = 0;
};

class CTraceFilter : public ITraceFilter
{
	using FilterCallbackFn = std::function<bool(IHandleEntity*, int)>;

public:
	// @todo: sig ctracefiltersimple constructor and use it

	CTraceFilter(const IHandleEntity* pSkipEntity, ETraceType iTraceType = TRACE_EVERYTHING)
		: pSkip(pSkipEntity), iTraceType(iTraceType) { }

	CTraceFilter(FilterCallbackFn&& checkCallback, ETraceType iTraceType = TRACE_EVERYTHING)
		: checkCallback(std::move(checkCallback)), iTraceType(iTraceType) { }

	bool ShouldHitEntity(IHandleEntity* pHandleEntity, int fContentsMask) override
	{
		// if given user defined callback - check it
		if (checkCallback != nullptr)
			return checkCallback(pHandleEntity, fContentsMask);

		// otherwise skip entity if given
		return pHandleEntity != pSkip;
	}

	ETraceType GetTraceType() const override
	{
		return iTraceType;
	}

private:
	const IHandleEntity* pSkip = nullptr;
	FilterCallbackFn checkCallback = nullptr;
	ETraceType iTraceType = TRACE_EVERYTHING;
};

class ITraceListData
{
public:
	virtual			~ITraceListData() { }
	virtual void	Reset() = 0;
	virtual bool	IsEmpty() = 0;
	virtual bool	CanTraceRay(const Ray_t& ray) = 0;
};

class IEntityEnumerator
{
public:
	// this gets called with each handle
	virtual bool EnumEntity(IHandleEntity* pHandleEntity) = 0;
};

using ShouldHitFunc_t = bool(__cdecl*)(CBaseEntity*, int);
class CTraceFilterSimple {
private:
	template < typename t = TAddress >
	__forceinline static t GetMethod(TAddress this_ptr, size_t index) {
		return (t)this_ptr.to< t* >()[index];
	}
public:
	void* pTraceFilter;
	const CBaseEntity* pPassEntity;
	int             iCollisionGroup;
	ShouldHitFunc_t pFunction;

public:
	CTraceFilterSimple() {
		static auto TraceLine = TUtil::FindAddress(TSTRING("client.dll"), TSTRING("55 8B EC 83 E4 F0 83 EC 7C 56 52"));
		static auto TraceFilterSimple = TraceLine.add(0x3D).to();
		pTraceFilter = TraceFilterSimple.as< void* >();
		pPassEntity = 0;
		iCollisionGroup = 0;
		pFunction = 0;
	}

	CTraceFilterSimple(const CBaseEntity* pEntityToPass, int m_nCollisionGroup = 0, ShouldHitFunc_t ShouldHitFunc = nullptr)
	{
		static auto TraceLine = TUtil::FindAddress(TSTRING("client.dll"), TSTRING("55 8B EC 83 E4 F0 83 EC 7C 56 52"));
		static auto TraceFilterSimple = TraceLine.add(0x3D).to();
		pTraceFilter = TraceFilterSimple.as< void* >();
		pPassEntity = pEntityToPass;
		iCollisionGroup = m_nCollisionGroup; 
		pFunction = ShouldHitFunc;
	}

	bool ShouldHitEntity(CBaseEntity* entity, int contents_mask) {
		void* pOriginalTraceFilter = *(void**)pTraceFilter;
		return GetMethod< bool(__thiscall*)(void*, CBaseEntity*, int) >(pOriginalTraceFilter, 0)(pOriginalTraceFilter, entity, contents_mask);
	}

	void SetPassEntity(CBaseEntity* pass_ent1) {
		pPassEntity = pass_ent1;
	}

	void SetCollisionGroup(int m_nCollisionGroup) {
		iCollisionGroup = m_nCollisionGroup;
	}
};


struct virtualmeshlist_t;
struct AABB_t;
class ICollideable;
class CPhysCollide;
class CBrushQuery;
class IEngineTrace
{
public:
	virtual int GetPointContents(const Vector3D& vecAbsPosition, int fContentsMask = MASK_ALL, IHandleEntity** ppEntity = nullptr) = 0;
	virtual int GetPointContents_WorldOnly(const Vector3D& vecAbsPosition, int fContentsMask = MASK_ALL) = 0;
	virtual int GetPointContents_Collideable(ICollideable* pCollide, const Vector3D& vecAbsPosition) = 0;
	virtual void ClipRayToEntity(const Ray_t& ray, unsigned int fMask, IHandleEntity* pEntity, Trace_t* pTrace) = 0;
	virtual void ClipRayToCollideable(const Ray_t& ray, unsigned int fMask, ICollideable* pCollide, Trace_t* pTrace) = 0;
	virtual void TraceRay(const Ray_t& ray, unsigned int fMask, ITraceFilter* pTraceFilter, Trace_t* pTrace) = 0;
	virtual void SetupLeafAndEntityListRay(const Ray_t& ray, ITraceListData* pTraceData) = 0;
	virtual void SetupLeafAndEntityListBox(const Vector3D& vecBoxMin, const Vector3D& vecBoxMax, ITraceListData* pTraceData) = 0;
	virtual void TraceRayAgainstLeafAndEntityList(const Ray_t& ray, ITraceListData* pTraceData, unsigned int fMask, ITraceFilter* pTraceFilter, Trace_t* pTrace) = 0;
	virtual void SweepCollideable(ICollideable* pCollide, const Vector3D& vecAbsStart, const Vector3D& vecAbsEnd, const Vector3D& vecAngles, unsigned int fMask, ITraceFilter* pTraceFilter, Trace_t* pTrace) = 0;
	virtual void EnumerateEntities(const Ray_t& ray, bool bTriggers, IEntityEnumerator* pEnumerator) = 0;
	virtual void EnumerateEntities(const Vector3D& vecAbsMins, const Vector3D& vecAbsMaxs, IEntityEnumerator* pEnumerator) = 0;
	virtual ICollideable* GetCollideable(IHandleEntity* pEntity) = 0;
	virtual int GetStatByIndex(int nIndex, bool bClear) = 0;
	virtual void GetBrushesInAABB(const Vector3D& vecMins, const Vector3D& vecMaxs, CUtlVector<int>* pOutput, int fContentsMask = MASK_ALL) = 0;
	virtual CPhysCollide* GetCollidableFromDisplacementsInAABB(const Vector3D& vecMins, const Vector3D& vecMaxs) = 0;
	virtual int GetNumDisplacements() = 0;
	virtual void GetDisplacementMesh(int nIndex, virtualmeshlist_t* pMeshTriList) = 0;
	virtual bool GetBrushInfo(int iBrush, CUtlVector<BrushSideInfo_t>* pBrushSideInfoOut, int* pContentsOut) = 0;
	virtual bool PointOutsideWorld(const Vector3D& vecPoint) = 0;
	virtual int GetLeafContainingPoint(const Vector3D& vecPoint) = 0;
	virtual ITraceListData* AllocTraceListData() = 0;
	virtual void FreeTraceListData(ITraceListData* pListData) = 0;
	virtual int GetSetDebugTraceCounter(int iValue, EDebugTraceCounterBehavior behavior) = 0;
	virtual int GetMeshesFromDisplacementsInAABB(const Vector3D& vecMins, const Vector3D& vecMaxs, virtualmeshlist_t* pOutputMeshes, int nMaxOutputMeshes) = 0;
	virtual void GetBrushesInCollideable(ICollideable* pCollideable, CBrushQuery& BrushQuery) = 0;
	virtual bool IsFullyOccluded(int nOcclusionKey, const AABB_t& aabb1, const AABB_t& aabb2, const Vector3D& vecShadow) = 0;
	virtual void SuspendOcclusionTests() = 0;
	virtual void ResumeOcclusionTests() = 0;
	virtual void FlushOcclusionQueries() = 0;
};
