#include "AppWindow.h"
#include "Utility/GameController.h"
#include "Utility/KeyboardController.h"
#include "Utility/ShaderLibrary.h"
#include "Utility/TextureLibrary.h"

std::shared_ptr<AppWindow> AppWindow::Create(AppWindowSpecification const& spec)
{
	return std::make_shared<AppWindow>(spec);
}

AppWindow::AppWindow(AppWindowSpecification const& spec)
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	handle = SDL_CreateWindow(spec.title.data(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, spec.width, spec.height, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(handle);
	
	glewExperimental = true;
	if (glewInit() != GLEW_OK)
		printf("Failed to initialise GLEW!\n%s\n", glewGetErrorString(glGetError()));

	printf("OpenGL Version(%s)\n", glGetString(GL_VERSION));
	glViewport(0, 0, spec.width, spec.height);

	keyboard = KeyboardController::Create();
	controllerManager = GameControllerManager::Create();

	shaderLibrary = ShaderLibrary::Create();
	textureLibrary = TextureLibrary::Create();
}

AppWindow::~AppWindow()
{
	shaderLibrary = nullptr;
	textureLibrary = nullptr;

	SDL_GL_DeleteContext(context);
	SDL_DestroyWindow(handle);
}

void AppWindow::PollEvents()
{
	SDL_Event evt;
	while (SDL_PollEvent(&evt))
		HandleEvent(&evt);
}

void AppWindow::HandleEvent(SDL_Event* event)
{
	switch (event->type)
	{
		case SDL_QUIT:
		{
			bShouldClose = true;
		} break;

		case SDL_WINDOWEVENT:
		{
			bShouldClose = (event->window.event == SDL_WINDOWEVENT_CLOSE);
		} break;

		case SDL_MOUSEMOTION:
		{
			if (onMouseMotion)
				onMouseMotion(event->motion.xrel, event->motion.yrel);
		} break;

		case SDL_CONTROLLERDEVICEADDED:
		case SDL_CONTROLLERDEVICEREMOVED:
		case SDL_CONTROLLERBUTTONUP:
		case SDL_CONTROLLERBUTTONDOWN:
		case SDL_CONTROLLERAXISMOTION:
		{
			if (controllerManager)
				controllerManager->HandleEvent(event);
		} break;

		case SDL_KEYUP:
		case SDL_KEYDOWN:
		{
			keyboard->HandleEvent(event);
		} break;
	}
}

void AppWindow::Present()
{
	SDL_UpdateWindowSurface(handle);
	SDL_GL_SwapWindow(handle);
}

glm::vec2 AppWindow::GetDimensions() const
{
	s32 w, h;
	SDL_GetWindowSize(handle, &w, &h);

	return glm::vec2((f32)w, (f32)h);
}