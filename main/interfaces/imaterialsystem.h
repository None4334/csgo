
using MaterialHandle_t = std::uint16_t;

using VertexFormat_t = std::uint64_t;

enum EPreviewImageRetVal : int
{
	MATERIAL_PREVIEW_IMAGE_BAD = 0,
	MATERIAL_PREVIEW_IMAGE_OK,
	MATERIAL_NO_PREVIEW_IMAGE,
};

enum EMaterialPropertyType : int
{
	MATERIAL_PROPERTY_NEEDS_LIGHTMAP = 0,
	MATERIAL_PROPERTY_OPACITY,
	MATERIAL_PROPERTY_REFLECTIVITY,
	MATERIAL_PROPERTY_NEEDS_BUMPED_LIGHTMAPS
};

enum EMaterialVarFlags : int
{
	MATERIAL_VAR_DEBUG = (1 << 0),
	MATERIAL_VAR_NO_DEBUG_OVERRIDE = (1 << 1),
	MATERIAL_VAR_NO_DRAW = (1 << 2),
	MATERIAL_VAR_USE_IN_FILLRATE_MODE = (1 << 3),
	MATERIAL_VAR_VERTEXCOLOR = (1 << 4),
	MATERIAL_VAR_VERTEXALPHA = (1 << 5),
	MATERIAL_VAR_SELFILLUM = (1 << 6),
	MATERIAL_VAR_ADDITIVE = (1 << 7),
	MATERIAL_VAR_ALPHATEST = (1 << 8),
	MATERIAL_VAR_PSEUDO_TRANSLUCENT = (1 << 9), 
	MATERIAL_VAR_ZNEARER = (1 << 10),
	MATERIAL_VAR_MODEL = (1 << 11),
	MATERIAL_VAR_FLAT = (1 << 12),
	MATERIAL_VAR_NOCULL = (1 << 13),
	MATERIAL_VAR_NOFOG = (1 << 14),
	MATERIAL_VAR_IGNOREZ = (1 << 15),
	MATERIAL_VAR_DECAL = (1 << 16),
	MATERIAL_VAR_ENVMAPSPHERE = (1 << 17), 
	MATERIAL_VAR_AOPREPASS = (1 << 18),
	MATERIAL_VAR_ENVMAPCAMERASPACE = (1 << 19), 
	MATERIAL_VAR_BASEALPHAENVMAPMASK = (1 << 20),
	MATERIAL_VAR_TRANSLUCENT = (1 << 21),
	MATERIAL_VAR_NORMALMAPALPHAENVMAPMASK = (1 << 22),
	MATERIAL_VAR_NEEDS_SOFTWARE_SKINNING = (1 << 23),
	MATERIAL_VAR_OPAQUETEXTURE = (1 << 24),
	MATERIAL_VAR_MULTIPLY = (1 << 25),
	MATERIAL_VAR_SUPPRESS_DECALS = (1 << 26),
	MATERIAL_VAR_HALFLAMBERT = (1 << 27),
	MATERIAL_VAR_WIREFRAME = (1 << 28),
	MATERIAL_VAR_ALLOWALPHATOCOVERAGE = (1 << 29),
	MATERIAL_VAR_ALPHA_MODIFIED_BY_PROXY = (1 << 30),
	MATERIAL_VAR_VERTEXFOG = (1 << 31)
};

enum ECreateRenderTargetFlags : unsigned int
{
	CREATERENDERTARGETFLAGS_HDR = 0x00000001,
	CREATERENDERTARGETFLAGS_AUTOMIPMAP = 0x00000002,
	CREATERENDERTARGETFLAGS_UNFILTERABLE_OK = 0x00000004,
	CREATERENDERTARGETFLAGS_NOEDRAM = 0x00000008,
	CREATERENDERTARGETFLAGS_TEMP = 0x00000010
};

