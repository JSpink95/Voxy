#pragma once
#include "CoreTypes.h"
#include "Utility/EventDispatcher.h"

enum class TickGroup
{
	PRE_PHYSICS, POST_PHYSICS,
	Max
};

DECLARE_EVENT_SYSTEM_ARG1(TickGroup, f32);

struct TickDispatcher
{
	static Reference<TickDispatcher> Create()
	{
		return MakeRef<TickDispatcher>();
	}

	void DispatchTicks(f32 deltaTime);
	void SubscribeTickListener(EventListener_TickGroup* listener, TickGroup group = TickGroup::PRE_PHYSICS);

	Array<EventDispatcher_TickGroup, static_cast<u64>(TickGroup::Max)> groups;
};