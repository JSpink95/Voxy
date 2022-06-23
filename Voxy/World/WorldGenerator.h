#pragma once
#include "CoreTypes.h"
#include "World/Biome.h"

struct Chunk;
struct Generator;

struct WorldGenerator
{
	static Reference<WorldGenerator> Create(s32 seed = 1337)
	{
		return MakeRef<WorldGenerator>(seed);
	}

	WorldGenerator(s32 seed = 1337);

	BiomeType GetBiome(f64 x, f64 y);
	std::tuple<f32, BiomeType> GetNoise(f64 x, f64 y);
	f32 GetNoise(f64 x, f64 y, BiomeType type);
	u32 GetNoiseInArea(f64 x, f64 y, f64 hw, f64 hh, Array<u32, static_cast<size_t>(BiomeType::Max)>& heights);
	void GenerateChunkData(Reference<Chunk> chunk, f64 wx, f64 wz);

	Reference<BiomeGenerator> biome = nullptr;
	Reference<Generator> biomeBlender = nullptr;
	Array<Reference<Generator>, static_cast<size_t>(BiomeType::Max)> generators;
};