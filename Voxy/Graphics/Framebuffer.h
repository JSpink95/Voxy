#pragma once
#include "CoreTypes.h"

struct FramebufferSpecification
{
	glm::uvec2 dimensions = glm::uvec2(1);

	List<GLenum> colorAttachments = {};
	GLenum depthAttachment = GL_NONE;
};

struct Framebuffer
{
	static auto Create(FramebufferSpecification const& specification)
	{
		return MakeRef<Framebuffer>(specification);
	}

	Framebuffer(FramebufferSpecification const& specification);

	void Invalidate();

	void Bind();
	void Unbind();

	u32 GetColorAttachment(u32 index) const;
	u32 GetDepthAttachment() const;

private:
	FramebufferSpecification specification;
	u32 id = 0;
	List<u32> colorAttachments = {};
	u32 depthAttachment = 0;
};