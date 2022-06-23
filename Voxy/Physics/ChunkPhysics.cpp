#include "Physics/ChunkPhysics.h"

#include "Game/GameWorld.h"
#include "Game/Entity.h"
#include "Game/Components.h"

#include "World/Chunk.h"
#include "World/ChunkMap.h"

#include "Physics/IntersectionTests.h"

#include <glm\gtx\projection.hpp>

ChunkPhysics::ChunkPhysics(Reference<ChunkMap> chunks, Reference<GameWorld> world)
	: chunks(chunks)
	, world(world)
{

}

void ChunkPhysics::OnTickGroup(f32 deltaTime)
{
	auto entities = world->registry.view<TransformComponent, RigidBodyComponent, BoxCollider>();

	// update velocities/positions
	for (auto [entity, tc, rb, bc] : entities.each())
	{
		if (!rb.isSimulating)
			continue;

		rb.lastTranslation = rb.translation;
		rb.velocity.y += Gravity * deltaTime;
		rb.translation += rb.velocity * deltaTime;
	}

	for (u32 iteration = 0u; iteration < 8; ++iteration)
	{
		// resolve collisions
		for (auto [entity, tc, rb, bc] : entities.each())
		{
			if (!rb.isSimulating)
				continue;

			auto minBounds = glm::vec3(
				  glm::min(rb.translation.x - bc.halfSize.x, rb.lastTranslation.x - bc.halfSize.x)
				, glm::min(rb.translation.y - bc.halfSize.y, rb.lastTranslation.y - bc.halfSize.y)
				, glm::min(rb.translation.z - bc.halfSize.z, rb.lastTranslation.z - bc.halfSize.z));

			auto maxBounds = glm::vec3(
				  glm::max(rb.translation.x + bc.halfSize.x, rb.lastTranslation.x + bc.halfSize.x)
				, glm::max(rb.translation.y + bc.halfSize.y, rb.lastTranslation.y + bc.halfSize.y)
				, glm::max(rb.translation.z + bc.halfSize.z, rb.lastTranslation.z + bc.halfSize.z));

			std::vector<Block*> blocks = {};
			chunks->GetBlocksInBounds(minBounds, maxBounds, blocks);

			std::vector<Box> collidables = {};
			std::transform(blocks.begin(), blocks.end(), std::back_inserter(collidables), [](Block* block) -> Box
				{
					return Box::Create(block->location, glm::vec3(0.5f));
				});

			if (collidables.size() > 0)
			{
				auto movement = rb.translation - rb.lastTranslation;
				SweepResult sweep = Collision::SweepBoxBoxes(Box::Create(rb.lastTranslation, bc.halfSize), movement, collidables);
				if (sweep.collision)
				{
					auto impulse = -(rb.velocity * sweep.normal) * sweep.normal;
					rb.velocity += impulse;
					rb.translation = rb.lastTranslation + rb.velocity * deltaTime;
				}
			}
		}
	}

	// update transforms
	for (auto [entity, tc, rb, bc] : entities.each())
	{
		if (!rb.isSimulating)
			continue;

		tc.position = rb.translation;
	}
}

Block* ChunkPhysics::RayCast(glm::vec3 const& origin, glm::vec3 const& direction, f32 reach)
{
	auto minBounds = glm::vec3(
		  glm::min(origin.x, origin.x + direction.x * reach)
		, glm::min(origin.y, origin.y + direction.y * reach)
		, glm::min(origin.z, origin.z + direction.z * reach));

	auto maxBounds = glm::vec3(
		  glm::max(origin.x, origin.x + direction.x * reach)
		, glm::max(origin.y, origin.y + direction.y * reach)
		, glm::max(origin.z, origin.z + direction.z * reach));

	std::vector<Block*> blocks = {};
	chunks->GetBlocksInBounds(minBounds, maxBounds, blocks);

	if (blocks.empty())
	{
		return nullptr;
	}

	Ray ray = { origin, direction };

	Block* result = nullptr;
	f32 closestHit = 100000.0f;

	for (Block* block : blocks)
	{
		Box box = Box::Create(block->location, glm::vec3(0.5f));
		auto hit = Collision::RayBoxOverlap(ray, box);

		if (hit.hit && hit.distance < closestHit)
		{
			result = block;
			closestHit = hit.distance;
		}
	}

	return result;
}

void ChunkPhysics::SphereOverlapDynamic(glm::vec3 const& centre, f32 radius, List<Reference<Entity>>& entities)
{
	auto spheres = world->registry.view<TransformComponent, SphereOverlapComponent>();

	Sphere originSphere =
	{
		.centre = centre,
		.radius = radius
	};

	for (auto [id, tc, soc] : spheres.each())
	{
		Sphere sphere =
		{
			.centre = tc.position,
			.radius = soc.radius
		};

		if (Collision::SphereSphereOverlap(originSphere, sphere))
		{
			entities.push_back(Entity::Create(world, id));
		}
	}
}