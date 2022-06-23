#pragma once
#include "CoreTypes.h"
#include "World/Block.h"

#include <atomic>

struct VertexMesh;

struct Chunk: std::enable_shared_from_this<Chunk>
{
	static constexpr u64 Size = 16u;
	static constexpr u64 Height = 128u;

	struct Vertex
	{
		glm::vec3 a_Position = glm::vec3(0.0f);
		glm::ivec3 a_TextureIds = glm::ivec3(0);
		int a_NeighbourActive = 0u;
	};

	static inline Reference<Chunk> Create(glm::ivec2 const& id)
	{
		return MakeRef<Chunk>(id);
	}

	Chunk(glm::ivec2 const& id);

	Block* GetHighestBlock(s32 x, s32 z);

	Block* Get(s64 x, s64 y, s64 z);
	Block const* Get(s64 x, s64 y, s64 z) const;

	inline bool IsActive(s32 x, s32 y, s32 z) const
	{
		auto block = Get(x, y, z);
		return block && block->active;
	}

	bool IsBlockOccluded(s32 x, s32 y, s32 z) const;

	void Thread_LoadChunkMeshData();
	void InvalidateMesh();

	glm::ivec2 id = {};
	Array<Block, Size* Size* Height> blocks = {};
	List<Vertex> meshData = {};

	bool dirtyMeshFlag = false;
	Reference<VertexMesh> mesh = nullptr;
};
