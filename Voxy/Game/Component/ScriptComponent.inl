template<Scriptable T, typename...TArgs> inline Reference<T> ScriptComponent::Add(TArgs&&...args)
{
	Reference<T> script = MakeRef<T>(std::forward<TArgs>(args)...);
	script->owner = owner;

	Context context = { script };

	scripts.push_back(context);
	return script;
}
template<Scriptable T> inline Reference<T> ScriptComponent::Get()
{
	for (auto context : scripts)
	{
		if (auto script = Cast<T>(context.script))
			return script;
	}

	return nullptr;
}
