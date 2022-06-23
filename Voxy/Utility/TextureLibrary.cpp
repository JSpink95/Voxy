#include "Utility/TextureLibrary.h"
#include "Utility/FileIO.h"

#include "Graphics/Texture.h"

TextureLibrary::TextureLibrary()
{
	List<std::string> texturePaths;
	FileIO::RecursiveParseForFiles("textures", ".png", texturePaths);

	TextureSpecification defaultTextureSpecification =
	{
		.filterMode = GL_NEAREST,
		.wrapMode = GL_CLAMP_TO_EDGE
	};

	for (auto& texturePath : texturePaths)
	{
		Console::Log("Found Texture {0}", texturePath);
		textures.emplace(texturePath, Texture::Create(texturePath, defaultTextureSpecification));
	}
}

Reference<Texture> TextureLibrary::FindTexture(std::string_view path)
{
	auto it = textures.find(path.data());
	if (it == textures.end())
	{
		Console::LogWarning("Failed to find texture \"{0}\"!", path);
		return nullptr;
	}

	return it->second;
}