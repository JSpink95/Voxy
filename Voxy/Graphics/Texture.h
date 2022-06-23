#pragma once
#include "CoreTypes.h"

struct TextureSpecification
{
	GLenum filterMode = GL_LINEAR;
	GLenum wrapMode = GL_CLAMP_TO_EDGE;
};

struct Texture
{
	static Reference<Texture> Create(std::string_view path, TextureSpecification const& specification);

	Texture(std::string_view path, TextureSpecification const& specification);
	~Texture();

	void SetFilterMode(GLenum filter);
	void SetWrapMode(GLenum wrap);

	u32 id = 0u;
};