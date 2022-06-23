#pragma once
#include "CoreTypes.h"
#include "Game/Component/Component.h"

struct BoxCollider : public Component
{
	glm::vec3 halfSize = glm::vec3(0.5f);
};