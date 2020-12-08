#include <rigidbody.h>
#include <physics_engine.h>
#include <engine\log.h>
#include "mathematics/transform.h"


namespace neko::physics {
physx::PxMaterial* RigidActor::InitMaterial(
    physx::PxPhysics* physics,
    const PhysicsMaterial& material) const {
    return physics->createMaterial(material.staticFriction, material.dynamicFriction, material.bouciness);}

physx::PxShape* RigidActor::InitBoxShape(
    physx::PxPhysics* physics,
    physx::PxMaterial* material,
    const BoxCollider& boxCollider) const {
    return physics->createShape(physx::PxBoxGeometry(boxCollider.size.x, boxCollider.size.y, boxCollider.size.z), *material);
}

physx::PxShape* RigidActor::InitSphereShape(
    physx::PxPhysics* physics,
    physx::PxMaterial* material,
    const SphereCollider& sphereCollider) const {
    return physics->createShape(physx::PxSphereGeometry(sphereCollider.radius), *material);
}

void RigidStatic::Init(physx::PxPhysics* physics, const RigidStaticData& rigidStatic, const SphereCollider& shape, const  Vec3f& position, const  EulerAngles& eulerAngle)
{
    physx::PxTransform transform = physx::PxTransform(ConvertToPxVec(position), ConvertToPxQuat(Quaternion::FromEuler(eulerAngle)));
    rigidActor_ = physics->createRigidStatic(transform);
    if (!rigidActor_)
        std::cerr << "create actor failed!";
    material_ = InitMaterial(physics, shape.material);
    if (!material_)
        std::cerr << "createMaterial failed!";
    shape_ = InitSphereShape(physics, material_, shape);
    if (!shape_)
        std::cerr << "createShpae failed!";
    rigidActor_->attachShape(*shape_);
    if (!shape_)
        std::cerr << "create shape failed!";
    InitRigidStatic(physics, rigidStatic);
}

void RigidStatic::Init(physx::PxPhysics* physics, const RigidStaticData& rigidStatic, const BoxCollider& shape, const  Vec3f& position, const  EulerAngles& eulerAngle)
{
    physx::PxTransform transform = physx::PxTransform(ConvertToPxVec(position), ConvertToPxQuat(Quaternion::FromEuler(eulerAngle)));
    rigidActor_ = physics->createRigidStatic(transform);
    if (!rigidActor_)
        std::cerr << "create actor failed!";
    material_ = InitMaterial(physics, shape.material);
    if (!material_)
        std::cerr << "createMaterial failed!";
    shape_ = InitBoxShape(physics, material_, shape);
    if (!shape_)
        std::cerr << "createShpae failed!";
    rigidActor_->attachShape(*shape_);
    if (!shape_)
        std::cerr << "create shape failed!";
    InitRigidStatic(physics, rigidStatic);
}

void RigidStatic::InitRigidStatic(
    physx::PxPhysics* physics,
    const RigidStaticData& rigidStatic) const
{
    
}

void RigidDynamic::Init(physx::PxPhysics* physics, const RigidDynamicData& rigidDynamic, const SphereCollider& shape, const  Vec3f& position, const  EulerAngles& eulerAngle)
{
    physx::PxTransform transform = physx::PxTransform(ConvertToPxVec(position), ConvertToPxQuat(Quaternion::FromEuler(eulerAngle)));
    rigidActor_ = physics->createRigidDynamic(transform);
    if (!rigidActor_)
        std::cerr << "create actor failed!";
    material_ = InitMaterial(physics, shape.material);
    if (!material_)
        std::cerr << "createMaterial failed!";
    shape_ = InitSphereShape(physics, material_, shape);
    if (!shape_)
        std::cerr << "createShpae failed!";
    rigidActor_->attachShape(*shape_);
    if (!shape_)
        std::cerr << "create shape failed!";
    SetRigidDynamicData(rigidDynamic);
}


void RigidDynamic::Init(physx::PxPhysics* physics, const RigidDynamicData& rigidDynamic, const BoxCollider& shape, const  Vec3f& position, const  EulerAngles& eulerAngle)
{
    physx::PxTransform transform = physx::PxTransform(ConvertToPxVec(position), ConvertToPxQuat(Quaternion::FromEuler(eulerAngle)));
    rigidActor_ = physics->createRigidDynamic(transform);
    if (!rigidActor_)
        std::cerr << "create actor failed!";
    material_ = InitMaterial(physics, shape.material);
    if (!material_)
        std::cerr << "createMaterial failed!";
    shape_ = InitBoxShape(physics, material_, shape);
    if (!shape_)
        std::cerr << "createShpae failed!";
    rigidActor_->attachShape(*shape_);
    if (!shape_)
        std::cerr << "create shape failed!";
    SetRigidDynamicData(rigidDynamic);
}

void RigidDynamic::SetRigidDynamicData(
    const RigidDynamicData& rigidDynamic) {
    rigidDynamicData_ = rigidDynamic;
}

void RigidDynamic::FixedUpdate(seconds dt)
{
    //rigidActor_->setLinearVelocity(ConvertToPxVec(rigidDynamicData_.linearVelocity));
    //rigidActor_->setAngularVelocity(ConvertToPxVec(rigidDynamicData_.angularVelocity));
    rigidActor_->setLinearDamping(rigidDynamicData_.linearDamping);
    rigidActor_->setAngularDamping(rigidDynamicData_.angularDamping);
    rigidActor_->setMass(rigidDynamicData_.mass);
    rigidActor_->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !rigidDynamicData_.useGravity);
    rigidActor_->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, rigidDynamicData_.isKinematic);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, rigidDynamicData_.freezeRotation.x);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, rigidDynamicData_.freezeRotation.y);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, rigidDynamicData_.freezeRotation.z);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, rigidDynamicData_.freezePosition.x);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, rigidDynamicData_.freezePosition.y);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, rigidDynamicData_.freezePosition.z);
    rigidDynamicData_.linearVelocity = ConvertFromPxVec(rigidActor_->getLinearVelocity());
    rigidDynamicData_.angularVelocity = ConvertFromPxVec(rigidActor_->getAngularVelocity());
    rigidDynamicData_.linearDamping = rigidActor_->getLinearDamping();
    rigidDynamicData_.angularDamping = rigidActor_->getAngularDamping();
    rigidDynamicData_.mass = rigidActor_->getMass();
    rigidDynamicData_.useGravity = (rigidActor_->getActorFlags() & physx::PxActorFlags(physx::PxActorFlag::eDISABLE_GRAVITY)) != physx::PxActorFlags(physx::PxActorFlag::eDISABLE_GRAVITY);
    rigidDynamicData_.isKinematic = (rigidActor_->getRigidBodyFlags() & physx::PxRigidBodyFlags(physx::PxRigidBodyFlag::eKINEMATIC)) == physx::PxRigidBodyFlags(physx::PxRigidBodyFlag::eKINEMATIC);
    rigidDynamicData_.freezeRotation = {
        (rigidActor_->getRigidDynamicLockFlags() & physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X)) == physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X),
        (rigidActor_->getRigidDynamicLockFlags() & physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y)) == physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y),
        (rigidActor_->getRigidDynamicLockFlags() & physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z)) == physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z) };
    rigidDynamicData_.freezePosition = {
        (rigidActor_->getRigidDynamicLockFlags() & physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X)) == physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X),
        (rigidActor_->getRigidDynamicLockFlags() & physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y)) == physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y),
        (rigidActor_->getRigidDynamicLockFlags() & physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z)) == physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z) };
}

