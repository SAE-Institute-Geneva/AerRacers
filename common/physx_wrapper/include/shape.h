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

#include "PxPhysicsAPI.h"
#include "physics/collider.h"

namespace neko::physics::px {
class PhysicsEngine;
struct PhysicsShape{
public:
    PhysicsShape() = default;
    virtual ~PhysicsShape() = default;
    virtual void Init(physx::PxPhysics* physics) = 0;
    physx::PxShape* GetPxShape() const { return shape_; }
    physx::PxMaterial* GetPxMaterial() const { return material_; }
protected:
    physx::PxMaterial* material_ = nullptr;

    physx::PxShape* shape_ = nullptr;
};

struct SpherePhysicsShape : public physics::SphereCollider, public PhysicsShape {
public:
    SpherePhysicsShape() = default;
    ~SpherePhysicsShape() = default;
    void Init(physx::PxPhysics* physics) override;
};

struct BoxPhysicsShape : public physics::BoxCollider, public PhysicsShape {
public:
    BoxPhysicsShape() = default;
    ~BoxPhysicsShape() = default;
    void Init(physx::PxPhysics* physics) override;
};

class BoxPhysicsShapeManager :
    public ComponentManager<BoxPhysicsShape, EntityMask(ComponentType::BOX_COLLIDER)>
{
public:
    explicit BoxPhysicsShapeManager(
        EntityManager& entityManager);

    void FixedUpdate(seconds dt);
protected:
};

class SpherePhysicsShapeManager :
    public ComponentManager<SpherePhysicsShape, EntityMask(ComponentType::SPHERE_COLLIDER)>
{
public:
    explicit SpherePhysicsShapeManager(
        EntityManager& entityManager);

    void FixedUpdate(seconds dt);
protected:
};
}

