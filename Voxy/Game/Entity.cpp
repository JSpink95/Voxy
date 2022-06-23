#include "Game/Entity.h"

Reference<Entity> Entity::Create(Reference<GameWorld> gameWorld)
{
	Reference<Entity> result = MakeRef<Entity>();
	result->gameWorld = gameWorld;
	result->id = gameWorld->registry.create();

	result->AddComponent<TickHandlerComponent>();

	return result;
}

Reference<Entity> Entity::Create(Reference<GameWorld> gameWorld, EntityID entityId)
{
	Reference<Entity> result = MakeRef<Entity>();
	result->gameWorld = gameWorld;
	result->id = entityId;

	return result;
}