#pragma once
#include "CoreTypes.h"

#include "Game/Component/Component.h"
#include "Game/Component/CameraComponent.h"
#include "Game/Component/TransformComponent.h"
#include "Game/Component/ScriptComponent.h"
#include "Game/Component/ParticleSystemComponent.h"
#include "Game/Component/ColliderComponent.h"
#include "Game/Component/RigidBodyComponent.h"
#include "Game/Component/HealthComponent.h"
#include "Game/Component/UIComponent.h"
#include "Game/Component/ComponentTickHandler.h"
#include "Game/Component/MeshComponent.h"
#include "Game/Component/RotatingComponent.h"
#include "Game/Component/OverlapComponent.h"

// cheap component used to mark objects for destruction
struct KillComponent: public Component
{
};
