#include <rigidbody.h>
#include <physics_engine.h>
#include <engine\log.h>
#include "mathematics/transform.h"


namespace neko::physics {
ColliderType RigidActor::GetType() const
{
    if (shape_ == nullptr) {
        return ColliderType::INVALID;
    }

    switch(shape_->getGeometryType()) {
    case physx::PxGeometryType::eSPHERE:
        return ColliderType::SPHERE;
        break;
    case physx::PxGeometryType::ePLANE:
        return ColliderType::INVALID;
        break;
    case physx::PxGeometryType::eCAPSULE:
        return ColliderType::INVALID;
        break;
    case physx::PxGeometryType::eBOX:
        return ColliderType::BOX;
        break;
    case physx::PxGeometryType::eCONVEXMESH:
        return ColliderType::INVALID;
        break;
    case physx::PxGeometryType::eTRIANGLEMESH:
        return ColliderType::INVALID;
        break;
    case physx::PxGeometryType::eHEIGHTFIELD:
        return ColliderType::INVALID;
        break;
    case physx::PxGeometryType::eGEOMETRY_COUNT:
        return ColliderType::INVALID;
        break;
    case physx::PxGeometryType::eINVALID:
        return ColliderType::INVALID;
        break;
    default:
        return ColliderType::INVALID;;
    }
}

SphereColliderData RigidActor::GetSphereColliderData() const
{
    SphereColliderData sphereColliderData;
    sphereColliderData.offset = ConvertFromPxVec(shape_->getLocalPose().p);
    sphereColliderData.radius = shape_->getGeometry().sphere().radius;
    sphereColliderData.isTrigger = (shape_->getFlags() & physx::PxShapeFlags(physx::PxShapeFlag::eTRIGGER_SHAPE)) == physx::PxShapeFlags(physx::PxShapeFlag::eTRIGGER_SHAPE);
    sphereColliderData.material.bouciness = material_->getRestitution();
    sphereColliderData.material.staticFriction = material_->getStaticFriction();
    sphereColliderData.material.dynamicFriction = material_->getDynamicFriction();
    return sphereColliderData;
}

BoxColliderData RigidActor::GetBoxColliderData() const
{
    BoxColliderData boxColliderData;
    boxColliderData.offset = ConvertFromPxVec(shape_->getLocalPose().p);
    boxColliderData.size = ConvertFromPxVec(shape_->getGeometry().box().halfExtents * 2.0f);
    boxColliderData.isTrigger = (shape_->getFlags() & physx::PxShapeFlags(physx::PxShapeFlag::eTRIGGER_SHAPE)) == physx::PxShapeFlags(physx::PxShapeFlag::eTRIGGER_SHAPE);
    boxColliderData.material.bouciness = material_->getRestitution();
    boxColliderData.material.staticFriction = material_->getStaticFriction();
    boxColliderData.material.dynamicFriction = material_->getDynamicFriction();
    return boxColliderData;
}

void RigidActor::SetSphereColliderData(
    const physics::SphereColliderData& sphereColliderData) const
{
    physx::PxTransform transform = shape_->getLocalPose();
    transform.p = ConvertToPxVec(sphereColliderData.offset);
    shape_->setLocalPose(transform);
    physx::PxSphereGeometry geometry = shape_->getGeometry().sphere();
    geometry.radius = sphereColliderData.radius;
    shape_->setGeometry(geometry);
    shape_->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, sphereColliderData.isTrigger);
    material_->setRestitution(sphereColliderData.material.bouciness);
    material_->setStaticFriction(sphereColliderData.material.staticFriction);
    material_->setDynamicFriction(sphereColliderData.material.dynamicFriction);
}

void RigidActor::SetBoxColliderData(
    const physics::BoxColliderData& boxColliderData) const
{
    physx::PxTransform transform = shape_->getLocalPose();
    transform.p = ConvertToPxVec(boxColliderData.offset);
    shape_->setLocalPose(transform);
    physx::PxBoxGeometry geometry = shape_->getGeometry().box();
    geometry.halfExtents = ConvertToPxVec(boxColliderData.size)/2.0f;
    shape_->setGeometry(geometry);
    if (boxColliderData.isTrigger) {
        shape_->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
        shape_->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, boxColliderData.isTrigger);
    } else {
        shape_->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, boxColliderData.isTrigger);
        shape_->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
    }
    material_->setRestitution(boxColliderData.material.bouciness);
    material_->setStaticFriction(boxColliderData.material.staticFriction);
    material_->setDynamicFriction(boxColliderData.material.dynamicFriction);
}

physx::PxMaterial* RigidActor::InitMaterial(
    physx::PxPhysics* physics,
    const PhysicsMaterial& material) const {
    return physics->createMaterial(material.staticFriction, material.dynamicFriction, material.bouciness);}

physx::PxShape* RigidActor::InitBoxShape(
    physx::PxPhysics* physics,
    physx::PxMaterial* material,
    const BoxColliderData& boxCollider) const {
    return physics->createShape(physx::PxBoxGeometry(boxCollider.size.x, boxCollider.size.y, boxCollider.size.z), *material);
}

