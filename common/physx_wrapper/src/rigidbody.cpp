#include <rigidbody.h>

#include <imgui.h>

#include "physics_engine.h"
#include "physx_utility.h"


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
    if (!shape_)
    {
        logDebug("No shape_");
        return sphereColliderData;
    }
    sphereColliderData.offset = ConvertFromPxVec(shape_->getLocalPose().p);
    sphereColliderData.radius = shape_->getGeometry().sphere().radius;
    sphereColliderData.isTrigger = (shape_->getFlags() & physx::PxShapeFlags(physx::PxShapeFlag::eTRIGGER_SHAPE)) == physx::PxShapeFlags(physx::PxShapeFlag::eTRIGGER_SHAPE);
    return sphereColliderData;
}

BoxColliderData RigidActor::GetBoxColliderData() const
{
    BoxColliderData boxColliderData;
    if (!shape_)
    {
        logDebug("No shape_");
        return boxColliderData;
    }
    boxColliderData.offset = ConvertFromPxVec(shape_->getLocalPose().p);
    boxColliderData.size = ConvertFromPxVec(shape_->getGeometry().box().halfExtents * 2.0f);
    boxColliderData.isTrigger = (shape_->getFlags() & physx::PxShapeFlags(physx::PxShapeFlag::eTRIGGER_SHAPE)) == physx::PxShapeFlags(physx::PxShapeFlag::eTRIGGER_SHAPE);
    return boxColliderData;
}

PhysicsMaterial RigidActor::GetMaterial() const
{
    PhysicsMaterial physicsMaterial;
    if (!shape_)
    {
        logDebug("No material_");
        return physicsMaterial;
    }
    physicsMaterial.bouciness = material_->getRestitution();
    physicsMaterial.staticFriction = material_->getStaticFriction();
    physicsMaterial.dynamicFriction = material_->getDynamicFriction();
    return physicsMaterial;
}

void RigidActor::SetSphereColliderData(
    const physics::SphereColliderData& sphereColliderData) const
{
    if (!shape_)
    {
        logDebug("No shape_");
        return;
    }
    physx::PxTransform transform = shape_->getLocalPose();
    transform.p = ConvertToPxVec(sphereColliderData.offset);
    shape_->setLocalPose(transform);
    physx::PxSphereGeometry geometry = shape_->getGeometry().sphere();
    geometry.radius = sphereColliderData.radius;
    shape_->setGeometry(geometry);
    shape_->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, sphereColliderData.isTrigger);

}

void RigidActor::SetBoxColliderData(
    const physics::BoxColliderData& boxColliderData) const
{
    if (!shape_)
    {
        logDebug("No shape_");
        return;
    }
    physx::PxTransform transform = shape_->getLocalPose();
    transform.p = ConvertToPxVec(boxColliderData.offset);
    shape_->setLocalPose(transform);
    physx::PxBoxGeometry geometry = shape_->getGeometry().box();
    geometry.halfExtents = ConvertToPxVec(boxColliderData.size / 2.0f);
    shape_->setGeometry(geometry);
    if (boxColliderData.isTrigger) {
        shape_->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, false);
        shape_->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, boxColliderData.isTrigger);
    } else {
        shape_->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, boxColliderData.isTrigger);
        shape_->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, true);
    }
}

