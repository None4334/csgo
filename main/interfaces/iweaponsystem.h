
class CCSWeaponData;
class IWeaponSystem
{
public:
	CCSWeaponData* GetWeaponData(short nItemDefinitionIndex)
	{
		return TUtil::CallVFunc<CCSWeaponData*>(this, 2, nItemDefinitionIndex);
	}
};
