#pragma once
#include "CoreTypes.h"
#include "Utility/EventDispatcher.h"

enum class Key: u8
{
	A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	SPACE, LSHIFT, RSHIFT, LALT, RALT, ESCAPE, ENTER,
	UP, DOWN, LEFT, RIGHT,
	N1, N2, N3, N4, N5, N6, N7, N8, N9, N0,
	NUMPAD1, NUMPAD2, NUMPAD3, NUMPAD4, NUMPAD5, NUMPAD6, NUMPAD7, NUMPAD8, NUMPAD9, NUMPAD0,
	Max
};

static constexpr u64 MAX_KEY = static_cast<u64>(Key::Max);

DECLARE_EVENT_SYSTEM_ARG1(KeyPressed, Key);
DECLARE_EVENT_SYSTEM_ARG1(KeyReleased, Key);

struct KeyboardController
	: public EventDispatcher_KeyPressed
	, public EventDispatcher_KeyReleased
{
	static Reference<KeyboardController> Create()
	{
		return MakeRef<KeyboardController>();
	}

	KeyboardController();

	bool IsKeyDown(Key key) const;
	void HandleEvent(SDL_Event* event);
	
	Array<bool, MAX_KEY> states = {};
};

struct KeyEventLambda
	: public AutoEventListener_KeyPressed
	, public AutoEventListener_KeyReleased
{
	KeyEventLambda(Reference<KeyPressedDispatcher> pressed, Reference<KeyReleasedDispatcher> released);

	void OnKeyPressed(Key key) override;
	void OnKeyReleased(Key key) override;

	std::function<void(Key)> pressedLambda;
	std::function<void(Key)> releasedLambda;
};
