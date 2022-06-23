#include "World/WorldGenerator.h"
#include "World/Generator.h"
#include "World/Chunk.h"
#include "World/ChunkMap.h"

#include "Game/Item.h"

constexpr f64 GlobalBaseHeight = 48.0;
constexpr f64 BiomeBlendChunks = 0.5;
constexpr f64 BiomeBlendSize = BiomeBlendChunks * (f64)Chunk::Size;

u64 GetBlockTypeForBiome(BiomeType biome, s32 distanceToMaxBlockY)
{
	if (distanceToMaxBlockY >= 12)
	{
		return ItemID::Block_Stone;
	}

	switch (biome)
	{
		default:
		case BiomeType::Hilly:
		case BiomeType::Flatland:
		{
			if (distanceToMaxBlockY <= 1)
			{
				return ItemID::Block_Grass;
			}

			if (distanceToMaxBlockY <= 3)
			{
				return ItemID::Block_Dirt;
			}

			return ItemID::Block_Stone;
		}
		case BiomeType::Mountain:
		{
			return ItemID::Block_Stone;
		}
		case BiomeType::Desert:
		{
			return ItemID::Block_Sand;
		}
	}
}

static Array<glm::vec4, static_cast<size_t>(BiomeType::Max)> BiomeColorMap =
{
	glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
	glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
	glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
	glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
};

f32 SoftMaximum(f32 x, f32 y, f32 smoothing = 1.0f)
{
	f32 maximum = glm::max(x, y);
	f32 minimum = glm::min(x, y);
	return maximum + glm::log(smoothing + glm::exp(minimum - maximum));
}

WorldGenerator::WorldGenerator(s32 seed/* = 1337*/)
{
	GeneratorSpecification biomeGeneratorSpec = {};
	biomeGeneratorSpec.seed = seed;
	biomeGeneratorSpec.noiseType = FastNoiseLite::NoiseType_Cellular;
	biomeGeneratorSpec.frequency = 0.0025f;
	biomeGeneratorSpec.cellular.distanceFunction = FastNoiseLite::CellularDistanceFunction_Hybrid;
	biomeGeneratorSpec.cellular.returnType = FastNoiseLite::CellularReturnType_CellValue;
	biomeGeneratorSpec.cellular.jitter = 1.0f;
	biomeGeneratorSpec.normalised = true;

	biome = BiomeGenerator::Create(biomeGeneratorSpec);

#if 1
	GeneratorSpecification biomeBlendGeneratorSpec = {};
	biomeBlendGeneratorSpec.seed = seed;
	biomeBlendGeneratorSpec.noiseType = FastNoiseLite::NoiseType_Perlin;
	biomeBlendGeneratorSpec.frequency = 100.0f;
	biomeBlendGeneratorSpec.amplitude = 5.0f;
	biomeBlendGeneratorSpec.normalised = false;

	biomeBlender = Generator::Create(biomeBlendGeneratorSpec);
#endif

	GeneratorSpecification flatlandGeneratorSpec = {};
	flatlandGeneratorSpec.seed = seed;
	flatlandGeneratorSpec.frequency = 0.01f;
	flatlandGeneratorSpec.amplitude = 3.0f;
	flatlandGeneratorSpec.fractal.type = FastNoiseLite::FractalType_Ridged;
	flatlandGeneratorSpec.fractal.octaves = 2;
	flatlandGeneratorSpec.normalised = false;

	generators.at(static_cast<u32>(BiomeType::Flatland)) = Generator::Create(flatlandGeneratorSpec);

	GeneratorSpecification hillyGeneratorSpec = {};
	hillyGeneratorSpec.seed = seed;
	hillyGeneratorSpec.frequency = 0.002f;
	hillyGeneratorSpec.amplitude = 16.0f;
	hillyGeneratorSpec.fractal.type = FastNoiseLite::FractalType_PingPong;
	hillyGeneratorSpec.fractal.octaves = 6;
	hillyGeneratorSpec.normalised = false;

	generators.at(static_cast<u32>(BiomeType::Hilly)) = Generator::Create(hillyGeneratorSpec);

	GeneratorSpecification mountainGeneratorSpec = {};
	mountainGeneratorSpec.seed = seed;
	mountainGeneratorSpec.noiseType = FastNoiseLite::NoiseType_OpenSimplex2;
	mountainGeneratorSpec.frequency = 0.005f;
	mountainGeneratorSpec.amplitude = 48.0f;
	mountainGeneratorSpec.fractal.type = FastNoiseLite::FractalType_Ridged;
	mountainGeneratorSpec.fractal.octaves = 8;
	mountainGeneratorSpec.normalised = true;

	generators.at(static_cast<u32>(BiomeType::Mountain)) = Generator::Create(mountainGeneratorSpec);

	GeneratorSpecification desertGeneratorSpec = {};
	desertGeneratorSpec.seed = seed;
	desertGeneratorSpec.noiseType = FastNoiseLite::NoiseType_Perlin;
	desertGeneratorSpec.frequency = 0.01f;
	desertGeneratorSpec.amplitude = 5.0f;
	desertGeneratorSpec.fractal.type = FastNoiseLite::FractalType_Ridged;
	desertGeneratorSpec.normalised = false;

	generators.at(static_cast<u32>(BiomeType::Desert)) = Generator::Create(desertGeneratorSpec);
}

