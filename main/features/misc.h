



namespace TMisc {

	namespace TBuyBot {
		void Run(bool bReset);
	}

	namespace TMovement {
		void InfiniteDuck(CUserCmd* pCmd);
		void BunnyHop(CUserCmd* pCmd, CBaseEntity* pLocalPlayer);
		void Quickstop(CUserCmd* pCmd, CBaseEntity* pLocalPlayer);
		void AirStrafe(CUserCmd* pCmd, CBaseEntity* pBaseEntity);
		void FixMovement(Vector3D& angViewWish, CUserCmd* pCmd);
	}

	namespace TOther {
		void PreserveDeathNotices(CBaseEntity* pLocalPlayer);
		inline bool bClearNotices = false;
	}
}
