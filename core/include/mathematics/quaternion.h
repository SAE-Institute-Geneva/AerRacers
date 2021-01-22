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
#include <engine/component.h>

#include "mathematics/matrix.h"
#include "mathematics/trigo.h"
#include "mathematics/vector.h"

namespace neko
{
struct Quaternion
{
	union
	{
		struct
		{
			float x;
			float y;
			float z;
			float w;
		};
		float coord[4]{};
	};

	Quaternion() = default;

	explicit Quaternion(float same)
		: x(same), y(same), z(same), w(same)
	{
	}

	Quaternion(float X, float Y, float Z, float W) noexcept
		: x(X), y(Y), z(Z), w(W)
	{
	}

	/**
	 * \brief Adding explicit constructor for quaternion-like type
	 */
	template<class U>
	explicit Quaternion(U u) noexcept : x(u.x), y(u.y), z(u.z), w(u.w)
	{}

	const float& operator[](size_t p_axis) const
	{
		return coord[p_axis];
	}

	float& operator[](size_t p_axis)
	{

		return coord[p_axis];
	}

	//The dot product between two rotations.
	static float Dot(Quaternion a, Quaternion b)
	{
		return	a.x * b.x +
		          a.y * b.y +
		          a.z * b.z +
		          a.w * b.w;
	}

	//Converts this quaternion to one with the same orientation but with a magnitude of 1.
	static Quaternion Normalized(Quaternion quaternion)
	{
		return quaternion / Magnitude(quaternion);
	}

	static float Magnitude(Quaternion quaternion)
	{
		return std::sqrt(quaternion.x * quaternion.x +
		                 quaternion.y * quaternion.y +
		                 quaternion.z * quaternion.z +
		                 quaternion.w * quaternion.w);
	}

	//Rotates the Quaternion of angle degrees around axis.
	static Quaternion AngleAxis(radian_t rad, neko::Vec3f axis)
	{
		if (axis.SquareMagnitude() == 0.0f)
			return Quaternion::Identity();

		Quaternion result = Quaternion::Identity();
		axis = axis.Normalized();
		axis *= Sin(rad/2.0f);
		result.x = axis.x;
		result.y = axis.y;
		result.z = axis.z;
		result.w = Cos(rad/2.0f);

		return Normalized(result);
	}


	//Returns the angle in degrees between two rotations a and b.
	static degree_t Angle(Quaternion a, Quaternion b)
	{

		return 2.0f * Acos(std::abs(Dot(a, b)));
	}

	[[nodiscard]] Quaternion Conjugate() const
	{
		return Quaternion(-x, -y, -z, w);
	}

	//Returns the Inverse of rotation.
	[[nodiscard]] Quaternion Inverse() const
	{
		const Quaternion conj = Conjugate();
		const float mag = Magnitude(*this);

		return conj / (mag * mag);
	}

	/*
	Returns a rotation that rotates z degrees around the z axis,
	x degrees around the x axis, and y degrees around the y axis;
	applied in that order
	*/
	static Quaternion FromEuler(const EulerAngles& angle)
	{
		const auto cy = Cos(angle.x * 0.5f);
		const auto sy = Sin(angle.x * 0.5f);
		const auto cp = Cos(angle.y * 0.5f);
		const auto sp = Sin(angle.y * 0.5f);
		const auto cr = Cos(angle.z * 0.5f);
		const auto sr = Sin(angle.z * 0.5f);

		return Quaternion(
			cy * cp * cr + sy * sp * sr,
			cy * cp * sr - sy * sp * cr,
			sy * cp * sr + cy * sp * cr,
			sy * cp * cr - cy * sp * sr
		);
	}

