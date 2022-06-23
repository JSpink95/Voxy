#include "Graphics/UI/UIElement.h"

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexMesh.h"

Reference<UIElement> UIElement::Create(UIElementSpecification const& spec)
{
	return MakeRef<UIElement>(spec);
}

UIElement::UIElement(UIElementSpecification const& spec)
	: x(spec.x)
	, y(spec.y)
	, w(spec.w)
	, h(spec.h)
	, mesh(spec.mesh)
{

}

glm::vec4 UIElement::GetBounds() const
{
	return glm::vec4(x, y, x + w, y + h);
}

void UIElement::Submit(Reference<Shader> shader)
{
	if (mesh == nullptr)
	{
		Console::LogError("Cannot render an object without a mesh!");
		return;
	}

	glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
	transform = glm::scale(transform, glm::vec3(w, h, 1.0f));
	shader->SetMat4("_Transform", transform);

	mesh->Draw(GL_TRIANGLES);
}
