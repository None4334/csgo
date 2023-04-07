
struct TEntityObject;

struct TBoundingBox
{
	float x = 0.0f;
	float y = 0.0f;
	float w = 0.0f;
	float h = 0.0f;
};

struct TPlayerHurtInfo;
struct TFireBulletInfo;
namespace TVisuals
{

	void PerTick();
	namespace TLocal {
		void Removals();
		void RemoveSmoke();
		void ScopeLines();
		void View();
		void FireBulletRoutine();
		void Hitmarker(TPlayerHurtInfo* Object);
	}

	void DoPostScreenEffects(IClientModeShared* thisptr, CViewSetup* pSetup);

	void RunVisuals(ISurface* SurfaceHandle, unsigned int uPanel, bool bForceRepaint, bool bForce);
	void Player(TEntityObject* pEntityObject);
	void DroppedWeapons(TEntityObject* pEntityObject);

	inline ViewMatrix_t matCurrentWorldMatrix;

	namespace TUtils {
		bool GetBoundingBox(CBaseEntity* pPlayer, TBoundingBox& pBBox);
		bool WorldToScreen(const Vector3D& vecOrigin, Vector2D& vecScreen);
	}
}
