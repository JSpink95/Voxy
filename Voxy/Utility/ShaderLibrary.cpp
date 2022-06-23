#include "Utility/ShaderLibrary.h"
#include "Utility/FileIO.h"

#include "Graphics/Shader.h"

namespace Helper
{
	GLenum TypeFromExtension(std::string const& extension)
	{
		if (extension == ".vert")
		{
			return GL_VERTEX_SHADER;
		}
		
		if (extension == ".geom")
		{
			return GL_GEOMETRY_SHADER;
		}

		if (extension == ".frag")
		{
			return GL_FRAGMENT_SHADER;
		}

		return GL_NONE;
	}
}

Reference<ShaderLibrary> ShaderLibrary::Create()
{
	return MakeRef<ShaderLibrary>();
}

ShaderLibrary::ShaderLibrary()
{
	List<std::filesystem::path> shaderpaths;
	FileIO::RecursiveParseForFiles("shaders\\", { ".vert", ".geom", ".frag" }, shaderpaths);

	std::unordered_map<std::string, ShaderSpecification> specs;
	for (auto& path : shaderpaths)
	{
		std::string id = path.string();
		id = id.substr(0, id.find_last_of('.'));

		if (specs.find(id) == specs.end())
		{
			Console::Log("Found shader {0}", id);
			specs.emplace(id, ShaderSpecification{});
		}

		std::string pathString = path.extension().string();
		GLenum shaderType = Helper::TypeFromExtension(pathString);
		specs.at(id).shaders.push_back({ path.string(), shaderType });
	}

	for (auto&[id, spec] : specs)
	{
		shaders.emplace(id, Shader::Create(spec));
	}
}

Reference<Shader> ShaderLibrary::FindShader(std::string_view id)
{
	if (auto it = shaders.find(id.data()); it != shaders.end())
	{
		return it->second;
	}

	Console::LogWarning("Failed to find shader {0}", id.data());
}

void ShaderLibrary::ReloadShaders()
{
	for (auto& [id, shader] : shaders)
	{
		shader->InvalidateProgram();
	}
}