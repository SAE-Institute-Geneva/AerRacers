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
#include "mathematics/matrix.h"
#include "mathematics/quaternion.h"


namespace neko::Transform3d
{
Mat4f Translate(const Mat4f& transform, const Vec3f& translation);
Mat4f TranslationMatrixFrom(const Vec3f& translation);

Mat4f Scale(const Mat4f& transform, const Vec3f& scale);
Mat4f ScalingMatrixFrom(const Vec3f& scale);

Mat4f Rotate(const Mat4f& transform, degree_t angle, const Vec3f& axis);
Mat4f Rotate(const Mat4f& transform, radian_t angle, const Vec3f& axis);
Mat4f Rotate(const Mat4f& transform, const Quaternion& quaternion);
Mat4f Rotate(const Mat4f& transform, const EulerAngles& eulerAngles);
Mat4f RotationMatrixFrom(degree_t angle, const Vec3f& axis);
Mat4f RotationMatrixFrom(radian_t angle, const Vec3f& axis);
Mat4f RotationMatrixFrom(const EulerAngles& eulerAngles);
Mat4f RotationMatrixFrom(const Quaternion& quaternion);

Mat4f Transform(const Vec3f& pos, const Quaternion& rot, const Vec3f& scale);
Mat4f Transform(const Vec3f& pos, const EulerAngles& rot, const Vec3f& scale);

Mat4f Perspective(radian_t fovY, float aspect, float near, float far);
Mat4f Orthographic(float left, float right, float bottom, float top, float nearPlane = 0.0f, float farPlane = 100.0f);
}
 