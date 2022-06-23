#include "Game/Component/RotatingComponent.h"

#include "Game/Entity.h"
#include "Game/Component/TransformComponent.h"

void RotatingComponent::OnTick(f32 deltaTime)
{
	switch (axis)
	{
		case AxisRotation::X:
		{
			glm::quat rotation = glm::quat(glm::radians(glm::vec3(rps * deltaTime, 0.0f, 0.0f)));
			owner->GetComponent<TransformComponent>().rotation *= rotation;
		} break;

		case AxisRotation::Y:
		{
			glm::quat rotation = glm::quat(glm::radians(glm::vec3(0.0f, rps * deltaTime, 0.0f)));
			owner->GetComponent<TransformComponent>().rotation *= rotation;
		} break;

		case AxisRotation::Z:
		{
			glm::quat rotation = glm::quat(glm::radians(glm::vec3(0.0f, 0.0f, rps * deltaTime)));
			owner->GetComponent<TransformComponent>().rotation *= rotation;
		} break;
	}
}