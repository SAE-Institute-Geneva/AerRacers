#pragma once
/* ----------------------------------------------------
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

 Author : Simon Canas
 Co-Author :
 Date :
---------------------------------------------------------- */
#include "mathematics/vector.h"

namespace neko
{
constexpr std::uint8_t kMaxLights = 32;
struct Light
{
	Vec3f ambient   = Vec3f(0.2f);
	Vec3f diffuse   = Vec3f::one;
	float specular  = 1.0f;
	float intensity = 1.0f;
};

struct BasicLight : Light
{
	BasicLight() = default;
	explicit BasicLight(const Vec3f& pos) : position(pos) {}

	Vec3f position = Vec3f::zero;
};

struct PointLight : BasicLight
{
	PointLight() = default;
	explicit PointLight(const Vec3f& pos, const float rad) : BasicLight(pos), radius(rad) {}

	float radius = 10.0f;
};

struct DirectionalLight : Light
{
	DirectionalLight() = default;
	explicit DirectionalLight(const Vec3f& dir) : direction(dir) {}

	Vec3f direction = Vec3f::down;
};

struct SpotLight : PointLight
{
	SpotLight() = default;
	SpotLight(const Vec3f& pos,
		const Vec3f& dir,
		const float blend,
		const degree_t angle,
		const float radius)
	   : PointLight(pos, radius), direction(dir), blend(blend), angle(angle)
	{}

	Vec3f direction = Vec3f::down;
	float blend     = 0.8f;
	degree_t angle  = degree_t(25.0f);
};
}    // namespace neko
