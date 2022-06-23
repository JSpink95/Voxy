#pragma once
#include "CoreTypes.h"
#include "Graphics/BufferUtils.h"

struct VertexMeshSpecification
{
	std::vector<GLenum> attributes = {};

	// index data
	u64 indexCount = 0u;
	u32 const* indices = nullptr;

	// vertex data
	u64 vertexCount = 0u;
	void const* vertices = nullptr;

	u64 Stride() const;
};

struct VertexMesh
{
	static inline std::shared_ptr<VertexMesh> Create(VertexMeshSpecification const& spec)
	{
		return std::make_shared<VertexMesh>(spec);
	}

	VertexMesh(VertexMeshSpecification const& spec);
	~VertexMesh();

	void Draw(GLenum mode);

	u32 id = 0u;
	
	u64 indexCount = 0u;
	u64 vertexCount = 0u;

	std::shared_ptr<Buffer> indexBuffer = nullptr;
	std::shared_ptr<Buffer> vertexBuffer = nullptr;
};