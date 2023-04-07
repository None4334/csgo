#include "../includes.h"


HANDLE TUtil::CreatUserThread(LPVOID Routine, LPVOID Arguments, LPDWORD ThreadIDOut) {
	return CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)Routine, Arguments, NULL, ThreadIDOut);
}

void TUtil::ConsoleStart(const char* title) {
	AllocConsole();
	freopen_s(reinterpret_cast<_iobuf**>(__acrt_iob_func(0)), "conin$", "r", static_cast<_iobuf*>(__acrt_iob_func(0)));
	freopen_s(reinterpret_cast<_iobuf**>(__acrt_iob_func(1)), "conout$", "w", static_cast<_iobuf*>(__acrt_iob_func(1)));
	freopen_s(reinterpret_cast<_iobuf**>(__acrt_iob_func(2)), "conout$", "w", static_cast<_iobuf*>(__acrt_iob_func(2)));
	SetConsoleTitleA(title);
}

void TUtil::ConsoleShutdown() {
	fclose(static_cast<_iobuf*>(__acrt_iob_func(0)));
	fclose(static_cast<_iobuf*>(__acrt_iob_func(1)));
	fclose(static_cast<_iobuf*>(__acrt_iob_func(2)));
	FreeConsole();
}

bool TUtil::FileDoesExist(const std::string& name) {
	struct stat buffer;
	return (stat(name.c_str(), &buffer) == 0);
}

TAddress TUtil::Find(TAddress start, size_t len, const std::string& pat) {
	uint8_t* scan_start, * scan_end;
	std::vector< TPatternPair > pattern{};
	std::stringstream			  stream{ pat };
	std::string				      w;

	if (!start || !len || pat.empty())
		return{};

	while (stream >> w) {
		if (w[0] == '?')
			pattern.push_back({ 0, true });

		else if (std::isxdigit(w[0]) && std::isxdigit(w[1]))
			pattern.push_back({ (uint8_t)std::strtoul(w.data(), 0, 16), false });
	}

	scan_start = start.as< uint8_t* >();
	scan_end = scan_start + len;

	auto result = std::search(scan_start, scan_end, pattern.begin(), pattern.end(),
		[](const uint8_t b, const TPatternPair& p) {
			return b == p.first || p.second;
		});

	if (result == scan_end)
		return{};

	return (uintptr_t)result;
}

TAddress TUtil::FindAddress(std::string szModuleName, const std::string& pat)
{
	auto pModuleHandle = GetModuleBaseHandle(szModuleName);
	const std::uint8_t* uModuleAddress = static_cast<std::uint8_t*>(pModuleHandle);
	const IMAGE_DOS_HEADER* pDosHeader = static_cast<IMAGE_DOS_HEADER*>(pModuleHandle);
	const IMAGE_NT_HEADERS* pNtHeaders = reinterpret_cast<const IMAGE_NT_HEADERS*>(uModuleAddress + pDosHeader->e_lfanew);
	return Find(pModuleHandle, pNtHeaders->OptionalHeader.SizeOfImage, pat);
}

std::uintptr_t TUtil::FindPattern(const std::string_view szModuleName, const std::string_view szPattern)
{
	void* hModule = GetModuleBaseHandle(szModuleName);

	const std::uint8_t* uModuleAddress = static_cast<std::uint8_t*>(hModule);
	const IMAGE_DOS_HEADER* pDosHeader = static_cast<IMAGE_DOS_HEADER*>(hModule);
	const IMAGE_NT_HEADERS* pNtHeaders = reinterpret_cast<const IMAGE_NT_HEADERS*>(uModuleAddress + pDosHeader->e_lfanew);

	return FindPattern(uModuleAddress, pNtHeaders->OptionalHeader.SizeOfImage, szPattern);
}

std::vector<std::optional<std::uint8_t>> PatternToBytes(const std::string_view szPattern)
{
	std::vector<std::optional<std::uint8_t>> vecBytes = { };
	auto itBegin = szPattern.cbegin();
	const auto itEnd = szPattern.cend();

	while (itBegin < itEnd)
	{
		if (*itBegin == '?')
		{
			if (++itBegin; *itBegin == '?')
				++itBegin;

			vecBytes.emplace_back(std::nullopt);
		}

		else if (*itBegin != ' ')
		{
			std::uint8_t uByte = static_cast<std::uint8_t>(((*itBegin >= 'A' ? (((*itBegin - 'A') & (~('a' ^ 'A'))) + 10) : (*itBegin <= '9' ? *itBegin - '0' : 0x0)) | 0xF0) << 4);

			if (++itBegin; *itBegin != ' ')
				uByte |= static_cast<std::uint8_t>(*itBegin >= 'A' ? (((*itBegin - 'A') & (~('a' ^ 'A'))) + 10) : (*itBegin <= '9' ? *itBegin - '0' : 0x0));

			vecBytes.emplace_back(uByte);
		}

		++itBegin;
	}

	return vecBytes;
}
std::uintptr_t TUtil::FindPattern(const std::uint8_t* uRegionStart, const std::uintptr_t uRegionSize, const std::string_view szPattern)
{
	const std::vector<std::optional<std::uint8_t>> vecBytes = PatternToBytes(szPattern);

	for (std::uintptr_t i = 0U; i < uRegionSize - vecBytes.size(); ++i)
	{
		bool bByteFound = true;

		for (std::uintptr_t s = 0U; s < vecBytes.size(); ++s)
		{
			if (vecBytes[s].has_value() && uRegionStart[i + s] != vecBytes[s].value())
			{
				bByteFound = false;
				break;
			}
		}

		if (bByteFound)
			return reinterpret_cast<std::uintptr_t>(&uRegionStart[i]);
	}


	return 0U;
}

