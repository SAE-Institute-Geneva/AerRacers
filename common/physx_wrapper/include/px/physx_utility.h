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

 Author : Floreau Luca
 Co-Author :
 Date : 28.12.2020
---------------------------------------------------------- */


namespace neko::physics {

static inline physx::PxVec2 ConvertToPxVec(const Vec2f& vec)
{
    return physx::PxVec2(vec.x, vec.y);
}

static inline physx::PxVec3 ConvertToPxVec(const Vec3f& vec)
{
    return physx::PxVec3(vec.x, vec.y, vec.z);
}

static inline physx::PxVec4 ConvertToPxVec(const Vec4f& vec)
{
    return physx::PxVec4(vec.x, vec.y, vec.z, vec.w);
}

static inline physx::PxQuat ConvertToPxQuat(const Quaternion& quat)
{
    return physx::PxQuat(quat.x, quat.y, quat.z, quat.w);
}

static inline Vec2f ConvertFromPxVec(const physx::PxVec2& vec)
{
    return Vec2f(vec.x, vec.y);
}

static inline Vec3f ConvertFromPxVec(const physx::PxVec3& vec)
{
    return Vec3f(vec.x, vec.y, vec.z);
}

static inline Vec4f ConvertFromPxVec(const physx::PxVec4& vec)
{
    return Vec4f(vec.x, vec.y, vec.z, vec.w);
}

static inline Quaternion ConvertFromPxQuat(const physx::PxQuat& quat)
{
    return Quaternion(quat.x, quat.y, quat.z, quat.w);
}

}
