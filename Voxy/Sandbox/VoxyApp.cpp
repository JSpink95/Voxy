#include "Sandbox/VoxyApp.h"

#include <iostream>

#include "CoreTypes.h"
#include "AppWindow.h"

#include "Graphics/Shader.h"
#include "Graphics/VertexMesh.h"
#include "Graphics/VertexBatcher.h"
#include "Graphics/Texture.h"
#include "Graphics/StandardMesh.h"
#include "Graphics/Framebuffer.h"

#include "Graphics/Renderer/ChunkRenderer.h"
#include "Graphics/Renderer/ParticleRenderer.h"
#include "Graphics/Renderer/MeshRenderer.h"

#include "Utility/MathUtility.h"
#include "Utility/FastNoiseLite.h"
#include "Utility/GameController.h"
#include "Utility/KeyboardController.h"
#include "Utility/TextureLibrary.h"
#include "Utility/ShaderLibrary.h"

#include "World/Chunk.h"
#include "World/ChunkMap.h"
#include "World/WorldGenerator.h"
#include "World/ChunkManager.h"

#include "Physics/IntersectionTests.h"
#include "Physics/ChunkPhysics.h"

#include "Game/Entity.h"
#include "Game/Components.h"
#include "Game/PlayerController.h"
#include "Game/BlockInteractor.h"
#include "Game/ParticleSystem.h"
#include "Game/GameUI.h"
#include "Game/Item.h"

#include <glm/gtc/random.hpp>

#include <random>

glm::vec3 SunlightPosition = glm::vec3(1.0f, 3.5f, 1.75f) * 1000.0f;
glm::vec3 SunlightDirection = -glm::normalize(SunlightPosition);

Reference<Entity> CreateBlockDestroyEntity(Reference<GameWorld> world, glm::vec3 const& location)
{
	auto particleEmitter = Entity::Create(world);

	particleEmitter->AddComponent<TransformComponent>().position = location;
	auto& particleSystemComponent = particleEmitter->AddComponent<ParticleSystemComponent>();

	particleSystemComponent.system->maxParticles = 500;
	particleSystemComponent.system->emitMode = Particle::EmitMode::Immediate;
	particleSystemComponent.system->looping = false;

	auto emitter = MakeRef<Particle::BoxEmitter>();
	emitter->halfSize = glm::vec3(0.5f);
	emitter->strength = { 0.75f, 1.25f };
	emitter->lifetime = { 1.5f, 2.0f };

	particleSystemComponent.system->emitter = emitter;

	auto updatePosVel = MakeRef<Particle::UpdatePositionVelocity>();
	updatePosVel->drag = 1.0f;

	particleSystemComponent.system->updaters.push_back(MakeRef<Particle::UpdateLifetime>());
	particleSystemComponent.system->updaters.push_back(updatePosVel);

	return particleEmitter;
}

Reference<Entity> SpawnItemPickupEntity(Reference<AppWindow> window, Reference<GameWorld> world, u64 id, glm::vec3 const& location)
{
	constexpr glm::vec3 blockSize = glm::vec3(0.2f);

	auto block = Entity::Create(world);

	auto& transform = block->AddComponent<TransformComponent>();
	transform.position = location + glm::vec3(glm::linearRand(-0.1f, 0.1f), glm::linearRand(-0.1f, 0.1f), glm::linearRand(-0.1f, 0.1f));
	transform.scale = blockSize;

	auto& rigidbody = block->AddComponent<RigidBodyComponent>();
	rigidbody.translation = transform.position;
	rigidbody.velocity.y = 2.0f;

	block->AddComponent<BoxCollider>().halfSize = blockSize;
	block->AddComponent<RotatingComponent>().rps = 45.0f;

	ItemConfig itemConfig = ItemID::GetConfigForID(id);

	switch (itemConfig.type)
	{
		case ItemType::BLOCK:
		{
			List<Reference<Texture>> textures = {};// 
			auto textureList = GBlockConfig::GetBlockConfig(id)->textures;

			std::transform(textureList.begin(), textureList.end(), std::back_inserter(textures), [&window](std::string const& path) -> auto
			{
				return window->textureLibrary->FindTexture(path);
			});

			StaticMeshConfig config =
			{
				.transparent = false,
				.topology = GL_TRIANGLES,
				.textures = textures
			};

			auto& staticMesh = block->AddComponent<StaticMeshComponent>(config);
			staticMesh.mesh->shader = window->shaderLibrary->FindShader("shaders\\block");
			staticMesh.mesh->source = Graphics::CreateBlockMesh();

		} break;

		case ItemType::TOOL:
		case ItemType::CONSUMABLE:
		{

		} break;
	}

	return block;
}

