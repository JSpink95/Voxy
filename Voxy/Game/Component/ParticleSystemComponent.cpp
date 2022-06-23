#include "Game/Component/ParticleSystemComponent.h"
#include "Game/ParticleSystem.h"

ParticleSystemComponent::ParticleSystemComponent()
{
	system = ParticleSystem::Create();
}

void ParticleSystemComponent::OnTick(f32 deltaTime)
{
	if (system)
	{
		system->Update(owner, deltaTime);
	}
}