void RigidActor::SetMaterial(const PhysicsMaterial& physicsMaterial) const
{
    material_->setRestitution(physicsMaterial.bouciness);
    material_->setStaticFriction(physicsMaterial.staticFriction);
    material_->setDynamicFriction(physicsMaterial.dynamicFriction);
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

void RigidStatic::Init(physx::PxPhysics* physics, const RigidStaticData& rigidStatic, const  Vec3f& position, const  EulerAngles& eulerAngle)
{
    physx::PxTransform transform = physx::PxTransform(ConvertToPxVec(position), ConvertToPxQuat(Quaternion::FromEuler(eulerAngle)));
    rigidActor_ = physics->createRigidStatic(transform);
    if (!rigidActor_)
        std::cerr << "create actor failed!";
    material_ = InitMaterial(physics, rigidStatic.material);
    if (!material_)
        std::cerr << "createMaterial failed!";
    switch(rigidStatic.colliderType) {
    case ColliderType::INVALID:
        break;
    case ColliderType::BOX:
        shape_ = InitBoxShape(physics, material_, rigidStatic.boxColliderData);
        break;
    case ColliderType::SPHERE:
        shape_ = InitSphereShape(physics, material_, rigidStatic.sphereColliderData);
        break;
    default: ;
    }
    if (!shape_)
        std::cerr << "createShpae failed!";
    rigidActor_->attachShape(*shape_);
    if (!shape_)
        std::cerr << "create shape failed!";
    SetRigidStaticData(rigidStatic);
}

void RigidStatic::SetRigidStaticData(
    const RigidStaticData& rigidStaticData) const
{
    if (!rigidActor_)
    {
        logDebug("No rigidActor");
        return;
    }
    switch (rigidStaticData.colliderType) {
    case ColliderType::INVALID:
        break;
    case ColliderType::BOX:
        rigidActor_->detachShape(*shape_);
        SetBoxColliderData(rigidStaticData.boxColliderData);
        rigidActor_->attachShape(*shape_);
        break;
    case ColliderType::SPHERE:
        rigidActor_->detachShape(*shape_);
        SetSphereColliderData(rigidStaticData.sphereColliderData);
        rigidActor_->attachShape(*shape_);
        break;
    default:;
    }
}

RigidStaticData RigidStatic::GetRigidStaticData() const
{
    RigidStaticData rigidStaticData;
    if (!rigidActor_)
    {
        logDebug("No rigidActor");
        return rigidStaticData;
    }
    rigidStaticData.colliderType = GetType();
    switch (rigidStaticData.colliderType) {
    case ColliderType::INVALID:
        break;
    case ColliderType::BOX:
        rigidActor_->detachShape(*shape_);
        rigidStaticData.boxColliderData = GetBoxColliderData();
        rigidActor_->attachShape(*shape_);
        break;
    case ColliderType::SPHERE:
        rigidActor_->detachShape(*shape_);
        rigidStaticData.sphereColliderData = GetSphereColliderData();
        rigidActor_->attachShape(*shape_);
        break;
    default:;
    }

    return rigidStaticData;
}

void RigidDynamic::Init(physx::PxPhysics* physics, const RigidDynamicData& rigidDynamic, const  Vec3f& position, const  EulerAngles& eulerAngle)
{
    physx::PxTransform transform = physx::PxTransform(ConvertToPxVec(position), ConvertToPxQuat(Quaternion::FromEuler(eulerAngle)));
    rigidActor_ = physics->createRigidDynamic(transform);
    if (!rigidActor_)
    {
        std::cerr << "create actor failed!";
        return;
    }
    material_ = InitMaterial(physics, rigidDynamic.material);
    if (!material_)
    {
        std::cerr << "createMaterial failed!";
        return;
    }
    switch (rigidDynamic.colliderType) {
    case ColliderType::INVALID:
        break;
    case ColliderType::BOX:
        shape_ = InitBoxShape(physics, material_, rigidDynamic.boxColliderData);
        break;
    case ColliderType::SPHERE:
        shape_ = InitSphereShape(physics, material_, rigidDynamic.sphereColliderData);
        break;
    default:;
    }
    if (!shape_)
    {
        std::cerr << "createShpae failed!";
        return;
    }
    rigidActor_->attachShape(*shape_);
    SetRigidDynamicData(rigidDynamic);
}

void RigidDynamic::SetRigidDynamicData(
    const RigidDynamicData& rigidDynamicData) const
{
    if (!rigidActor_)
    {
        logDebug("No rigidActor");
        return;
    }
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
    if (!rigidActor_)
    {
        logDebug("No rigidActor");
        return rigidDynamicData;
    }
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
    if (!rigidActor_)
    {
        logDebug("No rigidActor");
        return dynamicData;
    }
    dynamicData.linearVelocity = ConvertFromPxVec(rigidActor_->getLinearVelocity());
    dynamicData.angularVelocity = ConvertFromPxVec(rigidActor_->getAngularVelocity());
    return dynamicData;
}

void RigidDynamic::AddForceAtPosition(const Vec3f& force, const Vec3f& position) const
{
    if (!rigidActor_)
    {
        logDebug("No rigidActor");
        return;
    }
    physx::PxRigidBodyExt::addForceAtLocalPos(*rigidActor_, ConvertToPxVec(force), ConvertToPxVec(position));
}

void RigidDynamic::AddForce(const Vec3f& force) const
{
    if (!rigidActor_)
    {
        logDebug("No rigidActor");
        return;
    }
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
        transform = GetComponent(entity).GetPxRigidDynamic()->getGlobalPose();
        transform3dManager_.SetPosition(entity, ConvertFromPxVec(transform.p));
        physx::PxVec3 x = transform.q.getBasisVector0();
        physx::PxVec3 y = transform.q.getBasisVector1();
        physx::PxVec3 z = transform.q.getBasisVector2();
        transform3dManager_.SetRotation(entity, Quaternion::ToEulerAngles(ConvertFromPxQuat(transform.q)));
        RigidDynamic rigidDynamic = GetComponent(entity);
        rigidDynamic.FixedUpdate(dt);
        SetComponent(entity, rigidDynamic);
    }
}

RigidDynamicViewer::RigidDynamicViewer(
    EntityManager& entityManager,
    PhysicsEngine& physicsEngine)
    : entityManager_(entityManager),
    physicsEngine_(physicsEngine),
    rigidDynamicData_() { }