enum ETextureFlags : unsigned int
{
	TEXTUREFLAGS_POINTSAMPLE = 0x00000001,
	TEXTUREFLAGS_TRILINEAR = 0x00000002,
	TEXTUREFLAGS_CLAMPS = 0x00000004,
	TEXTUREFLAGS_CLAMPT = 0x00000008,
	TEXTUREFLAGS_ANISOTROPIC = 0x00000010,
	TEXTUREFLAGS_HINT_DXT5 = 0x00000020,
	TEXTUREFLAGS_SRGB = 0x00000040,
	TEXTUREFLAGS_NORMAL = 0x00000080,
	TEXTUREFLAGS_NOMIP = 0x00000100,
	TEXTUREFLAGS_NOLOD = 0x00000200,
	TEXTUREFLAGS_ALL_MIPS = 0x00000400,
	TEXTUREFLAGS_PROCEDURAL = 0x00000800,
	TEXTUREFLAGS_ONEBITALPHA = 0x00001000,
	TEXTUREFLAGS_EIGHTBITALPHA = 0x00002000,
	TEXTUREFLAGS_ENVMAP = 0x00004000,
	TEXTUREFLAGS_RENDERTARGET = 0x00008000,
	TEXTUREFLAGS_DEPTHRENDERTARGET = 0x00010000,
	TEXTUREFLAGS_NODEBUGOVERRIDE = 0x00020000,
	TEXTUREFLAGS_SINGLECOPY = 0x00040000,
	TEXTUREFLAGS_UNUSED_00080000 = 0x00080000,
	TEXTUREFLAGS_UNUSED_00100000 = 0x00100000,
	TEXTUREFLAGS_UNUSED_00200000 = 0x00200000,
	TEXTUREFLAGS_UNUSED_00400000 = 0x00400000,
	TEXTUREFLAGS_NODEPTHBUFFER = 0x00800000,
	TEXTUREFLAGS_UNUSED_01000000 = 0x01000000,
	TEXTUREFLAGS_CLAMPU = 0x02000000,
	TEXTUREFLAGS_VERTEXTEXTURE = 0x04000000,
	TEXTUREFLAGS_SSBUMP = 0x08000000,
	TEXTUREFLAGS_UNUSED_10000000 = 0x10000000,
	TEXTUREFLAGS_BORDER = 0x20000000,
	TEXTUREFLAGS_UNUSED_40000000 = 0x40000000,
	TEXTUREFLAGS_UNUSED_80000000 = 0x80000000
};

enum EClearFlags : unsigned int
{
	VIEW_CLEAR_COLOR = 0x1,
	VIEW_CLEAR_DEPTH = 0x2,
	VIEW_CLEAR_FULL_TARGET = 0x4,
	VIEW_NO_DRAW = 0x8,
	VIEW_CLEAR_OBEY_STENCIL = 0x10,
	VIEW_CLEAR_STENCIL = 0x20
};

enum ERenderTargetSizeMode : int
{
	RT_SIZE_NO_CHANGE = 0,					
	RT_SIZE_DEFAULT,						
	RT_SIZE_PICMIP,							
	RT_SIZE_HDR,							
	RT_SIZE_FULL_FRAME_BUFFER,				
	RT_SIZE_OFFSCREEN,						
	RT_SIZE_FULL_FRAME_BUFFER_ROUNDED_UP,	
	RT_SIZE_REPLAY_SCREENSHOT,				
	RT_SIZE_LITERAL							
};

enum EMaterialRenderTargetDepth : unsigned int
{
	MATERIAL_RT_DEPTH_SHARED = 0x0,
	MATERIAL_RT_DEPTH_SEPARATE = 0x1,
	MATERIAL_RT_DEPTH_NONE = 0x2,
	MATERIAL_RT_DEPTH_ONLY = 0x3
};

