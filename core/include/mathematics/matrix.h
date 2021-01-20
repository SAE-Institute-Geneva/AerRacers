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

#include "basic.h"
#include "engine/assert.h"
#include "engine/intrinsincs.h"
#include "mathematics/angle.h"
#include "mathematics/trigo.h"
#include "mathematics/vector.h"

namespace neko
{
template<typename T>
class Mat2;

template<typename T>
class Mat3;

template<typename T>
class Mat4;

//-----------------------------------------------------------------------------
// Mat2
//-----------------------------------------------------------------------------
template<typename T>
class Mat2
{
public:
	static const Mat2<T> Identity;
	static const Mat2<T> Zero;

	//-----------------------------------------------------------------------------
	// Constructors
	//-----------------------------------------------------------------------------
	Mat2() : columns_(Identity.columns_) {}

	Mat2(const Mat2& m) noexcept : columns_(m.columns_) {}

	explicit Mat2(const Mat3<T>& mat)
	{
		columns_[0] = Vec2<T>(mat[0]);
		columns_[1] = Vec2<T>(mat[1]);
	}

	explicit Mat2(const Mat4<T>& mat)
	{
		columns_[0] = Vec2<T>(mat[0]);
		columns_[1] = Vec2<T>(mat[1]);
	}

	explicit Mat2(const std::array<Vec2<T>, 2>& v) : columns_(v) {}

	//-----------------------------------------------------------------------------
	// Operators
	//-----------------------------------------------------------------------------
	Mat2& operator=(Mat2 m)
	{
		columns_ = m.columns_;
		return *this;
	}

	const T& operator()(size_t row, size_t column) const
	{
		return columns_[column][row];
	}

	T& operator()(size_t row, size_t column)
	{
		return columns_[column][row];
	}

	const Vec2<T>& operator[](size_t column) const
	{
		return columns_[column];
	}

	Vec2<T>& operator[](size_t column)
	{
		return columns_[column];
	}

	//-----------------------------------------------------------------------------
	// Formulas
	//-----------------------------------------------------------------------------
	[[nodiscard]] float Determinant() const
	{
		return columns_[0][0] * columns_[1][1] - columns_[0][1] * columns_[1][0];
	}

private:
	std::array<Vec2<T>, 2> columns_;
};
using Mat2f = Mat2<float>;

template<typename T>
inline const Mat2<T> Mat2<T>::Identity = Mat2<T>(
	std::array<Vec2<T>, 1> {Vec2<T>(1, 0), Vec2<T>(0, 1)});

template<typename T>
inline const Mat2<T> Mat2<T>::Zero = Mat2<T>(
	std::array<Vec2<T>, 2> {Vec2<T>::zero, Vec2<T>::zero});

//-----------------------------------------------------------------------------
// Mat3
//-----------------------------------------------------------------------------
template<typename T>
class Mat3
{
public:
	const static Mat3<T> Identity;
	const static Mat3<T> Zero;

	Mat3() : columns_(Identity.columns_) {}

	Mat3& operator=(Mat3 m)
	{
		columns_ = m.columns_;
		return *this;
	}

	Mat3(const Mat3& m) noexcept : columns_(m.columns_) {}

	explicit Mat3(const std::array<Vec3<T>, 3>& v) : columns_(v) {}

	explicit Mat3(const Mat4<T>& mat)
	{
		columns_[0] = Vec3<T>(mat[0]);
		columns_[1] = Vec3<T>(mat[1]);
		columns_[2] = Vec3<T>(mat[2]);
	}

	//-----------------------------------------------------------------------------
	// Operators
	//-----------------------------------------------------------------------------
	const T& operator()(size_t row, size_t column) const
	{
		return columns_[column][row];
	}

	T& operator()(size_t row, size_t column)
	{
		return columns_[column][row];
	}

	const Vec3 <T>& operator[](size_t column) const
	{
		return columns_[column];
	}

	Vec3 <T>& operator[](size_t column)
	{
		return columns_[column];
	}

	Mat3<T> operator+(const Mat3<T>& rhs) const
	{
		std::array<Vec3<T>, 3> v;
		for (int i = 0; i < 4; i++)
		{
			v[i] = columns_[i] + rhs.columns_[i];
		}
		return Mat3<T>(v);
	}

	Mat3<T> operator*(const Mat3<T>& rhs) const noexcept
	{ return Multiply(rhs); }

	Mat3<T> operator*(float rhs) const noexcept
	{
		Mat3<T> m;
		for (int col = 0; col < 3; col++)
		{
			for (int row = 0; row < 3; row++)
			{
				m[col][row] = columns_[col][row] * rhs;
			}
		}
		return m;
	}

