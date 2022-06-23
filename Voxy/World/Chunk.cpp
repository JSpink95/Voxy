#include "World/Chunk.h"
#include "Graphics/VertexMesh.h"

constexpr size_t value = (1 << 2);

BEGIN_BITCODE_DEF(SurroundingNeighbours)
	BITCODE_VALUE(Left	, 0);
	BITCODE_VALUE(Right	, 1);
	BITCODE_VALUE(Bottom, 2);
	BITCODE_VALUE(Top	, 3);
	BITCODE_VALUE(Back	, 4);
	BITCODE_VALUE(Front	, 5);
	constexpr u32 All = (Left | Right | Bottom | Top | Back | Front);
END_BITCODE_DEF()

namespace Utils
{
	int GetSurroundingActiveNeighbourCodes(Chunk const* chunk, s32 x, s32 y, s32 z)
	{
		int result = 0u;

		if (chunk->IsActive(x - 1, y, z))
		{
			result |= SurroundingNeighbours::Left;
		}

		if (chunk->IsActive(x + 1, y, z))
		{
			result |= SurroundingNeighbours::Right;
		}

		if (chunk->IsActive(x, y - 1, z))
		{
			result |= SurroundingNeighbours::Bottom;
		}

		if (chunk->IsActive(x, y + 1, z))
		{
			result |= SurroundingNeighbours::Top;
		}

		if (chunk->IsActive(x, y, z - 1))
		{
			result |= SurroundingNeighbours::Back;
		}

		if (chunk->IsActive(x, y, z + 1))
		{
			result |= SurroundingNeighbours::Front;
		}

		return result;
	}
}

Chunk::Chunk(glm::ivec2 const& id)
	: id(id)
{
}

Block* Chunk::GetHighestBlock(s32 x, s32 z)
{
	if (x < 0 || z < 0)
	{
		return nullptr;
	}

	if (x >= Size || z >= Size)
	{
		return nullptr;
	}

	Block* found = nullptr;
	for (u32 y = Height - 1; y >= 0; --y)
	{
		if (IsActive(x, y, z))
		{
			found = Get(x, y, z);
			break;
		}
	}

	return found;
}

Block* Chunk::Get(s64 x, s64 y, s64 z)
{
	if (x < 0 || y < 0 || z < 0)
	{
		return nullptr;
	}

	if (x >= Size || y >= Height || z >= Size)
	{
		return nullptr;
	}

	u64 index = x + (y * Size) + (z * Size * Height);
	return &blocks.at(index);
}

Block const* Chunk::Get(s64 x, s64 y, s64 z) const
{
	if (x < 0 || y < 0 || z < 0)
	{
		return nullptr;
	}

	if (x >= Size || y >= Height || z >= Size)
	{
		return nullptr;
	}

	u64 index = x + (y * Size) + (z * Size * Height);
	return &blocks.at(index);
}

bool Chunk::IsBlockOccluded(s32 x, s32 y, s32 z) const
{
	s32 bitcode = Utils::GetSurroundingActiveNeighbourCodes(this, x, y, z);
	return (bitcode == SurroundingNeighbours::All);
}

void Chunk::Thread_LoadChunkMeshData()
{
	meshData.clear();

	//for (Block& block : blocks)
	//{
	//	if (!block.active)
	//	{
	//		continue;
	//	}
	//
	//	Vertex vertex =
	//	{
	//		.a_Position = block.location,
	//		.a_TextureIds = BlockUtility::GetTextureIdsForBlockType(block.type),
	//		.a_NeighbourActive = Utils::GetSurroundingActiveNeighbourCodes(this, (s32)glm::floor(block.location.x), (s32)glm::floor(block.location.y), (s32)glm::floor(block.location.z)),
	//	};
	//
	//	meshData.push_back(vertex);
	//}

	for (u32 y = 0; y < Height; ++y)
	{
		for (u32 x = 0; x < Size; ++x)
		{
			for (u32 z = 0; z < Size; ++z)
			{
				auto block = Get(x, y, z);
				if (block && block->active && !IsBlockOccluded(x, y, z))
				{
					Vertex vertex =
					{
						.a_Position = glm::vec3((float)x, (float)y, (float)z) + 0.5f,
						.a_TextureIds = BlockUtility::GetTextureIdsForBlockType(block->type),
						.a_NeighbourActive = Utils::GetSurroundingActiveNeighbourCodes(this, x, y, z),
					};
	
					meshData.push_back(vertex);
				}
			}
		}
	}
}

void Chunk::InvalidateMesh()
{
	VertexMeshSpecification vertexMeshSpec = {};
	vertexMeshSpec.attributes = { GL_FLOAT_VEC3, GL_INT_VEC3, GL_INT };
	vertexMeshSpec.indexCount = 0;
	vertexMeshSpec.vertexCount = meshData.size();
	vertexMeshSpec.vertices = meshData.data();

	mesh = VertexMesh::Create(vertexMeshSpec);
	meshData.clear();
}
