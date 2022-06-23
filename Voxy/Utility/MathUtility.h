#pragma once
#include "CoreTypes.h"

namespace MathUtils
{
	glm::quat CreateRotation(
		glm::vec3 const& euler);

	glm::mat4 CreateTransform(
		  glm::vec3 const& position
		, glm::quat const& rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f)
		, glm::vec3 const& scale = glm::vec3(1.0f));

	glm::mat4 CreatePerspectiveProjection(
		  float fov
		, glm::vec2 const& windowSize
		, glm::vec2 const& nearFarClipRegion);

	glm::mat4 CreateLookAtViewMatrix(
		  glm::vec3 const& position
		, glm::vec3 const& target
		, glm::vec3 const& up = glm::vec3(0.0f, 1.0f, 0.0f));
}