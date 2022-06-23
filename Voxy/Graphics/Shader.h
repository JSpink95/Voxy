#pragma once
#include "CoreTypes.h"

#include <unordered_map>

struct ShaderSpecification
{
	std::vector<std::tuple<std::string, GLenum>> shaders;
};

struct ShaderUniformCache
{
	u32 program = 0u;
	std::unordered_map<std::string, int> uniforms = {};

	int FindOrAddUniform(std::string_view name);
};

struct Shader
{
	static inline std::shared_ptr<Shader> Create(ShaderSpecification const& spec)
	{
		return std::make_shared<Shader>(spec);
	}

	Shader(ShaderSpecification const& spec);
	~Shader();

	Reference<Shader> Clone();

	// essentially a hot reload
	void InvalidateProgram();

	void Enable();
	void Disable();

	void SetMat4(std::string_view name, glm::mat4 const& value);
	void SetTexture(std::string_view name, u32 textureId, u32 textureLocation);
	void SetFloat(std::string_view name, f32 value);
	void SetFloats(std::string_view name, f32* values, u64 count);
	void SetVec3s(std::string_view name, glm::vec3* values, u64 count);
	void SetVec4s(std::string_view name, glm::vec4* values, u64 count);

	ShaderSpecification specification = {};
	u32 program = 0u;
	ShaderUniformCache uniformCache = {};
};