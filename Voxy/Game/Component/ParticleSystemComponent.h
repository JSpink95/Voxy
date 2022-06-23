#pragma once
#include "CoreTypes.h"
#include "Game/Component/Component.h"

struct ParticleSystem;

struct ParticleSystemComponent : public SceneComponent
{
	ParticleSystemComponent();

	void OnTick(f32 deltaTime) override;

	Reference<ParticleSystem> system = nullptr;
};
