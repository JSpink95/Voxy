//template<TickableComponent T> inline void TickHandlerComponent::RegisterComponent()
//{
//	auto onTick = [this](f32 deltaTime)
//	{
//		owner->GetComponent<T>().OnTick(deltaTime);
//	};
//
//	listeners.push_back(onTick);
//}