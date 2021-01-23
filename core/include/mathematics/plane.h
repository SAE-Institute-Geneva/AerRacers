#pragma once
#include "mathematics/vector.h"

namespace neko
{
struct Plane
{
	Plane() = default;
	explicit Plane(const neko::Vec3f& point, const neko::Vec3f& normal)
		: point(point), normal(normal)
	{}

	explicit Plane(const Vec3f& pointA, const Vec3f& pointB, const Vec3f& pointC)
		: point(pointA), normal(CalculateNormalFrom(pointA, pointB, pointC))
	{}

	[[nodiscard]] static neko::Vec3f CalculateNormalFrom(
		const Vec3f& pointA,
		const Vec3f& pointB,
		const Vec3f& pointC)
	{
		Vec3f vecA = pointA - pointB;
		Vec3f vecB = pointC - pointB;
		return Vec3f::Cross(vecA, vecB).Normalized();
	}

	[[nodiscard]] float Distance(const neko::Vec3f& pointA) const
	{
		return Vec3f::Dot(point - pointA, normal);
	}

	[[nodiscard]] Vec3f IntersectPoint(const Vec3f& line, const Vec3f& linePoint) const
	{
		Vec3f diff  = linePoint - point;
		float prod1 = Vec3f::Dot(diff, normal);
		float prod2 = Vec3f::Dot(line, normal);
		float prod3 = prod1 / prod2;
		return linePoint - line * prod3;
	}

	neko::Vec3f point   = Vec3f::zero;
	neko::Vec3f normal  = Vec3f::up;
};
}
