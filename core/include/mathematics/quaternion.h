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
#include <engine/log.h>

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

    static EulerAngles ToEulerAngles(const Quaternion& q)
    {
        EulerAngles angles;
        // roll (x-axis rotation)
        float sinr_cosp = 2.0f * (q.w * q.x + q.y * q.z);
        float cosr_cosp = 1.0f - 2.0f * (q.x * q.x + q.y * q.y);
        angles.x         = Atan2(sinr_cosp, cosr_cosp);

        // pitch (y-axis rotation)
        float sinp = 2.0f * (q.w * q.y - q.z * q.x);
        if (std::abs(sinp) >= 1)
            angles.y = radian_t(std::copysign(M_PI / 2, sinp));    // use 90 degrees if out of range
        else
            angles.y = Asin(sinp);

        // yaw (z-axis rotation)
        float siny_cosp = 2.0f * (q.w * q.z + q.x * q.y);
        float cosy_cosp = 1.0f - 2.0f * (q.y * q.y + q.z * q.z);
        angles.z         = Atan2(siny_cosp, cosy_cosp);

        return angles;
    }
 //   /*
	//Returns a rotation that rotates z degrees around the z axis,
	//x degrees around the x axis, and y degrees around the y axis; 
	//applied in that order
	//*/
 //   static Quaternion FromEuler(const EulerAngles& angle)
 //   {
 //       const auto cy = Cos(angle.x * 0.5f);
 //       const auto sy = Sin(angle.x * 0.5f);
 //       const auto cp = Cos(angle.y * 0.5f);
 //       const auto sp = Sin(angle.y * 0.5f);
 //       const auto cr = Cos(angle.z * 0.5f);
 //       const auto sr = Sin(angle.z * 0.5f);

 //       return Quaternion(cy * cp * cr + sy * sp * sr,
 //           cy * cp * sr - sy * sp * cr,
 //           sy * cp * sr + cy * sp * cr,
 //           sy * cp * cr - cy * sp * sr);
 //   }

  //  static EulerAngles ToEulerAngles(Quaternion& q)
  //  {
  //      /*
		//reference
		//http://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles
		//http://www.euclideanspace.com/maths/geometry/rotations/conversions/quaternionToEuler/
		//*/

  //      EulerAngles eulerAngles;

  //      // Threshold for the singularities found at the north/south poles.
  //      const float SINGULARITY_THRESHOLD = 0.4999995f;

  //      const auto sqw = q.w * q.w;
  //      const auto sqx = q.x * q.x;
  //      const auto sqy = q.y * q.y;
  //      const auto sqz = q.z * q.z;
  //      const auto unit =
  //          sqx + sqy + sqz + sqw;    // if normalised is one, otherwise is correction factor
  //      const auto singularityTest = q.x * q.y + q.w * q.z;

  //      if (singularityTest > SINGULARITY_THRESHOLD * unit)
  //      {
  //          eulerAngles.z = 2.0f * Atan2(q.x, q.w);
  //          eulerAngles.y = radian_t(PI / 2.0f);
  //          eulerAngles.x = radian_t(0);
  //      }
  //      else if (singularityTest < -SINGULARITY_THRESHOLD * unit)
  //      {
  //          eulerAngles.z = -2.0f * Atan2(q.x, q.w);
  //          eulerAngles.y = radian_t(-PI / 2.0f);
  //          eulerAngles.x = radian_t(0);
  //      }
  //      else
  //      {
  //          eulerAngles.z = Atan2(2 * (q.w * q.y - q.x * q.z), sqw + sqx - sqy - sqz);
  //          eulerAngles.y = Asin(2 * singularityTest / unit);
  //          eulerAngles.x = Atan2(2 * (q.w * q.x - q.y * q.z), sqw - sqx + sqy - sqz);
  //      }

  //      return eulerAngles;
  //  }

	static Quaternion FromRotationMatrix(const Mat4f& mat)
	{
		Quaternion q;
		float trace = mat[0][0] + mat[1][1] + mat[2][2]; // I removed + 1.0f; see discussion with Ethan
		if (trace > 0) {// I changed M_EPSILON to 0
			float s = 0.5f / sqrtf(trace + 1.0f);
			q.w = 0.25f / s;
			q.x = (mat[2][1] - mat[1][2]) * s;
			q.y = (mat[0][2] - mat[2][0]) * s;
			q.z = (mat[1][0] - mat[0][1]) * s;
		}
		else {
			if (mat[0][0] > mat[1][1] && mat[0][0] > mat[2][2]) {
				float s = 2.0f * sqrtf(1.0f + mat[0][0] - mat[1][1] - mat[2][2]);
				q.w = (mat[2][1] - mat[1][2]) / s;
				q.x = 0.25f * s;
				q.y = (mat[0][1] + mat[1][0]) / s;
				q.z = (mat[0][2] + mat[2][0]) / s;
			}
			else if (mat[1][1] > mat[2][2]) {
				float s = 2.0f * sqrtf(1.0f + mat[1][1] - mat[0][0] - mat[2][2]);
				q.w = (mat[0][2] - mat[2][0]) / s;
				q.x = (mat[0][1] + mat[1][0]) / s;
				q.y = 0.25f * s;
				q.z = (mat[1][2] + mat[2][1]) / s;
			}
			else {
				float s = 2.0f * sqrtf(1.0f + mat[2][2] - mat[0][0] - mat[1][1]);
				q.w = (mat[1][0] - mat[0][1]) / s;
				q.x = (mat[0][2] + mat[2][0]) / s;
				q.y = (mat[1][2] + mat[2][1]) / s;
				q.z = 0.25f * s;
			}
		}
		q.w *= -1.0f;
		return q;
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

    /**
	 * \brief Interpolates between q1 and q2 by t and normalizes the result afterwards.
	 * \param t is clamped to the range [0, 1].
	 */
	//from https://stackoverflow.com/questions/46156903/how-to-lerp-between-two-quaternions
	static Quaternion Lerp(const Quaternion& q1, const Quaternion& q2, float t) {
		//const float dot = Quaternion::Dot(q1, q2);
		//Quaternion newQ2 = q2;
		//if (dot < 0.0f) {
		//	newQ2 = newQ2 * -1.0f;
		//}
	    return Normalized(Quaternion(
			q1.x + (q2.x - q1.x) * t,
            q1.y + (q2.y - q1.y) * t,
            q1.z + (q2.z - q1.z) * t,
            q1.w + (q2.w - q1.w) * t));
	}
    /**
	 * \brief Creates a rotation with the specified forward and upwards directions.
	 * \param lookAt The direction to look in.
	 * \param upDirection The vector that defines in which direction up is.
	 */
	static Quaternion LookRotation(const Vec3f& lookAt, const Vec3f& upDirection) {
		if(lookAt.Magnitude() == 0 || upDirection.Magnitude() == 0) {
			logDebug("Look rotation viewing vector is zero");
			return Quaternion(0, 0, 0, 1);
		}
		if (Vec3f::Cross(lookAt, upDirection) == Vec3f::zero) {
			logDebug("LookRotation vectors are colinear");
			return Quaternion(0, 0, 0, 1);
		}

		////from https://answers.unity.com/questions/467614/what-is-the-source-code-of-quaternionlookrotation.html
	 //   
		//
		//Vec3f vector = lookAt.Normalized();
		//Vec3f vector2 = Vec3f::Cross(upDirection, vector).Normalized();
		//Vec3f vector3 = Vec3f::Cross(vector, vector2);
		//float m00 = vector2.x;
		//float m01 = vector2.y;
		//float m02 = vector2.z;
		//float m10 = vector3.x;
		//float m11 = vector3.y;
		//float m12 = vector3.z;
		//float m20 = vector.x;
		//float m21 = vector.y;
		//float m22 = vector.z;


		//float num8 = (m00 + m11) + m22;
		//Quaternion quaternion;
		//if (num8 > 0.0f)
		//{
		//	float num = Sqrt(num8 + 1.0f);
		//	quaternion.w = num * 0.5f;
		//	num = 0.5f / num;
		//	quaternion.x = (m12 - m21) * num;
		//	quaternion.y = (m20 - m02) * num;
		//	quaternion.z = (m01 - m10) * num;
		//	return quaternion;
		//}
		//if ((m00 >= m11) && (m00 >= m22))
		//{
		//	float num7 = Sqrt(((1.0f + m00) - m11) - m22);
		//	float num4 = 0.5f / num7;
		//	quaternion.x = 0.5f * num7;
		//	quaternion.y = (m01 + m10) * num4;
		//	quaternion.z = (m02 + m20) * num4;
		//	quaternion.w = (m12 - m21) * num4;
		//	return quaternion;
		//}
		//if (m11 > m22)
		//{
		//	float num6 = Sqrt(((1.0f + m11) - m00) - m22);
		//	float num3 = 0.5f / num6;
		//	quaternion.x = (m10 + m01) * num3;
		//	quaternion.y = 0.5f * num6;
		//	quaternion.z = (m21 + m12) * num3;
		//	quaternion.w = (m20 - m02) * num3;
		//	return quaternion;
		//}
		//float num5 = Sqrt(((1.0f + m22) - m00) - m11);
		//float num2 = 0.5f / num5;
		//quaternion.x = (m20 + m02) * num2;
		//quaternion.y = (m21 + m12) * num2;
		//quaternion.z = 0.5f * num5;
		//quaternion.w = (m01 - m10) * num2;
		//return quaternion;
		
		
		//from https://gamedev.net/forums/topic/613595-quaternion-lookrotationlookat-up/4876373/
		 Vec3f forward = lookAt;
		 Vec3f up = upDirection;
		Vec3f::OrthoNormalize(forward, up);
		Vec3f right = Vec3f::Cross(up, forward);
		Mat3f mat({ right , up, forward });
		mat = mat.Transpose();
		Quaternion ret;
		ret.w = Sqrt(1.0f + mat[0][0] + mat[1][1] + mat[2][2]) * 0.5f;
		float w4_recip = 1.0f / (4.0f * ret.w);
		ret.x = (mat[2][1] - mat[1][2]) * w4_recip;
		ret.y = (mat[0][2] - mat[2][0]) * w4_recip;
		ret.z = (mat[1][0] - mat[0][1]) * w4_recip;
		return Quaternion::Normalized(ret);
		
		
	}
};
} 