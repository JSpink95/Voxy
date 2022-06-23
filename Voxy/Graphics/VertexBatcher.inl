template<VertexType TVertex>
struct VertexBatcher<TVertex>::InternalData
{
	static constexpr u64 MaxVertices = 10000u;
	static constexpr u64 VertexStride = sizeof(TVertex);

	InternalData();

	TVertex* vertices = nullptr;
	TVertex* vertexIterator = nullptr;
	Reference<VertexMesh> mesh = nullptr;
	u64 vertexStride = 0u;
	u64 vertexCount = 0u;
};
template<VertexType TVertex>
VertexBatcher<TVertex>::InternalData::InternalData()
{
	vertices = new TVertex[MaxVertices];
	vertexIterator = vertices;

	VertexMeshSpecification specification = {};
	specification.attributes = TVertex::Attributes;
	specification.vertexCount = MaxVertices;
	specification.vertices = nullptr;

	mesh = VertexMesh::Create(specification);
	vertexStride = specification.Stride();
}
template<VertexType TVertex>
VertexBatcher<TVertex>::VertexBatcher()
	: internalData(std::make_unique<InternalData>())
{

}
template<VertexType TVertex>
VertexBatcher<TVertex>::~VertexBatcher()
{
}
template<VertexType TVertex>
void VertexBatcher<TVertex>::BeginBatch()
{
	internalData->vertexIterator = internalData->vertices;
}
template<VertexType TVertex>
u64 VertexBatcher<TVertex>::EndBatch()
{
	internalData->vertexCount = std::distance(internalData->vertices, internalData->vertexIterator);

	internalData->mesh->vertexBuffer->Set(internalData->vertices, internalData->vertexCount * internalData->vertexStride);

	return internalData->vertexCount;
}
template<VertexType TVertex>
void VertexBatcher<TVertex>::SubmitVertexStream(TVertex* vertices, u64 count)
{
	for (u64 index = 0u; index < count; ++index)
	{
		*internalData->vertexIterator = vertices[index];
		++internalData->vertexIterator;
	}
}
template<VertexType TVertex>
Reference<VertexMesh> VertexBatcher<TVertex>::GetMesh()
{
	return internalData->mesh;
}
template<VertexType TVertex>
u64 VertexBatcher<TVertex>::GetVertexCount() const
{
	return internalData->vertexCount;
}