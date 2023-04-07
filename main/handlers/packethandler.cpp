#include "../../includes.h"
#include "packethandler.h"
#include "entityhandler.h"
#include "../gvars.h"
#include "../sdk.h"
#include "../math.h"
#include "convarmanager.h"
#include "cmdmgr.h"
#include "eventlogger.h"
#include "../ui/vars.h"

void TPacketHandler::TProcessPacket::FinishCommandProcess(CUserCmd* pCommand,bool& bSendPacket) {

	auto& correct = dqCommandCorrectionData.emplace_front();

	correct.iCommandNumber = pCommand->iCommandNumber;
	correct.iChokedCommands = TInterfaces::ClientState->nChokedCommands + 1;
	correct.iTickCount = TInterfaces::Globals->iTickCount;

	if (bSendPacket)
		dqChokedNumber.clear();
	else
		dqChokedNumber.emplace_back(correct.iCommandNumber);

	while (dqCommandCorrectionData.size() > (int)(2.0f / TInterfaces::Globals->flIntervalPerTick))
		dqCommandCorrectionData.pop_back();

	auto& out = dqCommandFixData.emplace_back();

	out.bIsOutgoing = bSendPacket;
	out.bIsUsed = false;
	out.iCommandNumber = pCommand->iCommandNumber;
	out.iPreviousCommandNumber = 0;

	while (dqCommandFixData.size() > (int)(1.0f / TInterfaces::Globals->flIntervalPerTick))
		dqCommandFixData.pop_front();

	if (!bSendPacket && !TInterfaces::GetGameRules()->IsValveDS())
	{
		if (TInterfaces::ClientState->pNetChannel->iChokedPackets > 0 && !(TInterfaces::ClientState->pNetChannel->iChokedPackets % 4))
		{
			auto backup_choke = TInterfaces::ClientState->pNetChannel->iChokedPackets;
			TInterfaces::ClientState->pNetChannel->iChokedPackets = 0;

			TInterfaces::ClientState->pNetChannel->SendDatagram(NULL);
			--TInterfaces::ClientState->pNetChannel->iOutSequenceNr;

			TInterfaces::ClientState->pNetChannel->iChokedPackets = backup_choke;
		}
	}
}


