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
#include <mathematics/vector.h>
#include "mathematics/trigo.h"


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
		float coord[4];
	};

	Quaternion()
	{
		x = y = z = w = 0;
	}

	explicit Quaternion(float same)
		: x(same), y(same), z(same), w(same)
	{
	}

	Quaternion(float X, float Y, float Z, float W) noexcept
		: x(X), y(Y), z(Z), w(W)
	{
	}


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

	Quaternion Conjugate() const
	{
		return Quaternion(-x, -y, -z, w);
	}

	//Returns the Inverse of rotation.
	Quaternion Inverse() const
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
        const auto cy = Cos(angle.z * 0.5f);
        const auto sy = Sin(angle.z * 0.5f);
        const auto cp = Cos(angle.y * 0.5f);
        const auto sp = Sin(angle.y * 0.5f);
        const auto cr = Cos(angle.x * 0.5f);
        const auto sr = Sin(angle.x * 0.5f);
		Quaternion q;
		q.w = cr * cp * cy + sr * sp * sy;
		q.x = sr * cp * cy - cr * sp * sy;
		q.y = cr * sp * cy + sr * cp * sy;
		q.z = cr * cp * sy - sr * sp * cy;
		//double c1 = Cos(angle.z);
		//double s1 = Sin(angle.z);
		//double c2 = Cos(angle.y);
		//double s2 = Sin(angle.y);
		//double c3 = Cos(angle.x);
		//double s3 = Sin(angle.x);
		//q.w = sqrtf(1.0 + c1 * c2 + c1 * c3 - s1 * s2 * s3 + c2 * c3) / 2.0;
		//double w4 = (4.0 * q.w);
		//q.x = (c2 * s3 + c1 * s3 + s1 * s2 * c3) / w4;
		//q.y = (s1 * c2 + s1 * c3 + c1 * s2 * s3) / w4;
		//q.z = (-s1 * s3 + c1 * s2 * c3 + s2) / w4;
		return q;
        //return Quaternion(
        //    cy * cp * cr + sy * sp * sr,
        //    cy * cp * sr - sy * sp * cr,
        //    sy * cp * sr + cy * sp * cr,
        //    sy * cp * cr - cy * sp * sr
        //);
    }

	static EulerAngles ToEulerAngles(Quaternion& q) {
		EulerAngles angles;
		// roll (x-axis rotation)
		double sinr_cosp = 2 * (q.w * q.x + q.y * q.z);
		double cosr_cosp = 1 - 2 * (q.x * q.x + q.y * q.y);
		angles.x = -Atan2(sinr_cosp, cosr_cosp);

		// pitch (y-axis rotation)
		double sinp = 2 * (q.w * q.y - q.z * q.x);
		if (std::abs(sinp) >= 1)
			angles.y = radian_t(std::copysign(M_PI / 2, sinp)); // use 90 degrees if out of range
		else
			angles.y = Asin(sinp);

		// yaw (z-axis rotation)
		double siny_cosp = 2 * (q.w * q.z + q.x * q.y);
		double cosy_cosp = 1 - 2 * (q.y * q.y + q.z * q.z);
		angles.z = -Atan2(siny_cosp, cosy_cosp);
		//double sqw = q.w * q.w;
		//double sqx = q.x * q.x;
		//double sqy = q.y * q.y;
		//double sqz = q.z * q.z;
		//double unit = sqx + sqy + sqz + sqw; // if normalised is one, otherwise is correction factor
		//double test = q.x * q.y + q.z * q.w;
		//if (test > 0.499 * unit) { // singularity at north pole
		//	angles.z = 2 * Atan2(q.x, q.w);
		//	angles.y = radian_t(M_PI / 2);
		//	angles.x = radian_t(0);
		//	return angles;
		//}
		//if (test < -0.499 * unit) { // singularity at south pole
		//	angles.z = -2 * Atan2(q.x, q.w);
		//	angles.y = radian_t(-M_PI / 2);
		//	angles.x = radian_t(0);
		//	return angles;
		//}
		//angles.z = Atan2(2 * q.y * q.w - 2 * q.x * q.z, sqx - sqy - sqz + sqw);
		//angles.y = Asin(2 * test / unit);
		//angles.x = Atan2(2 * q.x * q.w - 2 * q.y * q.z, -sqx + sqy - sqz + sqw);

		return angles;
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

	Quaternion operator*(const float rhs) const
	{
		return Quaternion(
			x * rhs,
			y * rhs,
			z * rhs,
			w * rhs);
	}

	Vec3f operator*(const Vec3f& rhs) const
	{
		const Vec3f u(x, y, z);
		return 2.0f * Vec3f::Dot(u, rhs) * u
		       + (w * w - Vec3f::Dot(u, u)) * rhs
		       + 2.0f * w * Vec3f::Cross(u, rhs);
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