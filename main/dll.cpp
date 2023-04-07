#include "../includes.h"
#include "dll.h"
#include "handlers/entityhandler.h"
#include "hooks.h"
#include "ui/ui.h"
#include "renderer.h"
#include "handlers/convarmanager.h"
#include "ui/vars.h"
#include "handlers/eventmanager.h"
#include "handlers/eventlogger.h"
#include "features/chams.h"
#include "features/skins/skins.h"

DWORD WINAPI ExitRoutine(LPVOID lpParam)
{
	while (!GetAsyncKeyState(VK_END))
		TWinApi::Ke32SleepThread(500);

	if (TDll::bConsoleAttached) 
	    TUtil::ConsoleShutdown();

	if (TDll::bEntityListenerCreated)
	    TEntityListener::EntityListener.Destroy();

	if (TDll::bConvarsInitialized)
		TConvarManager::Restore();

	THooks::Unhook();
	TDirectXRender::LockAndClear();
	TEventListener::EventListener.Destroy();
	TEventLogger::stkEventLogs.clear();
	TEntityListener::ClearAndDeallocateMemory();
	TUserInterface::Shutdown();

	FreeLibraryAndExitThread((HMODULE)lpParam, 0);
}

DWORD WINAPI MainRoutine(LPDWORD lpParam) {

	while (!TDll::CheckCriticalModulesLoaded())
		TWinApi::Ke32SleepThread(1000);

#ifdef DEBUG_MODE
	TUtil::ConsoleStart(TSTRING("K32App"));
	TDll::bConsoleAttached = true;
#endif

#ifdef DEBUG_MODE
	std::cout << "Initializing Interfaces\n";
#endif

	TInterfaces::InitializeInterface();

	TEntityListener::EntityListener.Setup();
	TDll::bEntityListenerCreated = true;

	TNetvarManager::Setup();

	TConvarManager::Initialize();
	TDll::bConvarsInitialized = true;

	TConfigManager::Initialize();

	TSurfaceRender::Initialize();
	TModelChams::Initialize();
	TEventListener::EventListener.Setup();
	TPaintKits::Initialize();

	THooks::Hook();

#ifdef DEBUG_MODE
	std::cout << "Finished Initializing\n";
#endif

	return EXIT_SUCCESS;
}

VOID TDll::InitLdrDll(HMODULE hHandle) {
	hModuleHandle = hHandle;
	TUtil::CreatUserThread(&MainRoutine, hModuleHandle);
	TUtil::CreatUserThread(&ExitRoutine, hModuleHandle);
}

bool TDll::CheckCriticalModulesLoaded() {
	return TWinApi::Ke32GetModuleHandle(SERVERBROWSER_STR)
		&& TWinApi::Ke32GetModuleHandle(CLIENT_STR)
	    && TWinApi::Ke32GetModuleHandle(ENGINE_STR);
}

VOID TDll::RaiseError(DWORD ErrorCode) {

}