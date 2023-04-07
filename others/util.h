
using TPatternPair = std::pair< uint8_t, bool >;
namespace TUtil {

	HANDLE CreatUserThread(LPVOID Routine, LPVOID Arguments = NULL, LPDWORD ThreadIDOut = NULL);

	void ConsoleStart(const char* title);
	void ConsoleShutdown();

	std::uintptr_t FindPattern(const std::string_view szModuleName, const std::string_view szPattern);
	std::uintptr_t FindPattern(const std::uint8_t* uRegionStart, const std::uintptr_t uRegionSize, const std::string_view szPattern);

	bool FileDoesExist(const std::string& name);

	template <typename T, typename ... Args_t>
	constexpr T CallVFunc(void* thisptr, std::size_t nIndex, Args_t... argList)
	{
		using VirtualFn = T(__thiscall*)(void*, decltype(argList)...);
		return (*static_cast<VirtualFn**>(thisptr))[nIndex](thisptr, argList...);
	}

	inline uintptr_t Rel32Fix(uintptr_t ptr)
	{
		auto offset = *(uintptr_t*)(ptr + 0x1);
		return (uintptr_t)(ptr + offset + 0x5);
	}

	template <typename T>
	constexpr auto RelativeToAbsolute(uintptr_t uAddress) noexcept
	{
		return (T)(uAddress + 4 + *reinterpret_cast<std::int32_t*>(uAddress));
	}


	TAddress FindAddress(std::string szModuleName, const std::string& pat);
	TAddress Find(TAddress start, size_t len, const std::string& pat);

	inline std::uintptr_t GetAbsoluteAddress(const std::uintptr_t uRelativeAddress)
	{
		return uRelativeAddress + 0x4 + *reinterpret_cast<std::int32_t*>(uRelativeAddress);
	}

	template <typename T = void*>
	constexpr T GetVFunc(void* thisptr, std::size_t nIndex)
	{
		return (*static_cast<T**>(thisptr))[nIndex];
	}

	std::uintptr_t* FindHudElement(const char* szName);

	std::wstring MultiByteToWide(const std::string& szString);
	std::string WideToMultiByte(const std::wstring& szString);
	void* GetModuleBaseHandle(const std::string_view szModuleName);
	void* GetExportAddress(const void* pModuleBase, const std::string_view szProcedureName);
	
}
