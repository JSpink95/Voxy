#include "Graphics/VertexMesh.h"

#include <numeric>

namespace GLUtility
{
	bool IsIntType(GLenum type)
	{
		switch (type)
		{
			case GL_INT:
			case GL_INT_VEC3:
			case GL_UNSIGNED_INT:
				return true;

			default:
				return false;
		}
	}
}

u64 VertexMeshSpecification::Stride() const
{
	return std::accumulate(attributes.begin(), attributes.end(), 0u, [](u64 value, GLenum attribute) -> u64
		{
			return value + GetAttributeSize(attribute);
		});
}

VertexMesh::VertexMesh(VertexMeshSpecification const& spec)
{
	glCreateVertexArrays(1, &id);
	glBindVertexArray(id);

	indexCount = spec.indexCount;
	vertexCount = spec.vertexCount;

	auto stride = spec.Stride();

	if (indexCount > 0u)
	{
		BufferSpecification indexBufferSpec = {};
		indexBufferSpec.target = GL_ELEMENT_ARRAY_BUFFER;
		indexBufferSpec.data = (void const*)spec.indices;
		indexBufferSpec.size = spec.indexCount * sizeof(u32);

		indexBuffer = Buffer::Create(indexBufferSpec);
	}

	BufferSpecification vertexBufferSpec = {};
	vertexBufferSpec.target = GL_ARRAY_BUFFER;
	vertexBufferSpec.size = spec.vertexCount * stride;
	vertexBufferSpec.data = spec.vertices;

	vertexBuffer = Buffer::Create(vertexBufferSpec);
	vertexBuffer->Bind();

	u32 attributeOffset = 0u;
	for (u64 attributeIndex = 0u; attributeIndex < spec.attributes.size(); ++attributeIndex)
	{
		glEnableVertexAttribArray(attributeIndex);

		if (GLUtility::IsIntType(spec.attributes.at(attributeIndex)))
		{
			glVertexAttribIFormat(
				  attributeIndex
				, GetElementCount(spec.attributes.at(attributeIndex))
				, GL_INT
				, attributeOffset);
		}
		else
		{
			glVertexAttribFormat(
				  attributeIndex
				, GetElementCount(spec.attributes.at(attributeIndex))
				, GL_FLOAT
				, GL_FALSE
				, attributeOffset);
		}


		glVertexAttribBinding(attributeIndex, 0);
		glBindVertexBuffer(attributeIndex, vertexBuffer->id, 0u, (GLsizei)stride);

		attributeOffset += GetAttributeSize(spec.attributes.at(attributeIndex));
	}

	glBindVertexArray(0);
}

VertexMesh::~VertexMesh()
{
	glDeleteVertexArrays(1, &id);
}

void VertexMesh::Draw(GLenum mode)
{
	if (id == 0 || vertexCount == 0u)
		return;

	glBindVertexArray(id);
	vertexBuffer->Bind();

	if (indexBuffer != nullptr)
	{
		indexBuffer->Bind();
		glDrawElements(mode, indexCount, GL_UNSIGNED_INT, nullptr);
	}
	else
	{
		glDrawArrays(mode, 0, vertexCount);
	}

}
