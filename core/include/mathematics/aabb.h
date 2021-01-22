#pragma once
/*
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include "mathematics/vector.h"
#include "matrix.h"
#include "const.h"
#include "engine/assert.h"
#include "transform.h"

namespace neko
{
struct Obb2d
{
    /// Get the center of the OBB.
	[[nodiscard]] Vec2f GetCenter() const { return center; }

	/// Get the direction of the OBB rotated 90 degrees clockwise.
	Vec2f GetRight() const
	{ return Vec2f(CalculateDirection().y, -CalculateDirection().x); }

	/// Get the direction of the OBB rotated 90 degrees clockwise.
	Vec2f GetUp() const
	{ return Vec2f(CalculateDirection()); }

	/// Set the center, the extends and the rotation of the OBB.
	void FromCenterExtendsRotation(const Vec2f& newCenter, const Vec2f& localExtends, degree_t rot)
	{
		center               = newCenter;
		localLowerLeftBound  = localExtends * -1.0f;
		localUpperRightBound = localExtends;
		rotation             = rot;
	}

	float GetExtendOnAxis(const Vec2f& axis) const
	{
		float extend;

		if (axis == GetUp())
		{
			extend = 0.5f * (localUpperRightBound.y - localLowerLeftBound.y);
			return extend;
		}

		if (axis == GetRight())
		{
			extend = 0.5f * (localUpperRightBound.x - localLowerLeftBound.x);
			return extend;
		}

		float rotationToAxis = Vec2f::AngleBetween(axis, CalculateDirection()).value();
		rotationToAxis       = std::fmod(rotationToAxis, PI);

		if ((rotationToAxis >= 0 && rotationToAxis <= PI / 2) ||
			(rotationToAxis >= -PI && rotationToAxis <= -PI / 2))
		{
			Vec2f lowerLeftToTopRight = localLowerLeftBound - localUpperRightBound;

			extend = (lowerLeftToTopRight.Magnitude() *
					  Vec2f::AngleBetween(lowerLeftToTopRight, axis)).value();
		}
		else
		{
			Vec2f upperLeftBound       = GetOppositeBound(localUpperRightBound, true);
			Vec2 lowerRightToUpperLeft = (upperLeftBound - GetCenter()) * 2;

			extend = (lowerRightToUpperLeft.Magnitude() *
					  Vec2f::AngleBetween(lowerRightToUpperLeft, axis)).value();
		}

		return extend;
	}

	[[nodiscard]] Vec2f GetOppositeBound(const Vec2f& bound, bool isUpper) const
	{
		Vec2f centerToProjectionOnDir;
		Vec2f oppositeBound;
		Vec2f boundToOppositeBound;
		Vec2f centerToBound = bound - GetCenter();

		if (isUpper)
		{
			centerToProjectionOnDir = CalculateDirection() * (centerToBound).Magnitude() *
			                          Cos(Vec2f::AngleBetween(centerToBound, CalculateDirection()));
			boundToOppositeBound = GetCenter() + centerToProjectionOnDir - bound;
			oppositeBound = localUpperRightBound + boundToOppositeBound + boundToOppositeBound;
		}
		else
		{
			centerToProjectionOnDir =
				CalculateDirection() * (centerToBound).Magnitude() *
				Cos(Vec2f::AngleBetween(centerToBound, CalculateDirection())) * -1;
			boundToOppositeBound = GetCenter() + centerToProjectionOnDir - bound;
			oppositeBound = localLowerLeftBound + boundToOppositeBound + boundToOppositeBound;
		}

		return oppositeBound;
	}

	[[nodiscard]] Vec2f ProjectOnAxis(const Vec2f& axis) const
	{
		float min = std::numeric_limits<float>::infinity();
		float max = -std::numeric_limits<float>::infinity();
		std::array<Vec2f, 4> corners;
		corners[0] = localLowerLeftBound.Rotate(rotation) + GetCenter();
		corners[1] =
			Vec2f(localUpperRightBound.x,
				localLowerLeftBound.y).Rotate(rotation) + GetCenter();
		corners[2] =
			Vec2f(localLowerLeftBound.x,
				localUpperRightBound.y).Rotate(rotation) + GetCenter();
		corners[3] = localUpperRightBound.Rotate(rotation) + GetCenter();
		for (auto& corner : corners)
		{
			float projection = Vec2f::Dot(corner, axis);
			if (projection < min) min = projection;
			if (projection > max) max = projection;
		}
		return Vec2f(min, max);
	}

	bool IntersectObb(Obb2d& obb1)
	{
		std::array<Vec2f, 4> perpendicularAxis =
		{
			GetUp(), GetRight(), obb1.GetUp(), obb1.GetRight()
		};

		// we need to find the minimal overlap and axis on which it happens
		const auto func = [this, obb1](const Vec2f& axis)
		{
			Vec2f proj1 = ProjectOnAxis(axis);
			Vec2f proj2 = obb1.ProjectOnAxis(axis);

			if (!(proj1.x <= proj2.y && proj1.y >= proj2.x) ||
				!(proj2.x <= proj1.y && proj2.y >= proj1.x))
			{
				return false;
			}

			return true;
		};

		if (std::all_of(perpendicularAxis.cbegin(), perpendicularAxis.cend(), func))
			return true;

		return false;

		/*for (auto& axis : perpendicularAxis)
		{
			Vec2f proj1 = ProjectOnAxis(axis);
			Vec2f proj2 = obb1.ProjectOnAxis(axis);

			if (!(proj1.x <= proj2.y && proj1.y >= proj2.x) ||
				!(proj2.x <= proj1.y && proj2.y >= proj1.x))
			{
				return false;
			}
		}*/
	}

	[[nodiscard]] Vec2f CalculateDirection() const    ///< return the normal of the upper side
	{
		Vec2f direction;
		direction.x = Cos(rotation);
		direction.y = Sin(rotation);
		return direction;
	}

	Vec2f localLowerLeftBound;     ///< the lower vertex
	Vec2f localUpperRightBound;    ///< the upper vertex
	Vec2f center;                  ///< the center of the obb
	radian_t rotation;             ///< the angle of rotation in rd
};

