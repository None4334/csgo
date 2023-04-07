
struct TAnimationRecord;

struct TPlayerHurtInfo {
	float iDamageHurt = 0.0f;
	int iHealth = -1;
	int iHurtHitgroup = -1;
	int iHitbox = -1;
	Vector3D vecEndHurtPos;
	bool bHasHurtInfo = false;
	bool bUsed = false;
	int iSystemTickCount = 0;
	int iTickCount = 0;
};

struct TFireBulletInfo {
	TDynamicStack<Vector3D, 20> stkImpacts;
	CBaseEntity* pOwnerEntity = nullptr;
	int iTickcount = 0;

	struct {
		bool bUsedForHitmarker = false;
		bool bUsedForTracerAndImpacts = false;

		bool FullyUsed() {
			return bUsedForHitmarker && bUsedForTracerAndImpacts;
		}

	} UsageInfo;

	TPlayerHurtInfo PlayerHurtInfo;
};


namespace TFireBullet {

	inline TDynamicStack<TFireBulletInfo,1500> stkFireBullets;
	inline TDynamicStack<TPlayerHurtInfo, 1500> stkPlayerHurt;

	void AddModelRecord(TEntityObject* Object, TAnimationRecord* pAnimationRecord);
	void RenderRecords();
	void RenderRecord(TEntityObject* Object, TModelShotRecord* Record);

	void Routine();
	void DrawTracer(const Vector3D& vecStart, const Vector3D& vecEnd);
	void OnEvent(IGameEvent* pEvent, FNV1A_t uNameHash);
}