BiomeType WorldGenerator::GetBiome(f64 x, f64 y)
{
	f32 offset = 0.0f;// biomeBlender->GetHeight(x, y);

	return biome->GetBiomeType(x + offset, y );
}

std::tuple<f32, BiomeType> WorldGenerator::GetNoise(f64 x, f64 y)
{
	auto biomeType = GetBiome(x, y);
	return std::make_tuple(GetNoise(x, y, biomeType), biomeType);
}

f32 WorldGenerator::GetNoise(f64 x, f64 y, BiomeType type)
{
	auto generator = generators.at(static_cast<size_t>(type));
	return generator->GetHeight(x, y);
}

u32 WorldGenerator::GetNoiseInArea(
	  f64 x, f64 y, f64 hw, f64 hh
	, Array<u32, static_cast<size_t>(BiomeType::Max)>& heights)
{
	s32 minx = (s32)(x - hw);
	s32 maxx = (s32)(x + hw);
	s32 miny = (s32)(y - hh);
	s32 maxy = (s32)(y + hh);

	u32 samples = 0u;
	for (s32 xx = minx; xx <= maxx; ++xx)
	{
		for (s32 yy = miny; yy <= maxy; ++yy)
		{
			auto biomeType = GetBiome((f32)xx, (f32)yy);
			++heights.at(static_cast<size_t>(biomeType));
			++samples;
		}
	}

	return samples;
}

void WorldGenerator::GenerateChunkData(Reference<Chunk> chunk, f64 wx, f64 wz)
{
	auto chunkWorldPos = Utils::ChunkIdToWorldCoord(chunk->id);

	for (u32 x = 0; x < Chunk::Size; ++x)
	{
		for (u32 z = 0; z < Chunk::Size; ++z)
		{
			// get the biome samples in an area
			Array<u32, static_cast<size_t>(BiomeType::Max)> blendMap = { 0u };
			auto samples = GetNoiseInArea(wx + (f64)x, wz + (f64)z, BiomeBlendSize, BiomeBlendSize, blendMap);
			auto thisBiome = GetBiome(wx + (f64)x, wz + (f64)z);

			// calculate the blend weights
			Array<f32, static_cast<size_t>(BiomeType::Max)> blendWeights = {};
			for (u32 index = 0u; index < blendMap.size(); ++index)
			{
				blendWeights.at(index) = (f32)blendMap.at(index) / (f32)samples;
			}

			// then get heights
			Array<f32, static_cast<size_t>(BiomeType::Max)> heights = {};
			for (u32 index = 0u; index < blendMap.size(); ++index)
			{
				f32 blendWeight = blendWeights.at(index);
				if (blendWeight > 0.0f)
					heights.at(index) = GetNoise((f64)x + wx, (f64)z + wz, static_cast<BiomeType>(index));
				else
					heights.at(index) = 0.0f;
			}

			// finally blend it
			f32 height = 0.0f;
			glm::vec4 color = glm::vec4(0.0f);

			for (u32 index = 0u; index < blendMap.size(); ++index)
			{
				f32 blendWeight = blendWeights.at(index);
				if (glm::epsilonEqual(blendWeight, 0.0f, glm::epsilon<f32>()))
					continue;

				height += heights.at(index) * blendWeight;
				color += BiomeColorMap.at(index) * blendWeight;
			}

			height = glm::max(height + (f32)GlobalBaseHeight, 1.0f);

			u32 actualHeight = glm::round(height);
			for (u32 y = 0; y < actualHeight; ++y)
			{
				if (auto block = chunk->Get(x, y, z))
				{
					block->active = true;
					block->location = chunkWorldPos + (glm::vec3(x, y, z) + 0.5f);
					block->type = GetBlockTypeForBiome(thisBiome, actualHeight - y);
				}
			}
		}
	}
}
