#pragma once
#include "CoreTypes.h"
#include "Graphics/UI/UIElement.h"

struct Entity;

struct ImageUI : UIElement
{
	static Reference<ImageUI> Create(UIElementSpecification const& spec);

	ImageUI(UIElementSpecification const& spec);
	void Submit(Reference<Shader> shader) override;

	Reference<Texture> image = nullptr;
};

struct HealthBarUI : UIElement
{
	static constexpr u32 MaxHealth = 10u;
	static Reference<HealthBarUI> Create(UIElementSpecification const& spec);

	HealthBarUI(UIElementSpecification const& spec);
	glm::vec4 GetBounds() const override;
	void Submit(Reference<Shader> shader) override;

	Reference<Texture> fullHeartTexture = nullptr;
	Reference<Texture> emptyHeartTexture = nullptr;
	Reference<Entity> player = nullptr;
};