struct Obb3d
{
	[[nodiscard]] Vec3f GetCenter() const
	{ return center; }

	[[nodiscard]] Vec3f GetRight() const
    { return RotateAxis(Vec3f::right, rotation); }

	[[nodiscard]] Vec3f GetUp() const
    { return RotateAxis(Vec3f::up, rotation); }

	[[nodiscard]] Vec3f GetForward() const
    { return RotateAxis(Vec3f::forward, rotation); }

    /// Set the center, the extends and the rotation of the OBB.
	void FromCenterExtendsRotation(
		const Vec3f& newCenter,
		const Vec3f& localExtends,
		const RadianAngles& rot)
	{
		center               = newCenter;
		localLowerLeftBound  = localExtends * -1.0f;
		localUpperRightBound = localExtends;
		rotation.x           = rot.x;
		rotation.y           = rot.y;
		rotation.z           = rot.z;
	}

	[[nodiscard]] float GetExtendOnAxis(const Vec3f& axis) const
	{
		float extend;

		if (axis == GetUp())
		{
			extend = 0.5f * (localUpperRightBound.y - localLowerLeftBound.y);
			return extend;
		}

		if (axis == GetRight())
		{
			extend = 0.5f * (localUpperRightBound.x - localLowerLeftBound.x);
			return extend;
		}

		if (axis == GetForward())
		{
			extend = 0.5f * (localUpperRightBound.z - localLowerLeftBound.z);
			return extend;
		}

		float rotationToAxis = Vec3f::AngleBetween(axis, GetUp()).value();
		rotationToAxis       = std::fmod(rotationToAxis, PI);

		if ((rotationToAxis >= 0 && rotationToAxis <= PI / 2) ||
			(rotationToAxis >= -PI && rotationToAxis <= -PI / 2))
		{
			Vec3f lowerLeftToTopRight = localLowerLeftBound - localUpperRightBound;

			extend = (lowerLeftToTopRight.Magnitude() *
					  Vec3f::AngleBetween(lowerLeftToTopRight, axis)).value();
		}
		else
		{
			Vec3f upperLeftBound        = GetOppositeBound(localUpperRightBound, true);
			Vec3f lowerRightToUpperLeft = (upperLeftBound - GetCenter()) * 2;

			extend = (lowerRightToUpperLeft.Magnitude() *
					  Vec3f::AngleBetween(lowerRightToUpperLeft, axis)).value();
		}

		return extend;
	}

