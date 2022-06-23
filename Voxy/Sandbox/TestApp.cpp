#include "Sandbox/TestApp.h"

#include "CoreTypes.h"
#include "AppWindow.h"

#include "Graphics/BufferUtils.h"
#include "Graphics/VertexMesh.h"
#include "Graphics/Shader.h"
#include "Graphics/Framebuffer.h"
#include "Graphics/Texture.h"
#include "Graphics/StandardMesh.h"

#include "Game/Entity.h"
#include "Game/Components.h"
#include "Game/GameWorld.h"

#include "Utility/TextureLibrary.h"
#include "Utility/ShaderLibrary.h"
#include "Utility/KeyboardController.h"

struct TextureVertex
{
	static inline List<GLenum> Attributes = { GL_FLOAT_VEC3, GL_FLOAT_VEC3, GL_FLOAT_VEC2 };

	glm::vec3 a_Position = glm::vec3(0.0f);
	glm::vec3 a_Normal = glm::vec3(0.0f);
	glm::vec2 a_Texcoord = glm::vec2(0.0f);
};

void InitGL()
{
	glEnable(GL_DEPTH);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
}

Reference<VertexMesh> CreatePlane(f32 size)
{
	f32 halfSize = size * 0.5f;

	List<TextureVertex> vertices =
	{
		TextureVertex{ glm::vec3(-halfSize, 0.0f, -halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
		TextureVertex{ glm::vec3( halfSize, 0.0f, -halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
		TextureVertex{ glm::vec3( halfSize, 0.0f,  halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
		TextureVertex{ glm::vec3(-halfSize, 0.0f,  halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
	};

	List<u32> indices =
	{
		0, 2, 1,
		0, 3, 2,
	};

	VertexMeshSpecification specification = {};
	specification.attributes = TextureVertex::Attributes;
	specification.vertices = vertices.data();
	specification.vertexCount = vertices.size();
	specification.indices = indices.data();
	specification.indexCount = indices.size();

	return VertexMesh::Create(specification);
}

Reference<VertexMesh> CreateBox(f32 size)
{
	f32 halfSize = size * 0.5f;

	List<TextureVertex> vertices =
	{
		// bottom
		TextureVertex{ glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
		TextureVertex{ glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
		TextureVertex{ glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
		TextureVertex{ glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
		TextureVertex{ glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
		TextureVertex{ glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },

		// top
		TextureVertex{ glm::vec3(-halfSize, halfSize, -halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
		TextureVertex{ glm::vec3( halfSize, halfSize,  halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
		TextureVertex{ glm::vec3( halfSize, halfSize, -halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
		TextureVertex{ glm::vec3(-halfSize, halfSize, -halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
		TextureVertex{ glm::vec3(-halfSize, halfSize,  halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
		TextureVertex{ glm::vec3( halfSize, halfSize,  halfSize), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
		
		// left
		TextureVertex{ glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
		TextureVertex{ glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
		TextureVertex{ glm::vec3(-halfSize,  halfSize,  halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
		TextureVertex{ glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
		TextureVertex{ glm::vec3(-halfSize,  halfSize,  halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
		TextureVertex{ glm::vec3(-halfSize,  halfSize, -halfSize), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
		
		// right
		TextureVertex{ glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
		TextureVertex{ glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
		TextureVertex{ glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
		TextureVertex{ glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
		TextureVertex{ glm::vec3( halfSize,  halfSize, -halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f) },
		TextureVertex{ glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) },
		
		// back
		TextureVertex{ glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
		TextureVertex{ glm::vec3( halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },
		TextureVertex{ glm::vec3( halfSize, -halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f) },
		TextureVertex{ glm::vec3(-halfSize, -halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f) },
		TextureVertex{ glm::vec3(-halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f) },
		TextureVertex{ glm::vec3( halfSize,  halfSize, -halfSize), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f) },
		
		// front
		TextureVertex{ glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
		TextureVertex{ glm::vec3( halfSize, -halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 0.0f) },
		TextureVertex{ glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f) },
		TextureVertex{ glm::vec3(-halfSize, -halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) },
		TextureVertex{ glm::vec3( halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(1.0f, 1.0f) },
		TextureVertex{ glm::vec3(-halfSize,  halfSize,  halfSize), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
	};

	VertexMeshSpecification specification = {};
	specification.attributes = TextureVertex::Attributes;
	specification.vertices = vertices.data();
	specification.vertexCount = vertices.size();

	return VertexMesh::Create(specification);
}

struct EventListener_KeyPressedListenerLambda: public EventListener_KeyPressed
{
	void OnKeyPressed(Key key) override
	{
		if (onPressed)
			onPressed(key);
	}

	std::function<void(Key)> onPressed = nullptr;
};

void TestApp::Run()
{
	AppWindowSpecification appWindowSpec =
	{
		.title = "TestApp (Shadows)",
		.width = 1920,
		.height = 1080,
	};

	auto window = AppWindow::Create(appWindowSpec);

	InitGL();

	FramebufferSpecification gbufferSpec =
	{
		.dimensions = glm::uvec2(1920, 1080),
		.colorAttachments = { GL_RGBA, GL_RGB32F, GL_RGB32F, },
		.depthAttachment = GL_DEPTH_COMPONENT,
	};

	auto gbuffer = Framebuffer::Create(gbufferSpec);

	FramebufferSpecification shadowBufferSpec =
	{
		.dimensions = glm::uvec2(1024),
		.colorAttachments = {},
		.depthAttachment = GL_DEPTH_COMPONENT,
	};

	auto shadowBuffer = Framebuffer::Create(shadowBufferSpec);

	auto planeMesh = CreatePlane(5.0f);
	auto boxMesh = CreateBox(1.0f);
	auto screenMesh = Graphics::CreateScreenQuad();

	auto world = GameWorld::Create(0);
	auto floor = Entity::Create(world);
	{
		floor->AddComponent<TransformComponent>().position = glm::vec3(0.0f);

		StaticMeshConfig config =
		{
			.transparent = false,
			.topology = GL_TRIANGLES,
			.textures = { window->textureLibrary->FindTexture("textures\\stone.png") }
		};

		auto& staticMeshComponent = floor->AddComponent<StaticMeshComponent>(config);
		staticMeshComponent.mesh->source = planeMesh;
		staticMeshComponent.mesh->shader = window->shaderLibrary->FindShader("shaders\\testapp\\texture");
	}

	auto box = Entity::Create(world);
	{
		box->AddComponent<TransformComponent>().position = glm::vec3(0.0f, 0.5f, 0.0f);

		StaticMeshConfig config =
		{
			.transparent = false,
			.topology = GL_TRIANGLES,
			.textures = { window->textureLibrary->FindTexture("textures\\dirt.png") }
		};

		auto& staticMeshComponent = box->AddComponent<StaticMeshComponent>(config);
		staticMeshComponent.mesh->source = boxMesh;
		staticMeshComponent.mesh->shader = window->shaderLibrary->FindShader("shaders\\testapp\\texture");
	}

	auto camera = Entity::Create(world);
	{
		auto& transform = camera->AddComponent<TransformComponent>();
		transform.position = glm::vec3(1.0f, 3.0f, 4.0f);
		transform.rotation = glm::inverse(glm::quat_cast(glm::lookAt(transform.position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f))));

		camera->AddComponent<CameraComponent>(80.0f, glm::vec2(1920.0f, 1080.0f), glm::vec2(0.01f, 50.0f));
	}

	auto sunlight = Entity::Create(world);
	{
		auto& transform = sunlight->AddComponent<TransformComponent>();
		transform.position = glm::vec3(-2.0f, 4.0f, -1.0f);
		transform.rotation = glm::inverse(glm::quat_cast(glm::lookAt(transform.position, glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f))));

		OrthoCameraSpecification cameraSpec =
		{
			.size = 10.0f,
			.nearFar = glm::vec2(0.0f, 10.0f),
		};
		sunlight->AddComponent<CameraComponent>(cameraSpec);
	}

	EventListener_KeyPressedListenerLambda keyPressedListener;
	keyPressedListener.onPressed = [&](Key key)
	{
		if (key == Key::R)
		{
			window->shaderLibrary->ReloadShaders();
		}
	};

	window->keyboard->SubscribeListener_KeyPressed(&keyPressedListener);

	constexpr float deltaTime = 1.0f / 60.0f;
	while (!window->bShouldClose)
	{
		auto entities = world->registry.view<TransformComponent, StaticMeshComponent>();

		gbuffer->Bind();
		{
			glCullFace(GL_BACK);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 projection = camera->GetComponent<CameraComponent>().GetProjection();
			glm::mat4 view = camera->GetComponent<CameraComponent>().GetView();

			for (auto [id, transform, staticMesh] : entities.each())
			{
				auto shader = staticMesh.mesh->shader;
				auto mesh = staticMesh.mesh->source;
				if (!shader || !mesh)
					continue;

				shader->Enable();
				shader->SetMat4("_Projection", projection);
				shader->SetMat4("_View", view);
				shader->SetMat4("_Transform", transform.GetMatrix());

				auto texture = staticMesh.config.textures.empty() ? nullptr : staticMesh.config.textures.at(0);
				if (texture != nullptr)
				{
					shader->SetTexture("_Texture", texture->id, 0);
				}

				mesh->Draw(staticMesh.config.topology);
			}
		}
		gbuffer->Unbind();

		shadowBuffer->Bind();
		{
			glClear(GL_DEPTH_BUFFER_BIT);

			glCullFace(GL_FRONT);

			auto shader = window->shaderLibrary->FindShader("shaders\\testapp\\shadow");

			glm::mat4 projection = sunlight->GetComponent<CameraComponent>().GetProjection();
			glm::mat4 view = sunlight->GetComponent<CameraComponent>().GetView();

			for (auto [id, transform, staticMesh] : entities.each())
			{
				auto mesh = staticMesh.mesh->source;
				if (!mesh)
					continue;

				shader->Enable();
				shader->SetMat4("_LightSpaceMatrix", projection * view);
				shader->SetMat4("_Transform", transform.GetMatrix());

				mesh->Draw(staticMesh.config.topology);
			}

			glCullFace(GL_BACK);
		}
		shadowBuffer->Unbind();

		// draw image
		{
			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			glm::mat4 projection = sunlight->GetComponent<CameraComponent>().GetProjection();
			glm::mat4 view = sunlight->GetComponent<CameraComponent>().GetView();

			auto shader = window->shaderLibrary->FindShader("shaders\\testapp\\combine");
			shader->Enable();
			shader->SetMat4("_ShadowMatrix", projection * view);

			shader->SetTexture("_Albedo", gbuffer->GetColorAttachment(0), 0);
			shader->SetTexture("_Position", gbuffer->GetColorAttachment(1), 1);
			shader->SetTexture("_Normal", gbuffer->GetColorAttachment(2), 2);
			shader->SetTexture("_Shadow", shadowBuffer->GetDepthAttachment(), 3);

			screenMesh->Draw(GL_TRIANGLES);
		}

		window->PollEvents();
		window->Present();
	}
}