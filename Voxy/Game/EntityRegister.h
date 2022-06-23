#pragma once
#include "CoreTypes.h"
#include <entt.hpp>

struct EntityRegister
{
	static auto Create()
	{
		return MakeRef<EntityRegister>();
	}

	entt::entity CreateEntity();
	void DeleteEntity(entt::entity entity);
	
	template<typename TComponent, typename...TArgs> TComponent& AddComponent(entt::entity entity, TArgs&&...args);
	template<typename TComponent> TComponent& GetComponent(entt::entity entity);
	template<typename TComponent> bool HasComponent(entt::entity entity);

	template<typename...TComponents, typename...TExclude> entt::view<TComponents...> FindAllWithComponents(entt::exclude_t<TExclude...> exclude = {});

	entt::registry registry;
};
#include "EntityRegister.inl"