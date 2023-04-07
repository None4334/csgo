
#define ANIMATION_LAYER_COUNT 13


class matrix3x4a_t;
class CBaseAnimating;
class CBoneAccessor
{
public:
	matrix3x4_t* GetBoneArrayForWrite()
	{
		return m_aBoneArray;
	}

	void SetBoneArrayForWrite(matrix3x4_t* bone_array)
	{
		m_aBoneArray = bone_array;
	}

	int GetReadableBones()
	{
		return m_ReadableBones;
	}

	void SetReadableBones(int flags)
	{
		m_ReadableBones = flags;
	}

	int GetWritableBones()
	{
		return m_WritableBones;
	}

	void SetWritableBones(int flags)
	{
		m_WritableBones = flags;
	}

	alignas(16) matrix3x4_t* m_aBoneArray;
	int m_ReadableBones;
	int m_WritableBones;
};



class CBaseEntity;
class CBaseCombatWeapon;
class CCSGOPlayerAnimState
{
public:
	void* m_pThis;
	bool					m_bIsReset;
	bool					m_bUnknownClientBoolean;
	char					m_aSomePad[2];
	int32_t					m_nTick;
	float_t					m_flFlashedStartTime;
	float_t					m_flFlashedEndTime;
	AimLayers_t				m_AimLayers;
	int32_t					m_iModelIndex;
	int32_t					m_iUnknownArray[3];
	CBaseEntity* m_pBasePlayer;
	CBaseCombatWeapon* m_pWeapon;
	CBaseCombatWeapon* m_pWeaponLast;
	float					m_flLastUpdateTime;
	int						m_nLastUpdateFrame;
	float					m_flLastUpdateIncrement;
	float					m_flEyeYaw;
	float					m_flEyePitch;
	float					m_flFootYaw;
	float					m_flFootYawLast;
	float					m_flMoveYaw;
	float					m_flMoveYawIdeal;
	float					m_flMoveYawCurrentToIdeal;
	float					m_flTimeToAlignLowerBody;
	float					m_flPrimaryCycle;
	float					m_flMoveWeight;
	float					m_flMoveWeightSmoothed;
	float					m_flAnimDuckAmount;
	float					m_flDuckAdditional;
	float					m_flRecrouchWeight;
	Vector3D					m_vecPositionCurrent;
	Vector3D					m_vecPositionLast;
	Vector3D					m_vecVelocity;
	Vector3D					m_vecVelocityNormalized;
	Vector3D					m_vecVelocityNormalizedNonZero;
	float					m_flVelocityLengthXY;
	float					m_flVelocityLengthZ;
	float					m_flSpeedAsPortionOfRunTopSpeed;
	float					m_flSpeedAsPortionOfWalkTopSpeed;
	float					m_flSpeedAsPortionOfCrouchTopSpeed;
	float					m_flDurationMoving;
	float					m_flDurationStill;
	bool					m_bOnGround;
	bool					m_bHitGroundAnimation;
	char					m_pad[2];
	float					m_flJumpToFall;
	float					m_flDurationInAir;
	float					m_flLeftGroundHeight;
	float					m_flLandAnimMultiplier;
	float					m_flWalkToRunTransition;
	bool					m_bLandedOnGroundThisFrame;
	bool					m_bLeftTheGroundThisFrame;
	float					m_flInAirSmoothValue;
	bool					m_bOnLadder;
	float					m_flLadderWeight;
	float					m_flLadderSpeed;
	bool					m_bWalkToRunTransitionState;
	bool					m_bDefuseStarted;
	bool					m_bPlantAnimStarted;
	bool					m_bTwitchAnimStarted;
	bool					m_bAdjustStarted;
	char					m_ActivityModifiers[20];
	float					m_flNextTwitchTime;
	float					m_flTimeOfLastKnownInjury;
	float					m_flLastVelocityTestTime;
	Vector3D					m_vecVelocityLast;
	Vector3D					m_vecTargetAcceleration;
	Vector3D					m_vecAcceleration;
	float					m_flAccelerationWeight;
	float					m_flAimMatrixTransition;
	float					m_flAimMatrixTransitionDelay;
	bool					m_bFlashed;
	float					m_flStrafeChangeWeight;
	float					m_flStrafeChangeTargetWeight;
	float					m_flStrafeChangeCycle;
	int						m_nStrafeSequence;
	bool					m_bStrafeChanging;
	float					m_flDurationStrafing;
	float					m_flFootLerp;
	bool					m_bFeetCrossed;
	bool					m_bPlayerIsAccelerating;
	AnimstatePose_t			m_tPoseParamMappings[20];
	float					m_flDurationMoveWeightIsTooHigh;
	float					m_flStaticApproachSpeed;
	int						m_nPreviousMoveState;
	float					m_flStutterStep;
	float					m_flActionWeightBiasRemainder;
	ProceduralFoot_t		m_footLeft;
	ProceduralFoot_t		m_footRight;
	float					m_flCameraSmoothHeight;
	bool					m_bSmoothHeightValid;
	float					m_flLastTimeVelocityOverTen;
	float					m_flAimYawMin;
	float					m_flAimYawMax;
	float					m_flAimPitchMin;
	float					m_flAimPitchMax;
	int						m_nAnimstateModelVersion;

	int SelectSequenceFromActivityModifier(int iActivity);
	float GetLayerIdealWeightFromSeqCycle(CAnimationLayer*);
	void IncrementLayerCycle(CAnimationLayer* pAnimationLayer, bool bIsLoop);
	bool IsLayerSequenceFinished(CAnimationLayer* pAnimationLayer, float_t flTime);
	void SetLayerSequence(CAnimationLayer* pAnimationLayer, int32_t iActivity);
	void SetLayerCycle(CAnimationLayer* pAnimationLayer, float_t flCycle);
	void SetLayerRate(CAnimationLayer* pAnimationLayer, float_t flRate);
	void SetLayerWeight(CAnimationLayer* pAnimationLayer, float_t flWeight);
	void SetLayerWeightRate(CAnimationLayer* pAnimationLayer, float_t flWeightRate);
	void IncrementLayerCycleWeightRateGeneric(CAnimationLayer* pAnimationLayer);
	float GetLayerSequenceCycleRate(CAnimationLayer* AnimationOverlay, int Sequence);
};