physics::RigidDynamicData RigidDynamic::GetRigidDynamicData() const
{
    return rigidDynamicData_;
}


void RigidDynamic::AddForceAtPosition(const Vec3f& force, const Vec3f& position) const
{
    physx::PxRigidBodyExt::addForceAtLocalPos(*rigidActor_, ConvertToPxVec(force), ConvertToPxVec(position));
}

void RigidDynamic::AddForce(const Vec3f& force)
{
    rigidActor_->addForce(ConvertToPxVec(force));

}

RigidStaticManager::RigidStaticManager(
    EntityManager& entityManager,
    Transform3dManager& transform3dManager)
    : ComponentManager<RigidStatic, EntityMask(ComponentType::RIGID_STATIC)>(entityManager),
      transform3dManager_(transform3dManager) {}

void RigidStaticManager::FixedUpdate(seconds dt)
{
    
}

RigidDynamicManager::RigidDynamicManager(
    EntityManager& entityManager,
    Transform3dManager& transform3dManager)
    : ComponentManager<RigidDynamic, EntityMask(ComponentType::RIGID_DYNAMIC)>(entityManager),
      transform3dManager_(transform3dManager) {}

void RigidDynamicManager::FixedUpdate(seconds dt)
{
    for (neko::Entity entity = 0.0f; entity < entityManager_.get().GetEntitiesSize(); entity++)
    {
        if (!entityManager_.get().HasComponent(entity, neko::EntityMask(neko::ComponentType::RIGID_DYNAMIC)))
        {
            continue;
        }
        physx::PxTransform transform;
        transform = physx::PxShapeExt::getGlobalPose(*GetComponent(entity).GetPxShape(), *GetComponent(entity).GetPxRigidDynamic());
        transform3dManager_.SetPosition(entity, ConvertFromPxVec(transform.p));
        transform3dManager_.SetRotation(entity, Quaternion::ToEulerAngles(ConvertFromPxQuat(transform.q)));
        RigidDynamic rigidDynamic = GetComponent(entity);
        rigidDynamic.FixedUpdate(dt);
        SetComponent(entity, rigidDynamic);
    }
}
}
