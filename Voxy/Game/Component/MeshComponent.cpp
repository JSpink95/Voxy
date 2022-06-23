#include "Game/Component/MeshComponent.h"

StaticMeshComponent::StaticMeshComponent(StaticMeshConfig const& config)
	: visible(true)
	, config(config)
{
	mesh = StaticMesh::Create();
	mesh->textures = config.textures;
}