	static Quaternion FromRotationMatrix(const Mat4f& mat)
	{
		const float T = 1 + mat[0][0] + mat[1][1] + mat[2][2];
		if (T <= 0)
		{
			if (mat[0][0] > mat[1][1] && mat[0][0] > mat[2][2])
			{
				const float S = sqrtf(1 + mat[0][0] - mat[1][1] - mat[2][2]) * 2.0f;
				const float w = (mat[2][1] - mat[1][2]) / S;
				const float x = 0.25f * S;
				const float y = (mat[0][1] + mat[1][0]) / S;
				const float z = (mat[0][2] + mat[2][0]) / S;

				return Quaternion(x, y, z, w);
			}
			else if (mat[1][1] > mat[2][2])
			{
				const float S = sqrtf(1 + mat[1][1] - mat[0][0] - mat[2][2]) * 2.0f;
				const float w = (mat[0][2] - mat[2][0]) / S;
				const float x = (mat[0][1] + mat[1][0]) / S;
				const float y = 0.25f * S;
				const float z = (mat[1][2] + mat[2][1]) / S;

				return Quaternion(x, y, z, w);
			}
			else
			{
				const float S = sqrtf(1 + mat[2][2] - mat[0][0] - mat[1][1]) * 2.0f;
				const float w = (mat[1][0] - mat[0][1]) / S;
				const float x = (mat[0][2] + mat[2][0]) / S;
				const float y = (mat[1][2] + mat[2][1]) / S;
				const float z = 0.25f * S;

				return Quaternion(x, y, z, w);
			}
		}

		const float w = sqrtf(T) / 2.0f;
		const float x = (mat[2][1] - mat[1][2]) / (w * 4.0f);
		const float y = (mat[0][2] - mat[2][0]) / (w * 4.0f);
		const float z = (mat[1][0] - mat[0][1]) / (w * 4.0f);

		return Quaternion(x, y, z, w);
	}

	static Quaternion Identity()
	{
		return Quaternion(0, 0, 0, 1);
	}

	//Operators
	Quaternion operator/(Quaternion rhs) const
	{
		return *this * rhs.Inverse();
	}

	Quaternion operator/(const float rhs) const {
		return Quaternion(
			x / rhs,
			y / rhs,
			z / rhs,
			w / rhs);
	}

	Quaternion& operator+=(const float rhs)
	{
		x /= rhs;
		y /= rhs;
		z /= rhs;
		w /= rhs;
		return *this;
	}

	Quaternion operator-(const Quaternion& rhs) const
	{
		return Quaternion(
			x - rhs.x,
			y - rhs.y,
			z - rhs.z,
			w - rhs.w);
	}
	Quaternion& operator-=(const Quaternion& rhs)
	{
		x -= rhs.x;
		y -= rhs.y;
		z -= rhs.z;
		w -= rhs.w;
		return *this;
	}

	Quaternion operator+(const Quaternion& rhs) const
	{
		return Quaternion(
			x + rhs.x,
			y + rhs.y,
			z + rhs.z,
			w + rhs.w);
	}

	Quaternion& operator+=(const Quaternion& rhs)
	{
		x += rhs.x;
		y += rhs.y;
		z += rhs.z;
		w += rhs.w;
		return *this;
	}


	Quaternion operator*(const Quaternion& rhs) const
	{
		return Quaternion(
			w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
			w * rhs.y + y * rhs.w + z * rhs.x - x * rhs.z,
			w * rhs.z + z * rhs.w + x * rhs.y - y * rhs.x,
			w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z);
	}

	Vec3f operator*(const Vec3f& rhs) const
	{
		const Vec3f u(x, y, z);
		return 2.0f * Vec3f::Dot(u, rhs) * u
		       + (w * w - Vec3f::Dot(u, u)) * rhs
		       + 2.0f * w * Vec3f::Cross(u, rhs);
	}

	Quaternion operator*(const float rhs) const {
		return Quaternion(
			x * rhs,
			y * rhs,
			z * rhs,
			w * rhs);
	}
	
	Quaternion& operator*=(const Quaternion& rhs)
	{
		x *= rhs.x;
		y *= rhs.y;
		z *= rhs.z;
		w *= rhs.w;
		return *this;
	}

	bool operator==(const Quaternion& right) const
	{
		return x == right.x && y == right.y && z == right.z && w == right.w;
	}

	bool operator!=(const Quaternion& right) const
	{
		return !(*this == right);
	}

	friend std::ostream& operator<<(std::ostream& os, const Quaternion& quat)
	{
		os << "Quaternion(" << quat.x << "," << quat.y << "," << quat.z << "," << quat.w << ")";
		return os;
	}
};
} 