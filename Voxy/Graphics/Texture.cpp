#include "Graphics/Texture.h"

#include <stb/stb_image.h>

Reference<Texture> Texture::Create(std::string_view path, TextureSpecification const& specification)
{
	return MakeRef<Texture>(path, specification);
}

Texture::Texture(std::string_view path, TextureSpecification const& specification)
{
	stbi_set_flip_vertically_on_load(1);

	s32 width, height, channels;
	auto pixels = stbi_load(path.data(), &width, &height, &channels, 0);
	{
		glCreateTextures(GL_TEXTURE_2D, 1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, specification.filterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, specification.filterMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, specification.wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, specification.wrapMode);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		glGenerateTextureMipmap(id);
	}
	stbi_image_free(pixels);
}

Texture::~Texture()
{
	glDeleteTextures(1, &id);
}

void Texture::SetFilterMode(GLenum filter)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::SetWrapMode(GLenum wrap)
{
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glBindTexture(GL_TEXTURE_2D, 0);
}