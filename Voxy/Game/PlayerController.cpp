#include "Game/PlayerController.h"
#include "Game/BlockInteractor.h"
#include "Game/Entity.h"
#include "Game/Components.h"
#include "Game/GameWorld.h"

#include "AppWindow.h"
#include "Utility/GameController.h"

#include "World/Block.h"
#include "World/ChunkManager.h"
#include "World/ChunkMap.h"

constexpr f32 MaxWalkSpeed = 7.5f;
constexpr f32 MaxSprintSpeed = 10.0f;
constexpr f32 MaxSpeed_NoPhysics = 50.0f;

constexpr f32 Acceleration = 3.0f;
constexpr f32 Deceleration = 1000.0f;
constexpr f32 JumpForce = 5.0f;

constexpr f32 MouseSpeed = 0.05f;
constexpr f32 JoystickAxisSpeed = 2.5f;

constexpr GameControllerButton AttackButton = GameControllerButton::RIGHT_SHOULDER;
constexpr GameControllerButton JumpButton = GameControllerButton::BUTTON_A;

constexpr f32 BlockDestroyTime = 1.0f;

void PlayerController::OnButtonPressed(GameControllerButton button)
{
	switch (button)
	{
		case AttackButton:
		{
			state.bAttackPressed = true;
		} break;

		case JumpButton:
		{
			state.bJumpPressed = true;
		} break;

		default: break;
	}
}

void PlayerController::OnButtonReleased(GameControllerButton button)
{
	switch (button)
	{
		case AttackButton:
		{
			state.bAttackPressed = false;
			owner->GetComponent<BlockInteractorComponent>().interactor->ResetHealth();
		} break;

		case JumpButton:
		{
			state.bJumpPressed = false;
		} break;

		default: break;
	}
}

void PlayerController::SetupController(Reference<GameController> controller)
{
	this->controller = controller;
	controller->SubscribeListener_ButtonPressed(this);
	controller->SubscribeListener_ButtonReleased(this);
}

void PlayerController::Update(f32 deltaTime)
{
	HandleInput(deltaTime);

	auto& transform = owner->GetComponent<TransformComponent>();
	auto& camera = owner->GetComponent<CameraComponent>();
	auto& rb = owner->GetComponent<RigidBodyComponent>();

	glm::vec3 movement(0.0f);
	camera.transform.rotation = glm::quat(glm::radians(glm::vec3(pitch, yaw, 0.0f)));

	movement += Forward() * -input.y;
	movement += Right() * -input.x;

	if (rb.isSimulating)
	{
		movement *= glm::vec3(1.0f, 0.0f, 1.0f);
		f32 moveVelocity = glm::length2(movement);
		if (moveVelocity > 0.0f)
		{
			movement = glm::normalize(movement);
		}

		if (glm::all(glm::equal(movement, glm::vec3(0.0f))))
		{
			rb.velocity.x = 0.0f;
			rb.velocity.z = 0.0f;
		}
		else
		{
			rb.velocity.x = movement.x * MaxWalkSpeed;
			rb.velocity.z = movement.z * MaxWalkSpeed;
		}

		rb.velocity.x = glm::clamp(rb.velocity.x, -MaxWalkSpeed, MaxWalkSpeed);
		rb.velocity.z = glm::clamp(rb.velocity.z, -MaxWalkSpeed, MaxWalkSpeed);
	}
	else
	{
		transform.position += movement * MaxSpeed_NoPhysics * deltaTime;
		rb.translation = transform.position;
		rb.lastTranslation = transform.position;
		rb.velocity = glm::vec3(0.0f);
	}
}

void PlayerController::HandleInput(f32 deltaTime)
{
	if (controller != nullptr)
	{
		input.x = controller->GetAxisValue(GameControllerAxis::LEFT_X);
		input.y = controller->GetAxisValue(GameControllerAxis::LEFT_Y);

		yaw += -controller->GetAxisValue(GameControllerAxis::RIGHT_X) * JoystickAxisSpeed;
		pitch = glm::clamp(pitch + -controller->GetAxisValue(GameControllerAxis::RIGHT_Y) * JoystickAxisSpeed, -89.0f, 89.0f);
	}

	if (CanJump() && state.bJumpPressed)
	{
		owner->GetComponent<RigidBodyComponent>().velocity.y = JumpForce;
	}

	if (state.bAttackPressed)
	{
		if (auto& bic = owner->GetComponent<BlockInteractorComponent>(); bic.interactor->HitBlock(1.0f * deltaTime))
		{
			owner->gameWorld->chunkManager->chunks->DestroyBlock(bic.interactor->selected->location);
		}
	}
}

glm::vec3 PlayerController::Eye() const
{
	auto& transform = owner->GetComponent<TransformComponent>();
	auto& camera = owner->GetComponent<CameraComponent>();

	return transform.position + camera.transform.position;
}

glm::vec3 PlayerController::Forward() const
{
	return owner->GetComponent<CameraComponent>().GetForwardVector();
}

glm::vec3 PlayerController::Right() const
{
	return glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), Forward()));
}

bool PlayerController::CanJump() const
{
	// TODO(jack): add ground check
	auto& rb = owner->GetComponent<RigidBodyComponent>();
	return glm::epsilonEqual<f32>(rb.velocity.y, 0.0f, glm::epsilon<f32>());
}

PlayerControllerComponent::PlayerControllerComponent()
{
	controller = PlayerController::Create();
}

void PlayerControllerComponent::Initialise()
{
	if (controller)
	{
		controller->owner = owner;
	}
}

void PlayerControllerComponent::OnTick(f32 deltaTime)
{
	if (controller)
	{
		controller->Update(deltaTime);
	}
}