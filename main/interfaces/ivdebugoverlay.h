
class OverlayText_t;
class IVDebugOverlay
{
public:
	virtual void			AddEntityTextOverlay(int iEntityIndex, int iLineOffset, float flDuration, int r, int g, int b, int a, const char* fmt, ...) = 0;
	virtual void			AddBoxOverlay(const Vector3D& vecOrigin, const Vector3D& vecAbsMin, const Vector3D& vecAbsMax, const Vector3D& angOrientation, int r, int g, int b, int a, float flDuration) = 0;
	virtual void			AddSphereOverlay(const Vector3D& vecOrigin, float flRadius, int nTheta, int nPhi, int r, int g, int b, int a, float flDuration) = 0;
	virtual void			AddTriangleOverlay(const Vector3D& p1, const Vector3D& p2, const Vector3D& p3, int r, int g, int b, int a, bool bNoDepthTest, float flDuration) = 0;
	virtual void			AddLineOverlay(const Vector3D& vecOrigin, const Vector3D& vecDest, int r, int g, int b, bool bNoDepthTest, float flDuration) = 0;
	virtual void			AddTextOverlay(const Vector3D& vecOrigin, float flDuration, const char* fmt, ...) = 0;
	virtual void			AddTextOverlay(const Vector3D& vecOrigin, int iLineOffset, float flDuration, const char* fmt, ...) = 0;
	virtual void			AddScreenTextOverlay(float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char* szText) = 0;
	virtual void			AddSweptBoxOverlay(const Vector3D& vecStart, const Vector3D& vecEnd, const Vector3D& vecMin, const Vector3D& vecMax, const Vector3D& angles, int r, int g, int b, int a, float flDuration) = 0;
	virtual void			AddGridOverlay(const Vector3D& vecOrigin) = 0;
	virtual void			AddCoordFrameOverlay(const matrix3x4_t& matFrame, float flScale, int vColorTable[3][3] = nullptr) = 0;
	virtual int				ScreenPosition(const Vector3D& vecPoint, Vector3D& vecScreen) = 0;
	virtual int				ScreenPosition(float flXPos, float flYPos, Vector3D& vecScreen) = 0;
	virtual OverlayText_t* GetFirst() = 0;
	virtual OverlayText_t* GetNext(OverlayText_t* pCurrent) = 0;
	virtual void			ClearDeadOverlays() = 0;
	virtual void			ClearAllOverlays() = 0;
	virtual void			AddTextOverlayRGB(const Vector3D& vecOrigin, int iLineOffset, float flDuration, float r, float g, float b, float a, const char* fmt, ...) = 0;
	virtual void			AddTextOverlayRGB(const Vector3D& vecOrigin, int iLineOffset, float flDuration, int r, int g, int b, int a, const char* fmt, ...) = 0;
	virtual void			AddLineOverlayAlpha(const Vector3D& vecOrigin, const Vector3D& dest, int r, int g, int b, int a, bool bNoDepthTest, float flDuration) = 0;
	virtual void			AddBoxOverlay2(const Vector3D& vecOrigin, const Vector3D& vecAbsMin, const Vector3D& vecAbsMax, const Vector3D& angOrientation, const Color& faceColor, const Color& edgeColor, float flDuration) = 0;
	virtual void			AddLineOverlay(const Vector3D& vecOrigin, const Vector3D& vecDest, int r, int g, int b, int a, float flThickness, float flDuration) = 0;
	virtual void			PurgeTextOverlays() = 0;
	virtual void			AddCapsuleOverlay(const Vector3D& vecAbsMin, const Vector3D& vecAbsMax, const float& flRadius, int r, int g, int b, int a, float flDuration) = 0;
	virtual void			DrawPill(Vector3D& vecAbsMin, Vector3D& vecAbsMax, float flRadius, int r, int g, int b, int a, float flDuration) = 0;

	void AddBoxOverlay(const Vector3D& vecOrigin, const Vector3D& vecAbsMin, const Vector3D& vecAbsMax, const Vector3D& angOrientation, Vector4D vecColor, float flDuration) {
		AddBoxOverlay(vecOrigin, vecAbsMin, vecAbsMax, angOrientation, vecColor.x, vecColor.y, vecColor.z, vecColor.w, flDuration);
	}
};


