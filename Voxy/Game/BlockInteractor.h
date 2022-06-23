#pragma once
#include "CoreTypes.h"
#include "Game/Component/Component.h"

struct AppWindow;
struct Entity;
struct ChunkPhysics;
struct Block;

struct BlockInteractor
{
	static Reference<BlockInteractor> Create()
	{
		return MakeRef<BlockInteractor>();
	}

	void Initialise(Reference<AppWindow> window, Reference<Entity> owner);

	bool HitBlock(f32 modifier);
	void ResetHealth();

	void Update(f32 deltaTime);

	f32 GetBlockHealthAsPercentage() const;

	Reference<Entity> owner = nullptr;
	Block* selected = nullptr;
private:
	void UpdateHighlightProxy();
	void UpdateDestroyProxy();
private:
	f32 blockHealth = 0.0f;
	f32 maxHealth = 100.0f;
	Reference<Entity> blockHighlightProxy = nullptr;
	Reference<Entity> blockDestroyProxy = nullptr;
};

struct BlockInteractorComponent : public SceneComponent
{
	BlockInteractorComponent(Reference<AppWindow> window);

	void Initialise() override;
	void OnTick(f32 deltaTime) override;

	Reference<AppWindow> window = nullptr;
	Reference<BlockInteractor> interactor = nullptr;
};