struct BlockDestroyed
	: public AutoEventListener_BlockDestroyed
{
	BlockDestroyed(Reference<AppWindow> window, Reference<GameWorld> world)
		: AutoEventListener_BlockDestroyed(world->chunkManager->chunks)
		, window(window)
		, world(world)
	{

	}

	void OnBlockDestroyed(BlockDestroyedArgs const& args)
	{
		// spawn an emitter at the destruction location
		if (Block* block = args.block)
		{
			CreateBlockDestroyEntity(world, block->location);
			SpawnItemPickupEntity(window, world, block->type, block->location);
		}

		// mark the chunk to reload
		world->chunkManager->Thread_ReloadChunk(args.chunk);
	}

	Reference<AppWindow> window = nullptr;
	Reference<GameWorld> world = nullptr;
};

struct SSAO
{
	static auto Create(Reference<AppWindow> window)
	{
		return MakeRef<SSAO>(window);
	}

	SSAO(Reference<AppWindow> window);

	u32 GetOutput() const;

	void Draw(Reference<Entity> camera, Reference<Framebuffer> gbuffer);

	Reference<AppWindow> window = nullptr;
	u32 noiseTexture = 0;
	Array<glm::vec3, 64> kernel = {};
	Reference<Framebuffer> framebuffer = nullptr;
	Reference<Framebuffer> blurFramebuffer = nullptr;
	Reference<VertexMesh> mesh = nullptr;
};

SSAO::SSAO(Reference<AppWindow> window)
	: window(window)
{
	std::default_random_engine rengine;

	std::uniform_real_distribution<f32> kernelDistribution(0.0f, 1.0f);

	for (u32 i = 0; i < kernel.size(); ++i)
	{
		kernel.at(i) =
		{
			/*.x = */kernelDistribution(rengine) * 2.0f - 1.0f,
			/*.y = */kernelDistribution(rengine) * 2.0f - 1.0f,
			/*.z = */kernelDistribution(rengine)
		};
		kernel.at(i) = glm::normalize(kernel.at(i));

		f32 scale = (f32)i / (f32)kernel.size();
		scale = glm::mix(0.1f, 1.0f, scale * scale);
		kernel.at(i) *= kernelDistribution(rengine) * scale;
	}

	std::uniform_real_distribution<f32> noiseDistribution(-1.0f, 1.0f);

	List<glm::vec3> noise = {};
	for (u32 i = 0; i < 16; ++i)
	{
		noise.push_back({ noiseDistribution(rengine), noiseDistribution(rengine), 0.0f });
	}

	glGenTextures(1, &noiseTexture);
	glBindTexture(GL_TEXTURE_2D, noiseTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 4, 4, 0, GL_RGB, GL_FLOAT, noise.data());
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	FramebufferSpecification spec =
	{
		.dimensions = window->GetDimensions(),
		.colorAttachments = { GL_RGBA32F }
	};

	framebuffer = Framebuffer::Create(spec);
	blurFramebuffer = Framebuffer::Create(spec);
	mesh = Graphics::CreateScreenQuad();
}

u32 SSAO::GetOutput() const
{
	return blurFramebuffer->GetColorAttachment(0);
}

void SSAO::Draw(Reference<Entity> camera, Reference<Framebuffer> gbuffer)
{
	framebuffer->Bind();
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = camera->GetComponent<CameraComponent>().GetProjection();
		glm::mat4 view = camera->GetComponent<CameraComponent>().GetView();

		auto shader = window->shaderLibrary->FindShader("shaders\\postprocess\\ssao");
		shader->Enable();
		shader->SetMat4("_Projection", projection);
		shader->SetMat4("_View", view);

		shader->SetTexture("_Position", gbuffer->GetColorAttachment(1), 0);
		shader->SetTexture("_Normal", gbuffer->GetColorAttachment(2), 1);
		shader->SetTexture("_Noise", noiseTexture, 2);

		shader->SetVec3s("_Samples", kernel.data(), kernel.size());

		mesh->Draw(GL_TRIANGLES);
	}
	framebuffer->Unbind();

	blurFramebuffer->Bind();
	{
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto shader = window->shaderLibrary->FindShader("shaders\\postprocess\\blur");
		shader->Enable();
		shader->SetTexture("_Texture", framebuffer->GetColorAttachment(0), 0);

		mesh->Draw(GL_TRIANGLES);
	}
	blurFramebuffer->Unbind();
}

