#include "World/Biome.h"
#include "World/Generator.h"

std::string StringFromBiomeType(BiomeType biome)
{
	switch (biome)
	{
		case BiomeType::Flatland:
			return "Flatland";
		case BiomeType::Hilly:
			return "Hilly";
		case BiomeType::Mountain:
			return "Mountain";
		case BiomeType::Desert:
			return "Desert";
		default:
			return "Invalid";
	}
}

BiomeGenerator::BiomeGenerator(GeneratorSpecification const& generatorSpec)
{
	generator = Generator::Create(generatorSpec);
}

BiomeType BiomeGenerator::GetBiomeType(f64 x, f64 y)
{
	f32 value = generator->GetHeight(x, y);
	value *= (static_cast<f32>(BiomeType::Max) - 1.0f);

	s32 biomeIndex = (s32)glm::round(value);
	return static_cast<BiomeType>(biomeIndex);
}
