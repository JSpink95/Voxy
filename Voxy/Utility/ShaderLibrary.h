#pragma once
#include "CoreTypes.h"

struct Shader;

struct ShaderLibrary
{
	static Reference<ShaderLibrary> Create();

	ShaderLibrary();

	Reference<Shader> FindShader(std::string_view id);
	void ReloadShaders();

	std::unordered_map<std::string, Reference<Shader>> shaders;
};
