#pragma once
#include "CoreTypes.h"
//#include "Game/Components.h"
#include "Utility/GameController.h"
#include "Game/Component/Component.h"

struct Block;

struct PlayerState
{
	bool bAttackPressed : 1;
	bool bJumpPressed : 1;

	PlayerState()
		: bAttackPressed(false)
		, bJumpPressed(false) {}
};

struct PlayerController
	: public EventListener_ButtonPressed
	, public EventListener_ButtonReleased
{
	static auto Create()
	{
		return MakeRef<PlayerController>();
	}

	void SetupController(Reference<GameController> controller);
	void OnButtonPressed(GameControllerButton button) override;
	void OnButtonReleased(GameControllerButton button) override;

	void Update(f32 deltaTime);

	glm::vec3 Eye() const;
	glm::vec3 Forward() const;
	glm::vec3 Right() const;

	bool CanJump() const;

	Reference<Entity> owner = nullptr;
private:
	void HandleInput(f32 deltaTime);

private:
	Reference<GameController> controller = nullptr;
	f32 yaw = 0.0f;
	f32 pitch = 0.0f;
	glm::vec2 input = glm::vec2(0.0f);
	PlayerState state;
};

struct PlayerControllerComponent: public SceneComponent
{
	PlayerControllerComponent();

	void Initialise() override;
	void OnTick(f32 deltaTime) override;

	Reference<PlayerController> controller = nullptr;
};