	//-----------------------------------------------------------------------------
	// Formulas
	//-----------------------------------------------------------------------------
	[[nodiscard]] Mat3<T> Multiply(const Mat3<T>& rhs) const noexcept
	{
		Mat3<T> v;
		for (int col = 0; col < 3; col++)
		{
			for (int row = 0; row < 3; row++)
			{
				v[col][row] = 0;
				for (int i = 0; i < 3; i++)
					v[col][row] += columns_[i][row] * rhs[col][i];
			}
		}

		return v;
	}

	[[nodiscard]] float Determinant() const
	{
		return
			columns_[0][0] * columns_[1][1] * columns_[2][2] +
			columns_[1][0] * columns_[2][1] * columns_[0][2] +
			columns_[2][0] * columns_[0][1] * columns_[1][2] -
			columns_[2][0] * columns_[1][1] * columns_[0][2] -
			columns_[0][0] * columns_[2][1] * columns_[1][2] -
			columns_[1][0] * columns_[0][1] * columns_[2][2];
	}

	[[nodiscard]] Mat3<T> Inverse() const
	{
		const float determinant = Determinant();
		if (determinant == 0.0f)
			return Zero;

		Mat3<T> inverse = Zero;
		for (int col = 0; col < 3; ++col)
		{
			for (int row = 0; row < 3; ++row)
			{
				const Mat2 sub(std::array<Vec2<T>, 2>
					               {
						               Vec2<T>(columns_[(col + 1) % 3][(row + 1) % 3],
						                       columns_[(col + 1) % 3][(row + 2) % 3]),
						               Vec2<T>(columns_[(col + 2) % 3][(row + 1) % 3],
						                       columns_[(col + 2) % 3][(row + 2) % 3])
					               });

				inverse[col][row] = ((col + row) % 2 == 0 ? 1.0f : -1.0f) * sub.Determinant();
			}
		}

		inverse = inverse.Transpose();
		inverse = inverse * (1 / determinant);
		return inverse;
	}

	[[nodiscard]] Mat3<T> Transpose() const
	{
		std::array<Vec3<T>, 3> v;
		for (int column = 0; column < 3; column++)
		{
			for (int row = 0; row < 3; row++)
			{
				v[row][column] = columns_[column][row];
			}
		}
		return Mat3<T>(v);
	}

	static EulerAngles EulerFromRotationMatrix(const Mat3<T>& mat)
	{
		if (mat[2][0] == 1.0f)
		{
			const degree_t theta = -degree_t(90.0f);
			const degree_t psi = Atan2(-mat[0][1], -mat[0][2]);

			return {theta, psi, degree_t(0.0f)};
		}
		else if (mat[2][0] == -1.0f)
		{
			const degree_t theta = degree_t(90.0f);
			const degree_t psi = Atan2(mat[0][1], mat[0][2]);

			return {theta, psi, degree_t(0.0f)};
		}
		else
		{
			const degree_t theta = -Asin(mat[2][0]);
			const degree_t psi = Atan2(mat[2][1] / Cos(theta), mat[2][2] / Cos(theta));
			const degree_t phi = Atan2(mat[1][0] / Cos(theta), mat[0][0] / Cos(theta));

			return {psi, theta, phi};
		}
	}

private:
	std::array<Vec3<T>, 3> columns_; //row vector
};
//-----------------------------------------------------------------------------
// Mat3 Definitions
//-----------------------------------------------------------------------------
using Mat3f = Mat3<float>;

template<typename T>
inline const Mat3<T> Mat3<T>::Identity = Mat3<T>(
	std::array<Vec3<T>, 3>
		{
			Vec3<T>(1, 0, 0),
			Vec3<T>(0, 1, 0),
			Vec3<T>(0, 0, 1)
		});

template<typename T>
inline const Mat3<T> Mat3<T>::Zero = Mat3<T>(
	std::array<Vec3<T>, 3>
		{
			Vec3<T>::zero,
			Vec3<T>::zero,
			Vec3<T>::zero
		});

//-----------------------------------------------------------------------------
// Mat4
//-----------------------------------------------------------------------------
template<typename T>
class alignas(4 * sizeof(T)) Mat4
{
public:
	const static Mat4<T> Identity;
	const static Mat4<T> Zero;

	Mat4() : columns_(Identity.columns_) {}
	Mat4(const Mat4& m) noexcept : columns_(m.columns_) {}
	explicit Mat4(const std::array<Vec4<T>, 4>& v) : columns_(v) {}
	explicit Mat4(const Mat3<T>& m) noexcept : columns_(Identity.columns_)
	{
		for (int col = 0; col < 3; col++)
			columns_[col] = Vec4f(m[col]);
	}

