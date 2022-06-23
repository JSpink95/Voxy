#include "Utility/GameController.h"

constexpr s16 DEAD_ZONE = 8000;

namespace Helper
{
	GameControllerAxis AxisFromSDL(SDL_GameControllerAxis axis)
	{
		switch (axis)
		{
			case SDL_CONTROLLER_AXIS_LEFTX:
				return GameControllerAxis::LEFT_X;
			case SDL_CONTROLLER_AXIS_LEFTY:
				return GameControllerAxis::LEFT_Y;
			case SDL_CONTROLLER_AXIS_RIGHTX:
				return GameControllerAxis::RIGHT_X;
			case SDL_CONTROLLER_AXIS_RIGHTY:
				return GameControllerAxis::RIGHT_Y;
			case SDL_CONTROLLER_AXIS_TRIGGERLEFT:
				return GameControllerAxis::LEFT_TRIGGER;
			case SDL_CONTROLLER_AXIS_TRIGGERRIGHT:
				return GameControllerAxis::RIGHT_TRIGGER;
			default:
				return GameControllerAxis::UNKNOWN;
		}
	}

	GameControllerButton ButtonFromSDL(SDL_GameControllerButton button)
	{
		switch (button)
		{
			case SDL_CONTROLLER_BUTTON_A:
				return GameControllerButton::BUTTON_A;
			case SDL_CONTROLLER_BUTTON_B:
				return GameControllerButton::BUTTON_B;
			case SDL_CONTROLLER_BUTTON_X:
				return GameControllerButton::BUTTON_X;
			case SDL_CONTROLLER_BUTTON_Y:
				return GameControllerButton::BUTTON_Y;
			case SDL_CONTROLLER_BUTTON_DPAD_UP:
				return GameControllerButton::DPAD_UP;
			case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
				return GameControllerButton::DPAD_DOWN;
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
				return GameControllerButton::DPAD_LEFT;
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
				return GameControllerButton::DPAD_RIGHT;
			case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
				return GameControllerButton::LEFT_SHOULDER;
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
				return GameControllerButton::RIGHT_SHOULDER;

				// not handled
			case SDL_CONTROLLER_BUTTON_BACK:
			case SDL_CONTROLLER_BUTTON_GUIDE:
			case SDL_CONTROLLER_BUTTON_START:
			case SDL_CONTROLLER_BUTTON_LEFTSTICK:
			case SDL_CONTROLLER_BUTTON_RIGHTSTICK:
			case SDL_CONTROLLER_BUTTON_MISC1:    /* Xbox Series X share button, PS5 microphone button, Nintendo Switch Pro capture button, Amazon Luna microphone button */
			case SDL_CONTROLLER_BUTTON_PADDLE1:  /* Xbox Elite paddle P1 */
			case SDL_CONTROLLER_BUTTON_PADDLE2:  /* Xbox Elite paddle P3 */
			case SDL_CONTROLLER_BUTTON_PADDLE3:  /* Xbox Elite paddle P2 */
			case SDL_CONTROLLER_BUTTON_PADDLE4:  /* Xbox Elite paddle P4 */
			case SDL_CONTROLLER_BUTTON_TOUCHPAD: /* PS4/PS5 touchpad button */
				return GameControllerButton::UNKNOWN;
		}
	}

	f32 ConvertRawAxisValue(s16 value)
	{
		if (value > -DEAD_ZONE && value < DEAD_ZONE)
			return 0.0f;

		return (f32)value / (f32)std::numeric_limits<s16>::max();
	}
}

GameController::GameController(ControllerID id)
	: id(id)
{
	controller = SDL_GameControllerOpen(id);
	std::fill(axisValues.begin(), axisValues.end(), 0);
	std::fill(buttonStates.begin(), buttonStates.end(), 0);
	previousButtonStates = buttonStates;
}

f32 GameController::GetAxisValue(GameControllerAxis axis)
{
	s16 value = axisValues.at((size_t)axis);
	return Helper::ConvertRawAxisValue(value);
}

s32 GameController::IsButtonDown(GameControllerButton button)
{
	return buttonStates.at(static_cast<size_t>(button));
}

s32 GameController::WasButtonPressed(GameControllerButton button)
{
	return !previousButtonStates.at(static_cast<size_t>(button)) && IsButtonDown(button);
}

void GameController::QueueButtonEvent(ButtonEvent const& event)
{
	queuedButtonEvents.push_back(event);
}

void GameController::Update()
{
	previousButtonStates = buttonStates;

	for (auto& event : queuedButtonEvents)
	{
		if (event.state == 0)
		{
			DispatchEvent_ButtonReleased((GameControllerButton)event.buttonId);
		}

		if (event.state == 1)
		{
			DispatchEvent_ButtonPressed((GameControllerButton)event.buttonId);
		}

		buttonStates.at(event.buttonId) = event.state;
	}

	queuedButtonEvents.clear();
}

GameControllerManager::GameControllerManager()
{
	s32 numJoysticks = SDL_NumJoysticks();
	Console::Log("Found {0} joysticks!", numJoysticks);

	for (s32 joystick = 0; joystick < numJoysticks; ++joystick)
	{
		if (SDL_IsGameController(joystick))
		{
			Console::Log("Found GameController at {0}", joystick);
			controllers.at(joystick) = GameController::Create(joystick);
		}
	}
}

GameControllerManager::~GameControllerManager()
{

}

void GameControllerManager::UpdateControllers()
{
	for (auto controller : controllers)
	{
		if (controller)
		{
			controller->Update();
		}
	}
}

void GameControllerManager::HandleEvent(SDL_Event* event)
{
	switch (event->type)
	{
		case SDL_CONTROLLERDEVICEADDED:
		{
			// TODO(jack): refresh input list
		} break;

		case SDL_CONTROLLERBUTTONDOWN:
		{
			auto controllerId = (size_t)event->cbutton.which;
			auto controller = controllers.at(controllerId);
			auto button = Helper::ButtonFromSDL((SDL_GameControllerButton)event->cbutton.button);

			controller->QueueButtonEvent({ (u64)button, 1 });
			//controller->buttonStates.at((size_t)button) = 1;
		} break;

		case SDL_CONTROLLERBUTTONUP:
		{
			auto controllerId = (size_t)event->cbutton.which;
			auto controller = controllers.at(controllerId);
			auto button = Helper::ButtonFromSDL((SDL_GameControllerButton)event->cbutton.button);

			controller->QueueButtonEvent({ (u64)button, 0 });
			//controller->buttonStates.at((size_t)button) = 0;
		} break;

		case SDL_CONTROLLERAXISMOTION:
		{
			auto controllerId = (size_t)event->caxis.which;
			auto controller = controllers.at(controllerId);
			auto axis = Helper::AxisFromSDL((SDL_GameControllerAxis)event->caxis.axis);

			controller->DispatchEvent_AxisMotion(axis, Helper::ConvertRawAxisValue(event->caxis.value));
			controller->axisValues.at((size_t)axis) = event->caxis.value;
		} break;
	}
}

Reference<GameController> GameControllerManager::GetController(ControllerID id)
{
	if (controllers.size() > id)
		return controllers.at(id);
	return nullptr;
}