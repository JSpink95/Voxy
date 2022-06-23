#pragma once
#include "CoreTypes.h"
#include "Graphics/VertexBatcher.h"

struct Entity;

struct ParticleVertex
{
	static inline List<GLenum> Attributes = { GL_FLOAT_VEC3, GL_FLOAT_VEC4 };

	glm::vec3 a_Position;
	glm::vec4 a_Color;
};

struct ParticleInstance
{
	glm::vec3 position = glm::vec3(0.0f);
	glm::vec3 velocity = glm::vec3(0.0f);
	f32 lifetime = 1.0f;
};


namespace Particle
{
	enum class EmitMode
	{
		Infinite, Immediate
	};

	template<typename T>
	struct Range
	{
		T min = T(0);
		T max = T(0);
	};

	struct Emitter
	{
		virtual void Emit(ParticleInstance& particle) = 0;
	};

	struct FountainEmitter: Emitter
	{
		void Emit(ParticleInstance& particle) final;

		f32 velocityRadius = 1.0f;
		Range<f32> strength = { 0.5f, 1.0f };
		Range<f32> lifetime = { 1.0f, 1.0f };
	};

	struct SphereEmitter : Emitter
	{
		void Emit(ParticleInstance& particle) final;

		f32 radius = 1.0f;
		Range<f32> strength = { 0.5f, 1.0f };
		Range<f32> lifetime = { 1.0f, 1.0f };
	};

	struct BoxEmitter : Emitter
	{
		void Emit(ParticleInstance& particle) final;

		glm::vec3 halfSize = glm::vec3(0.5f);
		Range<f32> strength = { 0.5f, 1.0f };
		Range<f32> lifetime = { 1.0f, 1.0f };
	};

	struct Updater
	{
		virtual void Update(ParticleInstance& particle, f32 deltaTime) = 0;
	};

	struct UpdateLifetime: Updater
	{
		void Update(ParticleInstance& particle, f32 deltaTime) final;
	};

	struct UpdatePositionVelocity: Updater
	{
		void Update(ParticleInstance& particle, f32 deltaTime) final;

		f32 drag = 1.0f;
		glm::vec3 accelerant = glm::vec3(0.0f, -9.81f, 0.0f);
	};
}

struct ParticleSystem
{
	static Reference<ParticleSystem> Create()
	{
		return MakeRef<ParticleSystem>();
	}

	bool ValidMesh() const;
	bool IsDead() const;

	Reference<VertexMesh> GetMesh();
	void Update(Reference<Entity> owner, f32 deltaTime);

	f32 emissionRate = 0.05f;
	u32 maxParticles = 200;
	bool looping = true;
	Particle::EmitMode emitMode = Particle::EmitMode::Infinite;
	Reference<Particle::Emitter> emitter = nullptr;
	List<Reference<Particle::Updater>> updaters = {};

private:
	void EmitNewParticle();

	u32 emissionCount = 0u;
	f32 timeSinceLastEmission = 0.0f;
	List<ParticleInstance> particles = {};
	Reference<VertexBatcher<ParticleVertex>> batcher = VertexBatcher<ParticleVertex>::Create();
};
