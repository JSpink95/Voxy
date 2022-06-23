#pragma once
#include "CoreTypes.h"
#include "Utility/EventDispatcher.h"

struct Chunk;
struct Block;

namespace Utils
{
	glm::ivec2 WorldCoordinatesToChunkId(f32 x, f32 z);
	glm::ivec2 WorldCoordinatesToChunkId(glm::vec3 const& coords);
	glm::vec3 ChunkIdToWorldCoord(glm::ivec2 const& chunkId);
	std::tuple<glm::ivec3, glm::ivec2> WorldCoordsToLocalBlockCoords(s32 x, s32 y, s32 z);
}

struct BlockDestroyedArgs
{
	Reference<Chunk> chunk = nullptr;
	Block* block = nullptr;
};

DECLARE_EVENT_SYSTEM_ARG1(BlockDestroyed, BlockDestroyedArgs const&);

struct ChunkMap
	: public EventDispatcher_BlockDestroyed
{
	static Reference<ChunkMap> Create()
	{
		return MakeRef<ChunkMap>();
	}

	auto begin() { return chunks.begin(); }
	auto end() { return chunks.end(); }

	auto begin() const { return chunks.begin(); }
	auto end() const { return chunks.end(); }

	Reference<Chunk> FindChunk(glm::ivec2 const& location) const;
	Reference<Chunk> FindOrAddChunk(glm::ivec2 const& location);
	void EmplaceChunk(Reference<Chunk> chunk);

	void DestroyBlock(glm::vec3 const& world);
	void DestroyBlock(s32 x, s32 y, s32 z);

	Reference<Chunk> GetChunkAt(glm::vec3 const& world);
	Reference<Chunk> GetChunkAt(s32 x, s32 y, s32 z);

	Block* GetHighestBlock(s32 x, s32 z);
	Block* GetBlockAt(glm::vec3 const& world);
	Block* GetBlockAt(s32 x, s32 y, s32 z);
	
	void GetBlocksInBounds(glm::vec3 const& min, glm::vec3 const& max, std::vector<Block*>& blocks);

	std::unordered_map<glm::ivec2, Reference<Chunk>> chunks = {};
};
