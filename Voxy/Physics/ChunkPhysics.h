#pragma once
#include "CoreTypes.h"
#include "Utility/TickDispatcher.h"

struct ChunkMap;
struct GameWorld;
struct Block;
struct Entity;

struct ChunkPhysics
	: public EventListener_TickGroup
{
	static constexpr f32 Gravity = -9.81f;// *9.81f;
	static Reference<ChunkPhysics> Create(Reference<ChunkMap> chunks, Reference<GameWorld> world)
	{
		return MakeRef<ChunkPhysics>(chunks, world);
	}

	ChunkPhysics(Reference<ChunkMap> chunks, Reference<GameWorld> world);
	void OnTickGroup(f32 deltaTime) override;

	Block* RayCast(glm::vec3 const& origin, glm::vec3 const& direction, f32 reach);

	// overlap tests
	void SphereOverlapDynamic(glm::vec3 const& centre, f32 radius, List<Reference<Entity>>& entities);

	Reference<ChunkMap> chunks = nullptr;
	Reference<GameWorld> world = nullptr;
};