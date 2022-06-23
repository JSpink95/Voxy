#include "Game/EntityRegister.h"

entt::entity EntityRegister::CreateEntity()
{
	return registry.create();
}

void EntityRegister::DeleteEntity(entt::entity entity)
{
	registry.destroy(entity);
}
