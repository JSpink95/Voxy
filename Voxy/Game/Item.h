#pragma once
#include "CoreTypes.h"

constexpr u16 NullItemID = 0u;

enum class ItemType
{
	BLOCK, CONSUMABLE, TOOL
};

struct ItemConfig
{
	u16 id = NullItemID;
	ItemType type = ItemType::BLOCK;
};

struct ItemID
{
	static const u16 Block_Dirt;
	static const u16 Block_Grass;
	static const u16 Block_Stone;
	static const u16 Block_Sand;

	static bool IsBlock(u16 id);
	static bool IsConsumable(u16 id);
	static bool IsTool(u16 id);

	static ItemConfig GetConfigForID(u16 id);
};