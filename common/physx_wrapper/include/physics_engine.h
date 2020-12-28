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


#include "collider.h"
#include "PxPhysicsAPI.h"
#include "raycast.h"
#include "rigidbody.h"
#include "engine/transform.h"
#include "mathematics/vector.h"

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


class PhysicsSimulationEventCallback : public physx::PxSimulationEventCallback
{
public:
    virtual void onConstraintBreak(
        physx::PxConstraintInfo* constraints,
        physx::PxU32 count) override
    {
        logDebug("onConstraintBreak");
    }
    virtual void onWake(physx::PxActor** actors, physx::PxU32 count) override
    {
        logDebug("onWake");
    }
    virtual void onSleep(physx::PxActor** actors, physx::PxU32 count) override
    {
        logDebug("onSleep");
    }
    virtual void onContact(
        const physx::PxContactPairHeader& pairHeader,
        const physx::PxContactPair* pairs,
        physx::PxU32 nbPairs) override
    {
        for (physx::PxU32 i = 0; i < nbPairs; i++)
        {
            const physx::PxContactPair& cp = pairs[i];

            if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND)
            {
                logDebug("onContact");
                onCollisionAction.Execute(pairHeader);
            }
        }
    }
    virtual void onTrigger(physx::PxTriggerPair* pairs, physx::PxU32 count) override
    {
        logDebug("onTrigger");
        onTriggerAction.Execute(pairs);
    }
    virtual void onAdvance(
        const physx::PxRigidBody* const* bodyBuffer,
        const physx::PxTransform* poseBuffer,
        const physx::PxU32 count) override
    {
        logDebug("onAdvance");
    }

    Action<const physx::PxContactPairHeader&> onCollisionAction;
    Action<physx::PxTriggerPair*> onTriggerAction;
};

class FixedUpdateInterface
{
public:
    virtual void FixedUpdate(seconds dt) = 0;
};

class OnCollisionInterface
{
public:
    virtual void OnCollision(const physx::PxContactPairHeader& pairHeader) = 0;
};


class OnTriggerInterface
{
public:
    virtual void OnTrigger(physx::PxTriggerPair* pairs) = 0;
};

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

class RigidDynamicViewer : public DrawImGuiInterface, public FixedUpdateInterface
{
public:
    explicit RigidDynamicViewer(EntityManager& entityManager, PhysicsEngine& physicsEngine);
    void DrawImGui() override;
    void SetSelectedEntity(Entity selectedEntity);
    void FixedUpdate(seconds dt) override;
    void Update(seconds dt);
protected:
    Entity selectedEntity_ = INVALID_ENTITY;
    EntityManager& entityManager_;
    RigidDynamicData rigidDynamicData_;
    DynamicData dynamicData_;
    PhysicsEngine& physicsEngine_;

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

    void AddRigidStatic(Entity entity, const RigidStaticData& body, BoxColliderData& shape);
    void AddRigidStatic(Entity entity, const RigidStaticData& body, const SphereColliderData& shape);

    void AddRigidDynamic(Entity entity, const RigidDynamicData& body, BoxColliderData& shape);
    void AddRigidDynamic(Entity entity, const RigidDynamicData& body, const SphereColliderData& shape);

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
        Vec3f origin,
        Vec3f direction,
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
    bool physicRunning_ = false;

    PhysicsSimulationEventCallback eventCallback_;

    EntityManager& entityManager_;
    neko::Transform3dManager& transform3d_;

    RigidDynamicManager rigidDynamicManager_;
    RigidStaticManager rigidStaticManager_;

    Action<seconds> fixedUpdateAction_;
};

}