void RigidDynamicViewer::DrawImGui()
{
    if (selectedEntity_ == INVALID_ENTITY)
        return;
    if (entityManager_.HasComponent(selectedEntity_, static_cast<EntityMask>(ComponentType::RIGID_DYNAMIC)))
    {
        RigidDynamicData rigidDynamicData = rigidDynamicData_;
        neko::Vec3f linearVelocity = dynamicData_.linearVelocity;
        ImGui::DragFloat3(
            "linearVelocity",
            linearVelocity.coord, 0);
        ImGui::DragFloat("linearDamping", &rigidDynamicData.linearDamping);
        neko::Vec3f angularVelocity = dynamicData_.angularVelocity;
        ImGui::DragFloat3(
            "angularVelocity",
            angularVelocity.coord, 0);
        ImGui::DragFloat(
            "angularDamping",
            &rigidDynamicData.angularDamping);
        ImGui::DragFloat("mass", &rigidDynamicData.mass, 0.5, 0.0f, 1000);
        ImGui::Checkbox("useGravity", &rigidDynamicData.useGravity);
        ImGui::Checkbox("isKinematic", &rigidDynamicData.isKinematic);
        ImGui::Text("freezePosition");
        ImGui::SameLine();
        ImGui::Checkbox("x", &rigidDynamicData.freezePosition.x);
        ImGui::SameLine();
        ImGui::Checkbox("y", &rigidDynamicData.freezePosition.y);
        ImGui::SameLine();
        ImGui::Checkbox("z", &rigidDynamicData.freezePosition.z);
        ImGui::Text("freezeRotation");
        ImGui::SameLine();
        ImGui::Checkbox("x#", &rigidDynamicData.freezeRotation.x);
        ImGui::SameLine();
        ImGui::Checkbox("y#", &rigidDynamicData.freezeRotation.y);
        ImGui::SameLine();
        ImGui::Checkbox("z#", &rigidDynamicData.freezeRotation.z);

        switch (rigidDynamicData.colliderType) {
        case neko::physics::ColliderType::INVALID:
            break;
        case neko::physics::ColliderType::BOX: {
            neko::physics::BoxColliderData boxColliderData = rigidDynamicData.boxColliderData;
            if (ImGui::CollapsingHeader("BoxCollider", true))
            {
                if (ImGui::DragFloat3(
                    "offset",
                    boxColliderData.offset.coord)) {
                }
                if (ImGui::DragFloat3(
                    "size",
                    boxColliderData.size.coord, 0.1f, 0.0f)) {
                }

                if (ImGui::TreeNode("Material"))
                {
                    ImGui::DragFloat(
                        "bouciness",
                        &rigidDynamicData.material.bouciness, 0.1f, 0.0f, 1.0f);
                    ImGui::DragFloat(
                        "staticFriction",
                        &rigidDynamicData.material.staticFriction, 0.1f, 0.0f, 1.0f);
                    ImGui::DragFloat(
                        "dynamicFriction",
                        &rigidDynamicData.material.dynamicFriction, 0.1f, 0.0f, 1.0f);
                    ImGui::TreePop();
                }
                ImGui::Checkbox("isTrigger", &boxColliderData.isTrigger);
                rigidDynamicData.boxColliderData = boxColliderData;
            }
            break;
        }
        case neko::physics::ColliderType::SPHERE: {
            neko::physics::SphereColliderData sphereColliderData = rigidDynamicData.sphereColliderData;
            if (ImGui::CollapsingHeader("SphereCollider", true))
            {
                ImGui::DragFloat3(
                    "offset",
                    sphereColliderData.offset.coord, 0);
                ImGui::DragFloat(
                    "radius",
                    &sphereColliderData.radius);

                if (ImGui::TreeNode("Material"))
                {
                    ImGui::DragFloat(
                        "bouciness",
                        &rigidDynamicData.material.bouciness, 0.1f, 0.0f, 1.0f);
                    ImGui::DragFloat(
                        "staticFriction",
                        &rigidDynamicData.material.staticFriction, 0.1f, 0.0f, 1.0f);
                    ImGui::DragFloat(
                        "dynamicFriction",
                        &rigidDynamicData.material.dynamicFriction, 0.1f, 0.0f, 1.0f);
                    ImGui::TreePop();
                }
                ImGui::Checkbox("isTrigger", &sphereColliderData.isTrigger);
                rigidDynamicData.sphereColliderData = sphereColliderData;
            }
            break;
        }
        default:;
            break;
        }

        if (!physicsEngine_.IsPhysicRunning())
        {
            physicsEngine_.GetRigidDynamic(selectedEntity_).SetRigidDynamicData(rigidDynamicData);
            rigidDynamicData_ = physicsEngine_.GetRigidDynamic(selectedEntity_).GetRigidDynamicData();
            dynamicData_ = physicsEngine_.GetRigidDynamic(selectedEntity_).GetDynamicData();
        }
    }
}

void RigidDynamicViewer::SetSelectedEntity(Entity selectedEntity)
{
    selectedEntity_ = selectedEntity;
    if (selectedEntity_ == INVALID_ENTITY)
        return;
    rigidDynamicData_ = physicsEngine_.GetRigidDynamic(selectedEntity_).GetRigidDynamicData();
}

void RigidDynamicViewer::FixedUpdate(seconds dt)
{
    if (selectedEntity_ == INVALID_ENTITY)
        return;
    if (entityManager_.HasComponent(selectedEntity_, static_cast<EntityMask>(ComponentType::RIGID_DYNAMIC)))
    {
        dynamicData_ = physicsEngine_.GetRigidDynamic(selectedEntity_).GetDynamicData();
    }
}
}
