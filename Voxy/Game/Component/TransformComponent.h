#pragma once
#include "CoreTypes.h"
#include "Game/Component/Component.h"

struct TransformComponent : public Component
{
	glm::vec3 position = { 0.0f, 0.0f, 0.0f };
	glm::quat rotation = { 1.0f, 0.0f, 0.0f, 0.0f };
	glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

	glm::mat4 GetMatrix() const;
};
