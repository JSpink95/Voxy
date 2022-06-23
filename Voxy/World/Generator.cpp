#include "World/Generator.h"

Generator::Generator(GeneratorSpecification const& spec)
	: noise(spec.seed)
	, amplitude(spec.amplitude)
	, normalised(spec.normalised)
{
	noise.SetNoiseType(spec.noiseType);
	noise.SetFrequency(spec.frequency);

	// setup fractal data
	noise.SetFractalType(spec.fractal.type);
	noise.SetFractalOctaves(spec.fractal.octaves);

	// setup cellular data
	noise.SetCellularDistanceFunction(spec.cellular.distanceFunction);
	noise.SetCellularReturnType(spec.cellular.returnType);
	noise.SetCellularJitter(spec.cellular.jitter);
}

f32 Generator::GetHeight(f64 x, f64 y)
{
	f32 value = noise.GetNoise((f32)x, (f32)y);

	if (normalised)
		value = value * 0.5f + 0.5f;

	return value * amplitude;
}