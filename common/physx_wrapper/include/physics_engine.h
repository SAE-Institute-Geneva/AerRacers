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
#include "raycast.h"
#include "rigidbody.h"
#include "engine/entity.h"
#include "engine/transform.h"

namespace neko::physics {

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
    RigidStaticManager& GetRididStaticManager() { return rigidStaticManager_; }
    RigidDynamicManager& GetRigidDynamicManager() { return rigidDynamicManager_; }

    void AddRigidStatic(Entity entity, RigidStaticData& body);

    void AddRigidDynamic(Entity entity, RigidDynamicData& body);

    void AddForceAtPosition(Entity entity, const Vec3f& force, const Vec3f& position) const;
    void AddForce(Entity entity, const Vec3f& force) const;

    [[nodiscard]] const RigidDynamicData& GetRigidDynamicData(Entity entity) const;
    void SetRigidDynamicData(Entity entity, const RigidDynamicData& rigidDynamicData) const;

    [[nodiscard]] const ColliderType& GetColliderType(Entity entity) const;

    [[nodiscard]] const BoxColliderData& GetBoxColliderData(Entity entity) const;
    void SetBoxColliderData(Entity entity, const BoxColliderData& boxColliderData) const;

    [[nodiscard]] const SphereColliderData& GetSphereColliderData(Entity entity) const;
    void SetSphereColliderData(Entity entity, const SphereColliderData& body) const;

    [[nodiscard]] const RigidDynamic& GetRigidDynamic(Entity entity) const;
    void SetRigidDynamic(Entity entity, const RigidDynamic& body);

    [[nodiscard]] const RigidStatic& GetRigidStatic(Entity entity) const;
    void SetRigidStatic(Entity entity, const RigidStatic& body);

    const PxRaycastInfo Raycast(
        const Vec3f& origin,
        const Vec3f& direction,
        float maxDistance) const;

    void RegisterCollisionListener(OnCollisionInterface& collisionInterface);
    void RegisterTriggerListener(OnTriggerInterface& triggerInterface);
    void RegisterFixedUpdateListener(FixedUpdateInterface& fixedUpdateInterface);

    void StartPhysic();
    void StopPhysic();

    bool IsPhysicRunning() const;

private:

    void CreateScene();
    bool Advance(physx::PxReal dt);
    static physx::PxFilterFlags ContactReportFilterShader(
        physx::PxFilterObjectAttributes attributes0,
        physx::PxFilterData filterData0,
        physx::PxFilterObjectAttributes attributes1,
        physx::PxFilterData filterData1,
        physx::PxPairFlags& pairFlags,
        const void* constantBlock,
        physx::PxU32 constantBlockSize);

    physx::PxFoundation* foundation_ = nullptr;
    physx::PxPhysics* physics_ = nullptr;
    physx::PxPvd* pvd_ = nullptr;
    physx::PxPvdTransport* transport_ = nullptr;
    physx::PxCooking* cooking_ = nullptr;
    physx::PxDefaultCpuDispatcher* cpuDispatcher_ = nullptr;
    physx::PxScene* scene_ = nullptr;
    float accumulator_ = 0.0f;
    seconds stepSize_ = seconds(1.0f / 60.0f);
    bool physicsStopped_ = false;
    bool physicRunning_ = false;

    PhysicsSimulationEventCallback eventCallback_;

    EntityManager& entityManager_;
    neko::Transform3dManager& transform3d_;

    RigidDynamicManager rigidDynamicManager_;
    RigidStaticManager rigidStaticManager_;

    Action<seconds> fixedUpdateAction_;
};

}
