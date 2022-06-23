#pragma once
#include "CoreTypes.h"

struct Chunk;

enum class BlockFace : u8
{
	BOTTOM, SIDE, TOP,
	Max
};

struct Block
{
	bool active = false;
	u16 type = 0u;
	glm::vec3 location = glm::vec3(0.0f);
};

struct BlockConfig
{
	f32 hardness = 100.0f;
	Array<std::string, static_cast<u64>(BlockFace::Max)> textures = { "", "", "" };
};

namespace BlockUtility
{
	constexpr u8 DirtTextureIndex = 0;
	constexpr u8 GrassTextureIndex = 1;
	constexpr u8 GrassSideTextureIndex = 2;
	constexpr u8 StoneTextureIndex = 3;
	constexpr u8 SandTextureIndex = 4;
	constexpr u8 TextureCount = 5;

	glm::ivec3 GetTextureIdsForBlockType(u64 type);
}

struct BlockConfigLibrary
{
	static auto Create()
	{
		return MakeRef<BlockConfigLibrary>();
	}

	BlockConfigLibrary();

	Reference<BlockConfig> FindConfig(u16 id);

private:
	std::unordered_map<u16, Reference<BlockConfig>> configs;
};

struct GBlockConfig
{
	static BlockConfig const* const GetBlockConfig(u16 type);
private:
	static std::unordered_map<u16, BlockConfig> configs;
};
