#include "World/ChunkManager.h"
#include "World/WorldGenerator.h"
#include "World/ChunkMap.h"
#include "World/Chunk.h"

#include "Game/Entity.h"
#include "Game/Components.h"

#include <future>

constexpr s32 ChunkViewDistance = 12;

glm::ivec4 CalculateViewBounds(glm::vec3 const& wp, s32 size)
{
	glm::ivec4 bounds = {};
	// x bounds
	bounds.x = (s32)(wp.x / Chunk::Size) - ChunkViewDistance;
	bounds.y = (s32)(wp.x / Chunk::Size) + ChunkViewDistance;
	// z bounds
	bounds.z = (s32)(wp.z / Chunk::Size) - ChunkViewDistance;
	bounds.w = (s32)(wp.z / Chunk::Size) + ChunkViewDistance;

	return bounds;
}

ChunkLoader::ChunkLoader(Reference<WorldGenerator> generator, bool generateData)
	: generator(generator)
	, generateNewData(generateData)
{

}

void ChunkLoader::Thread_Execute()
{
	if (chunks.size() == 0)
		return;

	Reference<Chunk> chunkToLoad = nullptr;

	{
		std::lock_guard<std::mutex> guard(chunkMutex);
		chunkToLoad = chunks.front();
		chunks.erase(chunks.begin());
	}

	if (chunkToLoad != nullptr)
	{
		if (generateNewData)
		{
			auto chunkId = chunkToLoad->id;
			generator->GenerateChunkData(chunkToLoad, (f64)(chunkId.x * (s32)Chunk::Size), (f64)(chunkId.y * (s32)Chunk::Size));
		}

		chunkToLoad->Thread_LoadChunkMeshData();
		chunkToLoad->dirtyMeshFlag = true;
	}
}

void ChunkLoader::ScheduleChunk(Reference<Chunk> chunk)
{
	std::lock_guard<std::mutex> guard(chunkMutex);
	chunks.push_back(chunk);
}

ChunkManager::ChunkManager(Reference<Entity> player, Reference<WorldGenerator> generator)
	: player(player)
	, generator(generator)
{
	chunks = ChunkMap::Create();
	for (u64 index = 0; index < chunkWorkerThreads.size() - 1; ++index)
		chunkWorkerThreads.at(index) = ChunkLoader::Create(generator, true);

	// setup the reloader thread
	chunkWorkerThreads.at(chunkWorkerThreads.size() - 1) = ChunkLoader::Create(generator, false);
}

ChunkManager::~ChunkManager()
{
}

void ChunkManager::OnTickGroup(f32 deltaTime)
{
	glm::ivec4 renderBounds = CalculateViewBounds(player->GetComponent<TransformComponent>().position, ChunkViewDistance);

	RemoveOutOfBoundsChunks(renderBounds);
	PrepareNewChunks(renderBounds);
	RegenerateChunkMeshes();
}

void ChunkManager::RemoveOutOfBoundsChunks(glm::ivec4 const& bounds)
{
	std::vector<glm::ivec2> chunksToRemove = {};
	for (auto [location, chunk] : *chunks)
	{
		if (location.x < bounds.x || location.x > bounds.y ||
			location.y < bounds.z || location.y > bounds.w)
			chunksToRemove.push_back(location);
	}

	for (auto chunkId : chunksToRemove)
		chunks->chunks.erase(chunkId);
}

void ChunkManager::PrepareNewChunks(glm::ivec4 const& bounds)
{
	std::vector<glm::ivec2> chunksToCreate = {};
	for (s32 x = bounds.x; x <= bounds.y; ++x)
	{
		for (s32 z = bounds.z; z <= bounds.w; ++z)
		{
			auto chunkId = glm::ivec2(x, z);
			if (chunks->chunks.find(chunkId) == chunks->chunks.end())
			{
				chunks->EmplaceChunk(Chunk::Create(chunkId));
				chunksToCreate.push_back(chunkId);
			}
		}
	}

	if (chunksToCreate.size() > 0)
	{
		std::lock_guard<std::mutex> guard(chunksToLoadMutex);
		for (auto chunkId : chunksToCreate)
			chunksToLoad.push_back(chunkId);
	}
}

void ChunkManager::RegenerateChunkMeshes()
{
	for (auto& [location, chunk] : *chunks)
	{
		if (chunk->dirtyMeshFlag)
		{
			chunk->InvalidateMesh();
			chunk->dirtyMeshFlag = false;
		}
	}
}
void ChunkManager::Thread_Run()
{
	IThreadRunnable::Thread_Run();

	for (auto& worker : chunkWorkerThreads)
		worker->Thread_Run();
}

void ChunkManager::Thread_Execute()
{
	Thread_LoadChunk();
	Thread_ReloadChunk();
}

void ChunkManager::Thread_LoadChunk()
{
	if (chunksToLoad.empty())
		return;

	glm::ivec2 chunkId = {};
	{
		std::lock_guard<std::mutex> guard(chunksToLoadMutex);

		glm::vec3 playerLocation = player->GetComponent<TransformComponent>().position;
		std::sort(chunksToLoad.begin(), chunksToLoad.end(), [&playerLocation](glm::ivec2 const& lhs, glm::ivec2 const& rhs) -> bool
			{
				glm::vec3 ca = glm::vec3(lhs.x, playerLocation.y, lhs.y) * (f32)Chunk::Size;
				glm::vec3 cb = glm::vec3(rhs.x, playerLocation.y, rhs.y) * (f32)Chunk::Size;

				f32 distA = glm::distance2(ca, playerLocation);
				f32 distB = glm::distance2(cb, playerLocation);

				return distA > distB;
			});

		chunkId = chunksToLoad.back();
		chunksToLoad.pop_back();
	}

	if (chunks->chunks.find(chunkId) == chunks->end())
		return;

	Reference<Chunk> chunk = chunks->chunks.at(chunkId);
	chunkWorkerThreads.at(workerThreadIndex)->ScheduleChunk(chunk);
	workerThreadIndex = (workerThreadIndex + 1) % (chunkWorkerThreads.size() - 1);
}

void ChunkManager::Thread_ReloadChunk()
{
	if (chunksToReload.empty())
		return;

	Reference<Chunk> chunk = nullptr;
	{
		std::lock_guard<std::mutex> guard(chunksToReloadMutex);
		chunk = chunksToReload.front();
		chunksToReload.erase(chunksToReload.begin());
	}

	chunkWorkerThreads.at(chunkWorkerThreads.size() - 1)->ScheduleChunk(chunk);
}

void ChunkManager::Thread_ReloadChunk(Reference<Chunk> chunk)
{
	std::lock_guard<std::mutex> guard(chunksToReloadMutex);
	chunksToReload.push_back(chunk);
}