	[[nodiscard]] Vec3f GetOppositeBound(const Vec3f& bound, bool isUpper) const
	{
		Vec3f centerToProjectionOnDir;
		Vec3f oppositeBound;
		Vec3f boundToOppositeBound;
		Vec3f centerToBound = bound - GetCenter();

		if (isUpper)
		{
			centerToProjectionOnDir = GetUp() * (centerToBound).Magnitude() *
			                          Cos(Vec3f::AngleBetween(centerToBound, GetUp()));
			boundToOppositeBound = GetCenter() + centerToProjectionOnDir - bound;
			oppositeBound = localUpperRightBound + boundToOppositeBound + boundToOppositeBound;
		}
		else
		{
			centerToProjectionOnDir = GetUp() * (centerToBound).Magnitude() *
			                          Cos(Vec3f::AngleBetween(centerToBound, GetUp())) * -1;
			boundToOppositeBound = GetCenter() + centerToProjectionOnDir - bound;
			oppositeBound = localLowerLeftBound + boundToOppositeBound + boundToOppositeBound;
		}

		return oppositeBound;
	}

	[[nodiscard]] Vec2f ProjectOnAxis(const Vec3f& axis) const
	{
		float min = std::numeric_limits<float>::infinity();
		float max = -std::numeric_limits<float>::infinity();

		std::array<Vec3f, 8> corners;
		corners[0] = RotateAxis(localLowerLeftBound, rotation) + GetCenter();
		corners[1] = RotateAxis(localLowerLeftBound * Vec3f(-1,  1,  1), rotation) + GetCenter();
		corners[2] = RotateAxis(localLowerLeftBound * Vec3f( 1,  1, -1), rotation) + GetCenter();
		corners[3] = RotateAxis(localLowerLeftBound * Vec3f(-1,  1, -1), rotation) + GetCenter();
		corners[4] = RotateAxis(localLowerLeftBound * Vec3f( 1, -1,  1), rotation) + GetCenter();
		corners[5] = RotateAxis(localLowerLeftBound * Vec3f(-1, -1,  1), rotation) + GetCenter();
		corners[6] = RotateAxis(localLowerLeftBound * Vec3f( 1, -1, -1), rotation) + GetCenter();
		corners[7] = RotateAxis(-localLowerLeftBound, rotation) + GetCenter();

		for (auto& corner : corners)
		{
			float projection = Vec3f::Dot(corner, axis);
			if (projection < min) min = projection;
			if (projection > max) max = projection;
		}
		return Vec2f(min, max);
	}

	bool IntersectObb(Obb3d& obb1)
	{
		std::array<Vec3f, 6> perpendicularAxis =
			{GetUp(), GetRight(), GetForward(), obb1.GetUp(), obb1.GetRight(), obb1.GetForward()};

		// we need to find the minimal overlap and axis on which it happens
		const auto func = [this, obb1](const Vec3f& axis)
		{
			Vec2f proj1 = ProjectOnAxis(axis);
			Vec2f proj2 = obb1.ProjectOnAxis(axis);

			if (!(proj1.x <= proj2.y && proj1.y >= proj2.x) ||
				!(proj2.x <= proj1.y && proj2.y >= proj1.x))
			{
				return false;
			}

			return true;
		};

		if (std::all_of(perpendicularAxis.cbegin(), perpendicularAxis.cend(), func))
			return true;

		return false;
	}

	static Vec3f RotateAxis(const Vec3f& axis, const RadianAngles& rot)    ///< return the normal of the upper side
	{
		EulerAngles euler(rot.x, rot.y, rot.z);
		Quaternion q = Quaternion::FromEuler(euler);
		Vec3f newAxis;
		newAxis.x = axis.x * (q.x * q.x + q.w * q.w - q.y * q.y - q.z * q.z) +
		            axis.y * (2 * q.x * q.y - 2 * q.w * q.z) +
		            axis.z * (2 * q.x * q.z + 2 * q.w * q.y);
		newAxis.y = axis.x * (2 * q.w * q.z + 2 * q.x * q.y) +
		            axis.y * (q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z) +
		            axis.z * (-2 * q.w * q.x + 2 * q.y * q.z);
		newAxis.z = axis.x * (-2 * q.w * q.y + 2 * q.x * q.z) +
		            axis.y * (2 * q.w * q.x + 2 * q.y * q.z) +
		            axis.z * (q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);
		return newAxis;
	}

