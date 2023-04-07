
class ILocalize
{
public:
	wchar_t* Find(const char* szTokenName)
	{
		return TUtil::CallVFunc<wchar_t*>(this, 11, szTokenName);
	}

	const wchar_t* FindSafe(const char* szTokenName)
	{
		return TUtil::CallVFunc<wchar_t*>(this, 12, szTokenName);
	}
};