constexpr s32 LightType_DIRECTIONAL = 0;
constexpr s32 LightType_POINT = 1;

__declspec(align(16)) struct Light
{
	glm::vec4 data = glm::vec4(0.0f);
	glm::vec4 color = glm::vec4(1.0f);
	s32 type = LightType_DIRECTIONAL;
};

struct LightBuffer
{
	Light lights[32];
};

void VoxyApp::Run()
{
	AppWindowSpecification spec = {};
	spec.title = "Voxy";
	spec.width = 1920;
	spec.height = 1080;

	auto window = AppWindow::Create(spec);
	KeyEventLambda keyListener(window->keyboard, window->keyboard);

	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glLineWidth(1.5f);

	FramebufferSpecification mainFramebufferSpec =
	{
		.dimensions = window->GetDimensions(),
		.colorAttachments =
		{
			GL_RGBA,		// albedo
			GL_RGB32F,		// position
			GL_RGB32F,		// normal
		},
		.depthAttachment = GL_DEPTH_COMPONENT
	};

	Reference<Framebuffer> mainFramebuffer = Framebuffer::Create(mainFramebufferSpec);

	// only render to the depth buffer for shadows
	FramebufferSpecification shadowFramebufferSpec =
	{
		.dimensions = glm::ivec2(4096),
		.colorAttachments = { },
		.depthAttachment = GL_DEPTH_COMPONENT
	};

	Reference<Framebuffer> shadowFramebuffer = Framebuffer::Create(shadowFramebufferSpec);

	//u64 seed = 1640263278;
	u64 seed = (u64)std::time(0);
	auto gameWorld = GameWorld::Create(seed);

	auto player = Entity::Create(gameWorld);
	{
		player->AddComponent<TransformComponent>().position = glm::vec3(-31.5f, 200.0f, -48.0f);
		player->AddComponent<CameraComponent>(70.0f, window->GetDimensions(), glm::vec2(0.01f, 1000.0f)).transform.position = glm::vec3(0.0f, 0.8f, 0.0f);

		auto& pcc = player->AddComponent<PlayerControllerComponent>();
		pcc.controller->SetupController(window->controllerManager->GetController(0));

		player->AddComponent<BlockInteractorComponent>(window);
		player->AddComponent<RigidBodyComponent>().translation = player->GetComponent<TransformComponent>().position;
		player->AddComponent<BoxCollider>().halfSize = glm::vec3(0.2f, 0.85f, 0.2f);
		player->AddComponent<HealthComponent>().health = 60.0f;
	}

	auto sunlight = Entity::Create(gameWorld);
	{
		sunlight->AddComponent<TransformComponent>();

		OrthoCameraSpecification cameraSpec =
		{
			.size = 15.0f,
			.nearFar = glm::vec2(-10.0f, 20.0f)
		};
		sunlight->AddComponent<CameraComponent>(cameraSpec);
	}

	gameWorld->InitialiseChunkManager(player);
	gameWorld->chunkManager->OnTickGroup(0.0f);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	auto crosshair = Entity::Create(gameWorld);
	{
		auto dimensions = window->GetDimensions();
		UIElementSpecification spec =
		{
			.x = dimensions.x / 2.0f,
			.y = dimensions.y / 2.0f,
			.w = 32.0f,
			.h = 32.0f,
			.mesh = Graphics::CreateUIElementMesh_Centre()
		};

		auto image = ImageUI::Create(spec);
		image->image = window->textureLibrary->FindTexture("textures\\icons\\crosshair.png");

		crosshair->AddComponent<UIComponent>().element = image;
	}

	auto healthbar = Entity::Create(gameWorld);
	{
		UIElementSpecification spec =
		{
			.x = 20.0f,
			.y = 20.0f,
			.w = 16.0f * 2.0f,
			.h = 16.0f * 2.0f,
			.mesh = Graphics::CreateUIElementMesh_BL()
		};

		auto healthbarUI = HealthBarUI::Create(spec);
		healthbarUI->player = player;
		healthbarUI->fullHeartTexture = window->textureLibrary->FindTexture("textures\\icons\\heart_full.png");
		healthbarUI->emptyHeartTexture = window->textureLibrary->FindTexture("textures\\icons\\heart_empty.png");

		healthbar->AddComponent<UIComponent>().element = healthbarUI;
	}

	BlockDestroyed onBlockDestroyed(window, gameWorld);

	keyListener.pressedLambda = [&](Key key) -> void
	{
		if (key == Key::R)
		{
			window->shaderLibrary->ReloadShaders();
		}

		if (key == Key::P)
		{
			auto& rb = player->GetComponent<RigidBodyComponent>();
			rb.isSimulating = !rb.isSimulating;
		}
	};

	auto crosshairMesh = Graphics::CreateUIElementMesh_Centre();
	auto healthBarMesh = Graphics::CreateUIElementMesh_BL();

	auto chunkRenderer = ChunkRenderer::Create(window, gameWorld);
	auto particleRenderer = ParticleRenderer::Create(window, gameWorld);
	auto staticMeshRenderer = StaticMeshRenderer::Create(window, gameWorld);

	auto tickDispatcher = TickDispatcher::Create();

	// prephysics
	tickDispatcher->SubscribeTickListener(gameWorld->chunkManager.get(), TickGroup::PRE_PHYSICS);

	// post physics
	tickDispatcher->SubscribeTickListener(gameWorld->chunkPhysics.get(), TickGroup::POST_PHYSICS);

	auto ssao = SSAO::Create(window);
	auto screenQuad = Graphics::CreateScreenQuad();
	auto skydome = Graphics::CreateSkyDomeMesh();
	auto image = Graphics::CreateImageMesh();

	BufferSpecification lightBufferSpec =
	{
		.target = GL_UNIFORM_BUFFER,
		.usage = GL_DYNAMIC_DRAW,
		.size = sizeof(LightBuffer),
		.data = nullptr
	};

	auto lightBufferObject = Buffer::Create(lightBufferSpec);

	{
		auto& playerRigidBody = player->GetComponent<RigidBodyComponent>();
		if (auto block = gameWorld->chunkManager->chunks->GetHighestBlock(playerRigidBody.translation.x, playerRigidBody.translation.z))
		{
			playerRigidBody.translation.y = block->location.y + 1.5f;
		}
	}

	constexpr float deltaTime = 1.0f / 60.0f;
	while (!window->bShouldClose)
	{
		{
			auto pos = player->GetComponent<TransformComponent>().position;
			auto [local, chunk] = Utils::WorldCoordsToLocalBlockCoords(pos.x, pos.y, pos.z);
			auto biome = gameWorld->worldGenerator->GetBiome(pos.x, pos.z);

			std::string title = std::format("{6} Biome({0}). Local({1}, {2}, {3}). Chunk({4}, {5})"
				, StringFromBiomeType(biome), local.x, local.y, local.z, chunk.x, chunk.y, seed);
			SDL_SetWindowTitle(window->handle, title.data());
		}

		window->controllerManager->UpdateControllers();

		tickDispatcher->DispatchTicks(deltaTime);

		auto deadEntities = gameWorld->registry.view<KillComponent>();
		for (auto [id, kc] : deadEntities.each())
			gameWorld->registry.destroy(id);

		auto tickableComponents = gameWorld->registry.view<TickHandlerComponent>();
		for (auto [id, thc] : tickableComponents.each())
			thc.OnTick(deltaTime);

		auto scriptedEntities = gameWorld->registry.view<ScriptComponent>();
		for (auto [id, sc] : scriptedEntities.each())
			sc.InvokeUpdate(window, deltaTime);

		mainFramebuffer->Bind();
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 projection = player->GetComponent<CameraComponent>().GetProjection();
			glm::mat4 view = player->GetComponent<CameraComponent>().GetView();

			////glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			//// render skydome
			//{
			//	auto shader = window->shaderLibrary->FindShader("shaders\\skydome");
			//	shader->Enable();
			//	shader->SetMat4("_Projection", projection);
			//	shader->SetMat4("_View", view);
			//
			//	skydome->Draw(GL_TRIANGLES);
			//}
			////glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			//
			//glClear(GL_DEPTH_BUFFER_BIT);

			chunkRenderer->backface = false;
			chunkRenderer->bounds.reset();

			chunkRenderer->Render(player);
			particleRenderer->Render(player);
			staticMeshRenderer->Render(player);
		}
		mainFramebuffer->Unbind();

		// do shadow mapping
		shadowFramebuffer->Bind();
		{
			glClear(GL_DEPTH_BUFFER_BIT);

			auto& transform = sunlight->GetComponent<TransformComponent>();

			glm::vec3 direction = -glm::normalize(SunlightPosition);
			transform.position = player->GetComponent<TransformComponent>().position;
			transform.rotation = glm::inverse(glm::quat_cast(glm::lookAt(transform.position, transform.position + direction, glm::vec3(0.0f, 1.0f, 0.0f))));

			chunkRenderer->backface = true;
			chunkRenderer->bounds = CalculateViewBounds(transform.position, 2);// glm::ivec4(transform.position.x, transform.position.y;

			chunkRenderer->Render(sunlight);
		}
		shadowFramebuffer->Unbind();

		// draw ssao
		ssao->Draw(player, mainFramebuffer);

		// draw framebuffer
		{
			glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			auto shader = window->shaderLibrary->FindShader("shaders\\postprocess\\screen");
			shader->Enable();
			shader->SetTexture("_AlbedoTexture", mainFramebuffer->GetColorAttachment(0), 0);
			shader->SetTexture("_PositionTexture", mainFramebuffer->GetColorAttachment(1), 1);
			shader->SetTexture("_NormalTexture", mainFramebuffer->GetColorAttachment(2), 2);
			shader->SetTexture("_DepthTexture", mainFramebuffer->GetDepthAttachment(), 3);
			shader->SetTexture("_SSAOTexture", ssao->GetOutput(), 4);
			shader->SetTexture("_ShadowTexture", shadowFramebuffer->GetDepthAttachment(), 5);
			shader->SetFloat("_LightCount", 2.0f);
			shader->SetMat4("_ShadowProjectionMatrix", sunlight->GetComponent<CameraComponent>().GetProjection());
			shader->SetMat4("_ShadowViewMatrix", sunlight->GetComponent<CameraComponent>().GetView());

			LightBuffer buffer = {};

			buffer.lights[0].data = glm::vec4(SunlightPosition, 1.0f);
			buffer.lights[0].color = glm::vec4(1.0f, 0.99f, 0.8f, 1.0f);
			buffer.lights[0].type = LightType_DIRECTIONAL;

			buffer.lights[1].data = glm::vec4(player->GetComponent<TransformComponent>().position, 6.0f);
			buffer.lights[1].color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
			buffer.lights[1].type = LightType_POINT;

			lightBufferObject->Set(&buffer, sizeof(LightBuffer));

			screenQuad->Draw(GL_TRIANGLES);
		}

		// render UI
		{
			auto uiShader = window->shaderLibrary->FindShader("shaders\\ui");

			glClear(GL_DEPTH_BUFFER_BIT);

			{
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

				uiShader->Enable();

				auto dimensions = window->GetDimensions();
				uiShader->SetMat4("_Projection", glm::ortho(0.0f, dimensions.x, 0.0f, dimensions.y, -1.0f, 1.0f));

				auto elements = gameWorld->registry.view<UIComponent>();
				for (auto [entity, ui] : elements.each())
				{
					ui.element->Submit(uiShader);
				}

				glDisable(GL_BLEND);
			}
		}

		// debug draw images
		{
			auto shader = window->shaderLibrary->FindShader("shaders\\image");
			shader->Enable();
			shader->SetMat4("_Projection", glm::ortho(0.0f, 1920.0f, 0.0f, 1080.0f, -1.0f, 1.0f));
			shader->SetMat4("_Transform", glm::translate(glm::mat4(1.0f), glm::vec3(16.0f, 540.0f, 0.0f)));
			shader->SetFloat("_Size", 256.0f);

			shader->SetFloat("_Aspect", 1.0f);
			shader->SetTexture("_Texture", shadowFramebuffer->GetDepthAttachment(), 0);

			//shader->SetFloat("_Aspect", 1080.0f / 1920.0f);
			//shader->SetTexture("_Texture", ssao->GetOutput(), 0);
			//shader->SetTexture("_Texture", mainFramebuffer->GetDepthAttachment(), 0);

			image->Draw(GL_POINTS);
		}

		window->PollEvents();
		window->Present();
	}
}