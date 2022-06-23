#pragma once
#include "CoreTypes.h"

struct AppWindow;
struct GameWorld;
struct Entity;

struct Renderer
{
	Renderer(Reference<AppWindow> window, Reference<GameWorld> world);
	virtual ~Renderer() = default;

	virtual void Render(Reference<Entity> camera) = 0;

	Reference<AppWindow> window = nullptr;
	Reference<GameWorld> world = nullptr;
};