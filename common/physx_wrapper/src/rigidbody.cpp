#include <rigidbody.h>
#include <physics_engine.h>
#include <engine\log.h>
#include "mathematics/transform.h"


namespace neko::physics::px {
void RigidStatic::Init(physx::PxPhysics* physics, const PhysicsShape& shape, const  Vec3f& position, const  EulerAngles& eulerAngle)
{
    physx::PxTransform transform = physx::PxTransform(ConvertToPxVec(position), ConvertToPxQuat(Quaternion::FromEuler(eulerAngle)));
    rigidActor_ = physics->createRigidStatic(transform);
    if (!rigidActor_)
        std::cerr << "create plane failed!";
    rigidActor_->attachShape(*shape.GetPxShape());
    if (!shape.GetPxShape())
        std::cerr << "create shape failed!";
}

physx::PxRigidStatic* RigidStatic::GetPxRigidStatic() const {
    return rigidActor_;
}

void RigidDynamic::Init(physx::PxPhysics* physics, const PhysicsShape& shape, const  Vec3f& position, const  EulerAngles& eulerAngle)
{
    physx::PxTransform transform = physx::PxTransform(ConvertToPxVec(position), ConvertToPxQuat(Quaternion::FromEuler(eulerAngle)));
    rigidActor_ = physics->createRigidDynamic(transform);
    if (!rigidActor_)
        std::cerr << "create plane failed!";
    rigidActor_->attachShape(*shape.GetPxShape());
    if (!shape.GetPxShape())
        std::cerr << "create shape failed!";
}

physx::PxRigidDynamic* RigidDynamic::GetPxRigidDynamic() const
{
    return rigidActor_;
}

RigidStaticManager::RigidStaticManager(
    EntityManager& entityManager,
    BoxPhysicsShapeManager& boxShapeManager,
    SpherePhysicsShapeManager& circlePhysicsShapeManager,
    Transform3dManager& transform3dManager)
    : ComponentManager<RigidStatic, EntityMask(ComponentType::RIGID_STATIC)>(entityManager),
      boxShapeManager_(boxShapeManager),
      spherePhysicsShapeManager_(circlePhysicsShapeManager),
      transform3dManager_(transform3dManager) {}

void RigidStaticManager::FixedUpdate(seconds dt)
{
    
}

RigidDynamicManager::RigidDynamicManager(
    EntityManager& entityManager,
    BoxPhysicsShapeManager& boxShapeManager,
    SpherePhysicsShapeManager& circlePhysicsShapeManager,
    Transform3dManager& transform3dManager)
    : ComponentManager<RigidDynamic, EntityMask(ComponentType::RIGID_DYNAMIC)>(entityManager),
      boxShapeManager_(boxShapeManager),
      spherePhysicsShapeManager_(circlePhysicsShapeManager),
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
        if (entityManager_.get().HasComponent(entity, EntityMask(ComponentType::BOX_COLLIDER))) {
            BoxPhysicsShape boxPhysicsShape = boxShapeManager_.GetComponent(entity);
            transform = physx::PxShapeExt::getGlobalPose(*boxPhysicsShape.GetPxShape(), *GetComponent(entity).GetPxRigidDynamic());
        }
        else if (entityManager_.get().HasComponent(entity, EntityMask(ComponentType::SPHERE_COLLIDER))) {
            SpherePhysicsShape spherePhysicsShape = spherePhysicsShapeManager_.GetComponent(entity);
            transform = physx::PxShapeExt::getGlobalPose(*spherePhysicsShape.GetPxShape(), *GetComponent(entity).GetPxRigidDynamic());
        }
        else {
            neko_assert(false, "No shape link to the rigidBody")
        }
        transform3dManager_.SetPosition(entity, ConvertFromPxVec(transform.p));
        transform3dManager_.SetRotation(entity, Quaternion::ToEulerAngles(ConvertFromPxQuat(transform.q)));
    }
    
}

void RigidDynamicManager::AddForceAtPosition(Vec3f force, Vec3f position)
{
    
}

void RigidDynamicManager::AddForce(Vec3f force)
{
    
}
}
