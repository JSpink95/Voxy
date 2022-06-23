#pragma once
#include "CoreTypes.h"
#include "Graphics/Renderer/Renderer.h"

struct StaticMeshRenderer : public Renderer
{
	static auto Create(Reference<AppWindow> window, Reference<GameWorld> world)
	{
		return MakeRef<StaticMeshRenderer>(window, world);
	}

	StaticMeshRenderer(Reference<AppWindow> window, Reference<GameWorld> world);
	void Render(Reference<Entity> camera) override;
};