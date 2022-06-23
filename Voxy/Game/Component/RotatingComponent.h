#pragma once
#include "CoreTypes.h"
#include "Game/Component/Component.h"

enum class AxisRotation
{
	X, Y, Z
};

struct RotatingComponent : public SceneComponent
{
	void OnTick(f32 deltaTime) override;

	f32 rps = 1.0f;
	AxisRotation axis = AxisRotation::Y;
};