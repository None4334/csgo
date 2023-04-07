

namespace TAntiAim {
	void Run(CUserCmd* pCmd,CBaseEntity* pLocalPlayer,bool& bSendPacket);
	void RegisterPitch(CUserCmd* pCmd, CBaseEntity* pLocalPlayer, bool& bSendPacket);
	void Fakelag(CUserCmd* pCmd, CBaseEntity* pLocalPlayer, bool& bSendPacket);
	void RegisterYaw(CUserCmd* pCmd, CBaseEntity* pLocalPlayer, bool& bSendPacket);
	void RegisterRoll(CUserCmd* pCmd, CBaseEntity* pLocalPlayer, bool& bSendPacket);
	void RegisterMovement(CUserCmd* pCmd , CBaseEntity* pLocalPlayer);
	void RegisterDuck(CUserCmd* pCmd, CBaseEntity* pLocalPlayer, bool& bSendPacket);
	inline bool bVisualDuck = false;
}