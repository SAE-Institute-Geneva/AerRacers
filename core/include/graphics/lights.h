#pragma once
#include "mathematics/vector.h"

namespace neko
{
struct Light
{
	Vec3f ambient = Vec3f(0.2f);
	Vec3f diffuse = Vec3f::one;
	float specular = 1.0f;
	float intensity = 1.0f;
};

struct BasicLight : Light
{
	BasicLight() = default;
	explicit BasicLight(const Vec3f& pos) : position(pos) {}

	Vec3f position = Vec3f::zero;
};

struct DirectionalLight : Light
{
	DirectionalLight() = default;
	explicit DirectionalLight(const Vec3f& dir) : direction(dir) {}

	Vec3f direction = Vec3f::down;
};

struct PointLight : BasicLight
{
	PointLight() = default;
	explicit PointLight(const Vec3f& pos, const float rad) : BasicLight(pos), radius(rad) {}

	float radius = 10.0f;
};

struct SpotLight : PointLight
{
	SpotLight() = default;
	SpotLight(const Vec3f& pos,
			const Vec3f& dir,
			const float blend,
			const degree_t angle,
			const float radius)
			: PointLight(pos, radius), direction(dir), blend(blend), angle(angle) {}

	Vec3f direction = Vec3f::down;
	float blend = 0.8f;
	degree_t angle  = degree_t(25.0f);
};
}
