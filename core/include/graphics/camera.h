#pragma once
#include "mathematics/matrix.h"
#include "utils/service_locator.h"

namespace neko
{
struct Camera
{
	Vec3f position         = Vec3f::back;
	Vec3f reverseDirection = Vec3f::back;

	float nearPlane = 0.1f;
	float farPlane  = 10'000.0f;

	virtual void Init();

	void WorldLookAt(const Vec3f& target);

	[[nodiscard]] Vec3f GetRight() const;
	[[nodiscard]] Vec3f GetUp() const;

	[[nodiscard]] EulerAngles GetRotation() const;
	[[nodiscard]] Mat4f GetRotationMat() const;

	[[nodiscard]] Mat4f GenerateViewMatrix() const;

	void SetDirectionFromEuler(const EulerAngles& angles);

	void Rotate(const EulerAngles& angles);

	[[nodiscard]] float GetAspect() const { return aspect_; }
	void SetAspect(const float aspect) { aspect_ = aspect; }
	void SetAspect(const float width, const float height) { aspect_ = width / height; }

	[[nodiscard]] virtual Mat4f GenerateProjectionMatrix() const {}

protected:
	float aspect_ = 1.0f;
};

struct Camera2D : Camera
{
    float size = 5.0f;
    [[nodiscard]] Mat4f GenerateProjectionMatrix() const override;
};

struct Camera3D : Camera
{
    degree_t fovY = degree_t(45.0f);

    [[nodiscard]] Mat4f GenerateProjectionMatrix() const override;
    [[nodiscard]] radian_t GetFovX() const;
};

/**
 * \brief Exclusively used for the CameraLocator, please do not use!
 */
struct NullCamera : Camera
{
    [[nodiscard]] Mat4f GenerateProjectionMatrix() const override;
};

using CameraLocator = Locator<Camera, NullCamera>;

}  