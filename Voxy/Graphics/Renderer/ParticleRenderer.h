#pragma once
#include "CoreTypes.h"
#include "Graphics/Renderer/Renderer.h"

struct ParticleRenderer : Renderer
{
	static auto Create(Reference<AppWindow> window, Reference<GameWorld> world)
	{
		return MakeRef<ParticleRenderer>(window, world);
	}

	ParticleRenderer(Reference<AppWindow> window, Reference<GameWorld> world);
	void Render(Reference<Entity> camera) override;
};
