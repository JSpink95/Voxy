#include "Utility/KeyboardController.h"

namespace Helper
{
	Key KeyFromSDL(SDL_Keycode keycode)
	{
		if (keycode >= SDLK_a && keycode <= SDLK_z)
			return static_cast<Key>(keycode - SDLK_a);

		if (keycode >= SDLK_0 && keycode <= SDLK_9)
			return static_cast<Key>(keycode - SDLK_0);

		if (keycode >= SDLK_KP_1 && keycode <= SDLK_KP_0)
			return static_cast<Key>(keycode - SDLK_KP_1);

		switch (keycode)
		{
			case SDLK_SPACE: return Key::SPACE;
			case SDLK_LSHIFT: return Key::LSHIFT;
			case SDLK_RSHIFT: return Key::RSHIFT;
			case SDLK_LALT: return Key::LALT;
			case SDLK_RALT: return Key::RALT;
			case SDLK_ESCAPE: return Key::ESCAPE;
			case SDLK_RETURN: return Key::ENTER;
			case SDLK_UP: return Key::UP;
			case SDLK_DOWN: return Key::DOWN;
			case SDLK_LEFT: return Key::LEFT;
			case SDLK_RIGHT: return Key::RIGHT;
			default: return Key::Max;
		}
	}
}

KeyboardController::KeyboardController()
{
	std::fill(states.begin(), states.end(), false);
}

bool KeyboardController::IsKeyDown(Key key) const
{
	return states.at(static_cast<u64>(key));
}

void KeyboardController::HandleEvent(SDL_Event* event)
{
	switch (event->type)
	{
		case SDL_KEYDOWN:
		{
			auto key = Helper::KeyFromSDL(event->key.keysym.sym);
			if (key != Key::Max)
			{
				states.at(static_cast<u64>(key)) = true;
				DispatchEvent_KeyPressed(key);
			}
		} break;

		case SDL_KEYUP:
		{
			auto key = Helper::KeyFromSDL(event->key.keysym.sym);
			if (key != Key::Max)
			{
				states.at(static_cast<u64>(key)) = false;
				DispatchEvent_KeyReleased(key);
			}
		} break;
	}
}

KeyEventLambda::KeyEventLambda(Reference<KeyPressedDispatcher> pressed, Reference<KeyReleasedDispatcher> released)
	: AutoEventListener_KeyPressed(pressed)
	, AutoEventListener_KeyReleased(released)
{

}

void KeyEventLambda::OnKeyPressed(Key key)
{
	if (pressedLambda)
		pressedLambda(key);
}

void KeyEventLambda::OnKeyReleased(Key key)
{
	if (releasedLambda)
		releasedLambda(key);
}