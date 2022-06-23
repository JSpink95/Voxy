#pragma once
#include "Game/Component/Component.h"

struct SphereOverlapComponent : public Component
{
	f32 radius = 1.0f;
};

struct BoxOverlapComponent : public Component
{
	glm::vec3 halfSize = glm::vec3(0.5f);
};