enum EImageFormat
{
	IMAGE_FORMAT_UNKNOWN = -1,
	IMAGE_FORMAT_RGBA8888 = 0,
	IMAGE_FORMAT_ABGR8888,
	IMAGE_FORMAT_RGB888,
	IMAGE_FORMAT_BGR888,
	IMAGE_FORMAT_RGB565,
	IMAGE_FORMAT_I8,
	IMAGE_FORMAT_IA88,
	IMAGE_FORMAT_P8,
	IMAGE_FORMAT_A8,
	IMAGE_FORMAT_RGB888_BLUESCREEN,
	IMAGE_FORMAT_BGR888_BLUESCREEN,
	IMAGE_FORMAT_ARGB8888,
	IMAGE_FORMAT_BGRA8888,
	IMAGE_FORMAT_DXT1,
	IMAGE_FORMAT_DXT3,
	IMAGE_FORMAT_DXT5,
	IMAGE_FORMAT_BGRX8888,
	IMAGE_FORMAT_BGR565,
	IMAGE_FORMAT_BGRX5551,
	IMAGE_FORMAT_BGRA4444,
	IMAGE_FORMAT_DXT1_ONEBITALPHA,
	IMAGE_FORMAT_BGRA5551,
	IMAGE_FORMAT_UV88,
	IMAGE_FORMAT_UVWQ8888,
	IMAGE_FORMAT_RGBA16161616F,
	IMAGE_FORMAT_RGBA16161616,
	IMAGE_FORMAT_UVLX8888,
	IMAGE_FORMAT_R32F,
	IMAGE_FORMAT_RGB323232F,
	IMAGE_FORMAT_RGBA32323232F,
	IMAGE_FORMAT_RG1616F,
	IMAGE_FORMAT_RG3232F,
	IMAGE_FORMAT_RGBX8888,
	IMAGE_FORMAT_NULL,
	IMAGE_FORMAT_ATI2N,
	IMAGE_FORMAT_ATI1N,
	IMAGE_FORMAT_RGBA1010102,
	IMAGE_FORMAT_BGRA1010102,
	IMAGE_FORMAT_R16F,
	IMAGE_FORMAT_D16,
	IMAGE_FORMAT_D15S1,
	IMAGE_FORMAT_D32,
	IMAGE_FORMAT_D24S8,
	IMAGE_FORMAT_LINEAR_D24S8,
	IMAGE_FORMAT_D24X8,
	IMAGE_FORMAT_D24X4S4,
	IMAGE_FORMAT_D24FS8,
	IMAGE_FORMAT_D16_SHADOW,
	IMAGE_FORMAT_D24X8_SHADOW,
	IMAGE_FORMAT_LINEAR_BGRX8888,
	IMAGE_FORMAT_LINEAR_RGBA8888,
	IMAGE_FORMAT_LINEAR_ABGR8888,
	IMAGE_FORMAT_LINEAR_ARGB8888,
	IMAGE_FORMAT_LINEAR_BGRA8888,
	IMAGE_FORMAT_LINEAR_RGB888,
	IMAGE_FORMAT_LINEAR_BGR888,
	IMAGE_FORMAT_LINEAR_BGRX5551,
	IMAGE_FORMAT_LINEAR_I8,
	IMAGE_FORMAT_LINEAR_RGBA16161616,

	IMAGE_FORMAT_LE_BGRX8888,
	IMAGE_FORMAT_LE_BGRA8888,

	NUM_IMAGE_FORMATS
};

struct MaterialVideoMode_t
{
	int				iWidth;
	int				iHeight;
	EImageFormat	Format;
	int				iRefreshRate;
};

struct MaterialSystemConfig_t
{
	MaterialVideoMode_t VideoMode;
	float				flMonitorGamma;
	float				flGammaTVRangeMin;
	float				flGammaTVRangeMax;
	float				flGammaTVExponent;
	bool				bGammaTVEnabled;
	bool				bTripleBuffered;
	int					nAASamples;
	int					nForceAnisotropicLevel;
	int					iSkipMipLevels;
	int					nDxSupportLevel;
	unsigned int		uFlags;
	bool				bEditMode;
	unsigned char		dProxiesTestMode;
	bool				bCompressedTextures;
	bool				bFilterLightmaps;
	bool				bFilterTextures;
	bool				bReverseDepth;
	bool				bBufferPrimitives;
	bool				bDrawFlat;
	bool				bMeasureFillRate;
	bool				bVisualizeFillRate;
	bool				bNoTransparency;
	bool				bSoftwareLighting;
	bool				bAllowCheats;
	char				nShowMipLevels;
	bool				bShowLowResImage;
	bool				bShowNormalMap;
	bool				bMipMapTextures;
	unsigned char		uFullbright;
	bool				bFastNoBump;
	bool				bSuppressRendering;
	bool				bDrawGray;
	bool				bShowSpecular;
	bool				bShowDiffuse;
	std::uint32_t		uWindowedSizeLimitWidth;
	std::uint32_t		uWindowedSizeLimitHeight;
	int					nAAQuality;
	bool				bShadowDepthTexture;
	bool				bMotionBlur;
	bool				bSupportFlashlight;
	bool				bPaintEnabled;
	std::byte			pad0[0xC];
};

