#include "World/Block.h"
#include "Game/Item.h"

#include "Utility/FileIO.h"

#include <json/json.hpp>

namespace BlockUtility
{
	glm::ivec3 GetTextureIdsForBlockType(u64 type)
	{
		static std::unordered_map<u64, glm::ivec3> textureMapping =
		{
			std::make_pair(ItemID::Block_Dirt, glm::ivec3(DirtTextureIndex)),
			std::make_pair(ItemID::Block_Grass, glm::ivec3(DirtTextureIndex, GrassSideTextureIndex, GrassTextureIndex)),
			std::make_pair(ItemID::Block_Stone, glm::ivec3(StoneTextureIndex)),
			std::make_pair(ItemID::Block_Sand, glm::ivec3(SandTextureIndex)),
		};

		if (auto it = textureMapping.find(type); it != textureMapping.end())
		{
			return it->second;
		}

		return textureMapping.at(ItemID::Block_Dirt);
	}
}
/*
  "dirt": {
	"id": 0,
	"config": {
	  "hardness": 50.0,
	  "textures": [ "textures\\dirt.png", "textures\\dirt.png", "textures\\dirt.png" ]
	}
  },
*/
BlockConfigLibrary::BlockConfigLibrary()
{
	std::string blockJson = FileIO::LoadToString("blocks.json");

	auto json = nlohmann::json::parse(blockJson);
	if (!json.is_null() && json.is_object())
	{
		for (auto& jobject : json.items())
		{
			std::string blockName = jobject.key();
			auto& items = jobject.value();
			u16 id = items.value("id", 0);

			Console::Log("Block: Found {0}!", blockName);

			if (auto configIt = items.find("config"); configIt != items.end())
			{
				Reference<BlockConfig> config = MakeRef<BlockConfig>();
				config->hardness = configIt->value<f32>("hardness", 0.0f);

				auto& jsonArray = configIt->at("textures");
				u32 index = 0;
				for (auto& arrayValue : jsonArray) {
					config->textures.at(index++) = arrayValue.get<std::string>();
				}
			}
			else
			{
				Console::LogError("Blocks: {0} has no valid config!", blockName);
			}
		}
	}
}

Reference<BlockConfig> BlockConfigLibrary::FindConfig(u16 id)
{
	if (auto it = configs.find(id); it != configs.end())
	{
		return it->second;
	}

	return nullptr;
}

//Dirt, Grass, Stone, Sand,
std::unordered_map<u16, BlockConfig> GBlockConfig::configs =
{
	// Dirt
	{
		ItemID::Block_Dirt,
		BlockConfig
		{
			.hardness = 50.0f,
			.textures = { "textures\\dirt.png", "textures\\dirt.png","textures\\dirt.png" }
		}
	},

	// Grass
	{
		ItemID::Block_Grass,
		BlockConfig
		{ 
			.hardness = 50.0f, 
			.textures = { "textures\\dirt.png", "textures\\grass_side.png", "textures\\grass.png" }
		}
	},

	// Stone
	{
		ItemID::Block_Stone,
		BlockConfig
		{
			.hardness = 100.0f,
			.textures = { "textures\\stone.png", "textures\\stone.png", "textures\\stone.png" }
		}
	},

	// Sand
	{
		ItemID::Block_Sand,
		BlockConfig
		{
			.hardness = 70.0f,
			.textures = { "textures\\sand.png", "textures\\sand.png", "textures\\sand.png" }
		}
	},
};

BlockConfig const* const GBlockConfig::GetBlockConfig(u16 type)
{
	if (!ItemID::IsBlock(type))
	{
		Console::LogWarning("ItemID({0}) is not a block type!", type);
		return nullptr;
	}

	if (auto it = configs.find(type); it != configs.end())
	{
		return &it->second;
	}

	return nullptr;
}