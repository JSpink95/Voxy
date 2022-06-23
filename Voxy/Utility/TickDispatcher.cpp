#include "Utility/TickDispatcher.h"

void TickDispatcher::DispatchTicks(f32 deltaTime)
{
	for (auto dispatcher : groups)
	{
		dispatcher.DispatchEvent_TickGroup(deltaTime);
	}
}

void TickDispatcher::SubscribeTickListener(EventListener_TickGroup* listener, TickGroup group/* = TickGroup::PRE_PHYSICS*/)
{
	groups.at(static_cast<u64>(group)).SubscribeListener_TickGroup(listener);
}
