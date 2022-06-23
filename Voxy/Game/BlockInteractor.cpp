#include "Game/BlockInteractor.h"

#include "AppWindow.h"

#include "Game/Entity.h"
#include "Game/Components.h"
#include "Game/PlayerController.h"

#include "Graphics/StandardMesh.h"
#include "Graphics/Shader.h"

#include "Utility/ShaderLibrary.h"
#include "Utility/TextureLibrary.h"

#include "Physics/ChunkPhysics.h"

#include "World/Chunk.h"

constexpr f32 BaseDamage = 100.0f;
constexpr f32 BaseBlockHealth = 100.0f;

void BlockInteractor::Initialise(Reference<AppWindow> window, Reference<Entity> owner)
{
	this->owner = owner;
	
	blockHighlightProxy = Entity::Create(owner->gameWorld);
	{
		auto& transform = blockHighlightProxy->AddComponent<TransformComponent>();
		transform.scale = glm::vec3(1.0f);

		StaticMeshConfig config = { false, GL_LINES };
		auto& staticMeshComponent = blockHighlightProxy->AddComponent<StaticMeshComponent>(config);

		staticMeshComponent.mesh->shader = window->shaderLibrary->FindShader("shaders\\line");
		staticMeshComponent.mesh->source = Graphics::CreateBlockMeshOutline(glm::vec4(0.1f));
	}

	blockDestroyProxy = Entity::Create(owner->gameWorld);
	{
		auto& transform = blockDestroyProxy->AddComponent<TransformComponent>();
		transform.scale = glm::vec3(1.015f); // make slightly bigger to hide block highlight when breaking shit

		StaticMeshConfig config = { true, GL_TRIANGLES };
		auto& staticMeshComponent = blockDestroyProxy->AddComponent<StaticMeshComponent>(config);

		staticMeshComponent.mesh->shader = window->shaderLibrary->FindShader("shaders\\animated_texture");
		staticMeshComponent.mesh->textures = { window->textureLibrary->FindTexture("textures\\block_break.png") };
		staticMeshComponent.mesh->source = Graphics::CreateAnimatedTextureBlockMesh();
	}
}

bool BlockInteractor::HitBlock(f32 modifier)
{
	f32 amountToDamage = modifier * BaseDamage;
	blockHealth -= amountToDamage;

	return (blockHealth <= 0.0f);
}

void BlockInteractor::ResetHealth()
{
	if (selected)
	{
		blockHealth = GBlockConfig::GetBlockConfig(selected->type)->hardness;
	}
	else
	{
		blockHealth = BaseBlockHealth;
	}

	maxHealth = blockHealth;
}

void BlockInteractor::Update(f32 deltaTime)
{
	auto physics = owner->gameWorld->chunkPhysics;
	auto& pcc = owner->GetComponent<PlayerControllerComponent>();

	Block* highlighted = physics->RayCast(pcc.controller->Eye(), pcc.controller->Forward(), 5.0f);
	if (highlighted != selected)
	{
		// new block under scope, reset health
		blockHealth = BaseBlockHealth;
		selected = highlighted;
		ResetHealth();
	}

	if (selected)
	{
		blockHighlightProxy->GetComponent<TransformComponent>().position = selected->location;
		blockDestroyProxy->GetComponent<TransformComponent>().position = selected->location;
	}

	UpdateHighlightProxy();
	UpdateDestroyProxy();
}

void BlockInteractor::UpdateHighlightProxy()
{
	auto& smc = blockHighlightProxy->GetComponent<StaticMeshComponent>();
	smc.visible = (selected != nullptr);
}

void BlockInteractor::UpdateDestroyProxy()
{
	f32 blockHealthPercentage = GetBlockHealthAsPercentage();
	auto& smc = blockDestroyProxy->GetComponent<StaticMeshComponent>();
	smc.visible = (selected != nullptr && blockHealthPercentage < 1.0f);

	// update block break texture property
	s32 textureIndex = (s32)glm::floor((1.0f - blockHealthPercentage) * 10.0f);
	smc.mesh->shader->Enable();
	smc.mesh->shader->SetFloat("_SubTextureIndex", (f32)textureIndex);
	smc.mesh->shader->Disable();
}

f32 BlockInteractor::GetBlockHealthAsPercentage() const
{
	return glm::clamp(blockHealth / maxHealth, 0.0f, 1.0f);
}

BlockInteractorComponent::BlockInteractorComponent(Reference<AppWindow> window)
	: window(window)
{
	interactor = BlockInteractor::Create();
}

void BlockInteractorComponent::Initialise()
{
	if (interactor != nullptr)
	{
		interactor->Initialise(window, owner);
	}
}

void BlockInteractorComponent::OnTick(f32 deltaTime)
{
	if (interactor != nullptr)
	{
		interactor->Update(deltaTime);
	}
}