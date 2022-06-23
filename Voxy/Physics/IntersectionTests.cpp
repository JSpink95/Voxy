#include "Physics/IntersectionTests.h"

#include <algorithm>

Box Box::Create(glm::vec3 const& centre, glm::vec3 const& halfSize)
{
	return { centre - halfSize, centre + halfSize };
}

namespace Collision
{
	bool PointBoxOverlap(glm::vec3 const& p, Box const& b)
	{
		return
			(p.x >= b.min.x && p.x <= b.max.x) &&
			(p.y >= b.min.y && p.y <= b.max.y) &&
			(p.z >= b.min.z && p.z <= b.max.z);
	}

	bool PointSphereOverlap(glm::vec3 const& p, Sphere const& s)
	{
		auto d = glm::distance2(p, s.centre);
		return d < (s.radius* s.radius);
	}

	HitResult RayBoxOverlap(Ray const& ray, Box const& box)
	{
		HitResult result = {};

		float tmin = (box.min.x - ray.origin.x) / ray.direction.x;
		float tmax = (box.max.x - ray.origin.x) / ray.direction.x;

		if (tmin > tmax)
			std::swap(tmin, tmax);

		float tymin = (box.min.y - ray.origin.y) / ray.direction.y;
		float tymax = (box.max.y - ray.origin.y) / ray.direction.y;

		if (tymin > tymax)
			std::swap(tymin, tymax);

		if ((tmin > tymax) || (tymin > tmax))
			return result;

		if (tymin > tmin)
			tmin = tymin;

		if (tymax < tmax)
			tmax = tymax;

		float tzmin = (box.min.z - ray.origin.z) / ray.direction.z;
		float tzmax = (box.max.z - ray.origin.z) / ray.direction.z;

		if (tzmin > tzmax)
			std::swap(tzmin, tzmax);

		if ((tmin > tzmax) || (tzmin > tmax))
			return result;

		if (tzmin > tmin)
			tmin = tzmin;

		if (tzmax < tmax)
			tmax = tzmax;

		result.hit = true;
		result.impact = ray.origin + ray.direction * tmin;
		result.distance = tmin;

		return result;
	}

	bool SphereSphereOverlap(Sphere const& a, Sphere const& b)
	{
		auto d = glm::length2(a.centre - b.centre);
		return d < (a.radius + b.radius)* (a.radius + b.radius);
	}

	bool SphereBoxOverlap(Sphere const& sphere, Box const& box)
	{
		f32 x = glm::max(box.min.x, glm::min(sphere.centre.x, box.max.x));
		f32 y = glm::max(box.min.y, glm::min(sphere.centre.y, box.max.y));
		f32 z = glm::max(box.min.z, glm::min(sphere.centre.z, box.max.z));

		auto d = glm::length2(glm::vec3(x, y, z) - sphere.centre);
		return d < sphere.radius;
	}

	bool BoxBoxOverlap(Box const& a, Box const& b)
	{
		return
			(a.min.x <= b.max.x && a.max.x >= b.min.x) &&
			(a.min.y <= b.max.y && a.max.y >= b.min.y) &&
			(a.min.z <= b.max.z && a.max.z >= b.min.z);
	}

	bool SweepBoxBox(Box const& a, glm::vec3 const& va, Box const& b, glm::vec3 const& vb, SweepResult& result)
	{
		if (BoxBoxOverlap(a, b))
		{
			result.collision = true;
			result.entryTime = 0.0f;
			return true;
		}

		auto v = vb - va;

		f32 first = 0.0f;
		f32 last = 1.0f;

		for (s32 index = 0; index < 3; ++index)
		{
			if (v[index] < 0.0f)
			{
				if (b.max[index] < a.min[index]) return false;
				if (a.max[index] < b.min[index]) first = glm::max((a.max[index] - b.min[index]) / v[index], first);
				if (b.max[index] > a.min[index]) last = glm::min((a.min[index] - b.max[index]) / v[index], last);
			}

			if (v[index] > 0.0f)
			{
				if (b.min[index] > a.max[index]) return false;
				if (b.max[index] < a.min[index]) first = glm::max((a.min[index] - b.max[index]) / v[index], first);
				if (a.max[index] > b.min[index]) last = glm::max((a.max[index] - b.min[index]) / v[index], last);
			}

			if (first > last) return false;
		}

		result.entryTime = first;
		result.exitTime = last;
		result.collision = true;
		return true;
	}

