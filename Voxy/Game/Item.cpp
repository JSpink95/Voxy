#include "Game/Item.h"

struct ItemIDGenerator
{
	template<ItemType type>
	static u16 NextID()
	{
		static u16 id = 0;
		return ++id;
	}
};

constexpr u16 ItemIDMax = 1000;

constexpr u16 BlockStartIndex		= ItemIDMax * 0;
constexpr u16 ConsumableStartIndex	= ItemIDMax * 1;
constexpr u16 ToolStartIndex		= ItemIDMax * 2;

// blocks
const u16 ItemID::Block_Dirt	= BlockStartIndex + ItemIDGenerator::NextID<ItemType::BLOCK>();
const u16 ItemID::Block_Grass	= BlockStartIndex + ItemIDGenerator::NextID<ItemType::BLOCK>();
const u16 ItemID::Block_Stone	= BlockStartIndex + ItemIDGenerator::NextID<ItemType::BLOCK>();
const u16 ItemID::Block_Sand	= BlockStartIndex + ItemIDGenerator::NextID<ItemType::BLOCK>();

bool ItemID::IsBlock(u16 id)
{
	return id > BlockStartIndex && id < (BlockStartIndex + ItemIDMax);
}

bool ItemID::IsConsumable(u16 id)
{
	return id > ConsumableStartIndex && id < (ConsumableStartIndex + ItemIDMax);
}

bool ItemID::IsTool(u16 id)
{
	return id > ToolStartIndex && id < (ToolStartIndex + ItemIDMax);
}

ItemConfig ItemID::GetConfigForID(u16 id)
{
	ItemConfig result =
	{
		.id = id
	};

	if (IsBlock(id))
	{
		result.type = ItemType::BLOCK;
	}

	if (IsConsumable(id))
	{
		result.type = ItemType::CONSUMABLE;
	}
	
	if (IsTool(id))
	{
		result.type = ItemType::TOOL;
	}

	return result;
}