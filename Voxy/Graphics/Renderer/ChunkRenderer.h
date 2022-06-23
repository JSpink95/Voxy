#pragma once
#include "CoreTypes.h"
#include "Graphics/Renderer/Renderer.h"

#include "World/Block.h"

#include <optional>

struct Texture;

struct ChunkRenderer : Renderer
{
	static auto Create(Reference<AppWindow> window, Reference<GameWorld> world)
	{
		return MakeRef<ChunkRenderer>(window, world);
	}

	ChunkRenderer(Reference<AppWindow> window, Reference<GameWorld> world);
	void Render(Reference<Entity> camera) override;

	bool backface = false;
	std::optional<glm::ivec4> bounds;
	Array<Reference<Texture>, BlockUtility::TextureCount> blockTextures = {};
};
