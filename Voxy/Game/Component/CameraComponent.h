#pragma once
#include "CoreTypes.h"
#include "Game/Component/Component.h"
#include "Game/Component/TransformComponent.h"

struct OrthoCameraSpecification
{
	float size = 1.0f;
	glm::vec2 nearFar = glm::vec2(0.0f, 1.0f);
};

struct CameraComponent : public Component
{
	CameraComponent(OrthoCameraSpecification const& spec);
	CameraComponent(float fov, glm::vec2 const& dimensions, glm::vec2 nearFar);

	glm::mat4 GetProjection() const;
	glm::mat4 GetView() const;

	glm::vec3 GetForwardVector() const;

	TransformComponent transform;
	glm::mat4 projection = glm::mat4(1.0f);
};
