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

#include <chrono>
#include <iostream>

#include "PxPhysicsAPI.h"
#include "rigidbody.h"
#include "engine/transform.h"
#include "mathematics/vector.h"
#include "mathematics/transform.h"

namespace neko::physics::px {

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

class PxAllocatorCallback
{
public:
    virtual ~PxAllocatorCallback() {}
    virtual void* allocate(size_t size, const char* typeName, const char* filename,
        int line) = 0;
    virtual void deallocate(void* ptr) = 0;
};

class UserErrorCallback : public physx::PxErrorCallback
{
public:
    virtual void reportError(
        physx::PxErrorCode::Enum code, const char* message, const char* file,
        int line)
    {
        // error processing implementation
    }
};

class PhysicsEngine
{
public:

    explicit PhysicsEngine(
        EntityManager& entityManager,
        Transform3dManager& transform3d);

    void Start();

    void Update(float dt);

    void Destroy();
    physx::PxPhysics* GetPhysx();
    physx::PxScene* GetScene();

    void AddRigidStatic(Entity entity, RigidStatic& body);
    [[nodiscard]] const RigidStatic& GetRigidStatic(Entity entity) const;
    void SetRigidStatic(Entity entity, const RigidStatic& body);

    void AddRigidDynamic(Entity entity, RigidDynamic& body);
    [[nodiscard]] const RigidDynamic& GetRigidDynamic(Entity entity) const;
    void SetRigidDynamic(Entity entity, const RigidDynamic& body);

    void AddBoxPhysicsShape(Entity entity, BoxPhysicsShape& body);
    [[nodiscard]] const BoxPhysicsShape& GetBoxPhysicsShape(Entity entity) const;
    void SetBoxPhysicsShape(Entity entity, const BoxPhysicsShape& body);

    void AddSpherePhysicsShape(Entity entity, SpherePhysicsShape& body);
    [[nodiscard]] const SpherePhysicsShape& GetSpherePhysicsShape(Entity entity) const;
    void SetSpherePhysicsShape(Entity entity, const SpherePhysicsShape& body);


private:
    void CreateScene();
    bool Advance(physx::PxReal dt);

    physx::PxFoundation* foundation_ = nullptr;
    physx::PxPhysics* physics_ = nullptr;
    physx::PxPvd* pvd_ = nullptr;
    physx::PxPvdTransport* transport = nullptr;
    physx::PxCooking* cooking_ = nullptr;
    physx::PxDefaultCpuDispatcher* cpuDispatcher_ = nullptr;
    physx::PxScene* scene_ = nullptr;
    float accumulator_ = 0.0f;
    seconds stepSize_ = seconds(1.0f / 60.0f);

    EntityManager& entityManager_;
    neko::Transform3dManager& transform3d_;

    BoxPhysicsShapeManager boxPhysicsShapeManager_;
    SpherePhysicsShapeManager spherePhysicsShapeManager_;
    RigidDynamicManager rigidDynamicManager_;
    RigidStaticManager rigidStaticManager_;


};

}
