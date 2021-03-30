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
#include "utils/json_utility.h"

namespace neko
{
constexpr std::uint8_t kMaxLights = 16;

/// An interface for each light type <br>
/// Is useless on its own
struct Light
{
	alignas(16) Vec3f diffuse   = Vec3f::one;
	alignas(4) float specular  = 1.0f;
	alignas(4) float intensity = 1.0f;
};

/// A basic light with infinite radius
struct BasicLight : Light
{
	BasicLight() = default;
	explicit BasicLight(const Vec3f& pos) : position(pos) {}

	alignas(16) Vec3f position = Vec3f::zero;
};

/// A point light with finite radius
struct PointLight : BasicLight
{
	PointLight() = default;
	explicit PointLight(const Vec3f& pos, const float rad) : BasicLight(pos), radius(rad) {}

	alignas(4) float radius = 10.0f;
};

/// A directional light with infinite range <br>
/// Represents the sun in most use cases
struct DirectionalLight : Light
{
	static DirectionalLight* Instance; // We only want one directional light in the scene

	DirectionalLight() = default;
	explicit DirectionalLight(const Vec3f& dir) : direction(dir) {}

	[[nodiscard]] json ToJson() const
	{
		json out         = json::object();
		out["diffuse"]   = GetJsonFromVector3(diffuse);
		out["ambient"]   = GetJsonFromVector3(ambient);
		out["direction"] = GetJsonFromVector3(direction);

		out["specular"]  = specular;
		out["intensity"] = intensity;
		return out;
	}

	void FromJson(const json& other)
	{
		diffuse   = GetVector3FromJson(other, "diffuse");
		ambient   = GetVector3FromJson(other, "ambient");
		direction = GetVector3FromJson(other, "direction");

		specular  = other["specular"].get<float>();
		intensity = other["intensity"].get<float>();
	}

	alignas(16) Vec3f ambient   = Vec3f::one * 0.5f;
	alignas(16) Vec3f direction = -Vec3f::one;
};
inline DirectionalLight* DirectionalLight::Instance = nullptr;

/// A spot light with finite range <br>
/// Only lights object in a certain area
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

	alignas(16) Vec3f direction = Vec3f::down;
	alignas(4) float blend     = 0.8f;
	degree_t angle  = degree_t(25.0f);
};
}    // namespace neko
