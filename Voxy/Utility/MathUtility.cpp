#include "Utility/MathUtility.h"

namespace MathUtils
{
	glm::quat CreateRotation(
		glm::vec3 const& euler)
	{
		return glm::quat(glm::radians(euler));
	}

	glm::mat4 CreateTransform(
		  glm::vec3 const& position
		, glm::quat const& rotation/* = glm::quat(1.0f, 0.0f, 0.0f, 0.0f)*/
		, glm::vec3 const& scale/* = glm::vec3(1.0f)*/)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position);
		//transform *= rotation;
		transform = glm::scale(transform, scale);

		return transform;
	}

	glm::mat4 CreatePerspectiveProjection(
		  float fov
		, glm::vec2 const& windowSize
		, glm::vec2 const& nearFarClipRegion)
	{
		return glm::perspectiveFov(fov, windowSize.x, windowSize.y, nearFarClipRegion.x, nearFarClipRegion.y);
	}

	glm::mat4 CreateLookAtViewMatrix(
		  glm::vec3 const& position
		, glm::vec3 const& target
		, glm::vec3 const& up/* = glm::vec3(0.0f, 1.0f, 0.0f)*/)
	{
		return glm::lookAt(position, target, up);
	}
}