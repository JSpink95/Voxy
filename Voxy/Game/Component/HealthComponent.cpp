#include "Game/Component/HealthComponent.h"

std::tuple<f32, s32> HealthComponent::ApplyHealth(f32 amount)
{
	f32 newHealth = health + amount;
	health = glm::clamp(newHealth, 0.0f, 100.0f);

	s32 state = (newHealth >= 100.0f) ? 1 : newHealth <= 0.0f ? -1 : 0;
	return std::make_tuple(newHealth, state);
}

f32 HealthComponent::Percentage01() const
{
	return (health / 100.0f);
}
