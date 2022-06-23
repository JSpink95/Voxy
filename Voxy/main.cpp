#include "Sandbox/TestApp.h"
#include "Sandbox/VoxyApp.h"

#include "CoreTypes.h"

#include <SDL.h>

void SetupSDL()
{
	//SDL_SetRelativeMouseMode(SDL_TRUE);
	SDL_GameControllerEventState(SDL_ENABLE);
}

int main()
{
	//auto result = SDL_Init(SDL_INIT_EVERYTHING);
	auto result = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
	
	if (result < 0)
	{
		Console::LogError("SDL Error {1} - {0}", SDL_GetError(), result);
		return result;
	}
	
	SetupSDL();
	
	VoxyApp::Run();
	//TestApp::Run();
	
	SDL_Quit();
}