	Vec3f localLowerLeftBound;     ///< the lower vertex
	Vec3f localUpperRightBound;    ///< the upper vertex
	Vec3f center;                  ///< the center of the obb
	RadianAngles rotation;
};

struct Aabb2d
{
    ///\brief Get the center of the AABB.
    [[nodiscard]] Vec2f CalculateCenter() const
	{ return 0.5f * (lowerLeftBound + upperRightBound); }

    ///\brief Get the extends of the AABB.
    [[nodiscard]] Vec2f CalculateExtends() const
	{ return upperRightBound - lowerLeftBound; }

    ///\brief Get the half-extends of the AABB.
    [[nodiscard]] Vec2f CalculateHalfExtends() const
	{ return (upperRightBound - lowerLeftBound) * 0.5f; }

    ///\brief Set the AABB from the center, extends.
	void FromCenterExtends(const Vec2f& center, const Vec2f& extends)
	{
		lowerLeftBound  = center - extends;
		upperRightBound = center + extends;
	}

	///\brief Enlarge AABB after a rotation
	void FromCenterExtendsRotation(
		const Vec2f& center,
		const Vec2f& extends,
		const degree_t rotation)
	{
		Vec2f relativeLowerLeftBound  = -extends;
		Vec2f relativeUpperRightBound = extends;
		radian_t newAngle             = rotation;
		std::array<Vec2f, 4> corners;
		corners[0] = relativeLowerLeftBound.Rotate(newAngle);
		corners[1] = Vec2f(relativeLowerLeftBound.x, relativeUpperRightBound.y).Rotate(newAngle);
		corners[2] = Vec2f(relativeUpperRightBound.x, relativeLowerLeftBound.y).Rotate(newAngle);
		corners[3] = relativeUpperRightBound.Rotate(newAngle);
		for (const Vec2f corner : corners)
		{
			if (corner.x > relativeUpperRightBound.x) { relativeUpperRightBound.x = corner.x; }
			if (corner.x < relativeLowerLeftBound.x) { relativeLowerLeftBound.x = corner.x; }
			if (corner.y > relativeUpperRightBound.y) { relativeUpperRightBound.y = corner.y; }
			if (corner.y < relativeLowerLeftBound.y) { relativeLowerLeftBound.y = corner.y; }
		}

		upperRightBound = relativeUpperRightBound + center;
		lowerLeftBound  = relativeLowerLeftBound + center;
	}

	///\brief Set the AABB from OBB.
	void FromObb(const Obb2d& obb)
	{
		FromCenterExtendsRotation(
			obb.GetCenter(),
			Vec2f(obb.GetExtendOnAxis(obb.GetRight()), obb.GetExtendOnAxis(obb.GetUp())),
			obb.rotation);
	}

	[[nodiscard]] bool DoContainPoint(const Vec2f& point) const
	{
		bool contains = point.x <= upperRightBound.x && point.x >= lowerLeftBound.x;
		contains      = point.y <= upperRightBound.y && point.y >= lowerLeftBound.y && contains;
		return contains;
	}

	[[nodiscard]] bool DoContainAabb(const Aabb2d& aabb) const
	{
		return (DoContainPoint(aabb.upperRightBound) && DoContainPoint(aabb.lowerLeftBound));
	}

	[[nodiscard]] bool DoIntersectAabb(const Aabb2d& aabb) const
	{
		bool x = Abs(aabb.CalculateCenter().x - CalculateCenter().x) <=
		         (aabb.CalculateHalfExtends().x + CalculateHalfExtends().x);
		bool y = Abs(aabb.CalculateCenter().y - CalculateCenter().y) <=
		         (aabb.CalculateHalfExtends().y + CalculateHalfExtends().y);

		return x && y;
	}