	bool SweepBoxBoxes(Box const& a, glm::vec3 const& va, std::vector<Box> const& boxes, SweepResult& result)
	{
		for (auto const& box : boxes)
		{
			SweepResult newResult = {};
			if (SweepBoxBox(a, va, box, glm::vec3(0.0f), newResult) && newResult.entryTime < result.entryTime)
			{
				result = newResult;
			}
		}

		return result.collision;
	}

	SweepResult SweepBoxBox(Box const& a, glm::vec3 const& vel, Box const& b)
	{
		f32 xInvEntry, yInvEntry, zInvEntry;
		f32 xInvExit, yInvExit, zInvExit;
		
		// find the distance between the objects on the near and far sides for both x and y 
		if (vel.x > 0.0f)
		{
			xInvEntry = b.min.x - a.max.x;
			xInvExit = b.max.x - a.min.x;
		}
		else
		{
			xInvEntry = b.max.x - a.min.x;
			xInvExit = b.min.x - a.max.x;
		}
		
		if (vel.y > 0.0f)
		{
			yInvEntry = b.min.y - a.max.y;
			yInvExit = b.max.y - a.min.y;
		}
		else
		{
			yInvEntry = b.max.y - a.min.y;
			yInvExit = b.min.y - a.max.y;
		}
		
		if (vel.z > 0.0f)
		{
			zInvEntry = b.min.z - a.max.z;
			zInvExit = b.max.z - a.min.z;
		}
		else
		{
			zInvEntry = b.max.z - a.min.z;
			zInvExit = b.min.z - a.max.z;
		}
		
		f32 xEntry, yEntry, zEntry;
		f32 xExit, yExit, zExit;
		
		if (vel.x == 0.0f)
		{
			xEntry = -std::numeric_limits<f32>::infinity();
			xExit = std::numeric_limits<f32>::infinity();
		}
		else
		{
			xEntry = xInvEntry / vel.x;
			xExit = xInvExit / vel.x;
		}
		
		if (vel.y == 0.0f)
		{
			yEntry = -std::numeric_limits<f32>::infinity();
			yExit = std::numeric_limits<f32>::infinity();
		}
		else
		{
			yEntry = yInvEntry / vel.y;
			yExit = yInvExit / vel.y;
		}
		
		if (vel.z == 0.0f)
		{
			zEntry = -std::numeric_limits<f32>::infinity();
			zExit = std::numeric_limits<f32>::infinity();
		}
		else
		{
			zEntry = zInvEntry / vel.z;
			zExit = zInvExit / vel.z;
		}
		
		// find the earliest/latest times of collisionfloat 
		SweepResult result = {};
		result.entryTime = std::max({ xEntry, yEntry, zEntry });
		result.exitTime = std::min({ xExit, yExit, zExit });
		
		// if there was no collision
		if (result.entryTime > result.exitTime ||
			(xEntry < 0.0f && yEntry < 0.0f && zEntry < 0.0f) ||
			xEntry > 1.0f || yEntry > 1.0f || zEntry > 1.0f)
		{
			result.collision = false;
			return result;
		}
		else // if there was a collision 
		{
			result.collision = true;
			result.normal = glm::vec3(0.0f);
		
			// calculate normal of collided surface
			if (xEntry > yEntry)
			{
				if (xEntry > zEntry)
				{
					result.normal.x = -glm::sign(vel.x);
				}
				else
				{
					result.normal.z = -glm::sign(vel.z);
				}
			}
			else
			{
				if (yEntry > zEntry)
				{
					result.normal.y = -glm::sign(vel.y);
				}
				else
				{
					result.normal.z = -glm::sign(vel.z);
				}
			}
		}
		
		return result;
	}

	SweepResult SweepBoxBoxes(Box const& a, glm::vec3 const& vel, std::vector<Box> const& boxes)
	{
		SweepResult result = {};

		for (auto const& box : boxes)
		{
			SweepResult newSweepResult = SweepBoxBox(a, vel, box);
			if (newSweepResult.collision && newSweepResult.entryTime < result.entryTime)
				result = newSweepResult;
		}

		return result;
	}
}