


struct TNetvarData
{
	Vector3D  vecPunch;
	Vector3D vecPunchVelocity;
	Vector3D vecViewPunchAngle;
	Vector3D vecViewOffset;
	Vector3D vecVelocity;
	Vector3D vecOrigin;
	int	iTick = 0;
	int nCommandNumber = INT_MAX;
	int nTickBase = 0;
	float  fllDuckAmount = 0.0f; 
	float flThirdpersonRecoil = 0.0f;
	float flDuckSpeed = 0.0f;
	float flVelocityModifier = 0.0f;
	float flFallVelocity = 0.0f;
	bool bFilled = false;
};

namespace TPrediction 
{
	void Initialize();
	void Start(CUserCmd* pCmd, CBaseEntity* pLocal);
	void End(CUserCmd* pCmd, CBaseEntity* pLocal);
	int GetTickBase(CUserCmd* pCmd, CBaseEntity* pLocal);
	float GetFixedVelocityModifier(CBaseEntity* pBasePlayer, int nCommandNumber, bool bBefore);
	void UpdateVelocityModifier();
	inline float flVelocityModifier = 0.0f;

	inline unsigned int* uPredictionRandomSeed = nullptr;
	inline CBaseEntity** pPredictionPlayer = nullptr;
	inline CMoveData moveData = { };
	inline int iOldFlags = 0;
	inline int iOldMoveType = 0;
	inline Vector3D vecOldVelocity;
	inline float flOldCurrentTime = 0.f;
	inline float flOldFrameTime = 0.f;
	inline int iOldTickCount = 0;
	inline bool bOverrideModifier = false;

	namespace TNetvarFix {
		inline TNetvarData arrNetvarData[MULTIPLAYER_BACKUP];
		void DetectPredictionError(TNetvarData* netData, int iTick);
		void Store(TNetvarData* NetData, CCommandContext* CmdContext, int iCommand);
		void Fix(int iCommand);
	}
};

