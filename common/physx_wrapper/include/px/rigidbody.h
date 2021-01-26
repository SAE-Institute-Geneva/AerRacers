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
#include "physics_callbacks.h"
#include "PxPhysicsAPI.h"
#include "engine/transform.h"

namespace neko::physics {
class PhysicsEngine;

struct DynamicData {
    Vec3f linearVelocity = Vec3f::zero;
    Vec3f angularVelocity = Vec3f::zero;
};

struct RigidActorData
{
    PhysicsMaterial material;
    ColliderType colliderType = ColliderType::INVALID;
    BoxColliderData boxColliderData;
    SphereColliderData sphereColliderData;
};

struct RigidDynamicData : public RigidActorData
{
    RigidDynamicData() = default;
    ~RigidDynamicData() = default;
    float linearDamping = 0.0f;
    float angularDamping = 0.0f;
    float mass = 1.0f;
    bool useGravity = true;
    bool isKinematic = false;
    Vec3<bool> freezePosition = Vec3<bool>(false);
    Vec3<bool> freezeRotation = Vec3<bool>(false);
};


struct RigidStaticData : public RigidActorData
{
    RigidStaticData() = default;
    ~RigidStaticData() = default;;
};


struct RigidActor {
public:
    physx::PxShape* GetPxShape() const { return shape_; }
    physx::PxMaterial* GetPxMaterial() const { return material_; }
    ColliderType GetColliderType() const;
    SphereColliderData GetSphereColliderData() const;
    BoxColliderData GetBoxColliderData() const;
    PhysicsMaterial GetMaterial() const;
    void SetSphereColliderData(const SphereColliderData& sphereColliderData) const;
    void SetBoxColliderData(const BoxColliderData& boxColliderData) const;
    void SetMaterial(const PhysicsMaterial& physicsMaterial) const;
protected:
    physx::PxMaterial* InitMaterial(physx::PxPhysics* physics, const PhysicsMaterial& material) const;
    physx::PxShape* InitBoxShape(physx::PxPhysics* physics, physx::PxMaterial* material, const BoxColliderData& boxCollider) const;
    physx::PxShape* InitSphereShape(physx::PxPhysics* physics, physx::PxMaterial* material, const SphereColliderData& sphereCollider) const;
    physx::PxMaterial* material_ = nullptr;
    physx::PxShape* shape_ = nullptr;
};

class RigidActorViewer{
public:
    json GetJsonFromRigidActor( const RigidActorData& rigidActorData) const;
    RigidActorData GetRigidActorFromJson( const json&);
    RigidActorData DrawImGuiRigidActor(const RigidActorData& rigidActorData);
};

struct RigidStatic : RigidActor {
    void Init(physx::PxPhysics* physics, const physics::RigidStaticData& rigidStatic, const  Vec3f& position, const  EulerAngles& eulerAngle);
    physx::PxRigidStatic* GetPxRigidStatic() const { return rigidActor_; }
    void SetRigidStaticData(const RigidStaticData& rigidStaticData) const;
    RigidStaticData GetRigidStaticData() const;
private:
    physx::PxRigidStatic* rigidActor_ = nullptr;
};

struct RigidDynamic : RigidActor {
public:
    void Init(physx::PxPhysics* physics, const physics::RigidDynamicData& rigidDynamic, const  Vec3f& position, const  EulerAngles& eulerAngle);
    void AddForceAtPosition(const Vec3f& force, const Vec3f& position) const;
    void AddForce(const Vec3f& force) const;
    void SetRigidDynamicData(const physics::RigidDynamicData& rigidDynamicData) const;
    void FixedUpdate(seconds dt);
    physics::RigidDynamicData GetRigidDynamicData() const;
    physics::DynamicData GetDynamicData() const;
    physx::PxRigidDynamic* GetPxRigidDynamic() const { return rigidActor_; }
private:
    physx::PxRigidDynamic* rigidActor_ = nullptr;
};

class RigidStaticManager :
    public ComponentManager<RigidStatic, EntityMask(ComponentType::RIGID_STATIC)>,
     public FixedUpdateInterface
{
public:

    explicit RigidStaticManager(
        EntityManager& entityManager,
        Transform3dManager& transform3dManager,
        PhysicsEngine& physicsEngine);

    void FixedUpdate(seconds dt);

    void AddRigidStatic(Entity entity, RigidStaticData& body);

protected:
    Transform3dManager& transform3dManager_;
    PhysicsEngine& physicsEngine_;
};

class RigidStaticViewer : public ComponentViewer, public FixedUpdateInterface, public RigidActorViewer
{
public:
    explicit RigidStaticViewer(Transform3dManager& transform3dManager, EntityManager& entityManager,
        PhysicsEngine& physicsEngine,
        RigidStaticManager& rigidStaticManager);
    void SetSelectedEntity(Entity selectedEntity);
    void FixedUpdate(seconds dt) override;