	//-----------------------------------------------------------------------------
	// Operators
	//-----------------------------------------------------------------------------
	const T& operator()(size_t row, size_t column) const { return columns_[column][row]; }
	T& operator()(size_t row, size_t column) { return columns_[column][row]; }

	const Vec4<T>& operator[](size_t column) const { return columns_[column]; }
	Vec4<T>& operator[](size_t column) { return columns_[column]; }

	Mat4& operator=(Mat4 m)
	{
		columns_ = m.columns_;
		return *this;
	}

	Mat4<T> operator+(const Mat4<T> & rhs) const
	{
		std::array<Vec4<T>, 4> v;
		for (int i = 0; i < 4; i++)
			v[i] = columns_[i] + rhs.columns_[i];
		return Mat4<T>(v);
	}

	Mat4<T> operator-(const Mat4<T> & rhs) const
	{
		std::array<Vec4<T>, 4> v;
		for (int i = 0; i < 4; i++)
			v[i] = columns_[i] - rhs.columns_[i];
		return Mat4<T>(v);
	}

	Mat4<T> operator*(const Mat4<T>& rhs) const noexcept
	{ return MultiplyIntrinsics(rhs); }

	Vec4<T> operator*(const Vec4<T>& rhs) const
	{
		Vec4<T> v;
		for (int column = 0; column < 4; column++)
		{
			v[column] = 0;
			for (int row = 0; row < 4; row++)
				v[column] += columns_[column][row] * rhs[row];
		}
		return Vec4<T>(v);
	}

	Mat4<T> operator*(float rhs) const noexcept
	{
		Mat4<T> m;
		for(int col = 0; col < 4; col++)
			m[col] = columns_[col] * rhs;
		return m;
	}

	Mat4<T> operator/(float rhs) const noexcept
	{
		Mat4<T> m;
		for(int col = 0; col < 4; col++)
			m[col] = columns_[col] / rhs;
		return m;
	}

	friend std::ostream& operator<<(std::ostream & os, const Mat4<T> & m)
	{
		for (int row = 0; row < 4; row++)
		{
			os << "Mat4[" << row << "]|";
			for (int col = 0; col < 4; col++)
			{
				os << m[col][row];
				if (col < 3) os << ", ";
			}
			os << "|\n";
		}
		return os;
	}

	//-----------------------------------------------------------------------------
	// Formulas
	//-----------------------------------------------------------------------------
	[[nodiscard]] Mat4<T> Transpose() const
	{
		std::array<Vec4<T>, 4> v;
		for (int column = 0; column < 4; column++)
			for (int row = 0; row < 4; row++)
				v[row][column] = columns_[column][row];
		return Mat4<T>(v);
	}

	[[nodiscard]] Mat4<T> MultiplyNaive(const Mat4<T> & rhs) const noexcept
	{
		std::array<Vec4<T>, 4> v;
		for (int column = 0; column < 4; column++)
		{
			for (int row = 0; row < 4; row++)
			{
				v[column][row] = 0;
				for (int i = 0; i < 4; i++)
					v[column][row] += columns_[i][row] * rhs[column][i];
			}
		}
		return Mat4<T>(v);
	}

	[[nodiscard]] Mat4<T> MultiplyTranspose(const Mat4<T>& rhs) const noexcept
	{
		const auto lhsT = this->Transpose();
		std::array<Vec4<T>, 4> v;
		for (int column = 0; column < 4; column++)
			for (int row = 0; row < 4; row++)
				v[column][row] = Vec4<T>::Dot(lhsT[row], Vec4<T>(rhs.columns_[column]));
		return Mat4<T>(v);
	}

	static T MatrixDifference(const Mat4<T>& rhs, const Mat4<T>& lhs)
	{
		T result = 0;
		for (int column = 0; column < 4; column++)
			for (int row = 0; row < 4; row++)
				result += std::abs(rhs[column][row] - lhs[column][row]);
		return result;
	};

	[[nodiscard]] float Determinant() const
	{
		float result = 0.0f;
		for (int i = 0; i < 4; i++)
		{
			const Mat3<T> m = Mat3<T>(std::array<Vec3<T>, 3>
				                          {
					                          Vec3<T>(&columns_[(i + 1) % 4][1]),
					                          Vec3<T>(&columns_[(i + 2) % 4][1]),
					                          Vec3<T>(&columns_[(i + 3) % 4][1])
				                          });
			result += (i % 2 == 1 ? -1.0f : 1.0f) * columns_[i][0] * m.Determinant();
		}
		return result;
	}

