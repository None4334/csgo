#include "../../includes.h"
#include "animation.h"

void CCSGOPlayerAnimState::IncrementLayerCycle(CAnimationLayer* Layer, bool bIsLoop)
{
	float flNewCycle = (Layer->flPlaybackRate * this->m_flLastUpdateIncrement) + Layer->flCycle;
	if (!bIsLoop && flNewCycle >= 1.0f)
		flNewCycle = 0.999f;

	flNewCycle -= (int)(flNewCycle);
	if (flNewCycle < 0.0f)
		flNewCycle += 1.0f;

	if (flNewCycle > 1.0f)
		flNewCycle -= 1.0f;

	Layer->flCycle = flNewCycle;
}

bool CCSGOPlayerAnimState::IsLayerSequenceFinished(CAnimationLayer* Layer, float flTime)
{
	return (Layer->flPlaybackRate * flTime) + Layer->flCycle >= 1.0f;
}

void CCSGOPlayerAnimState::SetLayerCycle(CAnimationLayer* pAnimationLayer, float flCycle)
{
	if (pAnimationLayer)
		pAnimationLayer->flCycle = flCycle;
}

void CCSGOPlayerAnimState::SetLayerRate(CAnimationLayer* pAnimationLayer, float flRate)
{
	if (pAnimationLayer)
		pAnimationLayer->flPlaybackRate = flRate;
}

void CCSGOPlayerAnimState::SetLayerWeight(CAnimationLayer* pAnimationLayer, float flWeight)
{
	if (pAnimationLayer)
		pAnimationLayer->flWeight = flWeight;
}

void CCSGOPlayerAnimState::SetLayerWeightRate(CAnimationLayer* pAnimationLayer, float flPrevious)
{
	if (pAnimationLayer)
		pAnimationLayer->flWeightDeltaRate = (pAnimationLayer->flWeight - flPrevious) / m_flLastUpdateIncrement;
}

float  CCSGOPlayerAnimState::GetLayerSequenceCycleRate(CAnimationLayer* AnimationOverlay, int Sequence) {
	return TUtil::CallVFunc<float>(this->m_pBasePlayer, 223, AnimationOverlay, Sequence);
}

void CCSGOPlayerAnimState::SetLayerSequence(CAnimationLayer* pAnimationLayer, int iActivity)
{
	int iSequence = this->SelectSequenceFromActivityModifier(iActivity);
	if (iSequence < 2)
		return;

	pAnimationLayer->flCycle = 0.0f;
	pAnimationLayer->flWeight = 0.0f;
	pAnimationLayer->nSequence = iSequence;
	pAnimationLayer->flPlaybackRate = GetLayerSequenceCycleRate(pAnimationLayer, iSequence);
}

float CCSGOPlayerAnimState::GetLayerIdealWeightFromSeqCycle(CAnimationLayer* pAnimationLayer)
{
	static auto  SmoothenedStepBounds = [](float edge0, float edge1, float x) -> float
	{
		x = std::clamp((x - edge0) / (edge1 - edge0), 0.0f, 1.0f);
		return x * x * (3 - 2 * x);
	};

	float flCycle = pAnimationLayer->flCycle;
	if (flCycle >= 0.999f)
		flCycle = 1;

	float flEaseIn = 0.2f;
	float flEaseOut = 0.2f;
	float flIdealWeight = 1;

	if (flEaseIn > 0 && flCycle < flEaseIn)
	{
		flIdealWeight = SmoothenedStepBounds(0, flEaseIn, flCycle);
	}
	else if (flEaseOut < 1 && flCycle > flEaseOut)
	{
		flIdealWeight = SmoothenedStepBounds(1.0f, flEaseOut, flCycle);
	}

	if (flIdealWeight < 0.0015f)
		return 0;

	return std::clamp(flIdealWeight, 0.0f, 1.0f);
}


void CCSGOPlayerAnimState::IncrementLayerCycleWeightRateGeneric(CAnimationLayer* pAnimationLayer)
{
	float flWeightPrevious = pAnimationLayer->flWeight;
	IncrementLayerCycle(pAnimationLayer, false);
	SetLayerWeight(pAnimationLayer, GetLayerIdealWeightFromSeqCycle(pAnimationLayer));
	SetLayerWeightRate(pAnimationLayer, flWeightPrevious);
}

int CCSGOPlayerAnimState::SelectSequenceFromActivityModifier(int iActivity)
{
	bool bIsPlayerDucked = m_flAnimDuckAmount > 0.55f;
	bool bIsPlayerRunning = m_flSpeedAsPortionOfWalkTopSpeed > 0.25f;

	int iLayerSequence = -1;
	switch (iActivity)
	{
	case ACT_CSGO_JUMP:
	{
		iLayerSequence = 15 + static_cast <int>(bIsPlayerRunning);
		if (bIsPlayerDucked)
			iLayerSequence = 17 + static_cast <int>(bIsPlayerRunning);
	}
	break;

	case ACT_CSGO_ALIVE_LOOP:
	{
		iLayerSequence = 8;
		if (m_pWeaponLast != m_pWeapon)
			iLayerSequence = 9;
	}
	break;

	case ACT_CSGO_IDLE_ADJUST_STOPPEDMOVING:
	{
		iLayerSequence = 6;
	}
	break;

	case ACT_CSGO_FALL:
	{
		iLayerSequence = 14;
	}
	break;

	case ACT_CSGO_IDLE_TURN_BALANCEADJUST:
	{
		iLayerSequence = 4;
	}
	break;

	case ACT_CSGO_LAND_LIGHT:
	{
		iLayerSequence = 20;
		if (bIsPlayerRunning)
			iLayerSequence = 22;

		if (bIsPlayerDucked)
		{
			iLayerSequence = 21;
			if (bIsPlayerRunning)
				iLayerSequence = 19;
		}
	}
	break;

	case ACT_CSGO_LAND_HEAVY:
	{
		iLayerSequence = 23;
		if (bIsPlayerDucked)
			iLayerSequence = 24;
	}
	break;

	case ACT_CSGO_CLIMB_LADDER:
	{
		iLayerSequence = 13;
	}
	break;
	default: break;
	}

	return iLayerSequence;
}