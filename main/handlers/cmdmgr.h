#include "animationsmanager.h"

struct TCommandData {
	Vector3D angViewPoint;
	Vector3D angModifiedViewPoint;
	CUserCmd UserCmd;
};

struct TShotInfo {
	TEntityObject* EntityObject = nullptr;
	bool bHasShotRecord = false;
	TAnimationRecord AnimationRecord;
};

struct TCommandInfo {

	TCommandInfo() {

	}

	TCommandInfo(Vector3D angViewPoint) {
		Data.angViewPoint = angViewPoint;

	}
	Vector3D vecEyePosition;
	int iTickbase = 0;
	int iAdjustedTicks = 0;
	int iTickbaseShift = 0;
	bool bSent = false;
	bool bLastAttack = false;
	TCommandData Data;
	TCommandData ModifiedData;
	TShotInfo RecordInfo;
};

namespace TCommandManager {

	inline TCommandInfo stkCommands[MULTIPLAYER_BACKUP];
	inline int nCurrentCommand = 0;

	TCommandInfo* GetCommandData(int CommandNumber);

	void OnNewCommand(Vector3D angViewPoint, CUserCmd* Cmd);
	void AdjustSimulationTime();
	void OnCommandEnd(Vector3D angViewPoint, CUserCmd* Cmd,bool& bSendPacket);

	TCommandInfo* GetSentCommandInfoFudge(int iApproxCommandNumber,int iMaxHigh,bool bCheckAttack = false);

	float GetTotalPacketTime();
	float GetIncomingPacketTime();
	float GetOutgoingPacketTime();
}
