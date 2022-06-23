#pragma once
#include "CoreTypes.h"
#include "Game/Component/Component.h"

struct AppWindow;

struct ScriptInterface
{
	virtual void Update(Reference<AppWindow> window, f32 deltaTime) = 0;

	Reference<Entity> owner = nullptr;
};

template<typename T>
concept Scriptable = Derived<T, ScriptInterface>;

struct ScriptComponent : public Component
{
	struct Context
	{
		Reference<ScriptInterface> script = nullptr;
	};

	void InvokeUpdate(Reference<AppWindow> window, f32 dt);

	template<Scriptable T, typename...TArgs>
	Reference<T> Add(TArgs&&...args);
	template<Scriptable T>
	Reference<T> Get();

	std::vector<Context> scripts = {};
};
#include "ScriptComponent.inl"