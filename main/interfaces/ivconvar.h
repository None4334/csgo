
enum EConVarFlag : int
{
	FCVAR_NONE = 0,
	FCVAR_UNREGISTERED = (1 << 0),	
	FCVAR_DEVELOPMENTONLY = (1 << 1),	
	FCVAR_GAMEDLL = (1 << 2),	
	FCVAR_CLIENTDLL = (1 << 3),	
	FCVAR_HIDDEN = (1 << 4),	
	FCVAR_PROTECTED = (1 << 5),
	FCVAR_SPONLY = (1 << 6),	
	FCVAR_ARCHIVE = (1 << 7),	
	FCVAR_NOTIFY = (1 << 8),	
	FCVAR_USERINFO = (1 << 9),
	FCVAR_CHEAT = (1 << 14),	
	FCVAR_PRINTABLEONLY = (1 << 10),	
	FCVAR_UNLOGGED = (1 << 11),
	FCVAR_NEVER_AS_STRING = (1 << 12),	
	FCVAR_SERVER = (1 << 13),	
	FCVAR_DEMO = (1 << 16),
	FCVAR_DONTRECORD = (1 << 17),
	FCVAR_RELOAD_MATERIALS = (1 << 20),
	FCVAR_RELOAD_TEXTURES = (1 << 21),	
	FCVAR_NOT_CONNECTED = (1 << 22),	
	FCVAR_MATERIAL_SYSTEM_THREAD = (1 << 23),	
	FCVAR_ARCHIVE_XBOX = (1 << 24),	
	FCVAR_ACCESSIBLE_FROM_THREADS = (1 << 25),
	FCVAR_SERVER_CAN_EXECUTE = (1 << 28),	
	FCVAR_SERVER_CANNOT_QUERY = (1 << 29),	
	FCVAR_CLIENTCMD_CAN_EXECUTE = (1 << 30),	
	FCVAR_MATERIAL_THREAD_MASK = (FCVAR_RELOAD_MATERIALS | FCVAR_RELOAD_TEXTURES | FCVAR_MATERIAL_SYSTEM_THREAD)
};

using FnCommandCallbackV1_t = void(__cdecl*)();
using FnChangeCallback_t = void(__cdecl*)(void*, const char*, float);

using CVarDLLIdentifier_t = int;
class CConBase;
class CConVar;
class CConCmd;
class IConVar : public IAppSystem
{
public:
	virtual CVarDLLIdentifier_t	AllocateDLLIdentifier() = 0;
	virtual void			RegisterConCommand(CConVar* pCommandBase, int iDefaultValue = 1) = 0;
	virtual void			UnregisterConCommand(CConVar* pCommandBase) = 0;
	virtual void			UnregisterConCommands(CVarDLLIdentifier_t id) = 0;
	virtual const char* GetCommandLineValue(const char* szVariableName) = 0;
	virtual CConBase* FindCommandBase(const char* szName) = 0;
	virtual const CConBase* FindCommandBase(const char* szName) const = 0;
	virtual CConVar* FindVar(const char* szVariableName) = 0;
	virtual const CConVar* FindVar(const char* szVariableName) const = 0;
	virtual CConCmd* FindCommand(const char* szName) = 0;
	virtual const CConCmd* FindCommand(const char* szName) const = 0;
	virtual void			InstallGlobalChangeCallback(FnChangeCallback_t callback) = 0;
	virtual void			RemoveGlobalChangeCallback(FnChangeCallback_t callback) = 0;
	virtual void			CallGlobalChangeCallbacks(CConVar* pVar, const char* szOldString, float flOldValue) = 0;
	virtual void			InstallConsoleDisplayFunc(void* pDisplayFunc) = 0;
	virtual void			RemoveConsoleDisplayFunc(void* pDisplayFunc) = 0;
	virtual void			ConsoleColorPrintf(const Color& color, const char* pFormat, ...) const = 0;
	virtual void			ConsolePrintf(const char* pFormat, ...) const = 0;
	virtual void			ConsoleDPrintf(const char* pFormat, ...) const = 0;
	virtual void			RevertFlaggedConVars(int nFlag) = 0;
};


class CConVar
{
public:
	const char* GetName()
	{
		return TUtil::CallVFunc<const char*>(this, 5);
	}

	float GetFloat() 
	{
		std::uint32_t uXored = *reinterpret_cast<std::uint32_t*>(&pParent->flValue) ^ reinterpret_cast<std::uint32_t>(this);
		return *reinterpret_cast<float*>(&uXored);
	}

	int GetInt() 
	{
		return static_cast<int>(pParent->iValue ^ reinterpret_cast<int>(this));
	}

	bool GetBool() 
	{
		return !!GetInt();
	}

	const char* GetString() const
	{
		char const* szValue = pParent->szString;
		return szValue ? szValue : "";
	}

	void SetValue(const char* szValue)
	{
		return TUtil::CallVFunc<void>(this, 14, szValue);
	}

	void SetValue(float flValue)
	{
		return TUtil::CallVFunc<void>(this, 15, flValue);
	}

	void SetValue(int iValue)
	{
		return TUtil::CallVFunc<void>(this, 16, iValue);
	}

	void SetValue(Color colValue)
	{
		return TUtil::CallVFunc<void>(this, 17, colValue);
	}

public:
	std::byte						pad0[0x4];			
	CConVar* pNext;				
	bool							bRegistered;		
	const char* szName;				
	const char* szHelpString;		
	int								nFlags;				
	FnCommandCallbackV1_t			pCallback;			
	CConVar* pParent;			//0x1C
	const char* szDefaultValue;		//0x20
	char* szString;			//0x24
	int								iStringLength;		//0x28
	float							flValue;			//0x2C
	int								iValue;				//0x30
	bool							bHasMin;			//0x34
	float							flMinValue;			//0x38
	bool							bHasMax;			//0x3C
	float							flMaxValue;			//0x40
	CUtlVector<FnChangeCallback_t>	fnChangeCallbacks;	//0x44
};

class CSpoofedConVar // @credits: markhc
{
public:
	CSpoofedConVar() = default;
	CSpoofedConVar(const char* szCVar);
	CSpoofedConVar(CConVar* pCVar);
	~CSpoofedConVar();

	// Check
	bool	IsSpoofed() const;
	void	Spoof();

	// Flags
	void	SetFlags(int iFlags) const;
	int		GetFlags() const;

	// Set
	void	SetBool(bool bValue) const;
	void	SetInt(int iValue) const;
	void	SetFloat(float flValue) const;
	void	SetString(const char* szValue) const;

private:
	CConVar* pOriginalCVar = nullptr;
	CConVar* pDummyCVar = nullptr;
	char		szDummyName[128] = { };
	char		szDummyValue[128] = { };
	char		szOriginalName[128] = { };
	char		szOriginalValue[128] = { };
	int			iOriginalFlags = 0;
};

