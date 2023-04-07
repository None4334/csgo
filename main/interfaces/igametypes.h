
enum EGameType : int
{
	GAMETYPE_UNKNOWN = -1,
	GAMETYPE_CLASSIC,
	GAMETYPE_GUNGAME,
	GAMETYPE_TRAINING,
	GAMETYPE_CUSTOM,
	GAMETYPE_COOPERATIVE,
	GAMETYPE_SKIRMISH,
	GAMETYPE_FREEFORALL
};


enum EGameMode : int
{
	GAMEMODE_UNKNOWN = 0,
	GAMEMODE_CASUAL,
	GAMEMODE_COMPETITIVE,
	GAMEMODE_WINGMAN,
	GAMEMODE_ARMSRACE,
	GAMEMODE_DEMOLITION,
	GAMEMODE_DEATHMATCH,
	GAMEMODE_GUARDIAN,
	GAMEMODE_COOPSTRIKE,
	GAMEMODE_DANGERZONE
};

class IGameTypes
{
public:
	int GetCurrentGameType()
	{
		return TUtil::CallVFunc<int>(this, 8);
	}

	int GetCurrentGameMode()
	{
		return TUtil::CallVFunc<int>(this, 9);
	}

	const char* GetCurrentMapName()
	{
		return TUtil::CallVFunc<const char*>(this, 10);
	}

	const char* GetCurrentGameTypeNameID()
	{
		return TUtil::CallVFunc<const char*>(this, 11);
	}

	const char* GetCurrentGameModeNameID()
	{
		return TUtil::CallVFunc<const char*>(this, 13);
	}
};
