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
 Date : 22.11.2020
---------------------------------------------------------- */

#include <engine/component.h>

namespace neko {
    class Transform3dManager;
}

namespace neko::physics
{
    struct PhysicsMaterial {
        float dynamicFriction = 0.6f;
        float staticFriction = 0.6f;
        float bouciness = 0.6f;
    };

    struct Collider
    {
        Collider() = default;
        ~Collider() = default;
        bool isTrigger = false;
        PhysicsMaterial material = PhysicsMaterial();
        Vec3f offset = Vec3f::zero;
    };

    struct SphereCollider : public Collider
    {
        float radius = 1.0f;
    };

    struct BoxCollider : public Collider
    {
    public:
        Vec3f size = Vec3f::one;
    };
} 