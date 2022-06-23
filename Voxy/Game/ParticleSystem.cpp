#include "Game/ParticleSystem.h"

#include "Game/Entity.h"
#include "Game/Components.h"

#include <glm/gtc/random.hpp>

namespace Particle
{
	static constexpr bool IsDead(ParticleInstance const& particle)
	{
		return particle.lifetime <= 0.0f;
	}

	void FountainEmitter::Emit(ParticleInstance& particle)
	{
		glm::vec2 disk = glm::diskRand(velocityRadius);
		particle.velocity = glm::normalize(glm::vec3(disk.x, 1.0f, disk.y));
		particle.velocity *= glm::linearRand(strength.min, strength.max);
		particle.lifetime = glm::linearRand(lifetime.min, lifetime.max);
	}
	
	void SphereEmitter::Emit(ParticleInstance& particle)
	{
		particle.position = glm::sphericalRand(radius);
		particle.velocity = glm::normalize(particle.position) * glm::linearRand(strength.min, strength.max);
		particle.lifetime = glm::linearRand(lifetime.min, lifetime.max);
	}

	void BoxEmitter::Emit(ParticleInstance& particle)
	{
		f32 radius = glm::length(halfSize);
		particle.position = glm::clamp(glm::sphericalRand(radius), -halfSize, halfSize);
		particle.velocity = glm::normalize(particle.position) * glm::linearRand(strength.min, strength.max);
		particle.lifetime = glm::linearRand(lifetime.min, lifetime.max);
	}
	
	void UpdateLifetime::Update(ParticleInstance& particle, f32 deltaTime)
	{
		particle.lifetime -= deltaTime;
	}

	void UpdatePositionVelocity::Update(ParticleInstance& particle, f32 deltaTime)
	{
		particle.velocity += accelerant * deltaTime * drag;
		particle.position += particle.velocity * deltaTime;
	}
}

bool ParticleSystem::ValidMesh() const
{
	return batcher->GetVertexCount() > 0;
}

bool ParticleSystem::IsDead() const
{
	return !looping && maxParticles == emissionCount && particles.empty();
}

Reference<VertexMesh> ParticleSystem::GetMesh()
{
	return batcher->GetMesh();
}

void ParticleSystem::Update(Reference<Entity> owner, f32 deltaTime)
{
	timeSinceLastEmission += deltaTime;
	particles.erase(std::remove_if(particles.begin(), particles.end(), Particle::IsDead), particles.end());

	if (looping || emissionCount < maxParticles)
	{
		switch (emitMode)
		{
			case Particle::EmitMode::Immediate:
			{
				if (particles.size() < maxParticles)
				{
					for (u32 index = particles.size(); index < maxParticles; ++index)
					{
						EmitNewParticle();
					}
				}
			} break;

			case Particle::EmitMode::Infinite:
			{

				if (timeSinceLastEmission >= emissionRate && particles.size() < maxParticles)
				{
					timeSinceLastEmission = 0.0f;
					EmitNewParticle();
				}
			} break;
		}
	}

	for (auto& particle : particles)
	{
		for (auto& updater : updaters)
			updater->Update(particle, deltaTime);
	}

	batcher->BeginBatch();

	for (auto& particle : particles)
	{
		ParticleVertex vertex =
		{
			.a_Position = particle.position,
			.a_Color = glm::vec4(1.0f, 0.0f, 1.0f, 1.0f)
		};

		batcher->SubmitVertexStream(&vertex, 1);
	}

	u64 count = batcher->EndBatch();
	batcher->GetMesh()->vertexCount = count;

	if (IsDead())
	{
		owner->AddComponent<KillComponent>();
	}
}

void ParticleSystem::EmitNewParticle()
{
	auto& particle = particles.emplace_back();
	if (emitter != nullptr)
	{
		emitter->Emit(particle);
	}

	++emissionCount;
}