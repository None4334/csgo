#include "../includes.h"
#include "dll.h"

static HINSTANCE Instance;
DWORD WINAPI MainThread(LPVOID lpParameter) {
    TDll::InitLdrDll(Instance);
    ExitThread(EXIT_SUCCESS);
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL,  DWORD fdwReason, LPVOID lpvReserved) 
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        Instance = hinstDLL;
        if (auto hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)&MainThread, NULL, NULL, NULL); hThread != nullptr)
            CloseHandle(hThread);

        break;
    }
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:

        if (lpvReserved != nullptr)
        {
            break; 
        }

        break;
    }
    return TRUE;  
}