#include "Game/Component/ScriptComponent.h"

void ScriptComponent::InvokeUpdate(Reference<AppWindow> window, f32 dt)
{
	for (auto context : scripts)
	{
		if (context.script)
		{
			context.script->Update(window, dt);
		}
	}
}