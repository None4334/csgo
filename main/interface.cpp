#include "../includes.h"

bool TInterfaces::InitializeInterface() {
	
	BaseClient = Capture<IBaseClientDll>(CLIENT_STR, TSTRING("VClient"));

	if (BaseClient) {
		ClientMode = **reinterpret_cast<IClientModeShared***>(TUtil::GetVFunc<std::uintptr_t>(BaseClient, 10) + 0x5);
		Globals = **reinterpret_cast<IGlobalVarsBase***>(TUtil::GetVFunc<std::uintptr_t>(BaseClient, 11) + 0xA);
	}

	 Surface = Capture<ISurface>(VGUI_STR, TSTRING("VGUI_Surface"));
	 ClientEntityList = Capture<IClientEntityList>(CLIENT_STR, TSTRING("VClientEntityList"));
	 EngineClient = Capture<IEngineClient>(ENGINE_STR, TSTRING("VEngineClient"));
	 InputSystem = Capture<IInputSystem>(INPUTSYSTEM_STR, TSTRING("InputSystemVersion"));
	 Panel = Capture<IVPanel>(VGUI2_STR, TSTRING("VGUI_Panel"));
	 ModelInfo = Capture<IVModelInfo>(ENGINE_STR, TSTRING("VModelInfoClient"));
	 Localize = Capture<ILocalize>(LOCALIZE_STR, TSTRING("Localize_"));
	 Convars = Capture<IConVar>(VSTDLIB_STR, TSTRING("VEngineCvar"));
	 MDLCache = Capture<IMDLCache>(DATACACHE_STR, TSTRING("MDLCache"));

	 MemAlloc = *static_cast<IMemAlloc**>(TUtil::GetExportAddress(TUtil::GetModuleBaseHandle(TIER0_STR), TSTRING("g_pMemAlloc")));

	 KeyValueSystem = reinterpret_cast<KeyValuesSystemFn>(TUtil::GetExportAddress(TUtil::GetModuleBaseHandle(VSTDLIB_STR), TSTRING("KeyValuesSystem")))();
	 Input = *reinterpret_cast<IInput**>(TUtil::FindPattern(CLIENT_STR, TSTRING("B9 ? ? ? ? F3 0F 11 04 24 FF 50 10")) + 0x1); 
	 ClientState = **reinterpret_cast<IClientState***>(TUtil::FindPattern(ENGINE_STR, TSTRING("A1 ? ? ? ? 8B 88 ? ? ? ? 85 C9 75 07")) + 0x1);
	 DirectDevice = **reinterpret_cast<IDirect3DDevice9***>(TUtil::FindPattern(SHADERPIDX9_STR, TSTRING("A1 ? ? ? ? 50 8B 08 FF 51 0C")) + 0x1); 
	 WeaponSystem = *reinterpret_cast<IWeaponSystem**>(TUtil::FindPattern(CLIENT_STR, TSTRING("8B 35 ? ? ? ? FF 10 0F B7 C0")) + 0x2);
	 GlowObjectManager = *reinterpret_cast<IGlowObjectManager**>(TUtil::FindPattern(CLIENT_STR, TSTRING("0F 11 05 ? ? ? ? 83 C8 01")) + 0x3);
	 EngineTrace = Capture<IEngineTrace>(ENGINE_STR, TSTRING("EngineTraceClient"));

	 GameMovement = Capture<IGameMovement>(CLIENT_STR, TSTRING("GameMovement"));
	 MaterialSystem = Capture<IMaterialSystem>(MATERIALSYSTEM_STR, TSTRING("VMaterialSystem"));
	 GameEvent = Capture<IGameEventManager2>(ENGINE_STR, TSTRING("GAMEEVENTSMANAGER002"));
	 DebugOverlay = Capture<IVDebugOverlay>(ENGINE_STR, TSTRING("VDebugOverlay"));
	 Prediction= Capture<IPrediction>(CLIENT_STR, TSTRING("VClientPrediction"));

	 ViewRenderBeams = *reinterpret_cast<IViewRenderBeams**>(TUtil::FindPattern(CLIENT_STR, TSTRING("B9 ? ? ? ? A1 ? ? ? ? FF 10 A1 ? ? ? ? B9")) + 0x1); 

	 StudioRender = Capture<IStudioRender>(STUDIORENDER_STR, TSTRING("VStudioRender"));
	 MoveHelper = **reinterpret_cast<IMoveHelper***>(TUtil::FindPattern(CLIENT_STR, TSTRING("8B 0D ? ? ? ? 8B 45 ? 51 8B D4 89 02 8B 01")) + 0x2);

	 GameTypes= Capture<IGameTypes>(MATCHMAKING_STR, TSTRING("VENGINE_GAMETYPES_VERSION"));

	 SteamClient = EngineClient->GetSteamAPIContext()->pSteamClient;

	 SteamUser = EngineClient->GetSteamAPIContext()->pSteamUser;

	 const void* hSteamAPI = TUtil::GetModuleBaseHandle(STEAM_API_STR);
	 const HSteamUser hSteamUser = reinterpret_cast<std::add_pointer_t<HSteamUser()>>(TUtil::GetExportAddress(hSteamAPI, TSTRING("SteamAPI_GetHSteamUser")))();
	 const HSteamPipe hSteamPipe = reinterpret_cast<std::add_pointer_t<HSteamPipe()>>(TUtil::GetExportAddress(hSteamAPI, TSTRING("SteamAPI_GetHSteamPipe")))();

	 SteamGameCoordinator = static_cast<ISteamGameCoordinator*>(SteamClient->GetISteamGenericInterface(hSteamUser, hSteamPipe, TSTRING("SteamGameCoordinator001")));
	 FileSystem = **reinterpret_cast<IFileSystem***>(TUtil::FindPattern(ENGINE_STR,TSTRING("8B 0D ? ? ? ? 83 C1 04 8B 01 FF 37 FF 50 1C 89 47 10")) + 0x2);

	 StringTableContainer = Capture<CNetworkStringTableContainer>(ENGINE_STR, TSTRING("VEngineClientStringTable001"));

	 RandomSeed = reinterpret_cast<RandomSeedfn>(TUtil::GetExportAddress(TUtil::GetModuleBaseHandle(VSTDLIB_STR), TSTRING("RandomSeed")));
	 RandomInt = reinterpret_cast<RandomIntfn>(TUtil::GetExportAddress(TUtil::GetModuleBaseHandle(VSTDLIB_STR), TSTRING("RandomInt")));
	 RandomFloat = reinterpret_cast<RandomFloatfn>(TUtil::GetExportAddress(TUtil::GetModuleBaseHandle(VSTDLIB_STR), TSTRING("RandomFloat")));

	 PhysicsProps = Capture<IPhysicsSurfaceProps>(PHYSICS_STR, TSTRING("VPhysicsSurfaceProps"));

	 ModelRender = Capture<IVModelRender>(ENGINE_STR, TSTRING("VEngineModel"));
	 RenderView = Capture<IVRenderView>(ENGINE_STR, TSTRING("VEngineRenderView"));

	 return BaseClient && ClientMode && Surface && ClientEntityList && EngineClient 
		 && KeyValueSystem && Input && ClientState && DirectDevice && InputSystem 
		 && Panel && ModelInfo && Globals && WeaponSystem && Localize  && Convars 
		 && MDLCache && MemAlloc &&  GlowObjectManager && EngineTrace  && GameMovement 
		 && MaterialSystem && GameEvent && DebugOverlay && Prediction && ViewRenderBeams
		 && MoveHelper && GameTypes && SteamClient && SteamUser && SteamGameCoordinator
		 && FileSystem && StringTableContainer && PhysicsProps;
}

