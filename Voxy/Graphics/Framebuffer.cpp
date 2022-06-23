#include "Graphics/Framebuffer.h"

#include <numeric>

constexpr u32 NullBufferId = 0;

namespace Helper
{
	GLenum GetFormat(GLenum internalFormat)
	{
		switch (internalFormat)
		{
			case GL_RGBA:
			case GL_RGBA32F:
				return GL_RGBA;

			case GL_RGB:
			case GL_RGB32F:
				return GL_RGB;
		}

		return GL_NONE;
	}

	GLenum GetTypeFromFormat(GLenum format)
	{
		switch (format)
		{
			case GL_RGBA:
			case GL_RGB:
				return GL_UNSIGNED_BYTE;

			case GL_RGBA32F:
			case GL_RGB32F:
				return GL_FLOAT;
		}

		return GL_UNSIGNED_BYTE;
	}

	u32 CreateColorAttachment(glm::uvec2 const& size, GLenum format)
	{
		u32 id = 0;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexImage2D(GL_TEXTURE_2D, 0, format, size.x, size.y, 0, GetFormat(format), GetTypeFromFormat(format), nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		return id;
	}

	u32 CreateDepthAttachment(glm::uvec2 const& size)
	{
		u32 id = 0;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, size.x, size.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		return id;
	}
}

Framebuffer::Framebuffer(FramebufferSpecification const& specification)
	: specification(specification)
{
	Invalidate();
}

void Framebuffer::Invalidate()
{
	if (id != NullBufferId)
	{
		glDeleteFramebuffers(1, &id);
	}

	glCreateFramebuffers(1, &id);

	glBindFramebuffer(GL_FRAMEBUFFER, id);

	for (u64 index = GL_COLOR_ATTACHMENT0; GLenum attachment : specification.colorAttachments)
	{
		u32 textureId = colorAttachments.emplace_back(Helper::CreateColorAttachment(specification.dimensions, attachment));
		glFramebufferTexture2D(GL_FRAMEBUFFER, index, GL_TEXTURE_2D, textureId, 0);
		++index;
	}

	if (specification.depthAttachment != GL_NONE)
	{
		depthAttachment = Helper::CreateDepthAttachment(specification.dimensions);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthAttachment, 0);
	}

	if (!colorAttachments.empty())
	{
		List<GLenum> buffers(colorAttachments.size());
		std::iota(buffers.begin(), buffers.end(), GL_COLOR_ATTACHMENT0);
		glDrawBuffers(buffers.size(), buffers.data());
	}
	else
	{
		glDrawBuffer(GL_NONE);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Framebuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glViewport(0, 0, specification.dimensions.x, specification.dimensions.y);
}

void Framebuffer::Unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0, 0, 1920, 1080);
}

u32 Framebuffer::GetColorAttachment(u32 index) const
{
	return colorAttachments.at(index);
}

u32 Framebuffer::GetDepthAttachment() const
{
	return depthAttachment;
}