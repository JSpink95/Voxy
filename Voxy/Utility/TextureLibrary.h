#pragma once
#include "CoreTypes.h"

struct Texture;

struct TextureLibrary
{
	static Reference<TextureLibrary> Create()
	{
		return MakeRef<TextureLibrary>();
	}

	TextureLibrary();

	Reference<Texture> FindTexture(std::string_view path);

	std::unordered_map<std::string, Reference<Texture>> textures;
};
