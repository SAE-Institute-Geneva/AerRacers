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

 Author : Canas Simon
 Co-Author : Floreau Luca
 Date : 21.07.2020
---------------------------------------------------------- */

#include "mathematics/vector.h"
#include "mathematics/aabb.h"
#include <graphics/camera.h>

namespace neko::aer {
struct Plane {
    Plane() {};

    explicit Plane(const neko::Vec3f& newPoint, const neko::Vec3f& newNormal) : point(newPoint), normal(newNormal)
    {
    }

    explicit Plane(
        const neko::Vec3f& pointA,
        const neko::Vec3f& pointB,
        const neko::Vec3f& pointC) //TODO clean + optimize
    {
        point = pointA;
        normal = CalculateNormalFrom(pointA, pointB, pointC);
    }

    [[nodiscard]] static neko::Vec3f CalculateNormalFrom(
        const neko::Vec3f& pointA,
        const neko::Vec3f& pointB,
        const neko::Vec3f& pointC)
    {
        neko::Vec3f vecA = pointA - pointB;
        neko::Vec3f vecB = pointC - pointB;
        return neko::Vec3f::Cross(vecA, vecB).Normalized();
    }

    [[nodiscard]] float Distance(const neko::Vec3f& targetPoint) const
    {
        return neko::Vec3f::Dot(targetPoint - point, normal);
    }

    neko::Vec3f normal; //Towards inside
    neko::Vec3f point;
};

class Frustum {
public:
    Frustum() {};

    explicit Frustum(const neko::Camera3D& camera);

    bool Contains(const neko::Vec3f& point);

    bool Contains(const neko::Aabb3d& aabb);

    float cameraRecoil = 0.0f;
private:
    enum Planes {
        NEAR_ = 0,
        FAR_ = 1,
        RIGHT = 2,
        LEFT = 3,
        TOP = 4,
        BOTTOM = 5
    };
    std::array<Plane, 6> planes_;
};
}