	[[nodiscard]] Mat4 Inverse() const
	{
		Mat4 inverse = Zero;
		float determinant = Determinant();
		if (Equal(determinant, 0.0f))
			return Zero;

		//Calculate the cofactor matrix
		for (int col = 0; col < 4; col++)
		{
			for (int row = 0; row < 4; row++)
			{
				const Mat3 sub(std::array<Vec3<T>, 3>
					               {
						               Vec3f(columns_[(col + 1) % 4][(row + 1) % 4],
						                     columns_[(col + 1) % 4][(row + 2) % 4],
						                     columns_[(col + 1) % 4][(row + 3) % 4]),
						               Vec3f(columns_[(col + 2) % 4][(row + 1) % 4],
						                     columns_[(col + 2) % 4][(row + 2) % 4],
						                     columns_[(col + 2) % 4][(row + 3) % 4]),
						               Vec3f(columns_[(col + 3) % 4][(row + 1) % 4],
						                     columns_[(col + 3) % 4][(row + 2) % 4],
						                     columns_[(col + 3) % 4][(row + 3) % 4])
					               });
				inverse[col][row] = ((col + row) % 2 == 0 ? 1.0f : -1.0f) * sub.Determinant();
			}
		}

		inverse = inverse.Transpose();
		inverse = inverse * (1.0f / determinant);
		return inverse;
	}

	static EulerAngles EulerFromRotationMatrix(const Mat4<T>& mat)
	{
		if (mat[2][0] == 1.0f)
		{
			const degree_t theta = -degree_t(90.0f);
			const degree_t psi = Atan2(-mat[0][1], -mat[0][2]);

			return {theta, psi, degree_t(0.0f)};
		}
		else if (mat[2][0] == -1.0f)
		{
			const degree_t theta = degree_t(90.0f);
			const degree_t psi = Atan2(mat[0][1], mat[0][2]);

			return {theta, psi, degree_t(0.0f)};
		}
		else
		{
			const degree_t theta = -Asin(mat[2][0]);
			const degree_t psi = Atan2(mat[2][1] / Cos(theta), mat[2][2] / Cos(theta));
			const degree_t phi = Atan2(mat[1][0] / Cos(theta), mat[0][0] / Cos(theta));

			return {psi, theta, phi};
		}
	}

	void Decompose(Vec3f& pos, EulerAngles& rot, Vec3f& scale)
	{
		const auto subMat = Mat3f(*this);
		pos = Vec3f(columns_[3]);
		scale = Vec3f(
			subMat[0].Magnitude(),
			subMat[1].Magnitude(),
			subMat[2].Magnitude());

		if (scale != Vec3f::zero)
		{
			const Mat3<T> rotMat = Mat3<T>(std::array<Vec3f, 3> {
				subMat[0] / scale.x,
				subMat[1] / scale.y,
				subMat[2] / scale.z
			}).Transpose();

			rot = Mat3<T>::EulerFromRotationMatrix(rotMat);
		}
		else
		{
			rot = EulerAngles::zero;
		}
	}

