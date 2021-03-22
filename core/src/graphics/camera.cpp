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
#include "graphics/camera.h"


#include "engine/assert.h"
#include "engine/engine.h"

namespace neko
{
//-----------------------------------------------------------------------------
// Camera
//-----------------------------------------------------------------------------
void Camera::Init()
{
	const auto& config = BasicEngine::GetInstance()->GetConfig();
	SetAspect(config.windowSize.x, config.windowSize.y);

	CameraLocator::provide(this);
}

void Camera::WorldLookAt(const Vec3f& target)
{
	const Vec3f direction = position - target;
	reverseDirection      = direction.Normalized();
}

Vec3f Camera::GetRight() const
{
	if (reverseDirection.x == 0.0f && reverseDirection.z == 0.0f)
		return Vec3f::Cross(Vec3f::forward, reverseDirection).Normalized();
	return Vec3f::Cross(Vec3f::up, reverseDirection).Normalized();
}

Vec3f Camera::GetUp() const
{
	const Vec3f right = GetRight();
	return Vec3f::Cross(reverseDirection, right).Normalized();
}

EulerAngles Camera::GetRotation() const
{
	const auto inverseView = GenerateViewMatrix().Inverse();
	return EulerAngles(Atan2(inverseView[1][2], inverseView[2][2]),
		Atan2(-inverseView[0][2],
			sqrtf(inverseView[1][2] * inverseView[1][2] + inverseView[2][2] * inverseView[2][2])),
		Atan2(inverseView[0][1], inverseView[0][0]));
}

Mat4f Camera::GetRotationMat() const
{
	const Vec3f right = GetRight();
	const Vec3f up    = GetUp();
	return Mat4f(std::array<Vec4f, 4> {Vec4f(right.x, up.x, reverseDirection.x, 0),
		Vec4f(right.y, up.y, reverseDirection.y, 0),
		Vec4f(right.z, up.z, reverseDirection.z, 0),
		Vec4f(0, 0, 0, 1)});
}

Mat4f Camera::GenerateViewMatrix() const
{
	const Mat4f rotation    = GetRotationMat();
	const Mat4f translation = Transform3d::Translate(Mat4f::Identity, -position);
	return rotation * translation;
}

void Camera::SetDirectionFromEuler(const EulerAngles& angles)
{
	const Quaternion q = Quaternion::FromEuler(angles);
	reverseDirection   = Vec3f(Transform3d::RotationMatrixFrom(q) * Vec4f(0, 0, 1, 0));
}

void Camera::Rotate(const EulerAngles& angles)
{
	const Quaternion pitch    = Quaternion::AngleAxis(angles.x, GetRight());
	const Quaternion yaw      = Quaternion::AngleAxis(angles.y, GetUp());
	const Quaternion roll     = Quaternion::AngleAxis(angles.z, reverseDirection);
	const Quaternion rotation = pitch * yaw * roll;
	reverseDirection = Vec3f(Transform3d::RotationMatrixFrom(rotation) * Vec4f(reverseDirection));
}

//-----------------------------------------------------------------------------
// Camera2D
//-----------------------------------------------------------------------------
Mat4f Camera2D::GenerateProjectionMatrix() const
{
	const float top    = size;
	const float bottom = -top;
	const float right  = size * aspect_;
	const float left   = -right;

	return Transform3d::Orthographic(left, right, bottom, top, nearPlane, farPlane);
}

//-----------------------------------------------------------------------------
// Camera3D
//-----------------------------------------------------------------------------
Mat4f Camera3D::GenerateProjectionMatrix() const
{
	return Transform3d::Perspective(fovY, aspect_, nearPlane, farPlane);
}

radian_t Camera3D::GetFovX() const { return 2.0f * Atan(Tan(fovY * 0.5f) * aspect_); }

//-----------------------------------------------------------------------------
// NullCamera
//-----------------------------------------------------------------------------
neko::Mat4f NullCamera::GenerateProjectionMatrix() const
{
	neko_assert(false, "[Error] No camera defined in CameraLocator!");
}
}    // namespace neko