#pragma once

#include <CoreTypes.h>

struct Ray
{
	glm::vec3 origin = glm::vec3(0.0f);
	glm::vec3 direction = glm::vec3(0.0f, 0.0f, -1.0f);
};

struct Sphere 
{
	glm::vec3 centre = glm::vec3(0.0f);
	f32 radius = 0.5f;
};

struct Box
{
	static Box Create(glm::vec3 const& centre, glm::vec3 const& halfSize);

	glm::vec3 min = glm::vec3(0.0f);
	glm::vec3 max = glm::vec3(1.0f);
};

struct HitResult
{
	bool hit = false;
	glm::vec3 impact = glm::vec3(0.0f);
	f32 distance = -1.0f;
};

struct SweepResult
{
	f32 entryTime = 1.0f;
	f32 exitTime = 1.0f;
	glm::vec3 normal = glm::vec3(1.0f);
	bool collision = false;
};

namespace Collision
{
	// point tests
	bool PointBoxOverlap(glm::vec3 const& p, Box const& b);
	bool PointSphereOverlap(glm::vec3 const& p, Sphere const& s);

	// ray tests
	HitResult RayBoxOverlap(Ray const& ray, Box const& box);

	// sphere tests
	bool SphereSphereOverlap(Sphere const& a, Sphere const& b);
	bool SphereBoxOverlap(Sphere const& sphere, Box const& box);

	// box tests
	bool BoxBoxOverlap(Box const& a, Box const& b);
	bool SweepBoxBox(Box const& a, glm::vec3 const& va, Box const& b, glm::vec3 const& vb, SweepResult& result);
	bool SweepBoxBoxes(Box const& a, glm::vec3 const& va, std::vector<Box> const& boxes, SweepResult& result);

	SweepResult SweepBoxBox(Box const& a, glm::vec3 const& vel, Box const& b);
	SweepResult SweepBoxBoxes(Box const& a, glm::vec3 const& vel, std::vector<Box> const& boxes);
}