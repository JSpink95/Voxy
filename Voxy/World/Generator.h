#pragma once
#include "CoreTypes.h"
#include "Utility/FastNoiseLite.h"

struct FractalSpecification
{
	FastNoiseLite::FractalType type = FastNoiseLite::FractalType_None;
	s32 octaves = 4;
};

struct CellularSpecification
{
	FastNoiseLite::CellularDistanceFunction distanceFunction = FastNoiseLite::CellularDistanceFunction_Hybrid;
	FastNoiseLite::CellularReturnType returnType = FastNoiseLite::CellularReturnType_CellValue;
	f32 jitter = 0.0f;
};

struct GeneratorSpecification
{
	s32 seed = 1337;
	FastNoiseLite::NoiseType noiseType = FastNoiseLite::NoiseType_Perlin;
	f32 frequency = 0.05f;
	f32 amplitude = 1.0f;
	FractalSpecification fractal = {};
	CellularSpecification cellular = {};
	bool normalised = false;
};

struct Generator
{
	static Reference<Generator> Create(GeneratorSpecification const& spec)
	{
		return MakeRef<Generator>(spec);
	}

	Generator(GeneratorSpecification const& spec);
	f32 GetHeight(f64 x, f64 y);

	FastNoiseLite noise;
	f32 amplitude = 1.0f;
	bool normalised = false;
};