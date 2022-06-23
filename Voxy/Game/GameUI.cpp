#include "Game/GameUI.h"

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexMesh.h"

#include "Game/Entity.h"
#include "Game/Components.h"

Reference<ImageUI> ImageUI::Create(UIElementSpecification const& spec)
{
	return MakeRef<ImageUI>(spec);
}

ImageUI::ImageUI(UIElementSpecification const& spec)
	: UIElement(spec)
{

}

void ImageUI::Submit(Reference<Shader> shader)
{
	if (image == nullptr)
	{
		Console::LogError("Cannot render an ImageUI without a texture!");
		return;
	}

	shader->SetTexture("_Texture", image->id, 0u);
	UIElement::Submit(shader);
}

Reference<HealthBarUI> HealthBarUI::Create(UIElementSpecification const& spec)
{
	return MakeRef<HealthBarUI>(spec);
}

HealthBarUI::HealthBarUI(UIElementSpecification const& spec)
	: UIElement(spec)
{

}

glm::vec4 HealthBarUI::GetBounds() const
{
	return glm::vec4(x, y, x + (f32)MaxHealth * w, y + h);
}

void HealthBarUI::Submit(Reference<Shader> shader)
{
	if (mesh == nullptr)
	{
		Console::LogError("Cannot render an object without a mesh!");
		return;
	}

	if (emptyHeartTexture == nullptr || fullHeartTexture == nullptr)
	{
		Console::LogError("Cannot render a HealthBarUI without a texture!");
		return;
	}

	f32 playerHealth = player->GetComponent<HealthComponent>().Percentage01();

	for (u32 health = 0; health < MaxHealth; ++health)
	{
		f32 percentage = (f32)health / (f32)MaxHealth;
		shader->SetTexture("_Texture", (percentage > playerHealth) ? emptyHeartTexture->id : fullHeartTexture->id, 0u);

		glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(x + w * health, y, 0.0f));
		transform = glm::scale(transform, glm::vec3(w, h, 1.0f));
		shader->SetMat4("_Transform", transform);

		// draw a heart per health
		mesh->Draw(GL_TRIANGLES);
	}
}