struct Texture_t
{
	std::byte			pad0[0xC];		
	IDirect3DTexture9* lpRawTexture;	
};

class ITexture
{
private:
	std::byte	pad0[0x50];
public:
	Texture_t** pTextureHandles;

	int GetActualWidth()
	{
		return TUtil::CallVFunc<int>(this, 3);
	}

	int GetActualHeight()
	{
		return TUtil::CallVFunc<int>(this, 4);
	}

	void IncrementReferenceCount()
	{
		TUtil::CallVFunc<void>(this, 10);
	}

	void DecrementReferenceCount()
	{
		TUtil::CallVFunc<void>(this, 11);
	}
};


enum ELightType : int
{
	MATERIAL_LIGHT_DISABLE = 0,
	MATERIAL_LIGHT_POINT,
	MATERIAL_LIGHT_DIRECTIONAL,
	MATERIAL_LIGHT_SPOT
};

enum ELightTypeOptimizationFlags
{
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0 = 1,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION1 = 2,
	LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION2 = 4,
	LIGHTTYPE_OPTIMIZATIONFLAGS_DERIVED_VALUES_CALCED = 8
};

struct LightDesc_t
{
	void InitDirectional(const Vector3D& vecDirection, const Vector3D& vecColor)
	{
		this->nType = MATERIAL_LIGHT_DIRECTIONAL;
		this->vecColor = vecColor;
		this->vecDirection = vecDirection;
		this->flRange = 0.0f;
		this->flAttenuation0 = 1.0f;
		this->flAttenuation1 = 0.f;
		this->flAttenuation2 = 0.f;

		fFlags = LIGHTTYPE_OPTIMIZATIONFLAGS_DERIVED_VALUES_CALCED;

		if (flAttenuation0)
			fFlags |= LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION0;
		if (flAttenuation1)
			fFlags |= LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION1;
		if (flAttenuation2)
			fFlags |= LIGHTTYPE_OPTIMIZATIONFLAGS_HAS_ATTENUATION2;

		if (nType == MATERIAL_LIGHT_SPOT)
		{
			flThetaDot = std::cosf(flTheta);
			flPhiDot = std::cosf(flPhi);

			if (const float flSpread = flThetaDot - flPhiDot; flSpread > 1.0e-10f)
				flOneOverThetaDotMinusPhiDot = 1.0f / flSpread;
			else
				flOneOverThetaDotMinusPhiDot = 1.0f;
		}
		else if (nType == MATERIAL_LIGHT_DIRECTIONAL)
		{
			vecPosition = vecDirection;
			vecPosition *= 2.0e6;
		}

		flRangeSquared = flRange * flRange;
	}

	ELightType	nType;
	Vector3D	vecColor;
	Vector3D	vecPosition;
	Vector3D	vecDirection;
	float		flRange;
	float		flFalloff;
	float		flAttenuation0;
	float		flAttenuation1;
	float		flAttenuation2;
	float		flTheta;
	float		flPhi;
	float		flThetaDot;
	float		flPhiDot;
	float		flOneOverThetaDotMinusPhiDot;
	std::uint32_t fFlags;
protected:
	float		flRangeSquared;
};

enum EStencilOperation : int
{
	STENCILOPERATION_KEEP = 1,
	STENCILOPERATION_ZERO = 2,
	STENCILOPERATION_REPLACE = 3,
	STENCILOPERATION_INCRSAT = 4,
	STENCILOPERATION_DECRSAT = 5,
	STENCILOPERATION_INVERT = 6,
	STENCILOPERATION_INCR = 7,
	STENCILOPERATION_DECR = 8,
	STENCILOPERATION_FORCE_DWORD = 0x7FFFFFFF
};