#define	TE_BEAMPOINTS			0	// beam effect between two points
#define TE_SPRITE				1	// additive sprite, plays 1 cycle
#define TE_BEAMDISK				2	// disk that expands to max radius over lifetime
#define TE_BEAMCYLINDER			3	// cylinder that expands to max radius over lifetime
#define TE_BEAMFOLLOW			4	// create a line of decaying beam segments until entity stops moving
#define TE_BEAMRING				5	// connect a beam ring to two entities
#define TE_BEAMSPLINE			6	
#define TE_BEAMRINGPOINT		7	
#define	TE_BEAMLASER			8	// fades according to viewpoint
#define TE_BEAMTESLA			9
#define MAX_BEAM_ENTS			10
#define NOISE_DIVISIONS			128
#pragma endregion

enum EBeamType : unsigned int
{
	FBEAM_STARTENTITY = 0x00000001,
	FBEAM_ENDENTITY = 0x00000002,
	FBEAM_FADEIN = 0x00000004,
	FBEAM_FADEOUT = 0x00000008,
	FBEAM_SINENOISE = 0x00000010,
	FBEAM_SOLID = 0x00000020,
	FBEAM_SHADEIN = 0x00000040,
	FBEAM_SHADEOUT = 0x00000080,
	FBEAM_ONLYNOISEONCE = 0x00000100,	// only calculate our noise once
	FBEAM_NOTILE = 0x00000200,
	FBEAM_USE_HITBOXES = 0x00000400,	// attachment indices represent hitbox indices instead when this is set.
	FBEAM_STARTVISIBLE = 0x00000800,	// has this client actually seen this beam's start entity yet?
	FBEAM_ENDVISIBLE = 0x00001000,	// has this client actually seen this beam's end entity yet?
	FBEAM_ISACTIVE = 0x00002000,
	FBEAM_FOREVER = 0x00004000,
	FBEAM_HALOBEAM = 0x00008000,	// when drawing a beam with a halo, don't ignore the segments and endwidth
	FBEAM_REVERSED = 0x00010000,
};

struct BeamTrail_t
{
	BeamTrail_t* pNext;
	float			flDie;
	Vector3D			vecOrigin;
	Vector3D			vecVelocity;
};

struct Beam_t
{
	Beam_t() = default;

	// Methods of IClientRenderable
	virtual const Vector3D& GetRenderOrigin() = 0;
	virtual const Vector3D& GetRenderAngles() = 0;
	virtual const matrix3x4_t& RenderableToWorldTransform() = 0;
	virtual void			GetRenderBounds(Vector3D& vecMins, Vector3D& vecMaxs) = 0;
	virtual bool			ShouldDraw() = 0;
	virtual bool			IsTransparent() = 0;
	virtual int				DrawModel(int nFlags) = 0;
	virtual void			ComputeFxBlend() = 0;
	virtual int				GetFxBlend() = 0;

	Vector3D			vecMins;
	Vector3D			vecMaxs;
	int* pQueryHandleHalo;
	float			flHaloProxySize;
	Beam_t* pNext;
	int				nType;
	int				nFlags;

	// Control points for the beam
	int				nAttachments;
	Vector3D			vecAttachment[MAX_BEAM_ENTS];
	Vector3D			vecDelta;

	// 0 .. 1 over lifetime of beam
	float			flTime;
	float			flFrequence;

	// Time when beam should die
	float			flDie;
	float			flWidth;
	float			flEndWidth;
	float			flFadeLength;
	float			flAmplitude;
	float			flLife;

	// Color
	float			r, g, b;
	float			flBrightness;

	// Speed
	float			flSpeed;

	// Animation
	float			flFrameRate;
	float			flFrame;
	int				nSegments;

	// Attachment entities for the beam
	CBaseHandle		hEntity[MAX_BEAM_ENTS];
	int				nAttachmentIndex[MAX_BEAM_ENTS];

	// Model info
	int				nModelIndex;
	int				nHaloIndex;
	float			flHaloScale;
	int				iFrameCount;
	float			flRgNoise[NOISE_DIVISIONS + 1];

	// Popcorn trail for beam follows to use
	BeamTrail_t* pTrail;

	// for TE_BEAMRINGPOINT
	float			flStartRadius;
	float			flEndRadius;

	// for FBEAM_ONLYNOISEONCE
	bool			bCalculatedNoise;
	float			flHDRColorScale;
};

struct BeamInfo_t
{
	BeamInfo_t()
	{
		nType = TE_BEAMPOINTS;
		nSegments = -1;
		pszModelName = nullptr;
		pszHaloName = nullptr;
		nModelIndex = -1;
		nHaloIndex = -1;
		bRenderable = true;
		nFlags = 0;
	}

	int				nType;

	// Entities
	CBaseEntity* pStartEntity;
	int				iStartAttachment;
	CBaseEntity* pEndEntity;
	int				iEndAttachment;

