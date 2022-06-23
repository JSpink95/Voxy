#include "Graphics/Renderer/ChunkRenderer.h"

#include "AppWindow.h"

#include "Graphics/Shader.h"
#include "Graphics/Texture.h"
#include "Graphics/VertexMesh.h"

#include "Game/Entity.h"
#include "Game/Components.h"
#include "Game/GameWorld.h"

#include "World/ChunkManager.h"
#include "World/ChunkMap.h"
#include "World/Chunk.h"

#include "Utility/ShaderLibrary.h"
#include "Utility/TextureLibrary.h"
#include "Utility/MathUtility.h"

ChunkRenderer::ChunkRenderer(Reference<AppWindow> window, Reference<GameWorld> world)
	: Renderer(window, world)
{
	blockTextures.at(BlockUtility::DirtTextureIndex) = window->textureLibrary->FindTexture("textures\\dirt.png");
	blockTextures.at(BlockUtility::GrassTextureIndex) = window->textureLibrary->FindTexture("textures\\grass.png");
	blockTextures.at(BlockUtility::GrassSideTextureIndex) = window->textureLibrary->FindTexture("textures\\grass_side.png");
	blockTextures.at(BlockUtility::StoneTextureIndex) = window->textureLibrary->FindTexture("textures\\stone.png");
	blockTextures.at(BlockUtility::SandTextureIndex) = window->textureLibrary->FindTexture("textures\\sand.png");
}

void ChunkRenderer::Render(Reference<Entity> camera)
{
	auto chunkShader = window->shaderLibrary->FindShader("shaders\\chunk");

	glm::mat4 projection = camera->GetComponent<CameraComponent>().GetProjection();
	glm::mat4 view = camera->GetComponent<CameraComponent>().GetView();

	chunkShader->Enable();
	chunkShader->SetMat4("_Projection", projection);
	chunkShader->SetMat4("_View", view);

	for (u8 textureIndex = 0; textureIndex < BlockUtility::TextureCount; ++textureIndex)
	{
		auto name = std::format("_Texture[{0}]", textureIndex);
		chunkShader->SetTexture(name.data(), blockTextures.at(textureIndex)->id, textureIndex);
	}

	chunkShader->SetFloat("_BackFace", backface ? 1.0f : 0.0f);

	for (auto [location, chunk] : *world->chunkManager->chunks)
	{
		if (bounds.has_value())
		{
			glm::ivec4 b = bounds.value();
			if (location.x < b.x || location.x > b.y ||
				location.y < b.z || location.y > b.w)
				continue;
		}

		glm::mat4 transform = MathUtils::CreateTransform(glm::vec3((f32)location.x, 0.0f, (f32)location.y) * (f32)Chunk::Size);
		chunkShader->SetMat4("_Transform", transform);

		if (chunk->mesh != nullptr)
			chunk->mesh->Draw(GL_POINTS);
	}
}