enum EStencilComparisonFunction : int
{
	STENCILCOMPARISONFUNCTION_NEVER = 1,
	STENCILCOMPARISONFUNCTION_LESS = 2,
	STENCILCOMPARISONFUNCTION_EQUAL = 3,
	STENCILCOMPARISONFUNCTION_LESSEQUAL = 4,
	STENCILCOMPARISONFUNCTION_GREATER = 5,
	STENCILCOMPARISONFUNCTION_NOTEQUAL = 6,
	STENCILCOMPARISONFUNCTION_GREATEREQUAL = 7,
	STENCILCOMPARISONFUNCTION_ALWAYS = 8,
	STENCILCOMPARISONFUNCTION_FORCE_DWORD = 0x7FFFFFFF
};

struct ShaderStencilState_t
{
	ShaderStencilState_t()
	{
		bEnable = false;
		PassOperation = FailOperation = ZFailOperation = STENCILOPERATION_KEEP;
		CompareFunction = STENCILCOMPARISONFUNCTION_ALWAYS;
		nReferenceValue = 0;
		uTestMask = uWriteMask = 0xFFFFFFFF;
	}

	bool						bEnable;
	EStencilOperation			FailOperation;
	EStencilOperation			ZFailOperation;
	EStencilOperation			PassOperation;
	EStencilComparisonFunction	CompareFunction;
	int							nReferenceValue;
	std::uint32_t				uTestMask;
	std::uint32_t				uWriteMask;
};

class IMatRenderContext : public IRefCounted
{
public:
	void BeginRender()
	{
		TUtil::CallVFunc<void>(this, 2);
	}

	void EndRender()
	{
		TUtil::CallVFunc<void>(this, 3);
	}

	void BindLocalCubemap(ITexture* pTexture)
	{
		TUtil::CallVFunc<void>(this, 5, pTexture);
	}

	void SetRenderTarget(ITexture* pTexture)
	{
		TUtil::CallVFunc<void>(this, 6, pTexture);
	}

	ITexture* GetRenderTarget()
	{
		return TUtil::CallVFunc<ITexture*>(this, 7);
	}

	void ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false)
	{
		TUtil::CallVFunc<void>(this, 12, bClearColor, bClearDepth, bClearStencil);
	}

	void SetLights(int nCount, const LightDesc_t* pLights)
	{
		TUtil::CallVFunc<void>(this, 17, nCount, pLights);
	}

	void SetAmbientLightCube(Vector4D vecCube[6])
	{
		TUtil::CallVFunc<void>(this, 18, vecCube);
	}

	void Viewport(int x, int y, int iWidth, int iHeight)
	{
		TUtil::CallVFunc<void>(this, 40, x, y, iWidth, iHeight);
	}

	void GetViewport(int& x, int& y, int& iWidth, int& iHeight)
	{
		TUtil::CallVFunc<void>(this, 41, std::ref(x), std::ref(y), std::ref(iWidth), std::ref(iHeight));
	}

	void ClearColor3ub(unsigned char r, unsigned char g, unsigned char b)
	{
		TUtil::CallVFunc<void>(this, 78, r, g, b);
	}

	void ClearColor4ub(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	{
		TUtil::CallVFunc<void>(this, 79, r, g, b, a);
	}

	void DrawScreenSpaceRectangle(IMaterial* pMaterial, int iDestX, int iDestY, int iWidth, int iHeight, float flTextureX0, float flTextureY0, float flTextureX1, float flTextureY1, int iTextureWidth, int iTextureHeight, void* pClientRenderable = nullptr, int nXDice = 1, int nYDice = 1)
	{
		TUtil::CallVFunc<void>(this, 114, pMaterial, iDestX, iDestY, iWidth, iHeight, flTextureX0, flTextureY0, flTextureX1, flTextureY1, iTextureWidth, iTextureHeight, pClientRenderable, nXDice, nYDice);
	}

	void PushRenderTargetAndViewport()
	{
		TUtil::CallVFunc<void>(this, 119);
	}

	void PopRenderTargetAndViewport()
	{
		TUtil::CallVFunc<void>(this, 120);
	}

	void SetLightingOrigin(float x, float y, float z)
	{
		TUtil::CallVFunc<void>(this, 158, x, y, z);
	}

	void SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor = true)
	{
		TUtil::CallVFunc<void>(this, 159, nLeft, nTop, nRight, nBottom, bEnableScissor);
	}

	void PopScissorRect()
	{
		TUtil::CallVFunc<void>(this, 160);
	}
};

