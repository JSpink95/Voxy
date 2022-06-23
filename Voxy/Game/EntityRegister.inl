template<typename TComponent, typename...TArgs> inline TComponent& EntityRegister::AddComponent(entt::entity entity, TArgs&&...args)
{
	return registry.emplace<TComponent, TArgs...>(entity, std::forward<TArgs>(args)...);
}
template<typename TComponent> inline TComponent& EntityRegister::GetComponent(entt::entity entity)
{
	return registry.get<TComponent>(entity);
}
template<typename TComponent> inline bool EntityRegister::HasComponent(entt::entity entity)
{
	return false;// return registry.has<TComponent>(entity);
}
template<typename...TComponents, typename...TExclude> inline entt::view<TComponents...> EntityRegister::FindAllWithComponents(entt::exclude_t<TExclude...> exclude/* = {}*/)
{
	return registry.view<TComponents...>(exclude);
}