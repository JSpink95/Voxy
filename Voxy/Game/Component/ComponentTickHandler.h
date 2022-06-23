#pragma once
#include "CoreTypes.h"
#include "Game/Component/Component.h"

//#include "Game/Entity.h"

struct TickHandlerComponent: public Component
{
	using ComponentTickFunction = std::function<void(f32)>;

	void OnTick(f32 deltaTime);

	//template<TickableComponent T>
	//void RegisterComponent();

	void RegisterTickFunction(ComponentTickFunction function);

	List<ComponentTickFunction> listeners;
};
#include "ComponentTickHandler.inl"