CGameProxyRules* TInterfaces::GetGameRules(bool bUpdateGameRules) {

	static CGameProxyRules** pGameRules;

	if (!pGameRules || bUpdateGameRules)
		pGameRules = *reinterpret_cast<CGameProxyRules***>(TUtil::FindPattern(CLIENT_STR, TSTRING("A1 ? ? ? ? 8B 0D ? ? ? ? 6A 00 68 ? ? ? ? C6")) + 0x1);

	return *pGameRules;
}

template <typename T>
T* TInterfaces::Capture(const std::string_view szModuleName, const std::string_view szInterface) {

	const auto GetRegisterList = [&szModuleName]() -> CInterfaceRegister*
	{
		void* oCreateInterface = nullptr;

		if (const auto hModule = TUtil::GetModuleBaseHandle(szModuleName); hModule != nullptr)
			oCreateInterface = TUtil::GetExportAddress(hModule, TSTRING("CreateInterface"));

		const std::uintptr_t uCreateInterfaceRelative = reinterpret_cast<std::uintptr_t>(oCreateInterface) + 0x5;
		const std::uintptr_t uCreateInterface = uCreateInterfaceRelative + 4U + *reinterpret_cast<std::int32_t*>(uCreateInterfaceRelative);
		return **reinterpret_cast<CInterfaceRegister***>(uCreateInterface + 0x6);
	};

	for (auto pRegister = GetRegisterList(); pRegister != nullptr; pRegister = pRegister->pNext)
	{
		if ((std::string_view(pRegister->szName).compare(0U, szInterface.length(), szInterface) == 0 &&
			std::atoi(pRegister->szName + szInterface.length()) > 0) ||
			szInterface.compare(pRegister->szName) == 0)
		{
			auto pInterface = pRegister->pCreateFn();

			return static_cast<T*>(pInterface);
		}
	}

	return nullptr;
}