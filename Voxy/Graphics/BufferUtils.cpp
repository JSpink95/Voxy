#include "Graphics/BufferUtils.h"

Buffer::Buffer(BufferSpecification const& spec)
	: target(spec.target)
	, usage(spec.usage)
{
	glGenBuffers(1, &id);
	Set(spec.data, spec.size);

	if (target == GL_UNIFORM_BUFFER)
	{
		glBindBufferBase(target, 5, id);
		//glBindBufferRange(target, 5, id, 0, spec.size);
	}
}

Buffer::~Buffer()
{
	glDeleteBuffers(1, &id);
}

void Buffer::Set(void const* blob, u64 blobSize)
{
	glBindBuffer(target, id);
	//if (target == GL_UNIFORM_BUFFER)
	//{
	//	glBufferSubData(target, 0, blobSize, blob);
	//}
	//else
	{
		glBufferData(target, blobSize, blob, usage);
	}
	glBindBuffer(target, 0);
}

void Buffer::Bind()
{
	glBindBuffer(target, id);
}