std::string TUtil::WideToMultiByte(const std::wstring& szString)
{
	std::string ret;
	int str_len;

	if (szString.empty())
		return {};

	str_len = WideCharToMultiByte(CP_UTF8, 0, szString.data(), (int)szString.size(), 0, 0, 0, 0);
	ret = std::string(str_len, 0);
	WideCharToMultiByte(CP_UTF8, 0, szString.data(), (int)szString.size(), &ret[0], str_len, 0, 0);

	return ret;
}

std::uintptr_t* TUtil::FindHudElement(const char* szName)
{
	static auto pHud = *reinterpret_cast<void**>(TUtil::FindPattern(CLIENT_STR, TSTRING("B9 ? ? ? ? 68 ? ? ? ? E8 ? ? ? ? 89")) + 0x1);

	using FindHudElementFn = std::uintptr_t* (__thiscall*)(void*, const char*);
	static auto oFindHudElement = reinterpret_cast<FindHudElementFn>(TUtil::FindPattern(CLIENT_STR, TSTRING("55 8B EC 53 8B 5D 08 56 57 8B F9 33 F6 39 77 28")));
	return oFindHudElement(pHud, szName);
}

std::wstring TUtil::MultiByteToWide(const std::string& szString)
{
	std::wstring ret;
	int str_len;

	if (szString.empty())
		return {};

	str_len = MultiByteToWideChar(CP_UTF8, 0, szString.data(), (int)szString.size(), nullptr, 0);
	ret = std::wstring(str_len, 0);
	MultiByteToWideChar(CP_UTF8, 0, szString.data(), (int)szString.size(), &ret[0], str_len);

	return ret;
}

void* TUtil::GetModuleBaseHandle(const std::string_view szModuleName)
{
	const _PEB32* pPEB = reinterpret_cast<_PEB32*>(__readfsdword(0x30));

	if (szModuleName.empty())
		return pPEB->ImageBaseAddress;

	const std::wstring wszModuleName(szModuleName.begin(), szModuleName.end());

	for (LIST_ENTRY* pListEntry = pPEB->Ldr->InLoadOrderModuleList.Flink; pListEntry != &pPEB->Ldr->InLoadOrderModuleList; pListEntry = pListEntry->Flink)
	{
		const _LDR_DATA_TABLE_ENTRY* pEntry = CONTAINING_RECORD(pListEntry, _LDR_DATA_TABLE_ENTRY, InLoadOrderLinks);

		if (pEntry->BaseDllName.Buffer && wszModuleName.compare(pEntry->BaseDllName.Buffer) == 0)
			return pEntry->DllBase;
	}

	return nullptr;
}

void* TUtil::GetExportAddress(const void* pModuleBase, const std::string_view szProcedureName)
{
	const std::uint8_t* pAddress = static_cast<const std::uint8_t*>(pModuleBase);
	const IMAGE_DOS_HEADER* pDosHeader = static_cast<const IMAGE_DOS_HEADER*>(pModuleBase);
	const IMAGE_NT_HEADERS* pNtHeaders = reinterpret_cast<const IMAGE_NT_HEADERS*>(pAddress + pDosHeader->e_lfanew);
	const IMAGE_OPTIONAL_HEADER* pOptionalHeader = &pNtHeaders->OptionalHeader;

	const std::uintptr_t uExportSize = pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;
	const std::uintptr_t uExportAddress = pOptionalHeader->DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;

	if (uExportSize <= 0U)
		return nullptr;

	const IMAGE_EXPORT_DIRECTORY* pExportDirectory = reinterpret_cast<const IMAGE_EXPORT_DIRECTORY*>(pAddress + uExportAddress);
	const std::uintptr_t* pNamesRVA = reinterpret_cast<const std::uintptr_t*>(pAddress + pExportDirectory->AddressOfNames);
	const std::uintptr_t* pFunctionsRVA = reinterpret_cast<const std::uintptr_t*>(pAddress + pExportDirectory->AddressOfFunctions);
	const std::uint16_t* pNameOrdinals = reinterpret_cast<const std::uint16_t*>(pAddress + pExportDirectory->AddressOfNameOrdinals);

	std::uintptr_t uRight = pExportDirectory->NumberOfNames;
	std::uintptr_t uLeft = 0;

	while (uRight != uLeft)
	{
		const std::uintptr_t uMiddle = uLeft + ((uRight - uLeft) >> 1U);
		const int iResult = szProcedureName.compare(reinterpret_cast<const char*>(pAddress + pNamesRVA[uMiddle]));

		if (iResult == 0)
			return const_cast<void*>(static_cast<const void*>(pAddress + pFunctionsRVA[pNameOrdinals[uMiddle]]));

		if (iResult > 0)
			uLeft = uMiddle;
		else
			uRight = uMiddle;
	}

	return nullptr;
}




