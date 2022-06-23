#pragma once
#include "CoreTypes.h"
#include "Graphics\VertexMesh.h"

template<VertexType TVertex>
struct VertexBatcher
{
	static Reference<VertexBatcher> Create()
	{
		return MakeRef<VertexBatcher>();
	}

	VertexBatcher();
	~VertexBatcher();

	void BeginBatch();
	u64 EndBatch();

	void SubmitVertexStream(TVertex* vertices, u64 count);

	u64 GetVertexCount() const;
	Reference<VertexMesh> GetMesh();

	struct InternalData;
	Unique<InternalData> internalData = nullptr;
};
#include "VertexBatcher.inl"