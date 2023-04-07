

struct WepData_t
{
	float m_flMaxRange = 0.0f;
	float m_flRangeModifier = 0.0f;
	float m_flWeaponDamage = 0.0f;
	float m_flPenetrationPower = 0.0f;
	float m_flPenetrationDistance = 0.0f;
	float m_flArmorRatio = 0.0f;
	CBaseCombatWeapon* m_Weapon = nullptr;
	CCSWeaponData* m_WeaponData = nullptr;

};

struct PenetrationData_t
{
	Vector3D vecShootPosition = Vector3D(0, 0, 0);
	Vector3D vecTargetPosition = Vector3D(0, 0, 0);
	Vector3D vecDirection = Vector3D(0, 0, 0);

	bool bSuccess = false;
	bool bFinished = false;

	int PenetrationCount = 4;
	int Hitbox = 0;
	int nPriority = 0;

	float flMinDamage = 0.0f;
	float flDamageModifier = 0.5f;
	float flPenetrationModifier = 1.0f;
	float flCurrentDamage = 0.0f;
	float flCurrentDistance = 0.0f;

	CGameTrace EnterTrace;
	CGameTrace ExitTrace;
};

namespace TEngineFireBullet
{
	bool TraceToExit(PenetrationData_t* m_PenetrationData, const Vector3D& vecStart, CGameTrace* pEnterTrace, CGameTrace* pExitTrace);
	bool HandleBulletPenetration(PenetrationData_t* m_PenetrationData);
	bool IsArmored(CBaseEntity* pPlayer, const int& nHitGroup);
	void ScaleDamage(CGameTrace* Trace, float& flDamage);
	void ClipTraceToPlayers(const Vector3D& vecStart, const Vector3D& vecEnd, CGameTrace* Trace, CTraceFilter* Filter, uint32_t nMask);
	Vector3D GetPointDirection(const Vector3D& vecShootPosition, const Vector3D& vecTargetPosition);
	void ScanPoint(PenetrationData_t* m_PenetrationData);
	bool SimulateFireBullet(PenetrationData_t* m_PenetrationData);
	bool IsPenetrablePoint(const Vector3D& vecShootPosition, const Vector3D& vecTargetPosition);
	void CacheWeaponData();

	inline WepData_t m_Data;
}
