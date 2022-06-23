#pragma once
#include "CoreTypes.h"

struct Entity;

struct Component
{
	virtual void Initialise() {}

	Reference<Entity> owner = nullptr;
};

struct SceneComponent : public Component
{
	virtual void OnTick(f32 deltaTime) {}
};

template<typename T>
concept TickableComponent = Derived<T, SceneComponent>;