class IMaterialVar
{
public:
	ITexture* GetTexture()
	{
		return TUtil::CallVFunc<ITexture*>(this, 1);
	}

	void SetFloat(float flValue)
	{
		TUtil::CallVFunc<void>(this, 4, flValue);
	}

	void SetInt(int iValue)
	{
		TUtil::CallVFunc<void>(this, 5, iValue);
	}

	void SetString(const char* szValue)
	{
		TUtil::CallVFunc<void>(this, 6, szValue);
	}

	void SetVector(float x, float y)
	{
		TUtil::CallVFunc<void>(this, 10, x, y);
	}

	void SetVector(float* Col)
	{
		TUtil::CallVFunc<void>(this, 11, Col[0], Col[1], Col[2]);
	}

	void SetVector(float x, float y, float z)
	{
		TUtil::CallVFunc<void>(this, 11, x, y, z);
	}

	void SetTexture(ITexture* pTexture)
	{
		TUtil::CallVFunc<void>(this, 15, pTexture);
	}

	void SetVectorComponent(float flValue, int iComponent)
	{
		TUtil::CallVFunc<void>(this, 26, flValue, iComponent);
	}
};

class IMaterial
{
public:
	virtual const char* GetName() const = 0;
	virtual const char* GetTextureGroupName() const = 0;
	virtual EPreviewImageRetVal GetPreviewImageProperties(int* iWidth, int* iHeight, EImageFormat* pImageFormat, bool* bTranslucent) const = 0;
	virtual EPreviewImageRetVal GetPreviewImage(unsigned char* pData, int iWidth, int iHeight, EImageFormat imageFormat) const = 0;
	virtual int				GetMappingWidth() = 0;
	virtual int				GetMappingHeight() = 0;
	virtual int				GetNumAnimationFrames() = 0;
	virtual bool			InMaterialPage() = 0;
	virtual	void			GetMaterialOffset(float* flOffset) = 0;
	virtual void			GetMaterialScale(float* flScale) = 0;
	virtual IMaterial* GetMaterialPage() = 0;
	virtual IMaterialVar* FindVar(const char* szName, bool* bFound, bool bComplain = true) = 0;
	virtual void			IncrementReferenceCount() = 0;
	virtual void			DecrementReferenceCount() = 0;
	inline void				AddReference() { IncrementReferenceCount(); }
	inline void				Release() { DecrementReferenceCount(); }
	virtual int 			GetEnumerationID() const = 0;
	virtual void			GetLowResColorSample(float s, float t, float* arrColor) const = 0;
	virtual void			RecomputeStateSnapshots() = 0;
	virtual bool			IsTranslucent() = 0;
	virtual bool			IsAlphaTested() = 0;
	virtual bool			IsVertexLit() = 0;
	virtual VertexFormat_t	GetVertexFormat() const = 0;
	virtual bool			HasProxy() const = 0;
	virtual bool			UsesEnvCubemap() = 0;
	virtual bool			NeedsTangentSpace() = 0;
	virtual bool			NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool			NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) = 0;
	virtual bool			NeedsSoftwareSkinning() = 0;
	virtual void			AlphaModulate(float flAlpha) = 0;
	virtual void			ColorModulate(float r, float g, float b) = 0;
	virtual void			SetMaterialVarFlag(EMaterialVarFlags flag, bool bEnable) = 0;
	virtual bool			GetMaterialVarFlag(EMaterialVarFlags flag) = 0;
	virtual void			GetReflectivity(Vector3D& vecReflect) = 0;
	virtual bool			GetPropertyFlag(EMaterialPropertyType type) = 0;
	virtual bool			IsTwoSided() = 0;
	virtual void			SetShader(const char* szShaderName) = 0;
	virtual int				GetNumPasses() = 0;
	virtual int				GetTextureMemoryBytes() = 0;
	virtual void			Refresh() = 0;
	virtual bool			NeedsLightmapBlendAlpha() = 0;
	virtual bool			NeedsSoftwareLighting() = 0;
	virtual int				ShaderParamCount() const = 0;
	virtual IMaterialVar** GetShaderParams() = 0;
	virtual bool			IsErrorMaterial() const = 0;
	virtual void			SetUseFixedFunctionBakedLighting(bool bEnable) = 0;
	virtual float			GetAlphaModulation() = 0;
	virtual void			GetColorModulation(float* r, float* g, float* b) = 0;
	virtual bool			IsTranslucentUnderModulation(float flAlphaModulation = 1.0f) const = 0;
	virtual IMaterialVar* FindVarFast(char const* szName, unsigned int* puToken) = 0;
	virtual void			SetShaderAndParams(CKeyValues* pKeyValues) = 0;
	virtual const char* GetShaderName() const = 0;
	virtual void			DeleteIfUnreferenced() = 0;
	virtual bool			IsSpriteCard() = 0;
	virtual void			CallBindProxy(void* pProxyData) = 0;
	virtual void			RefreshPreservingMaterialVars() = 0;
	virtual bool			WasReloadedFromWhitelist() = 0;
	virtual bool			SetTempExcluded(bool bSet, int nExcludedDimensionLimit) = 0;
	virtual int				GetReferenceCount() const = 0;

	inline void SetColorModulation(float* Col)
	{
		ColorModulate(Col[0], Col[1], Col[2]);
	}

};


