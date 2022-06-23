#pragma once
#include "CoreTypes.h"
#include "Game/Component/Component.h"

struct HealthComponent : public Component
{
	static constexpr f32 MaxHealth = 100.0f;

	std::tuple<f32, s32> ApplyHealth(f32 amount);
	f32 Percentage01() const;

	f32 health = 100.0f;
};
