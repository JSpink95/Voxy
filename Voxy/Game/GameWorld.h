#pragma once
#include "CoreTypes.h"
#include <entt.hpp>

struct WorldGenerator;
struct ChunkManager;
struct ChunkPhysics;
struct Entity;

using EntityID = entt::entity;

struct GameWorld: public std::enable_shared_from_this<GameWorld>
{
	static inline auto Create(u64 seed)
	{
		return MakeRef<GameWorld>(seed);
	}

	GameWorld(u64 seed);
	void InitialiseChunkManager(Reference<Entity> player);

	entt::registry registry;

	Reference<WorldGenerator> worldGenerator = nullptr;
	Reference<ChunkManager> chunkManager = nullptr;
	Reference<ChunkPhysics> chunkPhysics = nullptr;
};