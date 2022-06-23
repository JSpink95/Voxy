#include "Game/GameWorld.h"

#include "World/WorldGenerator.h"
#include "World/ChunkManager.h"
#include "Physics/ChunkPhysics.h"

GameWorld::GameWorld(u64 seed)
	: worldGenerator(WorldGenerator::Create(seed))
{

}

void GameWorld::InitialiseChunkManager(Reference<Entity> player)
{
	chunkManager = ChunkManager::Create(player, worldGenerator);
	chunkManager->Thread_Run();

	chunkPhysics = ChunkPhysics::Create(chunkManager->chunks, shared_from_this());
}
