template<typename TComponent, typename...TArgs>
inline TComponent& Entity::AddComponent(TArgs&&...args)
{
	auto& component = gameWorld->registry.emplace<TComponent>(id, std::forward<TArgs>(args)...);
	component.owner = shared_from_this();
	component.Initialise();

	if constexpr (std::is_base_of<SceneComponent, TComponent>::value)
	{
		GetComponent<TickHandlerComponent>().RegisterTickFunction([this](f32 deltaTime)
			{
				GetComponent<TComponent>().OnTick(deltaTime);
			});
	}

	return component;
}
template<typename TComponent>
inline TComponent& Entity::GetComponent()
{
	return gameWorld->registry.get<TComponent>(id);
}