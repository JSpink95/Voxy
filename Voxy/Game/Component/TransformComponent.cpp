#include "Game/Component/TransformComponent.h"

glm::mat4 TransformComponent::GetMatrix() const
{
	glm::mat4 matrix(1.0f);
	matrix = glm::translate(matrix, position);
	matrix *= glm::mat4_cast(rotation);
	matrix = glm::scale(matrix, scale);

	return matrix;
}
