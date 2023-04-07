
enum
{
	STUDIORENDER_DRAW_ENTIRE_MODEL = 0,
	STUDIORENDER_DRAW_OPAQUE_ONLY = 0x01,
	STUDIORENDER_DRAW_TRANSLUCENT_ONLY = 0x02,
	STUDIORENDER_DRAW_GROUP_MASK = 0x03,
	STUDIORENDER_DRAW_NO_FLEXES = 0x04,
	STUDIORENDER_DRAW_STATIC_LIGHTING = 0x08,
	STUDIORENDER_DRAW_ACCURATETIME = 0x10,
	STUDIORENDER_DRAW_NO_SHADOWS = 0x20,
	STUDIORENDER_DRAW_GET_PERF_STATS = 0x40,
	STUDIORENDER_DRAW_WIREFRAME = 0x80,
	STUDIORENDER_DRAW_ITEM_BLINK = 0x100,
	STUDIORENDER_SHADOWDEPTHTEXTURE = 0x200,
	STUDIORENDER_SSAODEPTHTEXTURE = 0x1000,
	STUDIORENDER_GENERATE_STATS = 0x8000,
};

enum EOverrideType : int
{
	OVERRIDE_NORMAL = 0,
	OVERRIDE_BUILD_SHADOWS,
	OVERRIDE_DEPTH_WRITE,
	OVERRIDE_SSAO_DEPTH_WRITE
};

struct StudioDecalHandle_t { int iUnused; };

struct MaterialLightingState_t
{
	Vector3D			vecAmbientCube[6];
	Vector3D			vecLightingOrigin;
	int				nLocalLightCount;
	LightDesc_t		localLightDesc[4];
};

struct DrawModelResults_t;
struct ColorMeshInfo_t;

struct studiohwdata_t;
struct StudioDecalHandle_t;
struct DrawModelState_t
{
	studiohdr_t* pStudioHdr;
	studiohwdata_t* pStudioHWData;
	IClientRenderable* pRenderable;
	const matrix3x4_t* pModelToWorld;
	StudioDecalHandle_t* pDecals;
	int						iDrawFlags;
	int						iLOD;
};

struct ModelRenderInfo_t
{
	Vector3D					vecOrigin;
	Vector3D					angAngles;
	std::byte				pad0[0x4];
	IClientRenderable* pRenderable;
	const Model_t* pModel;
	const matrix3x4_t* pModelToWorld;
	const matrix3x4_t* pLightingOffset;
	const Vector3D* pLightingOrigin;
	int						iFlags;
	int						nEntityIndex;
	int						iSkin;
	int						iBody;
	int						iHitboxSet;
	std::uint16_t	hInstance;
};

struct TModelShotRecord {

	TModelShotRecord() {
		RtlZeroMemory(this, sizeof TModelShotRecord);
	}

	int iPlayerIndex = 0;
	matrix3x4_t matModel[MAXSTUDIOBONES];
	matrix3x4_t matModelToWorld[MAXSTUDIOBONES];
	int iTickcount = 0;
	DrawModelState_t state;
	ModelRenderInfo_t info;
};

class IClientAlphaProperty;
struct EntityRenderableInfo_t
{
	IClientRenderable* pRenderable;		// 0x00
	IClientAlphaProperty* pAlphaProperty;		// 0x04
	int						iEnumCount;			// 0x08
	int						nRenderFrame;		// 0x0C
	unsigned short			uFirstShadow;		// 0x10
	unsigned short			uLeafList;			// 0x12
	short					sArea;				// 0x14
	std::uint16_t			uFlags;				// 0x16
	std::uint16_t			uFlags2;			// 0x18
	Vector3D					vecBloatedAbsMins;	// 0x1A
	Vector3D					vecBloatedAbsMaxs;	// 0x26
	Vector3D					vecAbsMins;			// 0x32
	Vector3D					vecAbsMaxs;			// 0x3E
	std::byte				pad0[0x4];			// 0x4A
}; // Size: 0x4E


struct DrawModelInfo_t
{
	studiohdr_t* pStudioHdr;
	studiohwdata_t* pHardwareData;
	StudioDecalHandle_t		hDecals;
	int						iSkin;
	int						iBody;
	int						iHitboxSet;
	IClientRenderable* pClientEntity;
	int						iLOD;
	ColorMeshInfo_t* pColorMeshes;
	bool					bStaticLighting;
	MaterialLightingState_t	lightingState;
};

class IStudioRender
{
public:
	void _SetColorModulation(float const* arrColor)
	{
		TUtil::CallVFunc<void>(this, 27, arrColor);
	}

	void SetAlphaModulation(float flAlpha)
	{
		TUtil::CallVFunc<void>(this, 28, flAlpha);
	}

	void SetColorModulation(float* Col)
	{
		float flColor[] = { Col[0], Col[1], Col[2]};
		_SetColorModulation(Col);
		SetAlphaModulation(Col[3]);
	}

	void SetColorModulation(float r,float g,float b,float a)
	{
		float flColor[] = {r, g, b};
		_SetColorModulation(flColor);
		SetAlphaModulation(a);
	}

	void ForcedMaterialOverride(IMaterial* pMaterial, EOverrideType nOverrideType = OVERRIDE_NORMAL, int nOverrides = 0)
	{
		TUtil::CallVFunc<void>(this, 33, pMaterial, nOverrideType, nOverrides);
	}
};
