#include "hwid.h"

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <WinInet.h>
#include <filesystem>
#include <comdef.h>
#include <WbemIdl.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <thread>
#include <vector>
#include <Psapi.h>
#include "md5.h"

#pragma comment(lib, "WinInet.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wbemuuid.lib")

std::wstring get_wmi_property(const wchar_t* className, const wchar_t* propertyName) {
	HRESULT hr = 0;
	ULONG retn = 0;

	IWbemLocator* locator = 0;
	IWbemServices* services = 0;
	IWbemClassObject* class_object = 0;
	IEnumWbemClassObject* enumerator = 0;

	std::wstring result = L"";
	std::wstring query = L"";

	auto initialized = true;

	hr = CoInitializeEx(0, COINIT_MULTITHREADED);
	if (FAILED(hr)) {
		initialized = false;

		if (hr != RPC_E_CHANGED_MODE)
			goto cleanup;
	}
	else
	{
		hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_DEFAULT, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE, NULL);
		if (FAILED(hr) && hr != RPC_E_TOO_LATE)
			goto cleanup;
	}

	hr = CoCreateInstance(CLSID_WbemLocator, 0, CLSCTX_INPROC_SERVER, IID_IWbemLocator, (LPVOID*)&locator);
	if (FAILED(hr))
		goto cleanup;

	hr = locator->ConnectServer(bstr_t(L"ROOT\\CIMV2"), NULL, NULL, 0, NULL, 0, 0, &services);
	if (FAILED(hr))
		goto cleanup;

	hr = CoSetProxyBlanket(services, RPC_C_AUTHN_WINNT, RPC_C_AUTHZ_NONE, NULL, RPC_C_AUTHN_LEVEL_CALL, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, EOAC_NONE);
	if (FAILED(hr))
		goto cleanup;

	wchar_t buf[4096];
	memset(buf, 0, sizeof(wchar_t) * 4096);
	swprintf(buf, sizeof(wchar_t) * 2048, L"SELECT * FROM %s", className);

	query = buf;

	hr = services->ExecQuery(bstr_t("WQL"), bstr_t(query.c_str()), WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY, NULL, &enumerator);
	if (FAILED(hr))
		goto cleanup;

	while (enumerator) {
		hr = enumerator->Next(WBEM_INFINITE, 1, &class_object, &retn);

		if (retn == 0) {
			break;
		}

		VARIANT vtProp;

		hr = class_object->Get(propertyName, 0, &vtProp, 0, 0);
		if (SUCCEEDED(hr) && vtProp.vt == VT_BSTR) {
			result = vtProp.bstrVal;
			break;
		}

	     VariantClear(&vtProp);
		class_object->Release();
	}

cleanup:

	if (services)
		services->Release();

	if (locator)
		locator->Release();

	if (enumerator)
		enumerator->Release();

	if (initialized)
		CoUninitialize();

	return result;
}

const char* CHardware::GetHardwareID() {
	std::wstring base_board_id = get_wmi_property(L"Win32_BaseBoard", L"SerialNumber");
	std::wstring bios_id = get_wmi_property(L"Win32_BIOS", L"SerialNumber");
	std::wstring processor_id = get_wmi_property(L"Win32_Processor", L"ProcessorId");

	auto base_board = std::string(base_board_id.begin(), base_board_id.end());
	auto bios = std::string(bios_id.begin(), bios_id.end());
	auto processor = std::string(processor_id.begin(), processor_id.end());
	std::string szData = md5(base_board) + "&" + md5(bios) + "&" + md5(processor) + "&" + md5("yhjgdjUV3114ARFYKLBP" + md5(base_board) + md5(bios) + md5(processor) + "sdaMNO0SD30c");
	return szData.c_str();
}