	// Points
	Vector3D			vecStart;
	Vector3D			vecEnd;

	int				nModelIndex;
	const char* pszModelName;
	int				nHaloIndex;
	const char* pszHaloName;
	float			flHaloScale;
	float			flLife;
	float			flWidth;
	float			flEndWidth;
	float			flFadeLength;
	float			flAmplitude;
	float			flBrightness;
	float			flSpeed;
	int				iStartFrame;
	float			flFrameRate;
	float			flRed;
	float			flGreen;
	float			flBlue;
	bool			bRenderable;
	int				nSegments;
	int				nFlags;

	// Rings
	Vector3D			vecCenter;
	float			flStartRadius;
	float			flEndRadius;
};
struct TRenderableInstance
{
	std::uint8_t uAlpha;
};

class CBeam;
class IViewRenderBeams
{
public:
	virtual void	InitBeams() = 0;
	virtual void	ShutdownBeams() = 0;
	virtual void	ClearBeams() = 0;
	virtual void	UpdateTempEntBeams() = 0;

	virtual void	DrawBeam(CBeam* pBeam, const TRenderableInstance& instance, ITraceFilter* pEntityBeamTraceFilter = nullptr) = 0;
	virtual void	DrawBeam(Beam_t* pBeam) = 0;

	virtual void	KillDeadBeams(CBaseEntity* pEntity) = 0;

	virtual Beam_t* CreateBeamEnts(BeamInfo_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamEntPoint(BeamInfo_t& beamInfo) = 0;
	virtual	Beam_t* CreateBeamPoints(BeamInfo_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamRing(BeamInfo_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamRingPoint(BeamInfo_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamCirclePoints(BeamInfo_t& beamInfo) = 0;
	virtual Beam_t* CreateBeamFollow(BeamInfo_t& beamInfo) = 0;

	virtual void	FreeBeam(Beam_t* pBeam) = 0;
	virtual void	UpdateBeamInfo(Beam_t* pBeam, BeamInfo_t& beamInfo) = 0;

	virtual void	CreateBeamEnts(int iStartEntity, int iEndEntity, int nModelIndex, int iHaloIndex, float flHaloScale,
		float flLife, float flWidth, float flEndWidth, float flFadeLength, float flAmplitude,
		float flBrightness, float flSpeed, int flStartFrame,
		float flFrameRate, float r, float g, float b, int iType = -1) = 0;
	virtual void	CreateBeamEntPoint(int iStartEntity, const Vector3D* pStart, int iEndEntity, const Vector3D* pEnd,
		int nModelIndex, int iHaloIndex, float flHaloScale,
		float flLife, float flWidth, float flEndWidth, float flFadeLength, float flAmplitude,
		float flBrightness, float flSpeed, int iStartFrame,
		float flFrameRate, float r, float g, float b) = 0;
	virtual void	CreateBeamPoints(Vector3D& vecStart, Vector3D& vecEnd, int nModelIndex, int iHaloIndex, float flHaloScale,
		float flLife, float flWidth, float flEndWidth, float flFadeLength, float flAmplitude,
		float flBrightness, float flSpeed, int iStartFrame,
		float flFrameRate, float r, float g, float b) = 0;
	virtual void	CreateBeamRing(int iStartEntity, int iEndEntity, int nModelIndex, int iHaloIndex, float flHaloScale,
		float flLife, float flWidth, float flEndWidth, float flFadeLength, float flAmplitude,
		float flBrightness, float flSpeed, int iStartFrame,
		float flFrameRate, float r, float g, float b, int iFlags = 0) = 0;
	virtual void	CreateBeamRingPoint(const Vector3D& vecCenter, float flStartRadius, float flEndRadius, int nModelIndex, int iHaloIndex, float flHaloScale,
		float flLife, float flWidth, float flEndWidth, float flFadeLength, float flAmplitude,
		float flBrightness, float flSpeed, int iStartFrame,
		float flFrameRate, float r, float g, float b, int iFlags = 0) = 0;
	virtual void	CreateBeamCirclePoints(int iType, Vector3D& vecStart, Vector3D& vecEnd,
		int nModelIndex, int iHaloIndex, float flHaloScale, float flLife, float flWidth,
		float flEndWidth, float flFadeLength, float flAmplitude, float flBrightness, float flSpeed,
		int iStartFrame, float flFrameRate, float r, float g, float b) = 0;
	virtual void	CreateBeamFollow(int startEnt, int nModelIndex, int iHaloIndex, float flHaloScale,
		float flLife, float flWidth, float flEndWidth, float flFadeLength, float r, float g, float b,
		float flBrightness) = 0;
};


