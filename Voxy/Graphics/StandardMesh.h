#pragma once
#include "CoreTypes.h"

struct VertexMesh;

namespace Graphics
{
	// Framebuffer
	Reference<VertexMesh> CreateScreenQuad();

	// SkyDome
	Reference<VertexMesh> CreateSkyDomeMesh();

	// Block
	Reference<VertexMesh> CreateBlockMesh();
	Reference<VertexMesh> CreateAnimatedTextureBlockMesh();
	Reference<VertexMesh> CreateBlockMeshOutline(glm::vec4 const& color  = { 1.0f, 0.0f, 1.0f, 1.0f });

	// UI
	Reference<VertexMesh> CreateUIElementMesh_Centre();
	Reference<VertexMesh> CreateUIElementMesh_BL();

	// Image
	Reference<VertexMesh> CreateImageMesh();
}