physx::PxShape* RigidActor::InitSphereShape(
    physx::PxPhysics* physics,
    physx::PxMaterial* material,
    const SphereColliderData& sphereCollider) const {
    return physics->createShape(physx::PxSphereGeometry(sphereCollider.radius), *material);
}

void RigidStatic::Init(physx::PxPhysics* physics, const RigidStaticData& rigidStatic, const SphereColliderData& shape, const  Vec3f& position, const  EulerAngles& eulerAngle)
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

void RigidStatic::Init(physx::PxPhysics* physics, const RigidStaticData& rigidStatic, const BoxColliderData& shape, const  Vec3f& position, const  EulerAngles& eulerAngle)
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

void RigidDynamic::Init(physx::PxPhysics* physics, const RigidDynamicData& rigidDynamic, const SphereColliderData& shape, const  Vec3f& position, const  EulerAngles& eulerAngle)
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


void RigidDynamic::Init(physx::PxPhysics* physics, const RigidDynamicData& rigidDynamic, const BoxColliderData& shape, const  Vec3f& position, const  EulerAngles& eulerAngle)
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
    const RigidDynamicData& rigidDynamicData) const
{
    rigidActor_->setLinearDamping(rigidDynamicData.linearDamping);
    rigidActor_->setAngularDamping(rigidDynamicData.angularDamping);
    rigidActor_->setMass(rigidDynamicData.mass);
    rigidActor_->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !rigidDynamicData.useGravity);
    rigidActor_->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, rigidDynamicData.isKinematic);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X, rigidDynamicData.freezeRotation.x);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y, rigidDynamicData.freezeRotation.y);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z, rigidDynamicData.freezeRotation.z);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X, rigidDynamicData.freezePosition.x);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y, rigidDynamicData.freezePosition.y);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z, rigidDynamicData.freezePosition.z); 
    switch (rigidDynamicData.colliderType) {
    case ColliderType::INVALID:
        break;
    case ColliderType::BOX:
        rigidActor_->detachShape(*shape_);
        SetBoxColliderData(rigidDynamicData.boxColliderData);
        rigidActor_->attachShape(*shape_);
        break;
    case ColliderType::SPHERE:
        rigidActor_->detachShape(*shape_);
        SetSphereColliderData(rigidDynamicData.sphereColliderData);
        rigidActor_->attachShape(*shape_);
        break;
    default:;
    }
}

void RigidDynamic::FixedUpdate(seconds dt)
{
}
physics::RigidDynamicData RigidDynamic::GetRigidDynamicData() const
{
    RigidDynamicData rigidDynamicData;
    rigidDynamicData.linearDamping = rigidActor_->getLinearDamping();
    rigidDynamicData.angularDamping = rigidActor_->getAngularDamping();
    rigidDynamicData.mass = rigidActor_->getMass();
    rigidDynamicData.useGravity = (rigidActor_->getActorFlags() & physx::PxActorFlags(physx::PxActorFlag::eDISABLE_GRAVITY)) != physx::PxActorFlags(physx::PxActorFlag::eDISABLE_GRAVITY);
    rigidDynamicData.isKinematic = (rigidActor_->getRigidBodyFlags() & physx::PxRigidBodyFlags(physx::PxRigidBodyFlag::eKINEMATIC)) == physx::PxRigidBodyFlags(physx::PxRigidBodyFlag::eKINEMATIC);
    rigidDynamicData.freezeRotation = {
        (rigidActor_->getRigidDynamicLockFlags() & physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X)) == physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X),
        (rigidActor_->getRigidDynamicLockFlags() & physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y)) == physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y),
        (rigidActor_->getRigidDynamicLockFlags() & physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z)) == physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z) };
    rigidDynamicData.freezePosition = {
        (rigidActor_->getRigidDynamicLockFlags() & physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X)) == physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X),
        (rigidActor_->getRigidDynamicLockFlags() & physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y)) == physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y),
        (rigidActor_->getRigidDynamicLockFlags() & physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z)) == physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z) };
    rigidDynamicData.colliderType = GetType();
    switch (rigidDynamicData.colliderType) {
    case ColliderType::INVALID:
        break;
    case ColliderType::BOX:
        rigidActor_->detachShape(*shape_);
        rigidDynamicData.boxColliderData = GetBoxColliderData();
        rigidActor_->attachShape(*shape_);
        break;
    case ColliderType::SPHERE:
        rigidActor_->detachShape(*shape_);
        rigidDynamicData.sphereColliderData = GetSphereColliderData();
        rigidActor_->attachShape(*shape_);
        break;
    default:;
    }

    return rigidDynamicData;
}

physics::DynamicData RigidDynamic::GetDynamicData() const
{
    DynamicData dynamicData;

    dynamicData.linearVelocity = ConvertFromPxVec(rigidActor_->getLinearVelocity());
    dynamicData.angularVelocity = ConvertFromPxVec(rigidActor_->getAngularVelocity());
    return dynamicData;
}

void RigidDynamic::AddForceAtPosition(const Vec3f& force, const Vec3f& position) const
{
    physx::PxRigidBodyExt::addForceAtLocalPos(*rigidActor_, ConvertToPxVec(force), ConvertToPxVec(position));
}

void RigidDynamic::AddForce(const Vec3f& force) const
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
