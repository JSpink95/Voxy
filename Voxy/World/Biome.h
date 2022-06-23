#pragma once
#include "CoreTypes.h"

struct Generator;
struct GeneratorSpecification;

enum class BiomeType : u8
{
	Flatland, Hilly, Mountain, Desert,
	Max
};

std::string StringFromBiomeType(BiomeType biome);

struct BiomeGenerator
{
	static Reference<BiomeGenerator> Create(GeneratorSpecification const& generatorSpec)
	{
		return MakeRef<BiomeGenerator>(generatorSpec);
	}

	BiomeGenerator(GeneratorSpecification const& generatorSpec);
	BiomeType GetBiomeType(f64 x, f64 y);

	Reference<Generator> generator = nullptr;
};