class IMaterialSystem
{
public:
	EImageFormat GetBackBufferFormat()
	{
		return TUtil::CallVFunc<EImageFormat>(this, 36);
	}

	IMaterial* CreateMaterial(const char* szName, CKeyValues* pKeyValues)
	{
		return TUtil::CallVFunc<IMaterial*>(this, 83, szName, pKeyValues);
	}

	IMaterial* FindMaterial(char const* szMaterialName, const char* szTextureGroupName = TEXTURE_GROUP_MODEL, bool bComplain = true, const char* pComplainPrefix = nullptr)
	{
		return TUtil::CallVFunc<IMaterial*>(this, 84, szMaterialName, szTextureGroupName, bComplain, pComplainPrefix);
	}

	MaterialHandle_t FirstMaterial()
	{
		return TUtil::CallVFunc<MaterialHandle_t>(this, 86);
	}

	MaterialHandle_t NextMaterial(MaterialHandle_t hMaterial)
	{
		return TUtil::CallVFunc<MaterialHandle_t>(this, 87, hMaterial);
	}

	MaterialHandle_t InvalidMaterial()
	{
		return TUtil::CallVFunc<MaterialHandle_t>(this, 88);
	}

	IMaterial* GetMaterial(MaterialHandle_t hMaterial)
	{
		return TUtil::CallVFunc<IMaterial*>(this, 89, hMaterial);
	}

	int	GetNumMaterials()
	{
		return TUtil::CallVFunc<int>(this, 90);
	}

	ITexture* FindTexture(char const* szTextureName, const char* szTextureGroupName, bool bComplain = true, int nAdditionalCreationFlags = 0)
	{
		return TUtil::CallVFunc<ITexture*>(this, 91, szTextureName, szTextureGroupName, bComplain, nAdditionalCreationFlags);
	}

	void BeginRenderTargetAllocation()
	{
		TUtil::CallVFunc<void>(this, 94);
	}

	void EndRenderTargetAllocation()
	{
		TUtil::CallVFunc<void>(this, 95);
	}

	void ForceBeginRenderTargetAllocation()
	{
		const bool bOldState = DisableRenderTargetAllocationForever();

		DisableRenderTargetAllocationForever() = false;
		BeginRenderTargetAllocation();
		DisableRenderTargetAllocationForever() = bOldState;
	}

	void ForceEndRenderTargetAllocation()
	{
		const bool bOldState = DisableRenderTargetAllocationForever();

		DisableRenderTargetAllocationForever() = false;
		EndRenderTargetAllocation();
		DisableRenderTargetAllocationForever() = bOldState;
	}

	ITexture* CreateRenderTargetTexture(int iWidth, int iHeight, ERenderTargetSizeMode sizeMode, EImageFormat format, EMaterialRenderTargetDepth depth = MATERIAL_RT_DEPTH_SHARED)
	{
		return TUtil::CallVFunc<ITexture*>(this, 96, iWidth, iHeight, sizeMode, format, depth);
	}

