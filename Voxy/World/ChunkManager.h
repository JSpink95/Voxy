#pragma once
#include "CoreTypes.h"
#include "Utility/ThreadRunnable.h"
#include "Utility/TickDispatcher.h"

struct Entity;
struct WorldGenerator;
struct ChunkMap;
struct Chunk;

glm::ivec4 CalculateViewBounds(glm::vec3 const& wp, s32 size);

struct ChunkLoader: public IThreadRunnable
{
	static Reference<ChunkLoader> Create(Reference<WorldGenerator> generator, bool generateData)
	{
		return MakeRef<ChunkLoader>(generator, generateData);
	}

	ChunkLoader(Reference<WorldGenerator> generator, bool generateData);

	void Thread_Execute();
	void ScheduleChunk(Reference<Chunk> chunk);

	bool generateNewData = true;
	Reference<WorldGenerator> generator = nullptr;
	std::mutex chunkMutex;
	List<Reference<Chunk>> chunks = {};
};

struct ChunkManager
	: public IThreadRunnable
	, public EventListener_TickGroup
{
	static Reference<ChunkManager> Create(Reference<Entity> player, Reference<WorldGenerator> generator)
	{
		return MakeRef<ChunkManager>(player, generator);
	}

	ChunkManager(Reference<Entity> player, Reference<WorldGenerator> generator);
	virtual ~ChunkManager();

	void OnTickGroup(f32 deltaTime) override;
	void Thread_Run() final;
	void Thread_Execute() final;

	void Thread_ReloadChunk(Reference<Chunk> chunk);

	// external references
	Reference<Entity> player = nullptr;
	Reference<WorldGenerator> generator = nullptr;

	// owned data
	Reference<ChunkMap> chunks = nullptr;

private:
	void RemoveOutOfBoundsChunks(glm::ivec4 const& bounds);
	void PrepareNewChunks(glm::ivec4 const& bounds);
	void RegenerateChunkMeshes();

	void Thread_LoadChunk();
	void Thread_ReloadChunk();

private:
	// thread data
	std::mutex chunksToLoadMutex;
	List<glm::ivec2> chunksToLoad = {};

	std::mutex chunksToReloadMutex;
	List<Reference<Chunk>> chunksToReload;

	u32 workerThreadIndex = 0u;
	Array<Reference<ChunkLoader>, 8u> chunkWorkerThreads = {};
};