	//-----------------------------------------------------------------------------
	// Intrinsics
	//-----------------------------------------------------------------------------
	[[nodiscard]] Mat4<T> MultiplyAoSoA(const Mat4<T>& rhs) const noexcept;
	[[nodiscard]] Mat4<T> MultiplyIntrinsics(const Mat4<T>& rhs) const noexcept;

private:
	std::array<Vec4<T>, 4> columns_;
};
using Mat4f = Mat4<float>;

//-----------------------------------------------------------------------------
// Mat4 Definitions
//-----------------------------------------------------------------------------
template<typename T>
inline const Mat4<T> Mat4<T>::Identity = Mat4<T>(
	std::array<Vec4f, 4>
		{
			Vec4<T>(1, 0, 0, 0),
			Vec4<T>(0, 1, 0, 0),
			Vec4<T>(0, 0, 1, 0),
			Vec4<T>(0, 0, 0, 1)
		});

template<typename T>
inline const Mat4<T> Mat4<T>::Zero = Mat4<T>(
	std::array<Vec4f, 4>
		{
			Vec4<T>::zero,
			Vec4<T>::zero,
			Vec4<T>::zero,
			Vec4<T>::zero
		});

//-----------------------------------------------------------------------------
// Mat4 Intrinsics
//-----------------------------------------------------------------------------
template<typename T>
inline Mat4<T> Mat4<T>::MultiplyAoSoA(const Mat4<T>& rhs) const noexcept
{
	const auto lhsT(Transpose());
	std::array<Vec4f, 4> v;
	for (int column = 0; column < 4; column++)
	{
		for (int row = 0; row < 4; row++)
		{
			const auto result = Vec4f::Dot(lhsT[row], rhs.columns_[column]);
			v[column][row] = result;
		}

	}
	return Mat4f(v);
}

#if defined(__SSE__) && !defined(__ANDROID__)
template<>
inline Mat4f Mat4f::Transpose() const
{
	std::array<Vec4f, 4> v;
	auto xmm3 = _mm_load_ps(&columns_[1][0]);
	auto xmm4 = _mm_load_ps(&columns_[3][0]);
	auto xmm0 = _mm_load_ps(&columns_[0][0]);
	auto xmm1 = _mm_load_ps(&columns_[2][0]);

	auto xmm2 = _mm_shuffle_ps(xmm0, xmm3, 136);
	xmm0 = _mm_shuffle_ps(xmm0, xmm3, 221);
	xmm3 = _mm_shuffle_ps(xmm1, xmm4, 136);
	xmm1 = _mm_shuffle_ps(xmm1, xmm4, 221);
	xmm4 = _mm_shuffle_ps(xmm2, xmm3, 136);
	xmm2 = _mm_shuffle_ps(xmm2, xmm3, 221);
	_mm_store_ps(&v[0][0], xmm4);
	xmm4 = _mm_shuffle_ps(xmm0, xmm1, 136);
	_mm_store_ps(&v[2][0], xmm2);
	xmm0 = _mm_shuffle_ps(xmm0, xmm1, 221);
	_mm_store_ps(&v[1][0], xmm4);
	_mm_store_ps(&v[3][0], xmm0);
	return Mat4f(v);
}

template<>
inline Mat4f Mat4f::MultiplyIntrinsics(const Mat4f& rhs) const noexcept
{
	std::array<Vec4f, 4> v;
	__m128 c1 = _mm_load_ps(&this->columns_[0][0]);
	__m128 c2 = _mm_load_ps(&this->columns_[1][0]);
	__m128 c3 = _mm_load_ps(&this->columns_[2][0]);
	__m128 c4 = _mm_load_ps(&this->columns_[3][0]);
	for (int column = 0; column < 4; column++)
	{
		__m128 x = _mm_load1_ps(&rhs.columns_[column][0]);
		__m128 y = _mm_load1_ps(&rhs.columns_[column][1]);
		__m128 z = _mm_load1_ps(&rhs.columns_[column][2]);
		__m128 w = _mm_load1_ps(&rhs.columns_[column][3]);
		x = _mm_mul_ps(c1, x);
		y = _mm_mul_ps(c2, y);
		z = _mm_mul_ps(c3, z);
		w = _mm_mul_ps(c4, w);

		x = _mm_add_ps(x, y);
		z = _mm_add_ps(z, w);
		x = _mm_add_ps(x, z);
		_mm_store_ps(&v[column][0], x);
	}
	return Mat4f(v);
}
#elif defined(EMSCRIPTEN) || defined(__arm__) || defined(__ANDROID__) || defined(__aarch64__)
template<>
inline Mat4f Mat4f::MultiplyIntrinsincs(const Mat4f& rhs) const noexcept
{
	std::array<Vec4f, 4> v;
	v4f c1 = *(v4f*) (&this->columns_[0][0]);
	v4f c2 = *(v4f*) (&this->columns_[1][0]);
	v4f c3 = *(v4f*) (&this->columns_[2][0]);
	v4f c4 = *(v4f*) (&this->columns_[3][0]);
	for (int column = 0; column < 4; column++)
	{
		float tmp = rhs.columns_[column][0];
		v4f rhsX  = {tmp, tmp, tmp, tmp};
		tmp       = rhs.columns_[column][1];
		v4f rhsY  = {tmp, tmp, tmp, tmp};
		tmp       = rhs.columns_[column][2];
		v4f rhsZ  = {tmp, tmp, tmp, tmp};
		tmp       = rhs.columns_[column][3];
		v4f rhsW  = {tmp, tmp, tmp, tmp};

		v4f x = (c1 * rhsX);
		v4f y = (c2 * rhsY);
		v4f z = (c3 * rhsZ);
		v4f w = (c4 * rhsW);

		x = (x + y);
		z = (z + w);
		x = (x + z);

		*(v4f*) (&v[column][0]) = x;
	}
	return Mat4f(v);
}
#endif
} // namespace neko