	ITexture* CreateNamedRenderTargetTextureEx(const char* szName, int iWidth, int iHeight, ERenderTargetSizeMode sizeMode, EImageFormat format, EMaterialRenderTargetDepth depth = MATERIAL_RT_DEPTH_SHARED, unsigned int fTextureFlags = 0U, unsigned int fRenderTargetFlags = CREATERENDERTARGETFLAGS_HDR)
	{
		return TUtil::CallVFunc<ITexture*>(this, 97, szName, iWidth, iHeight, sizeMode, format, depth, fTextureFlags, fRenderTargetFlags);
	}

	ITexture* CreateNamedRenderTargetTexture(const char* szName, int iWidth, int iHeight, ERenderTargetSizeMode sizeMode, EImageFormat format, EMaterialRenderTargetDepth depth = MATERIAL_RT_DEPTH_SHARED, bool bClampTexCoords = true, bool bAutoMipMap = false)
	{
		return TUtil::CallVFunc<ITexture*>(this, 98, szName, iWidth, iHeight, sizeMode, format, depth, bClampTexCoords, bAutoMipMap);
	}

	ITexture* CreateNamedRenderTargetTextureEx2(const char* szName, int iWidth, int iHeight, ERenderTargetSizeMode sizeMode, EImageFormat format, EMaterialRenderTargetDepth depth = MATERIAL_RT_DEPTH_SHARED, unsigned int fTextureFlags = 0U, unsigned int fRenderTargetFlags = CREATERENDERTARGETFLAGS_HDR)
	{
		return TUtil::CallVFunc<ITexture*>(this, 99, szName, iWidth, iHeight, sizeMode, format, depth, fTextureFlags, fRenderTargetFlags);
	}

	IMatRenderContext* GetRenderContext()
	{
		return TUtil::CallVFunc<IMatRenderContext*>(this, 115);
	}

	void FinishRenderTargetAllocation()
	{
		TUtil::CallVFunc<void>(this, 136);
	}

	void ReEnableRenderTargetAllocation()
	{
		TUtil::CallVFunc<void>(this, 137);
	}

	bool& DisableRenderTargetAllocationForever()
	{
		static auto uDisableRenderTargetAllocationForever = *reinterpret_cast<std::uintptr_t*>(TUtil::FindPattern(TSTRING("materialsystem.dll"), TSTRING("80 B9 ? ? ? ? ? 74 0F")) + 0x2);
		return *reinterpret_cast<bool*>(reinterpret_cast<std::uintptr_t>(this) + uDisableRenderTargetAllocationForever);
	}
};


class CMatRenderContextPtr : public CRefPtr<IMatRenderContext>
{
	typedef CRefPtr<IMatRenderContext> CBaseClass;
public:
	CMatRenderContextPtr() = default;

	CMatRenderContextPtr(IMatRenderContext* pInit) : CBaseClass(pInit)
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->BeginRender();
	}

	CMatRenderContextPtr(IMaterialSystem* pFrom) : CBaseClass(pFrom->GetRenderContext())
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->BeginRender();
	}

	~CMatRenderContextPtr()
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->EndRender();
	}

	IMatRenderContext* operator=(IMatRenderContext* pSecondContext)
	{
		if (pSecondContext != nullptr)
			pSecondContext->BeginRender();

		return CBaseClass::operator=(pSecondContext);
	}

	void SafeRelease()
	{
		if (CBaseClass::pObject != nullptr)
			CBaseClass::pObject->EndRender();

		CBaseClass::SafeRelease();
	}

	void AssignAddReference(IMatRenderContext* pFrom)
	{
		if (CBaseClass::pObject)
			CBaseClass::pObject->EndRender();

		CBaseClass::AssignAddReference(pFrom);
		CBaseClass::pObject->BeginRender();
	}

	void GetFrom(IMaterialSystem* pFrom)
	{
		AssignAddReference(pFrom->GetRenderContext());
	}

private:
	CMatRenderContextPtr(const CMatRenderContextPtr& pRefPtr);
	void operator=(const CMatRenderContextPtr& pSecondRefPtr);
};