void TPacketHandler::OnCommandStart(CUserCmd* pCommand, CBaseEntity* pPlayer) {

	CBaseCombatWeapon* pBaseWeapon = TBaseEntity::GetWeapon(pPlayer);

	CCSWeaponData* pWeaponData = nullptr;

	if (pBaseWeapon)
		pWeaponData = TBaseEntity::GetWeaponData(pBaseWeapon);

	if (!pPlayer || !pBaseWeapon || !pWeaponData)
		return;

	if (pBaseWeapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
	{

		const float flServerTime = TICKS_TO_TIME(pPlayer->GetTickBase());

		CWeaponCSBase* pWeaponBase = static_cast<CWeaponCSBase*>(pBaseWeapon);

		if (!pPlayer->CanShoot())
		{
			if ((pCommand->iButtons & IN_ATTACK || pCommand->iButtons & IN_SECOND_ATTACK))
			{
				if (pCommand->iButtons & IN_SECOND_ATTACK)
					pCommand->iButtons &= ~IN_SECOND_ATTACK;

				if (pCommand->iButtons & IN_ATTACK)
					pCommand->iButtons &= ~IN_ATTACK;
			}
		}
		else {

			float flFireReadyTime = pWeaponBase->GetFireReadyTime();

			float flFireReadyTimeDifference = pWeaponBase->GetFireReadyTime() - flServerTime;

	
			if (TGlobalVars::bRageMode) {
				if (flFireReadyTimeDifference < 0.0f)
				{
					if ((pCommand->iButtons & IN_ATTACK || pCommand->iButtons & IN_SECOND_ATTACK))
					{
						if (pCommand->iButtons & IN_SECOND_ATTACK)
							pCommand->iButtons &= ~IN_SECOND_ATTACK;

						if (pCommand->iButtons & IN_ATTACK)
							pCommand->iButtons &= ~IN_ATTACK;
					}
				}
			}
		}
	}
	else {

		if (!pPlayer->CanShoot())
		{
			bool bIsNonAimWeapon = !pWeaponData->IsGun() && pBaseWeapon->GetItemDefinitionIndex() != WEAPON_TASER;

			if (pCommand->iButtons & IN_ATTACK && !bIsNonAimWeapon && pBaseWeapon->GetItemDefinitionIndex() != WEAPON_REVOLVER)
				pCommand->iButtons &= ~IN_ATTACK;
			else if ((pCommand->iButtons & IN_ATTACK || pCommand->iButtons & IN_SECOND_ATTACK) && (pWeaponData->nWeaponType == WEAPONTYPE_KNIFE || pBaseWeapon->GetItemDefinitionIndex() == WEAPON_REVOLVER))
			{
				if (pCommand->iButtons & IN_SECOND_ATTACK)
					pCommand->iButtons &= ~IN_SECOND_ATTACK;

				if (pCommand->iButtons & IN_ATTACK)
					pCommand->iButtons &= ~IN_ATTACK;
			}
		}
	}
}

void TPacketHandler::HandleSimulationTime(CBaseEntity* pPlayer) {

}

void TPacketHandler::OnCommandEnd(CUserCmd* pCommand, CBaseEntity* pPlayer, bool& SendPacket, bool OnlyUpdate) {

	if (!TVariables::TAntiAim::TOthers::Fakeduck.KeyState.bState)
	{
		TCommandInfo* CmdObject = TCommandManager::GetCommandData(pCommand->iCommandNumber);
		TCommandInfo* LastCmdObject = TCommandManager::GetCommandData(pCommand->iCommandNumber - 1);

		CBaseCombatWeapon* pBaseWeapon = TBaseEntity::GetWeapon(pPlayer);

		CCSWeaponData* pWeaponData = nullptr;

		if (pBaseWeapon)
			pWeaponData = TBaseEntity::GetWeaponData(pBaseWeapon);

		if (pBaseWeapon && pWeaponData)
		{
			if (CmdObject && LastCmdObject) {

				if (pBaseWeapon->GetItemDefinitionIndex() != WEAPON_HEALTHSHOT)
				{
					if (pWeaponData->nWeaponType == WEAPONTYPE_GRENADE)
					{
						CBaseCSGrenade* pGrenade = (CBaseCSGrenade*)pBaseWeapon;

						if (pGrenade->GetThrowTime())
						{
							SendPacket = true;

							if (!OnlyUpdate)
								CmdObject->bLastAttack = true;
						}
						else if (LastCmdObject->bLastAttack) {
							SendPacket = true;

							if (!OnlyUpdate)
								LastCmdObject->bLastAttack = false;
						}
					}
					else if (pWeaponData->nWeaponType == WEAPONTYPE_KNIFE)
					{
						if ((pCommand->iButtons & IN_ATTACK || pCommand->iButtons & IN_SECOND_ATTACK))
						{
							SendPacket = true;

							if (!OnlyUpdate)
								CmdObject->bLastAttack = true;
						}
						else if (LastCmdObject->bLastAttack) {
							SendPacket = true;

							if (!OnlyUpdate)
								LastCmdObject->bLastAttack = false;
						}
					}
					else if (pWeaponData->IsGun())
					{
						if (pBaseWeapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
						{
							const float flServerTime = TICKS_TO_TIME(pPlayer->GetTickBase());

							CWeaponCSBase* pWeaponBase = static_cast<CWeaponCSBase*>(pBaseWeapon);

							float flFireReadyTimeDifference = pWeaponBase->GetFireReadyTime() - flServerTime;

							if (pCommand->iButtons & IN_ATTACK && flFireReadyTimeDifference < 0.0f)
							{
								SendPacket = true;

								if (!OnlyUpdate)
									CmdObject->bLastAttack = true;
							}
							else if (LastCmdObject->bLastAttack) {
								SendPacket = true;

								if (!OnlyUpdate)
									LastCmdObject->bLastAttack = false;
							}
						}
						else
						{
							if (pCommand->iButtons & IN_ATTACK || pCommand->iButtons & IN_SECOND_ATTACK)
							{
								SendPacket = true;

								if (!OnlyUpdate)
									CmdObject->bLastAttack = true;
							}
							else if (LastCmdObject->bLastAttack) {
								SendPacket = true;

								if (!OnlyUpdate)
									LastCmdObject->bLastAttack = false;
							}

						}
					}
				}
			}
		}
	}

	if (!OnlyUpdate) {
		bLastPacketState = bSendPacket;
		bSendPacket = SendPacket;
	}
}
