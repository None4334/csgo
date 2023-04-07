#include "../includes.h"

VOID TWinApi::Ke32SleepThread(DWORD Time) {
	Sleep(Time);
}

HMODULE TWinApi::Ke32GetModuleHandle(LPCSTR ModuleName) {
	return GetModuleHandleA(ModuleName);
}
