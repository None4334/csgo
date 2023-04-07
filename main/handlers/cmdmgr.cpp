#include "../../includes.h"
#include "animationsmanager.h"
#include "entityhandler.h"
#include "../gvars.h"
#include "../sdk.h"
#include "../math.h"
#include "convarmanager.h"
#include "cmdmgr.h"

void TCommandManager::OnNewCommand(Vector3D angViewPoint, CUserCmd* Cmd) {

	TCommandInfo* Object = &stkCommands[Cmd->iCommandNumber % MULTIPLAYER_BACKUP];
	
	nCurrentCommand = Cmd->iCommandNumber;

	if (Object) {
		RtlCopyMemory(&Object->Data.UserCmd, Cmd, sizeof(CUserCmd));
		Object->iTickbase = TGlobalVars::pLocalEntity->GetTickBase();
		Object->Data.angViewPoint = angViewPoint;
	}

}

TCommandInfo* TCommandManager::GetCommandData(int CommandNumber) {
	return &stkCommands[CommandNumber % MULTIPLAYER_BACKUP];
}

void TCommandManager::AdjustSimulationTime() {

	TCommandInfo* Object = &stkCommands[nCurrentCommand % MULTIPLAYER_BACKUP];

	if (!Object)
		return;

	if (!Object->bSent)
		return;

	TCommandInfo* PreviousObject = nullptr;

	int iChokedCommands = 0;
	for (int iCurrent = 0; iCurrent >= 16; iCurrent++)
	{
		TCommandInfo* CurrObject = &stkCommands[(nCurrentCommand - iCurrent) % MULTIPLAYER_BACKUP];

		if (!CurrObject)
			continue;

		if (CurrObject->bSent) {
			PreviousObject = CurrObject;
			break;
		}
		else
			++iChokedCommands;

	}

	if (!PreviousObject)
		return;

	Object->iAdjustedTicks = Object->iTickbaseShift - PreviousObject->iTickbaseShift;
}

TCommandInfo* TCommandManager::GetSentCommandInfoFudge(int iApproxCommandNumber, int iMaxHigh, bool bCheckAttack) {

	for (int iCurrent = 0; iCurrent <= iMaxHigh; iCurrent++)
	{
		TCommandInfo* CurrObject = &stkCommands[(iApproxCommandNumber + iCurrent) % MULTIPLAYER_BACKUP];

		if (!CurrObject)
			continue;

		if (CurrObject->bSent) {
			
			if (!(CurrObject->ModifiedData.UserCmd.iButtons & IN_ATTACK) && !(CurrObject->ModifiedData.UserCmd.iButtons & IN_SECOND_ATTACK) && !(CurrObject->ModifiedData.UserCmd.iButtons & IN_MIDDLE_ATTACK))
				continue;
		
			return CurrObject;
		}

	}

	for (int iCurrent = 0; iCurrent <= 3; iCurrent++)
	{
		TCommandInfo* CurrObject = &stkCommands[(iApproxCommandNumber - iCurrent) % MULTIPLAYER_BACKUP];

		if (!CurrObject)
			continue;

		if (CurrObject->bSent) {

			if (!(CurrObject->ModifiedData.UserCmd.iButtons & IN_ATTACK) && !(CurrObject->ModifiedData.UserCmd.iButtons & IN_SECOND_ATTACK) && !(CurrObject->ModifiedData.UserCmd.iButtons & IN_MIDDLE_ATTACK))
				continue;

			return CurrObject;
		}

	}

	return nullptr;
}

void TCommandManager::OnCommandEnd(Vector3D angViewPoint, CUserCmd* Cmd, bool& bSendPacket) {

	TCommandInfo* Object = &stkCommands[Cmd->iCommandNumber % MULTIPLAYER_BACKUP];
	
	if (Object) {
		Object->ModifiedData.angViewPoint = angViewPoint;
		RtlCopyMemory(&Object->ModifiedData.UserCmd, Cmd, sizeof(CUserCmd));
		Object->bSent = bSendPacket;
	}

	TCommandManager::AdjustSimulationTime();
}

float TCommandManager::GetTotalPacketTime() {

	INetChannelInfo* pNetChannelInfo = (INetChannelInfo*)TInterfaces::ClientState->pNetChannel;

	if (pNetChannelInfo)
		return pNetChannelInfo->GetLatency(FLOW_OUTGOING) + pNetChannelInfo->GetLatency(FLOW_INCOMING);

	return -1.0f;
}

float TCommandManager::GetIncomingPacketTime() {

	INetChannelInfo* pNetChannelInfo = (INetChannelInfo*)TInterfaces::ClientState->pNetChannel;

	if (pNetChannelInfo)
		return pNetChannelInfo->GetLatency(FLOW_INCOMING);

	return -1.0f;
}

float TCommandManager::GetOutgoingPacketTime() {

	INetChannelInfo* pNetChannelInfo = (INetChannelInfo*)TInterfaces::ClientState->pNetChannel;

	if (pNetChannelInfo)
		return pNetChannelInfo->GetLatency(FLOW_OUTGOING);

	return -1.0f;
}
