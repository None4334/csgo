
class TDetourHook
{
public:
	TDetourHook() = default;

	explicit TDetourHook(void* pFunction, void* pDetour)
		: pBaseFn(pFunction), pReplaceFn(pDetour) { }

	bool Create(void* pFunction, void* pDetour)
	{
		pBaseFn = pFunction;

		if (pBaseFn == nullptr)
			return false;

		pReplaceFn = pDetour;

		if (pReplaceFn == nullptr)
			return false;

		const MH_STATUS status = MH_CreateHook(pBaseFn, pReplaceFn, &pOriginalFn);

		if (status != MH_OK)
			return false;

		if (!this->Replace())
			return false;

		return true;
	}

	bool Replace()
	{
		if (pBaseFn == nullptr)
			return false;

		if (bIsHooked)
			return false;

		const MH_STATUS status = MH_EnableHook(pBaseFn);

		if (status != MH_OK)
			return false;

		bIsHooked = true;
		return true;
	}


	bool Remove()
	{
		if (!this->Restore())
			return false;

		const MH_STATUS status = MH_RemoveHook(pBaseFn);

		if (status != MH_OK)
			return false;

		return true;
	}

	bool Restore()
	{

		if (!bIsHooked)
			return false;

		const MH_STATUS status = MH_DisableHook(pBaseFn);

		if (status != MH_OK)
			return false;

		bIsHooked = false;
		return true;
	}

	template <typename Fn>
	Fn GetOriginal()
	{
		return static_cast<Fn>(pOriginalFn);
	}

	inline bool IsHooked() const
	{
		return bIsHooked;
	}

private:

	bool bIsHooked = false;
	void* pBaseFn = nullptr;
	void* pReplaceFn = nullptr;
	void* pOriginalFn = nullptr;
};

class ProtectGuard
{
public:

	ProtectGuard(void* pVirtualAddress, DWORD Length, DWORD Protect)
	{
		VirtualAddress = pVirtualAddress;
		dwLength = Length;

		VirtualProtect(VirtualAddress, dwLength, Protect, &dwOldProtect);
	}

	~ProtectGuard()
	{
		VirtualProtect(VirtualAddress, dwLength, dwOldProtect, &dwOldProtect);
	}

private:

	void* VirtualAddress;
	DWORD dwLength;
	DWORD dwOldProtect;
};


class VMTHook
{
public:
	bool bHooked;
	uintptr_t* uShadowVtable;
	VMTHook() : pClassBase(nullptr), iMethodCount(0), uShadowVtable(nullptr), uOriginalVtable(nullptr) {}
	VMTHook(void* base) : pClassBase(base), iMethodCount(0), uShadowVtable(nullptr), uOriginalVtable(nullptr) {}
	~VMTHook()
	{
		RestoreTable();

		delete[] uShadowVtable;
	}

	inline void Setup(void* base = nullptr)
	{
		if (base != nullptr)
			pClassBase = base;

		if (pClassBase == nullptr)
			return;

		uOriginalVtable = *(uintptr_t**)pClassBase;
		iMethodCount = GetMethodCount(uOriginalVtable);

		if (iMethodCount == 0)
			return;

		uShadowVtable = new uintptr_t[iMethodCount + 1]();

		uShadowVtable[0] = uOriginalVtable[-1];
		std::memcpy(&uShadowVtable[1], uOriginalVtable, iMethodCount * sizeof(uintptr_t));

		try
		{
			auto guard = ProtectGuard{ pClassBase, sizeof(uintptr_t), PAGE_READWRITE };
			*(uintptr_t**)pClassBase = &uShadowVtable[1];
		}
		catch (...)
		{
			delete[] uShadowVtable;
		}
	}

	template<typename T>
	inline void Hook(uint32_t index, T method)
	{
		uShadowVtable[index + 1] = reinterpret_cast<uintptr_t>(method);
		bHooked = true;
	}

	inline void Unhook(uint32_t index)
	{
		uShadowVtable[index + 1] = uOriginalVtable[index];
		bHooked = false;
	}

	template<typename T>
	inline T GetOriginal(uint32_t index)
	{
		return (T)uOriginalVtable[index];
	}

	inline void RestoreTable()
	{
		try
		{
			if (uOriginalVtable != nullptr)
			{
				auto guard = ProtectGuard{ pClassBase, sizeof(uintptr_t), PAGE_READWRITE };
				*(uintptr_t**)pClassBase = uOriginalVtable;
				uOriginalVtable = nullptr;
			}
		}
		catch (...) {}
	}

private:

	inline uint32_t GetMethodCount(uintptr_t* vtable_start)
	{
		uint32_t len = -1;

		do ++len; while (vtable_start[len]);

		return len;
	}

	void* pClassBase;
	uint32_t iMethodCount;
	uintptr_t* uOriginalVtable;
};

