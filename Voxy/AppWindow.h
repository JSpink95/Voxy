#pragma once
#include "CoreTypes.h"
#include <functional>

struct KeyboardController;
struct GameControllerManager;

struct ShaderLibrary;
struct TextureLibrary;

struct AppWindowSpecification
{
	std::string_view title = "";
	int width = 0;
	int height = 0;
};

using OnMouseMotion = std::function<void(s32, s32)>;

struct AppWindow
{
	static std::shared_ptr<AppWindow> Create(AppWindowSpecification const& spec);

	AppWindow(AppWindowSpecification const& spec);
	~AppWindow();

	void PollEvents();
	void HandleEvent(SDL_Event* event);
	void Present();

	glm::vec2 GetDimensions() const;

	SDL_Window* handle = nullptr;
	SDL_GLContext context = nullptr;

	Reference<KeyboardController> keyboard = nullptr;
	Reference<GameControllerManager> controllerManager = nullptr;

	Reference<ShaderLibrary> shaderLibrary = nullptr;
	Reference<TextureLibrary> textureLibrary = nullptr;

	OnMouseMotion onMouseMotion = nullptr;
	bool bShouldClose = false;
};