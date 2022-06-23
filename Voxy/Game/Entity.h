#pragma once
#include "CoreTypes.h"
#include "Game/EntityRegister.h"
#include "Game/GameWorld.h"

#include "Game/Component/Component.h"
#include "Game/Component/ComponentTickHandler.h"

struct Entity
	: public std::enable_shared_from_this<Entity>
{
	static Reference<Entity> Create(Reference<GameWorld> gameWorld);
	static Reference<Entity> Create(Reference<GameWorld> gameWorld, EntityID entityId);

	inline auto Shared() { return shared_from_this(); }

	template<typename TComponent, typename...TArgs>
	TComponent& AddComponent(TArgs&&...args);
	template<typename TComponent>
	TComponent& GetComponent();

	entt::entity id = entt::null;
	Reference<GameWorld> gameWorld = nullptr;
};
#include "Entity.inl"