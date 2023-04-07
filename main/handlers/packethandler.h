

struct TCommandFixData
{
	bool bIsOutgoing = false;
	bool bIsUsed = false;
	int iPreviousCommandNumber = 0;
	int iCommandNumber = 0;
};

struct TCommandCorrectionData
{
	int iCommandNumber = 0;
	int iTickCount = 0;
	int iChokedCommands = 0;
};


namespace TPacketHandler {
	inline bool bSendPacket = false;
	inline bool bLastPacketState = false;

	void OnCommandStart(CUserCmd* pCommand, CBaseEntity* pPlayer);
	void HandleSimulationTime(CBaseEntity* pPlayer);
	void OnCommandEnd(CUserCmd* pCommand , CBaseEntity* pPlayer ,bool& SendPacket,bool OnlyUpdate = false);

	namespace TProcessPacket {
		inline std::deque <int> dqChokedNumber;
		inline std::deque <TCommandFixData> dqCommandFixData;
		inline std::deque <TCommandCorrectionData> dqCommandCorrectionData;
		void FinishCommandProcess(CUserCmd* pCommand,bool& bSendPacket);
	}

}