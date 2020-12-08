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
#include "collider.h"
#include "PxPhysicsAPI.h"
#include "mathematics/transform.h"

namespace neko::physics {
class PhysicsEngine;
struct RigidDynamicData
{
    RigidDynamicData() = default;
    ~RigidDynamicData() = default;
    Vec3f linearVelocity = Vec3f::zero;
    Vec3f angularVelocity = Vec3f::zero;
    float linearDamping = 0.0f;
    float angularDamping = 0.0f;
    float mass = 1.0f;
    bool useGravity = false;
    bool isKinematic = false;
    Vec3<bool> freezePosition = Vec3<bool>(false);
    Vec3<bool> freezeRotation = Vec3<bool>(true);
};


struct RigidStaticData
{
    RigidStaticData() = default;
    ~RigidStaticData() = default;
};

struct RigidActor {
public:
    physx::PxShape* GetPxShape() const { return shape_; }
    physx::PxMaterial* GetPxMaterial() const { return material_; }
protected:
    physx::PxMaterial* InitMaterial(physx::PxPhysics* physics, const PhysicsMaterial& material) const;
    physx::PxShape* InitBoxShape(physx::PxPhysics* physics, physx::PxMaterial* material, const BoxCollider& boxCollider) const;
    physx::PxShape* InitSphereShape(physx::PxPhysics* physics, physx::PxMaterial* material, const SphereCollider& sphereCollider) const;
    physx::PxMaterial* material_ = nullptr;
    physx::PxShape* shape_ = nullptr;
};

struct RigidStatic : RigidActor {
    void Init(physx::PxPhysics* physics, const physics::RigidStaticData& rigidStatic, const SphereCollider& shape, const  Vec3f& position, const  EulerAngles& eulerAngle);
    void Init(physx::PxPhysics* physics, const physics::RigidStaticData& rigidStatic, const BoxCollider& shape, const  Vec3f& position, const  EulerAngles& eulerAngle);
    physx::PxRigidStatic* GetPxRigidStatic() const { return rigidActor_; }
private:
    void InitRigidStatic(physx::PxPhysics* physics, const physics::RigidStaticData& rigidStatic) const;
    physx::PxRigidStatic* rigidActor_ = nullptr;

};

struct RigidDynamic : RigidActor {
public:
    void Init(physx::PxPhysics* physics, const physics::RigidDynamicData& rigidDynamic, const SphereCollider& shape, const  Vec3f& position, const  EulerAngles& eulerAngle);
    void Init(physx::PxPhysics* physics, const physics::RigidDynamicData& rigidDynamic, const BoxCollider& shape, const  Vec3f& position, const  EulerAngles& eulerAngle);
    void AddForceAtPosition(const Vec3f& force, const Vec3f& position) const;
    void AddForce(const Vec3f& force);
    void SetRigidDynamicData(const physics::RigidDynamicData& rigidDynamic);
    void FixedUpdate(seconds dt);
    physics::RigidDynamicData GetRigidDynamicData() const;
    physx::PxRigidDynamic* GetPxRigidDynamic() const { return rigidActor_; }
private:
    physx::PxRigidDynamic* rigidActor_ = nullptr;
    RigidDynamicData rigidDynamicData_;
};

class RigidStaticManager :
    public ComponentManager<RigidStatic, EntityMask(ComponentType::RIGID_STATIC)>
{
public:

    explicit RigidStaticManager(
        EntityManager& entityManager,
        Transform3dManager& transform3dManager);

    void FixedUpdate(seconds dt);
protected:
    Transform3dManager& transform3dManager_;
};

class RigidDynamicManager :
    public ComponentManager<RigidDynamic, EntityMask(ComponentType::RIGID_DYNAMIC)>
{
public:

    explicit RigidDynamicManager(
        EntityManager& entityManager,
        Transform3dManager& transform3dManager);

    void FixedUpdate(seconds dt);
protected:
    Transform3dManager& transform3dManager_;
};
}
