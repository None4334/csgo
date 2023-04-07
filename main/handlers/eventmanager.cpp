#include "../../includes.h"
#include "../sdk.h"
#include "entityhandler.h"
#include "../gvars.h"
#include "animationsmanager.h"
#include "../handlers/localengine.h"
#include "../handlers/eventmanager.h"
#include "../handlers/firebulletmanager.h"

void CEventListener::Setup()
{
	static std::deque<const char*> arrEvents = { TSTRING("player_hurt"),
		TSTRING("round_prestart"), 
		TSTRING("round_freeze_end"),
	    TSTRING("weapon_fire"),
	    TSTRING("bullet_impact"),
	    TSTRING("round_start"),
		TSTRING("round_end"),
	    TSTRING("item_purchase") };

	if (arrEvents.empty())
		return;

	for (auto szEvent : arrEvents)
	{
		TInterfaces::GameEvent->AddListener(this, szEvent, false);
		TInterfaces::GameEvent->FindListener(this, szEvent);
	}
}

void CEventListener::Destroy()
{
	TInterfaces::GameEvent->RemoveListener(this);
}


void CEventListener::FireGameEvent(IGameEvent* pEvent)
{
	if (pEvent == nullptr)
		return;

	const FNV1A_t uNameHash = FNV1A::Hash(pEvent->GetName());

	TFireBullet::OnEvent(pEvent,uNameHash);
}
