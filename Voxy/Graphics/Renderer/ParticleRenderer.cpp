#include "Graphics/Renderer/ParticleRenderer.h"

#include "AppWindow.h"

#include "Graphics/Shader.h"

#include "Game/Entity.h"
#include "Game/Components.h"
#include "Game/ParticleSystem.h"

#include "Utility/ShaderLibrary.h"

ParticleRenderer::ParticleRenderer(Reference<AppWindow> window, Reference<GameWorld> world)
	: Renderer(window, world)
{

}

void ParticleRenderer::Render(Reference<Entity> camera)
{
	auto particleSystems = world->registry.view<ParticleSystemComponent>();
	auto particleShader = window->shaderLibrary->FindShader("shaders\\particle");

	glm::mat4 projection = camera->GetComponent<CameraComponent>().GetProjection();
	glm::mat4 view = camera->GetComponent<CameraComponent>().GetView();

	particleShader->Enable();
	particleShader->SetMat4("_Projection", projection);
	particleShader->SetMat4("_View", view);

	for (auto [id, psc] : particleSystems.each())
	{
		if (psc.system->ValidMesh())
		{
			auto entity = Entity::Create(world, id);
			particleShader->SetMat4("_Transform", entity->GetComponent<TransformComponent>().GetMatrix());

			psc.system->GetMesh()->Draw(GL_POINTS);
		}
	}
}
