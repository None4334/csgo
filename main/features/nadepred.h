
class CBaseEntity;
class CBaseCombatWeapon;
class CCSWeaponData;
namespace TNadePrediction {
	inline std::vector< Vector3D > m_pPath;
	inline std::vector< Vector3D > m_pBounces;
	inline int       m_iId;
	inline float     m_flVelocity, m_flPower;
	inline Vector3D  m_vecStart, m_vecVelocity, m_vecMove;
	inline Vector3D m_vecEyeAngles;
	inline CBaseEntity* pPredictionEntity = nullptr;
	inline CBaseCombatWeapon* pPredictionBaseWeapon = nullptr;
	inline CCSWeaponData* pPredictionBaseWeaponData = nullptr;

	void SetupAndRunPrediction(CBaseEntity* pPredEntity);
	void   Reset();
	void   Render(CBaseEntity* pBasePlayer);
	void   RunPrediction();
	void   Simulate();
	void   Setup();
	size_t Advance(size_t iTick);
	bool   Detonate(size_t iTick, Trace_t& pEngineTrace);
	void   ResolveFlyCollisionBounce(Trace_t& pEngineTrace, Vector3D& vecStart, Vector3D& vecMove, Vector3D& vecVelocity, std::vector< Vector3D >& mBounces);
	void   PhysicsPushEntity(Vector3D& vecStart, const Vector3D& vecMove, Trace_t& pEngineTrace, CBaseEntity* pPlayer);
	void   PhysicsAddGravityMove(Vector3D& vecMove, Vector3D& vecVelocity);
	void   PhysicsClipVelocity(const Vector3D& vecIn, const Vector3D& vecNormal, Vector3D& vecOut, float flOverBounce);
}
