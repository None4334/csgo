

class CBaseEntity;
class CBaseCombatWeapon;
class CCSWeaponData;
namespace TLocalEngine {
	inline Vector3D vecViewAngles;
	inline Vector3D vecAbsOrigin;
	inline Vector3D vecOrigin;
	inline bool bInCreateMove = false;
	inline bool bInFakeDuck = false;
	inline float flVelocityModifier = 0.0f;
	inline int iVelocityModifierTick = 0;

	namespace TTraceSystem {
		void TraceHull(Vector3D& vecStart, Vector3D& vecEnd, Trace_t& pEngineTrace, CBaseEntity* pPlayer);
		void TraceHull(const Vector3D& src, const Vector3D& dst, const Vector3D& mins, const Vector3D& maxs, int mask, CBaseEntity* entity, int collision_group, CGameTrace* trace);
		void TraceLine(const Vector3D& src, const Vector3D& dst, int mask, CBaseEntity* entity, int collision_group, CGameTrace* trace);
		float ScaleDamage(CBaseEntity* pPlayer, float flDamage, float flArmorRatio, int iHitGroup);
		bool DoesHitHeadClipTrace(Vector3D vecStart, Vector3D vecEnd, CBaseEntity* pBaseEntity);
		bool TraceDidHitWorld(Trace_t* Trace);
		bool TraceDidHitNonWorldEntity(Trace_t* Trace);
		inline bool bEngineTraceRunning = false;
	}

}



