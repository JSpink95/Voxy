#pragma once
#include "CoreTypes.h"
#include "Utility/EventDispatcher.h"

#include <mutex>

using ControllerID = u8;

enum class GameControllerButton: u8
{
	BUTTON_A, BUTTON_B, BUTTON_X, BUTTON_Y,
	DPAD_LEFT, DPAD_RIGHT, DPAD_DOWN, DPAD_UP,
	LEFT_SHOULDER, RIGHT_SHOULDER,
	UNKNOWN,
	MAX
};

enum class GameControllerAxis: u8
{
	LEFT_X, LEFT_Y, RIGHT_X, RIGHT_Y,
	LEFT_TRIGGER, RIGHT_TRIGGER,
	UNKNOWN,
	MAX
};

struct ButtonEvent
{
	u64 buttonId = 0;
	s32 state = 0;
};

DECLARE_EVENT_SYSTEM_ARG2(AxisMotion, GameControllerAxis, f32)
DECLARE_EVENT_SYSTEM_ARG1(ButtonPressed, GameControllerButton)
DECLARE_EVENT_SYSTEM_ARG1(ButtonReleased, GameControllerButton)

struct GameController
	: EventDispatcher_AxisMotion
	, public EventDispatcher_ButtonPressed
	, public EventDispatcher_ButtonReleased
{
	static inline Reference<GameController> Create(ControllerID id)
	{
		return MakeRef<GameController>(id);
	}

	GameController(ControllerID id);

	f32 GetAxisValue(GameControllerAxis axis);
	s32 IsButtonDown(GameControllerButton button);
	s32 WasButtonPressed(GameControllerButton button);
	void QueueButtonEvent(ButtonEvent const& event);

	void Update();

	ControllerID id = 0u;
	SDL_GameController* controller = nullptr;
	Array<s16, static_cast<size_t>(GameControllerAxis::MAX)> axisValues = {};
	Array<s32, static_cast<size_t>(GameControllerButton::MAX)> buttonStates = {};
	Array<s32, static_cast<size_t>(GameControllerButton::MAX)> previousButtonStates = {};
	List<ButtonEvent> queuedButtonEvents = {};

private:
	std::mutex queuedButtonMutex;
};

struct GameControllerManager
{
	static Reference<GameControllerManager> Create()
	{
		return MakeRef<GameControllerManager>();
	}

	GameControllerManager();
	~GameControllerManager();

	void UpdateControllers();
	void HandleEvent(SDL_Event* event);

	Reference<GameController> GetController(ControllerID id);

	Array<Reference<GameController>, 8u> controllers = {};
};