#pragma once
#include "CoreTypes.h"
#include "Game/Component/Component.h"

struct Shader;
struct VertexMesh;
struct Texture;

struct StaticMesh
{
	static auto Create()
	{
		return MakeRef<StaticMesh>();
	}

	inline bool IsValid() const
	{
		return shader != nullptr && source != nullptr;
	}

	Reference<Shader> shader = nullptr;
	Reference<VertexMesh> source = nullptr;
	List<Reference<Texture>> textures = {}; // TODO(jack): need to make a system where we can specify the parameters elsewhere
};

struct StaticMeshConfig
{
	bool transparent = false;
	GLenum topology = GL_TRIANGLES;
	List<Reference<Texture>> textures = {};
};

struct StaticMeshComponent : public Component
{
	StaticMeshComponent(StaticMeshConfig const& config);

	bool visible = true;
	StaticMeshConfig config;

	Reference<StaticMesh> mesh = nullptr;
};