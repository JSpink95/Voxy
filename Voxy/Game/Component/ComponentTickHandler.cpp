#include "Game/Component/ComponentTickHandler.h"

void TickHandlerComponent::RegisterTickFunction(ComponentTickFunction function)
{
	listeners.push_back(function);
}

void TickHandlerComponent::OnTick(f32 deltaTime)
{
	for (auto listener : listeners)
	{
		listener(deltaTime);
	}
}
