#pragma once
#include "CoreTypes.h"
#include "Game/Component/Component.h"

struct RigidBodyComponent : public SceneComponent
{
	bool isSimulating = true;
	glm::vec3 velocity = glm::vec3(0.0f);
	glm::vec3 translation = glm::vec3(0.0f);
	glm::vec3 lastTranslation = glm::vec3(0.0f);
};
