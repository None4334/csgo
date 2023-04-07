#pragma once

#define TDEF_MAXANIMRECORDS 32

struct TAnimationInstance;
class CAnimationLayer;
struct TAnimationRecord {
public:

	TAnimationRecord() {

	}

	TAnimationRecord(TAnimationInstance* AnimationInstance, TAnimationRecord* AnimationRecords, int nRecordIndex) {
		OwningInstance = AnimationInstance;
		Records = AnimationRecords;
		nCurrentRecordIndex = nRecordIndex;
	}

	void CopyRecord(TAnimationRecord& Other);
	void FillRecord();
	bool IsValid();

	int nCurrentRecordIndex = 0;
	float flSimulationTime = 0.0f;
	float flCorrectSimulationTime = 0.0f;
	int Flags = 0;
	int nSimulationDifference = 0;
	Vector3D vecOrigin;
	Vector3D vecAbsAngles;
	Vector3D vecVelocity;
	Vector3D vecOriginFixed;
	Vector3D vecVelocityFixed;
	matrix3x4_t matModel[MAXSTUDIOBONES];
	CAnimationLayer AnimationLayers[13];

	float flDuckAmount = 0;
	float flLowerBodyYaw = 0;
	matrix3x4_t matLeft[MAXSTUDIOBONES];
	matrix3x4_t matCenter[MAXSTUDIOBONES];
	matrix3x4_t matRight[MAXSTUDIOBONES];

	bool bFilled = false;
	bool bValid = false;
private:
	TAnimationInstance* OwningInstance = nullptr;
	TAnimationRecord* Records = nullptr;
};

class TTempAnimationRecord {
public:
	
	void Apply(TAnimationRecord* AnimationRecord);
	void ApplyMatrix(matrix3x4_t* matModel);
	void Store(CBaseEntity* pBaseEntity);	
	void Restore();

	float flSimulationTime = 0.0f;
	int Flags = 0;
	Vector3D vecOrigin;
	Vector3D vecAbsOrigin;
	Vector3D vecVelocity;
	Vector3D vecAbsVelocity;
	CAnimationLayer AnimationLayers[13];
	matrix3x4_t matBackup[MAXSTUDIOBONES];
	CBaseEntity* pAnimationEntity;
};
struct TAnimationInstance
{
	TAnimationInstance(CBaseEntity* pBaseEntity, int nIndex) {
		pAnimationEntity = pBaseEntity;
		nEntityIndex = nIndex;

		PredictedRecord = TAnimationRecord(this, AnimationRecords, -1);

		for (int Current = 0; Current < TDEF_MAXANIMRECORDS; Current++)
			AnimationRecords[Current] = TAnimationRecord(this, AnimationRecords, Current);
	}


	CBaseEntity* pAnimationEntity = nullptr;
	int nEntityIndex = 0;
	TAnimationRecord PredictedRecord;
	TAnimationRecord AnimationRecords[TDEF_MAXANIMRECORDS];
	matrix3x4_t matCached[MAXSTUDIOBONES];
	Vector3D vecBoneOrigin[MAXSTUDIOBONES];
	int iSetupBonesTick = 0;
	bool bCanLagCompensate = true;
	bool bDormant = false;
	bool bActiveExploits = false;
	void ResetRecords();
};

namespace TAnimationUpdate {
	void OnNetUpdateEnd();
	void UpdateAnimations(TAnimationInstance* AnimationInstance);
	namespace TAnimationCorrection {
		void OnUpdateAnimation(TAnimationInstance* AnimationInstance);
		bool IsPredictionRequired(TAnimationInstance* AnimationInstance);
		void CalculateVelocity(TAnimationInstance* AnimationInstance);
		void SetupBones(CBaseEntity* pBaseEntity,matrix3x4_t* matBoneToWorld,int BoneMask , float flCurrentTime , bool bDisableInterpolation = true);
		void UpdateClientSideAnimations(TAnimationInstance* AnimationInstance);
		void UpdateLayersAndAnimationState(TAnimationInstance* AnimationInstance);
		void PredictAnimations(TAnimationInstance* AnimationInstance);
	}
	void ValidateAndSortRecords(TAnimationInstance* AnimationInstance);
	TAnimationRecord* GetLatestValidRecord(TAnimationInstance* AnimationInstance);
	TAnimationRecord* GetOldestValidRecord(TAnimationInstance* AnimationInstance);
}