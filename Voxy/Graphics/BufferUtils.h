#pragma once
#include "CoreTypes.h"

static u32 GetAttributeSize(GLenum attribute)
{
	switch (attribute)
	{
		default:
		case GL_FLOAT: return sizeof(f32);
		case GL_FLOAT_VEC2: return sizeof(f32) * 2;
		case GL_FLOAT_VEC3: return sizeof(f32) * 3;
		case GL_FLOAT_VEC4: return sizeof(f32) * 4;
		case GL_INT: return sizeof(s32);
		case GL_INT_VEC3: return sizeof(s32) * 3;
	}
}

static u32 GetElementCount(GLenum type)
{
	switch (type)
	{
		default:
		case GL_FLOAT: return 1;
		case GL_FLOAT_VEC2: return 2;
		case GL_FLOAT_VEC3: return 3;
		case GL_FLOAT_VEC4: return 4;
		case GL_INT: return 1;
		case GL_INT_VEC3: return 3;
	}
}

struct BufferSpecification
{
	GLenum target = GL_ARRAY_BUFFER;
	GLenum usage = GL_STATIC_DRAW;
	u64 size = 0u;
	void const* data = nullptr;
};

struct Buffer
{
	static inline std::shared_ptr<Buffer> Create(BufferSpecification const& spec)
	{
		return std::make_shared<Buffer>(spec);
	}

	Buffer(BufferSpecification const& spec);
	~Buffer();

	void Set(void const* blob, u64 blobSize);

	void Bind();

	u32 id = 0u;
	GLenum target = GL_ARRAY_BUFFER;
	GLenum usage = GL_STATIC_DRAW;
};