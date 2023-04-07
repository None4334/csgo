#define DEBUG_MODE

namespace TDll {

	static HMODULE hModuleHandle = NULL;

	static bool bConsoleAttached = false;
	static bool bEntityListenerCreated = false;
	static bool bConvarsInitialized = false;

	bool CheckCriticalModulesLoaded();
	VOID InitLdrDll(HMODULE hHandle);
	VOID RaiseError(DWORD ErrorCode);



}