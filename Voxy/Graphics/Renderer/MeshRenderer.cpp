#include "Graphics/Renderer/MeshRenderer.h"
#include "Game/GameWorld.h"

#include "Game/Entity.h"
#include "Game/Components.h"

#include "Graphics/Shader.h"
#include "Graphics/VertexMesh.h"
#include "Graphics/Texture.h"

StaticMeshRenderer::StaticMeshRenderer(Reference<AppWindow> window, Reference<GameWorld> world)
	: Renderer(window, world)
{

}

void StaticMeshRenderer::Render(Reference<Entity> camera)
{
	auto meshes = world->registry.view<TransformComponent, StaticMeshComponent>();

	glm::mat4 projection = camera->GetComponent<CameraComponent>().GetProjection();
	glm::mat4 view = camera->GetComponent<CameraComponent>().GetView();

	for (auto [id, tc, mc] : meshes.each())
	{
		if (!mc.visible || mc.mesh == nullptr || !mc.mesh->IsValid())
		{
			continue;
		}

		if (mc.config.transparent)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // standard blending
		}

		mc.mesh->shader->Enable();
		mc.mesh->shader->SetMat4("_Projection", projection);
		mc.mesh->shader->SetMat4("_View", view);
		mc.mesh->shader->SetMat4("_Transform", tc.GetMatrix());
		
		if (mc.mesh->textures.size() > 0)
		{
			// if we only have one texture assume it is not an array
			if (mc.mesh->textures.size() == 1)
			{
				mc.mesh->shader->SetTexture("_Texture", mc.mesh->textures.at(0)->id, 0);
			}
			else
			{
				for (u32 textureIndex = 0u; textureIndex < mc.mesh->textures.size(); ++textureIndex)
				{
					std::string uniform = std::format("_Textures[{0}]", textureIndex);
					mc.mesh->shader->SetTexture(uniform, mc.mesh->textures.at(textureIndex)->id, textureIndex);
				}
			}
		}

		mc.mesh->source->Draw(mc.config.topology);

		if (mc.config.transparent)
		{
			glDisable(GL_BLEND);
		}
	}
}
