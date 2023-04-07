#include "../../includes.h"
#include "eventlogger.h"
#include "../renderer.h"

void TEventLogger::PushString(std::string szText) {
	
	std::unique_lock<std::shared_mutex> lock(TDirectXRender::drawMutex);

	if (stkEventLogs.full()) 
		stkEventLogs.pop_front();
	
	auto Object = stkEventLogs.add_back(TEventLog{ szText, TInterfaces::Globals->iTickCount } );

}

void TEventLogger::Routine() {

	if (!stkEventLogs.size())
		return;

	if (stkEventLogs.first() && stkEventLogs.first()->iTickCount + 175 < TInterfaces::Globals->iTickCount) 
		stkEventLogs.pop_front();

	for (int Current = 0; Current < stkEventLogs.size(); Current++)
	{
		int CurrentIndex = Current;

		auto Object = stkEventLogs[Current];

		Vector2D vecPosition = Vector2D(2, 2 + (CurrentIndex * 12));

		std::string szText = (const char*)Object->Characters;
		TDirectXRender::Text(TDirectXRender::TDirectFonts::VerdanaMedium, szText, vecPosition, Vector4D(235, 235, 235, 235), false, true, Vector4D(24, 24, 24, 225));

	}
}


