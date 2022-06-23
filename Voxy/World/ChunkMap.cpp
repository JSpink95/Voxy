#include "World/ChunkMap.h"
#include "World/Chunk.h"

namespace Utils
{
	glm::ivec2 WorldCoordinatesToChunkId(f32 x, f32 z)
	{
		glm::ivec2 chunkId = glm::ivec2(0, 0);
		chunkId.x = (s32)(((x < 0.0f) ? ((x + 1.0f) - (f32)Chunk::Size) : x) / (f32)Chunk::Size);
		chunkId.y = (s32)(((z < 0.0f) ? ((z + 1.0f) - (f32)Chunk::Size) : z) / (f32)Chunk::Size);

		return chunkId;
	}

	glm::ivec2 WorldCoordinatesToChunkId(glm::vec3 const& coords)
	{
		return WorldCoordinatesToChunkId(glm::floor(coords.x), glm::floor(coords.z));
	}

	glm::vec3 ChunkIdToWorldCoord(glm::ivec2 const& chunkId)
	{
		return glm::vec3((f32)chunkId.x, 0.0f, (f32)chunkId.y) * (f32)Chunk::Size;
	}

	std::tuple<glm::ivec3, glm::ivec2> WorldCoordsToLocalBlockCoords(s32 x, s32 y, s32 z)
	{
		auto chunkId = WorldCoordinatesToChunkId(glm::vec3(x, y, z) + 0.5f);
		auto chunkWorldLocation = ChunkIdToWorldCoord(chunkId);

		glm::ivec3 localCoords = {
			(s32)((f32)x - chunkWorldLocation.x),
			(s32)((f32)y - chunkWorldLocation.y),
			(s32)((f32)z - chunkWorldLocation.z)
		};

		return std::make_tuple(localCoords, chunkId);
	}
}

Reference<Chunk> ChunkMap::FindChunk(glm::ivec2 const& location) const
{
	auto it = chunks.find(location);
	if (it != chunks.end())
		return it->second;

	return nullptr;
}

Reference<Chunk> ChunkMap::FindOrAddChunk(glm::ivec2 const& location)
{
	if (auto chunk = FindChunk(location))
		return chunk;

	Reference<Chunk> chunk = Chunk::Create(location);
	chunks.emplace(location, chunk);
	return chunk;
}

void ChunkMap::EmplaceChunk(Reference<Chunk> chunk)
{
	chunks.emplace(chunk->id, chunk);
}

void ChunkMap::DestroyBlock(glm::vec3 const& world)
{
	DestroyBlock((s32)glm::floor(world.x), (s32)glm::floor(world.y), (s32)glm::floor(world.z));
}

void ChunkMap::DestroyBlock(s32 x, s32 y, s32 z)
{
	Block* block = GetBlockAt(x, y, z);
	if (block != nullptr)
	{
		block->active = false;
		auto chunk = GetChunkAt(x, y, z);

		DispatchEvent_BlockDestroyed(BlockDestroyedArgs{ chunk, block });
	}
}

Reference<Chunk> ChunkMap::GetChunkAt(glm::vec3 const& world)
{
	return GetChunkAt((s32)glm::floor(world.x), (s32)glm::floor(world.y), (s32)glm::floor(world.z));
}

Reference<Chunk> ChunkMap::GetChunkAt(s32 x, s32 y, s32 z)
{
	auto [local, chunkId] = Utils::WorldCoordsToLocalBlockCoords(x, y, z);
	return FindChunk(chunkId);
}

Block* ChunkMap::GetHighestBlock(s32 x, s32 z)
{
	auto [local, chunkId] = Utils::WorldCoordsToLocalBlockCoords(x, 0, z);
	if (auto chunk = FindChunk(chunkId))
	{
		return chunk->GetHighestBlock(local.x, local.z);
	}

	return nullptr;
}

Block* ChunkMap::GetBlockAt(glm::vec3 const& world)
{
	return GetBlockAt((s32)glm::floor(world.x), (s32)glm::floor(world.y), (s32)glm::floor(world.z));
}

Block* ChunkMap::GetBlockAt(s32 x, s32 y, s32 z)
{
	auto [local, chunkId] = Utils::WorldCoordsToLocalBlockCoords(x, y, z);

	auto chunk = FindChunk(chunkId);
	if (chunk == nullptr)
		return nullptr;

	return chunk->Get(local.x, local.y, local.z);
}

void ChunkMap::GetBlocksInBounds(glm::vec3 const& min, glm::vec3 const& max, std::vector<Block*>& blocks)
{
	auto ToId = [](f32 value)
	{
		return glm::floor(value);
	};

	for (s32 x = (s32)ToId(min.x); x <= (s32)ToId(max.x); ++x)
	{
		for (s32 y = (s32)ToId(min.y); y <= (s32)ToId(max.y); ++y)
		{
			for (s32 z = (s32)ToId(min.z); z <= (s32)ToId(max.z); ++z)
			{
				Block* block = GetBlockAt(x, y, z);
				if (block != nullptr && block->active)
					blocks.push_back(block);
			}
		}
	}
}