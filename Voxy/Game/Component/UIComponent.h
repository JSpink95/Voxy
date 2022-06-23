#pragma once
#include "CoreTypes.h"
#include "Game/Component/Component.h"

struct UIElement;

struct UIComponent : public Component
{
	Reference<UIElement> element = nullptr;
};
