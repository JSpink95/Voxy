#pragma once
#include "CoreTypes.h"

struct Texture;
struct VertexMesh;
struct Shader;

struct UIElementSpecification
{
	f32 x = 0.0f, y = 0.0f;
	f32 w = 1.0f, h = 1.0f;
	Reference<VertexMesh> mesh = nullptr;
};

struct UIElement
{
	static Reference<UIElement> Create(UIElementSpecification const& spec);

	UIElement(UIElementSpecification const& spec);
	virtual ~UIElement() = default;

	virtual glm::vec4 GetBounds() const;
	virtual void Submit(Reference<Shader> shader);

	f32 x = 0.0f, y = 0.0f;
	f32 w = 1.0f, h = 1.0f;
	Reference<VertexMesh> mesh = nullptr;
};
