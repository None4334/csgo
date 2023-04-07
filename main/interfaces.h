
#include "interfaces/ibaseclient.h"
#include "interfaces/icliententitylist.h"
#include "interfaces/isurface.h"
#include "interfaces/iclientmodeshared.h"
#include "interfaces/ienginetrace.h"
#include "interfaces/ivmodelinfo.h"
#include "interfaces/inetchannel.h"
#include "interfaces/iengine.h"
#include "interfaces/ikeyvalues.h"
#include "interfaces/iinput.h"
#include "interfaces/iinputsystem.h"
#include "interfaces/ivpanel.h"
#include "interfaces/ivglobalvars.h"
#include "interfaces/iweaponsystem.h"
#include "interfaces/ilocalize.h"
#include "interfaces/ivconvar.h"
#include "interfaces/imdlcache.h"
#include "interfaces/imemalloc.h"
#include "interfaces/iglowobject.h"
#include "interfaces/iprediction.h"
#include "interfaces/imaterialsystem.h"
#include "interfaces/ieventgamemanager.h"
#include "interfaces/ivdebugoverlay.h"
#include "interfaces/surfacerender.h"
#include "interfaces/igameproxyrules.h"
#include "interfaces/igametypes.h"
#include "interfaces/isteamcoordinator.h"
#include "interfaces/isteamclienth.h"
#include "interfaces/ifilesystem.h"
#include "interfaces/inetworkstringtable.h"
#include "interfaces/iphysicssurfaceprops.h"
#include "interfaces/imodelrender.h"

#define ENGINE_STR				TSTRING("engine.dll")
#define CLIENT_STR				TSTRING("client.dll")
#define LOCALIZE_STR			TSTRING("localize.dll")
#define MATERIALSYSTEM_STR		TSTRING("materialsystem.dll")
#define VGUI_STR				TSTRING("vguimatsurface.dll")
#define VGUI2_STR				TSTRING("vgui2.dll")
#define SHADERPIDX9_STR			TSTRING("shaderapidx9.dll")
#define GAMEOVERLAYRENDERER_STR TSTRING("gameoverlayrenderer.dll")
#define PHYSICS_STR				TSTRING("vphysics.dll")
#define VSTDLIB_STR				TSTRING("vstdlib.dll")
#define TIER0_STR				TSTRING("tier0.dll")
#define INPUTSYSTEM_STR			TSTRING("inputsystem.dll")
#define STUDIORENDER_STR		TSTRING("studiorender.dll")
#define DATACACHE_STR			TSTRING("datacache.dll")
#define STEAM_API_STR			TSTRING("steam_api.dll")
#define MATCHMAKING_STR			TSTRING("matchmaking.dll")
#define SERVER_STR				TSTRING("server.dll")
#define SERVERBROWSER_STR		TSTRING("serverbrowser.dll")

class CInterfaceRegister
{
public:
	InstantiateInterfaceFn	pCreateFn;		
	const char* szName;			
	CInterfaceRegister* pNext;			
};

using RandomSeedfn = void(__cdecl*)(int);
using RandomIntfn = int(__cdecl*)(int, int);
using RandomFloatfn = float(__cdecl*)(float, float);

namespace TInterfaces {


	inline IBaseClientDll* BaseClient;
	inline IClientModeShared* ClientMode;
	inline ISurface* Surface;
	inline IClientEntityList* ClientEntityList;
	inline IEngineClient* EngineClient;
	inline IKeyValuesSystem* KeyValueSystem;
	inline IInput* Input;
	inline IClientState* ClientState;
	inline IDirect3DDevice9* DirectDevice;
	inline IInputSystem* InputSystem;
	inline IVPanel* Panel;
	inline IVModelInfo* ModelInfo;
	inline IGlobalVarsBase* Globals;
	inline IWeaponSystem* WeaponSystem;
	inline ILocalize* Localize;
	inline IConVar* Convars;
	inline IMDLCache* MDLCache;
	inline IMemAlloc* MemAlloc;
	inline IGlowObjectManager* GlowObjectManager;
	inline IEngineTrace* EngineTrace;
	inline IGameMovement* GameMovement;
	inline IMaterialSystem* MaterialSystem;
	inline IGameEventManager2* GameEvent;
	inline IVDebugOverlay* DebugOverlay;
	inline IPrediction* Prediction;
	inline IViewRenderBeams* ViewRenderBeams;
	inline IStudioRender* StudioRender;
	inline IMoveHelper* MoveHelper;
	inline IGameTypes* GameTypes;
	inline ISteamClient* SteamClient;
	inline ISteamUser* SteamUser;
	inline ISteamGameCoordinator* SteamGameCoordinator;
	inline IFileSystem* FileSystem;
	inline CNetworkStringTableContainer* StringTableContainer;
	inline IPhysicsSurfaceProps* PhysicsProps;
	inline IVModelRender* ModelRender;
	inline IVRenderView* RenderView;

	inline RandomSeedfn  RandomSeed;
	inline RandomIntfn  RandomInt;
	inline RandomFloatfn RandomFloat;

	CGameProxyRules* GetGameRules(bool bUpdateGameRules = false);
	bool InitializeInterface();

	template <typename T>
	T* Capture(const std::string_view szModuleName, const std::string_view szInterface);

}