	[[nodiscard]] bool DoIntersectRay(const Vec2f& dirRay, const Vec2f& origin) const
	{
		neko_assert(Vec2f(0, 0) != dirRay, "Null Ray Direction")

		if (DoContainPoint(origin)) return true;
		std::array<float, 4> touch {
			(lowerLeftBound.x - origin.x) / dirRay.x,
			(upperRightBound.x - origin.x) / dirRay.x,
			(lowerLeftBound.y - origin.y) / dirRay.y,
			(upperRightBound.y - origin.y) / dirRay.y,
		};

		float touchMin = std::max(std::min(touch[0], touch[1]), std::min(touch[2], touch[3]));
		float touchMax = std::min(std::max(touch[0], touch[1]), std::max(touch[2], touch[3]));

		// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
		if (touchMax < 0) return false;

		// if tmin > tmax, ray doesn't intersect AABB
		if (touchMin > touchMax) return false;

		return true;
	}

	Vec2f lowerLeftBound  = Vec2f::zero;    // the lower vertex
	Vec2f upperRightBound = Vec2f::zero;    // the upper vertex
};

struct Aabb3d
{
    ///\brief Get the center of the AABB.
    [[nodiscard]] Vec3f CalculateCenter() const
	{ return (lowerLeftBound + upperRightBound) * 0.5f; }

    ///\brief Get the extends of the AABB.
    [[nodiscard]] Vec3f CalculateExtends() const
	{ return upperRightBound - lowerLeftBound; }

    ///\brief Get the half-extends of the AABB.
    [[nodiscard]] Vec3f CalculateHalfExtends() const
	{ return (upperRightBound - lowerLeftBound) * 0.5f; }

    ///\brief Set the AABB from the center, extends.
	void FromCenterExtends(
		const Vec3f& center,
		const Vec3f& extends,
		const Vec3f& = Vec3f::zero)
	{
		lowerLeftBound  = center - extends;
		upperRightBound = center + extends;
	}

	///\brief Enlarge AABB after a rotation
	void FromCenterExtendsRotation(const Vec3f& center, Vec3f& extends, const RadianAngles& rotation)
	{
		Vec3f relativeLowerLeftBound  = -extends;
		Vec3f relativeUpperRightBound = extends;

		std::array<Vec3f, 8> corners {
			RotateAxis(relativeLowerLeftBound, rotation),
			RotateAxis(relativeLowerLeftBound * Vec3f(-1, 1, 1), rotation),
			RotateAxis(relativeLowerLeftBound * Vec3f(1, 1, -1), rotation),
			RotateAxis(relativeLowerLeftBound * Vec3f(-1, 1, -1), rotation),
			RotateAxis(relativeLowerLeftBound * Vec3f(1, -1, 1), rotation),
			RotateAxis(relativeLowerLeftBound * Vec3f(-1, -1, 1), rotation),
			RotateAxis(relativeLowerLeftBound * Vec3f(1, -1, -1), rotation),
			RotateAxis(-relativeLowerLeftBound, rotation),
		};

		for (const auto& corner : corners)
		{
			if (corner.x > relativeUpperRightBound.x) relativeUpperRightBound.x = corner.x;
			if (corner.x < relativeLowerLeftBound.x) relativeLowerLeftBound.x = corner.x;
			if (corner.y > relativeUpperRightBound.y) relativeUpperRightBound.y = corner.y;
			if (corner.y < relativeLowerLeftBound.y) relativeLowerLeftBound.y = corner.y;
			if (corner.z > relativeUpperRightBound.z) relativeUpperRightBound.z = corner.z;
			if (corner.z < relativeLowerLeftBound.z) relativeLowerLeftBound.z = corner.z;
		}

		lowerLeftBound  = relativeLowerLeftBound + center;
		upperRightBound = relativeUpperRightBound + center;
	}

	//\brief Set the AABB3d from OBB3d.
	void FromObb(const Obb3d& obb)
	{
		Vec3f extends = Vec3f(obb.GetExtendOnAxis(obb.GetRight()),
			obb.GetExtendOnAxis(obb.GetUp()),
			obb.GetExtendOnAxis(obb.GetForward()));
		FromCenterExtendsRotation(obb.GetCenter(), extends, obb.rotation);
	}

