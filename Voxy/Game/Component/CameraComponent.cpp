#include "Game/Component/CameraComponent.h"

#include "Game/Entity.h"

#include "Utility/MathUtility.h"

CameraComponent::CameraComponent(OrthoCameraSpecification const& spec)
{
	projection = glm::ortho(
		-spec.size, spec.size,
		-spec.size, spec.size,
		spec.nearFar.x, spec.nearFar.y);
}

CameraComponent::CameraComponent(float fov, glm::vec2 const& dimensions, glm::vec2 nearFar)
	: projection(MathUtils::CreatePerspectiveProjection(glm::radians(fov), dimensions, nearFar))
{

}

glm::mat4 CameraComponent::GetProjection() const
{
	return projection;
}

glm::mat4 CameraComponent::GetView() const
{
	return glm::inverse(owner->GetComponent<TransformComponent>().GetMatrix() * transform.GetMatrix());
}

glm::vec3 CameraComponent::GetForwardVector() const
{
	return glm::normalize(transform.rotation * glm::vec3(0.0f, 0.0f, -1.0f));
}
