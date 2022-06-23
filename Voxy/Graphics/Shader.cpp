#include "Graphics/Shader.h"
#include "Utility/FileIO.h"

#include <glm/gtc/type_ptr.hpp>

#include <algorithm>

namespace GLUtility
{
	bool CheckShaderCompilation(std::string_view path, GLuint shader)
	{
		GLint compileSuccess = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSuccess);

		if (compileSuccess == GL_FALSE)
		{
			GLint logLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

			std::vector<GLchar> log(logLength);
			glGetShaderInfoLog(shader, logLength, &logLength, log.data());

			Console::LogError("Shader:{1}\n{0}", log.data(), path);
			return false;
		}

		return true;
	}

	bool CheckProgramLinked(GLuint program)
	{
		GLint linkSuccess = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);

		if (linkSuccess == GL_FALSE)
		{
			GLint logLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

			std::vector<GLchar> log(logLength);
			glGetProgramInfoLog(program, logLength, &logLength, log.data());

			Console::LogError("{0}", log.data());
			return false;
		}

		return true;
	}
}

int ShaderUniformCache::FindOrAddUniform(std::string_view name)
{
	auto it = uniforms.find(name.data());
	if (it != uniforms.end())
		return it->second;

	return uniforms.emplace(name.data(), glGetUniformLocation(program, name.data())).second;
}

Shader::Shader(ShaderSpecification const& spec)
	: specification(spec)
{
	InvalidateProgram();
}

Shader::~Shader()
{
	glDeleteProgram(program);
}

Reference<Shader> Shader::Clone()
{
	return Shader::Create(specification);
}

void Shader::InvalidateProgram()
{
	if (program)
	{
		glDeleteProgram(program);
		program = 0u;
	}

	program = glCreateProgram();

	// reset the uniform cache
	uniformCache.program = program;
	uniformCache.uniforms.clear();

	std::vector<u32> shaders = {};
	for (auto& [path, type] : specification.shaders)
	{
		auto source = FileIO::LoadToString(path);
		auto shader = shaders.emplace_back(glCreateShader(type));

		auto csource = source.data();
		glShaderSource(shader, 1, &csource, nullptr);
		glCompileShader(shader);

		if (GLUtility::CheckShaderCompilation(path, shader))
		{
			glAttachShader(program, shader);
		}
	}

	glLinkProgram(program);
	GLUtility::CheckProgramLinked(program);

	auto DetachAndDelete = [this](GLuint shader)
	{
		glDetachShader(program, shader);
		glDeleteShader(shader);
	};

	std::for_each(shaders.begin(), shaders.end(), DetachAndDelete);
}

void Shader::Enable()
{
	glUseProgram(program);
}

void Shader::Disable()
{
	glUseProgram(0);
}

void Shader::SetMat4(std::string_view name, glm::mat4 const& value)
{
	if (auto location = uniformCache.FindOrAddUniform(name); location != -1)
	{
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
	}
}

void Shader::SetTexture(std::string_view name, u32 textureId, u32 textureLocation)
{
	glBindTextureUnit(textureLocation, textureId);
}

void Shader::SetFloat (std::string_view name, f32 value)
{
	if (auto location = uniformCache.FindOrAddUniform(name); location != -1)
	{
		glUniform1f(location, value);
	}
}

void Shader::SetFloats(std::string_view name, f32* values, u64 count)
{
	if (auto location = uniformCache.FindOrAddUniform(name); location != -1)
	{
		glUniform1fv(location, count, values);
	}
}

void Shader::SetVec3s(std::string_view name, glm::vec3* values, u64 count)
{
	if (auto location = uniformCache.FindOrAddUniform(name); location != -1)
	{
		glUniform3fv(location, count, glm::value_ptr(*values));
	}
}

void Shader::SetVec4s(std::string_view name, glm::vec4* values, u64 count)
{
	if (auto location = uniformCache.FindOrAddUniform(name); location != -1)
	{
		glUniform4fv(location, count, glm::value_ptr(*values));
	}
}