	[[nodiscard]] bool DoContainPoint(const Vec3f point) const
	{
		bool contains = point.x <= upperRightBound.x && point.x >= lowerLeftBound.x;
		contains      = point.y <= upperRightBound.y && point.y >= lowerLeftBound.y && contains;
		contains      = point.z <= upperRightBound.z && point.z >= lowerLeftBound.z && contains;
		return contains;
	}

	[[nodiscard]] bool DoContainAabb(const Aabb3d& aabb) const
	{
		return (DoContainPoint(aabb.upperRightBound) && DoContainPoint(aabb.lowerLeftBound));
	}

	[[nodiscard]] bool DoIntersectAabb(const Aabb3d& aabb) const
	{
		bool x = abs(aabb.CalculateCenter().x - CalculateCenter().x) <=
		         (aabb.CalculateHalfExtends().x + CalculateHalfExtends().x);
		bool y = abs(aabb.CalculateCenter().y - CalculateCenter().y) <=
		         (aabb.CalculateHalfExtends().y + CalculateHalfExtends().y);
		bool z = abs(aabb.CalculateCenter().z - CalculateCenter().z) <=
		         (aabb.CalculateHalfExtends().z + CalculateHalfExtends().z);

		return x && y && z;
	}

	[[nodiscard]] bool DoIntersectRay(const Vec3f& dirRay, const Vec3f& origin) const
	{
		neko_assert(Vec3f(0, 0, 0) != dirRay, "Null Ray Direction")
		if (DoContainPoint(origin)) return true;
		std::array<float, 6> touch {
			(lowerLeftBound.x - origin.x) / dirRay.x,
			(upperRightBound.x - origin.x) / dirRay.x,
			(lowerLeftBound.y - origin.y) / dirRay.y,
			(upperRightBound.y - origin.y) / dirRay.y,
			(lowerLeftBound.z - origin.z) / dirRay.z,
			(upperRightBound.z - origin.z) / dirRay.z,
		};

		float touchMin =
			std::max(
				std::max(
					std::min(touch[0], touch[1]),
					std::min(touch[2], touch[3])),
				std::min(touch[4], touch[5]));
		float touchMax =
			std::min(
				std::min(
					std::max(touch[0], touch[1]),
					std::max(touch[2], touch[3])),
				std::max(touch[4], touch[5]));

		// if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
		if (touchMax < 0) return false;

		// if tmin > tmax, ray doesn't intersect AABB
		if (touchMin > touchMax) return false;

		return true;
	}

	[[nodiscard]] bool IntersectPlane(const Vec3f& normal, const Vec3f& point) const
	{
		if (DoContainPoint(point)) return true;
		Vec3f extends = CalculateHalfExtends();
		Vec3f center  = CalculateCenter();

		float r = extends.x * std::abs(normal.x) + extends.y * std::abs(normal.y) +
		          extends.z * std::abs(normal.z);
		float d = Vec3f::Dot(normal, point);
		float s = Vec3f::Dot(normal, center) - d;
		return std::abs(s) <= r;
	}

	static Vec3f RotateAxis(const Vec3f& axis, const RadianAngles& rotation)    ///< return the normal of the upper side
	{
		EulerAngles euler(rotation.x, rotation.y, rotation.z);
		Quaternion q = Quaternion::FromEuler(euler);
		Vec3f newAxis;
		newAxis.x = axis.x * (q.x * q.x + q.w * q.w - q.y * q.y - q.z * q.z) +
		            axis.y * (2 * q.x * q.y - 2 * q.w * q.z) +
		            axis.z * (2 * q.x * q.z + 2 * q.w * q.y);
		newAxis.y = axis.x * (2 * q.w * q.z + 2 * q.x * q.y) +
		            axis.y * (q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z) +
		            axis.z * (-2 * q.w * q.x + 2 * q.y * q.z);
		newAxis.z = axis.x * (-2 * q.w * q.y + 2 * q.x * q.z) +
		            axis.y * (2 * q.w * q.x + 2 * q.y * q.z) +
		            axis.z * (q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z);
		return newAxis;
	}

	Vec3f lowerLeftBound  = Vec3f::zero;    // the lower vertex
	Vec3f upperRightBound = Vec3f::zero;    // the upper vertex
};

}

