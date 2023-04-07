
class CMoveData
{
public:
	bool			bFirstRunOfFunctions : 1;
	bool			bGameCodeMovedPlayer : 1;
	bool			bNoAirControl : 1;
	std::uintptr_t	hPlayerHandle;		
	int				nImpulseCommand;	
	Vector3D			angViewAngles;		
	Vector3D			angAbsViewAngles;	
	int				nButtons;			
	int				nOldButtons;		
	float			flForwardMove;
	float			flSideMove;
	float			flUpMove;
	float			flMaxSpeed;
	float			flClientMaxSpeed;
	Vector3D			vecVelocity;		
	Vector3D			vecTrailingVelocity;
	float			flTrailingVelocityTime;
	Vector3D			vecAngles;			
	Vector3D			vecOldAngles;
	float			flOutStepHeight;	
	Vector3D			vecOutWishVel;		
	Vector3D			vecOutJumpVel;		
	Vector3D			vecConstraintCenter;
	float			flConstraintRadius;
	float			flConstraintWidth;
	float			flConstraintSpeedFactor;
	bool			bConstraintPastRadius;
	Vector3D			vecAbsOrigin;
};

class CBaseEntity;
class IPhysicsSurfaceProps;
class CGameTrace;
enum ESoundLevel;
class IMoveHelper
{
public:
	virtual	const char* GetName(void* hEntity) const = 0;
	virtual void				SetHost(CBaseEntity* pHost) = 0;
	virtual void				ResetTouchList() = 0;
	virtual bool				AddToTouched(const CGameTrace& trace, const Vector3D& vecImpactVelocity) = 0;
	virtual void				ProcessImpacts() = 0;
	virtual void				Con_NPrintf(int nIndex, char const* fmt, ...) = 0;
	virtual void				StartSound(const Vector3D& vecOrigin, int iChannel, char const* szSample, float flVolume, ESoundLevel soundlevel, int fFlags, int iPitch) = 0;
	virtual void				StartSound(const Vector3D& vecOrigin, const char* soundname) = 0;
	virtual void				PlaybackEventFull(int fFlags, int nClientIndex, unsigned short uEventIndex, float flDelay, Vector3D& vecOrigin, Vector3D& vecAngles, float flParam1, float flParam2, int iParam1, int iParam2, int bParam1, int bParam2) = 0;
	virtual bool				PlayerFallingDamage() = 0;
	virtual void				PlayerSetAnimation(int playerAnimation) = 0;
	virtual IPhysicsSurfaceProps* GetSurfaceProps() = 0;
	virtual bool				IsWorldEntity(const unsigned long& hEntity) = 0;
};

class IGameMovement
{
public:
	virtual						~IGameMovement() { }
	virtual void				ProcessMovement(CBaseEntity* pEntity, CMoveData* pMove) = 0;
	virtual void				Reset() = 0;
	virtual void				StartTrackPredictionErrors(CBaseEntity* pEntity) = 0;
	virtual void				FinishTrackPredictionErrors(CBaseEntity* pEntity) = 0;
	virtual void				DiffPrint(char const* fmt, ...) = 0;
	virtual Vector3D const& GetPlayerMins(bool bDucked) const = 0;
	virtual Vector3D const& GetPlayerMaxs(bool bDucked) const = 0;
	virtual Vector3D const& GetPlayerViewOffset(bool bDucked) const = 0;
	virtual bool				IsMovingPlayerStuck() const = 0;
	virtual CBaseEntity* GetMovingPlayer() const = 0;
	virtual void				UnblockPusher(CBaseEntity* pEntity, CBaseEntity* pPusher) = 0;
	virtual void				SetupMovementBounds(CMoveData* pMove) = 0;
	
	inline CBaseEntity*& GetPlayer()
	{
		return *(CBaseEntity**)((DWORD)(this) + 0x4);
	}

};

class IPrediction
{
public:
	std::byte		pad0[0x4];						
	std::uintptr_t	hLastGround;					
	bool			bInPrediction;					
	bool			bIsFirstTimePredicted;			
	bool			bEnginePaused;					
	bool			bOldCLPredictValue;			
	int				iPreviousStartFrame;			
	int				nIncomingPacketNumber;			
	float			flLastServerWorldTimeStamp;		

	struct Split_t
	{
		bool		bIsFirstTimePredicted;			
		std::byte	pad0[0x3];						
		int			nCommandsPredicted;				
		int			nServerCommandsAcknowledged;
		int			iPreviousAckHadErrors;			
		float		flIdealPitch;					
		int			iLastCommandAcknowledged;		
		bool		bPreviousAckErrorTriggersFullLatchReset; 
		CUtlVector<CBaseHandle> vecEntitiesWithPredictionErrorsInLastAck; 
		bool		bPerformedTickShift;		
	};

	Split_t			Split[1];					

public:

	void DoPred()
	{
		*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this + 0x24)) = 1;
		*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this + 0x1C)) = 0;
		Split[0].iPreviousAckHadErrors = 1;
		Split[0].nCommandsPredicted = 0;
	}

	void ForceRepredict()
	{
		*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this + 0x1C)) = 0;
		*reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(this + 0x000C)) = -1;
		Split[0].nCommandsPredicted = 0;
		iPreviousStartFrame = -1;
	}

	void Update(int iStartFrame, bool bValidFrame, int nIncomingAcknowledged, int nOutgoingCommand)
	{
		TUtil::CallVFunc<void>(this, 3, iStartFrame, bValidFrame, nIncomingAcknowledged, nOutgoingCommand);
	}

	void GetLocalViewAngles(Vector3D& angView)
	{
		TUtil::CallVFunc<void>(this, 12, std::ref(angView));
	}

	void SetLocalViewAngles(Vector3D& angView)
	{
		TUtil::CallVFunc<void>(this, 13, std::ref(angView));
	}

	void CheckMovingGround(CBaseEntity* pEntity, double dbFrametime)
	{
		TUtil::CallVFunc<void>(this, 18, pEntity, dbFrametime);
	}

	void SetupMove(CBaseEntity* pEntity, CUserCmd* pCmd, IMoveHelper* pHelper, CMoveData* pMoveData)
	{
		TUtil::CallVFunc<void>(this, 20, pEntity, pCmd, pHelper, pMoveData);
	}

	void FinishMove(CBaseEntity* pEntity, CUserCmd* pCmd, CMoveData* pMoveData)
	{
		TUtil::CallVFunc<void>(this, 21, pEntity, pCmd, pMoveData);
	}
};

