

class CEventListener : public IGameEventListener2
{
public:
	void Setup();
	void Destroy();

	virtual void FireGameEvent(IGameEvent* pEvent) override;
	virtual int GetEventDebugID() override
	{
		return EVENT_DEBUG_ID_INIT;
	}
};

namespace TEventListener {
	inline CEventListener EventListener;
}
