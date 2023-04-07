
struct TAnimationInstance;

struct TNadeInfo;
struct TEntityObject
{
	CBaseEntity* pEntity = nullptr;
	int	nIndex = 0;
	int	nClass = 0;
	TAnimationInstance* AnimationInstance = nullptr;
	TNadeInfo* NadeInfo = nullptr;
	TDynamicStack<TModelShotRecord, 20> stkShotModel;

};

class CEntityListener : public IClientEntityListener 
{
public:
	void			Setup();
	void			Destroy();
	virtual void	OnEntityCreated(CBaseEntity* pEntity) override;
	virtual void	OnEntityDeleted(CBaseEntity* pEntity) override;
};




namespace TEntityListener {

	namespace TOnUpdate {
		void FillEntityList();
		void ValidateEntityList();
	}

	TEntityObject* GetEntityObject(CBaseEntity* pBaseEntity);

	void RemoveFromEntityList(int Index);
	void RemoveFromEntityList(CBaseEntity* Entity);
	bool CheckForEntityInStack(CBaseEntity* Entity, int Index = -1);
	bool VerifyEntityPlayerAndIntegrity(CBaseEntity* Entity);
	bool VerifyEntityIntegrity(TEntityObject* Object);
	void ClearAndDeallocateMemory();

	static CEntityListener EntityListener;
	inline std::vector<TEntityObject*> vecEntities;
}
