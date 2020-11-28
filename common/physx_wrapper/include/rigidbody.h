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
#include "shape.h"
#include "physics/rigidbody.h"
#include "mathematics/transform.h"

namespace neko::physics::px {
class PhysicsEngine;
struct RigidStatic : public physics::RigidStatic {
public:
    void Init(physx::PxPhysics* physics, const PhysicsShape& shape, const Vec3f& position, const  EulerAngles& eulerAngle);
    physx::PxRigidStatic* GetPxRigidStatic() const;
private:
    physx::PxRigidStatic* rigidActor_ = nullptr;

};

struct RigidDynamic : public physics::RigidDynamic {
public:
    void Init(physx::PxPhysics* physics, const PhysicsShape& shape, const  Vec3f& position, const  EulerAngles& eulerAngle);
    physx::PxRigidDynamic* GetPxRigidDynamic() const;
private:
    physx::PxRigidDynamic* rigidActor_ = nullptr;

};

class RigidStaticManager :
    public ComponentManager<RigidStatic, EntityMask(ComponentType::RIGID_STATIC)>
{
public:

    explicit RigidStaticManager(
        EntityManager& entityManager,
        BoxPhysicsShapeManager& boxShapeManager,
        SpherePhysicsShapeManager& circlePhysicsShapeManager,
        Transform3dManager& transform3dManager);

    void FixedUpdate(seconds dt);
protected:
    BoxPhysicsShapeManager& boxShapeManager_;
    SpherePhysicsShapeManager& spherePhysicsShapeManager_;
    Transform3dManager& transform3dManager_;
};

class RigidDynamicManager :
    public ComponentManager<RigidDynamic, EntityMask(ComponentType::RIGID_DYNAMIC)>
{
public:

    explicit RigidDynamicManager(
        EntityManager& entityManager,
        BoxPhysicsShapeManager& boxShapeManager,
        SpherePhysicsShapeManager& circlePhysicsShapeManager,
        Transform3dManager& transform3dManager);

    void FixedUpdate(seconds dt);
    void AddForceAtPosition(Vec3f force, Vec3f position);
    void AddForce(Vec3f force);
protected:
    BoxPhysicsShapeManager& boxShapeManager_;
    SpherePhysicsShapeManager& spherePhysicsShapeManager_;
    Transform3dManager& transform3dManager_;
};
}