    json GetJsonFromComponent(Entity) const override;
    void SetComponentFromJson(Entity, const json&) override;
    void DrawImGui(Entity) override;
protected:
    Entity selectedEntity_ = INVALID_ENTITY;
    Transform3dManager& transform3dManager_;
    RigidStaticData rigidStaticData_;
    PhysicsEngine& physicsEngine_;
    RigidStaticManager& rigidStaticManager_;
};

class RigidDynamicManager :
    public ComponentManager<RigidDynamic, EntityMask(ComponentType::RIGID_DYNAMIC)>,
    public FixedUpdateInterface
{
public:

    explicit RigidDynamicManager(
        EntityManager& entityManager,
        Transform3dManager& transform3dManager,
        PhysicsEngine& physicsEngine);

    void FixedUpdate(seconds dt);
    void AddRigidDynamic(Entity entity, RigidDynamicData& body);

    void AddForceAtPosition(Entity entity, const Vec3f& force, const Vec3f& position) const;
    void AddForce(Entity entity, const Vec3f& force) const;
    void SetLinearVelocity(Entity entity, const Vec3f& linearVelocity) const;
    void SetAngularVelocity(Entity entity, const Vec3f& angularVelocity) const;

    [[nodiscard]] const RigidDynamicData& GetRigidDynamicData(Entity entity) const;
    void SetRigidDynamicData(Entity entity, const RigidDynamicData& rigidDynamicData) const;

    //[[nodiscard]] const ColliderType& GetColliderType(Entity entity) const;

    //[[nodiscard]] const BoxColliderData& GetBoxColliderData(Entity entity) const;
    //void SetBoxColliderData(Entity entity, const BoxColliderData& boxColliderData) const;

    //[[nodiscard]] const SphereColliderData& GetSphereColliderData(Entity entity) const;
    //void SetSphereColliderData(Entity entity, const SphereColliderData& body) const;

    //[[nodiscard]] const RigidDynamic& GetRigidDynamic(Entity entity) const;
    //void SetRigidDynamic(Entity entity, const RigidDynamic& body);

protected:
    Transform3dManager& transform3dManager_;
    PhysicsEngine& physicsEngine_;
};


class RigidDynamicViewer final : public ComponentViewer, public FixedUpdateInterface, public RigidActorViewer
{
public:
    explicit RigidDynamicViewer(Transform3dManager& transform3dManager, EntityManager & entityManager,
                                PhysicsEngine& physicsEngine,
                                RigidDynamicManager& rigidDynamicManager);
    void SetSelectedEntity(Entity selectedEntity);
    void FixedUpdate(seconds dt) override;
    json GetJsonFromComponent(Entity) const override;
    void SetComponentFromJson(Entity, const json&) override;
    void DrawImGui(Entity) override;
protected:
    Entity selectedEntity_ = INVALID_ENTITY;
    Entity lastSelectedEntity_ = INVALID_ENTITY;
    Transform3dManager& transform3dManager_;
    PhysicsEngine& physicsEngine_;
    DynamicData dynamicData_;
    RigidDynamicData rigidDynamicData_;
    RigidDynamicManager& rigidDynamicManager_;

};


}
