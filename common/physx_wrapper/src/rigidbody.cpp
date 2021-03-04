#include <px/rigidbody.h>

#include <imgui.h>


#include "aer/tag.h"
#include "px/physics_engine.h"
#include "px/physx_utility.h"


namespace neko::physics {
ColliderType RigidActor::GetColliderType() const
{
    if (shape_ == nullptr) { return ColliderType::INVALID; }

    switch (shape_->getGeometryType()) {
        case physx::PxGeometryType::eSPHERE: return ColliderType::SPHERE;
            break;
        case physx::PxGeometryType::ePLANE: return ColliderType::INVALID;
            break;
        case physx::PxGeometryType::eCAPSULE: return ColliderType::INVALID;
            break;
        case physx::PxGeometryType::eBOX: return ColliderType::BOX;
            break;
        case physx::PxGeometryType::eCONVEXMESH: return ColliderType::INVALID;
            break;
        case physx::PxGeometryType::eTRIANGLEMESH: return ColliderType::INVALID;
            break;
        case physx::PxGeometryType::eHEIGHTFIELD: return ColliderType::INVALID;
            break;
        case physx::PxGeometryType::eGEOMETRY_COUNT: return ColliderType::INVALID;
            break;
        case physx::PxGeometryType::eINVALID: return ColliderType::INVALID;
            break;
        default: return ColliderType::INVALID;;
    }
}

SphereColliderData RigidActor::GetSphereColliderData() const
{
    SphereColliderData sphereColliderData;
    if (!shape_) {
        logDebug("No shape_");
        return sphereColliderData;
    }
    sphereColliderData.offset    = ConvertFromPxVec(shape_->getLocalPose().p);
    sphereColliderData.radius    = shape_->getGeometry().sphere().radius;
    sphereColliderData.isTrigger = (shape_->getFlags() &
                                    physx::PxShapeFlags(physx::PxShapeFlag::eTRIGGER_SHAPE)) ==
                                   physx::PxShapeFlags(physx::PxShapeFlag::eTRIGGER_SHAPE);
    return sphereColliderData;
}

BoxColliderData RigidActor::GetBoxColliderData() const
{
    BoxColliderData boxColliderData;
    if (!shape_) {
        logDebug("No shape_");
        return boxColliderData;
    }
    boxColliderData.offset    = ConvertFromPxVec(shape_->getLocalPose().p);
    boxColliderData.size      = ConvertFromPxVec(shape_->getGeometry().box().halfExtents * 2.0f);
    boxColliderData.isTrigger = (shape_->getFlags() &
                                 physx::PxShapeFlags(physx::PxShapeFlag::eTRIGGER_SHAPE)) ==
                                physx::PxShapeFlags(physx::PxShapeFlag::eTRIGGER_SHAPE);
    return boxColliderData;
}

PhysicsMaterial RigidActor::GetPhysicsMaterial() const
{
    PhysicsMaterial physicsMaterial;
    if (!shape_) {
        logDebug("No material_");
        return physicsMaterial;
    }
    physicsMaterial.bouciness = material_->getRestitution();
    physicsMaterial.staticFriction  = material_->getStaticFriction();
    physicsMaterial.dynamicFriction = material_->getDynamicFriction();
    return physicsMaterial;
}

void RigidActor::SetSphereColliderData(
    const physics::SphereColliderData& sphereColliderData) const
{
    if (!shape_) {
        logDebug("No shape_");
        return;
    }
    physx::PxTransform transform = shape_->getLocalPose();
    transform.p                  = ConvertToPxVec(sphereColliderData.offset);
    shape_->setLocalPose(transform);
    physx::PxSphereGeometry geometry = shape_->getGeometry().sphere();
    geometry.radius                  = sphereColliderData.radius;
    shape_->setGeometry(geometry);
    shape_->setFlag(physx::PxShapeFlag::eTRIGGER_SHAPE, sphereColliderData.isTrigger);
}

void RigidActor::SetBoxColliderData(
    const physics::BoxColliderData& boxColliderData) const
{
    if (!shape_) {
        logDebug("No shape_");
        return;
    }
    physx::PxTransform transform = shape_->getLocalPose();
    transform.p                  = ConvertToPxVec(boxColliderData.offset);
    shape_->setLocalPose(transform);
    physx::PxBoxGeometry geometry = shape_->getGeometry().box();
    geometry.halfExtents          = ConvertToPxVec(boxColliderData.size / 2.0f);
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
    material_->setRestitutionCombineMode(physx::PxCombineMode::eMIN);
    material_->setFrictionCombineMode(physx::PxCombineMode::eMIN);
}

physx::PxMaterial* RigidActor::InitMaterial(
    physx::PxPhysics* physics,
    const PhysicsMaterial& material) const
{
    return physics->createMaterial(material.staticFriction,
        material.dynamicFriction,
        material.bouciness);
}

physx::PxShape* RigidActor::InitBoxShape(
    physx::PxPhysics* physics,
    physx::PxMaterial* material,
    const BoxColliderData& boxCollider) const
{
    return physics->createShape(
        physx::PxBoxGeometry(boxCollider.size.x, boxCollider.size.y, boxCollider.size.z),
        *material);
}

physx::PxShape* RigidActor::InitSphereShape(
    physx::PxPhysics* physics,
    physx::PxMaterial* material,
    const SphereColliderData& sphereCollider) const
{
    return physics->createShape(physx::PxSphereGeometry(sphereCollider.radius), *material);
}

void RigidActor::SetFiltering(
    physx::PxShape* shape,
    physx::PxU32 filterGroup)
{
    physx::PxFilterData filterData;
    filterData.word0 = filterGroup; // word0 = own ID
    filterData.word1 = FilterGroup::EVERYTHING;
    // contact callback;
    shape->setSimulationFilterData(filterData);
    shape->setQueryFilterData(filterData);
}

json RigidActorViewer::GetJsonFromBoxCollider(const RigidActorData& rigidActorData) const
{
    json boxColliderJson           = json::object();
    boxColliderJson["exist"]          = rigidActorData.colliderType == ColliderType::BOX;
    boxColliderJson["isTrigger"]   = rigidActorData.boxColliderData.isTrigger;
    boxColliderJson["offset"]         = GetJsonFromVector3(rigidActorData.boxColliderData.offset);
    boxColliderJson["size"]           = GetJsonFromVector3(rigidActorData.boxColliderData.size);
    return boxColliderJson;
}

json RigidActorViewer::GetJsonFromSphereCollider(const RigidActorData& rigidActorData) const
{
    json sphereColliderJson         = json::object();
    sphereColliderJson["exist"]     = rigidActorData.colliderType == ColliderType::SPHERE;
    sphereColliderJson["isTrigger"] = rigidActorData.sphereColliderData.isTrigger;
    sphereColliderJson["offset"]    = GetJsonFromVector3(rigidActorData.sphereColliderData.offset);
    sphereColliderJson["radius"]      = rigidActorData.sphereColliderData.radius;
    return sphereColliderJson;
}

json RigidActorViewer::GetJsonFromMaterial(const RigidActorData& rigidActorData) const
{
    json physicsMaterialJson        = json::object();
    physicsMaterialJson["exist"]     = rigidActorData.colliderType != ColliderType::INVALID;
    physicsMaterialJson["bouciness"] = rigidActorData.material.bouciness;
    physicsMaterialJson["staticFriction"]  = rigidActorData.material.staticFriction;
    physicsMaterialJson["dynamicFriction"] = rigidActorData.material.dynamicFriction;
    return physicsMaterialJson;
}

RigidActorData RigidActorViewer::GetRigidActorFromJson(const json& rigidActorJson)
{
    RigidActorData rigidActorData;
    if (CheckJsonParameter(rigidActorJson, "sphereCollider", json::value_t::object))
    {
        if (CheckJsonParameter(rigidActorJson["sphereCollider"], "exist", json::value_t::boolean))
        {
            if (rigidActorJson["sphereCollider"]["exist"]) {
                rigidActorData.colliderType = ColliderType::SPHERE;
                if (CheckJsonParameter(
                        rigidActorJson["sphereCollider"], "isTrigger", json::value_t::boolean))
                {
                    rigidActorData.sphereColliderData.isTrigger =
                        rigidActorJson["sphereCollider"]["isTrigger"];
                }
                rigidActorData.sphereColliderData.offset =
                    GetVector3FromJson(rigidActorJson["sphereCollider"], "offset");
                if (CheckJsonNumber(
                        rigidActorJson["sphereCollider"], "radius"))
                {
                    rigidActorData.sphereColliderData.radius =
                        rigidActorJson["sphereCollider"]["radius"];
                }
            }
        }
    }
    if (CheckJsonParameter(rigidActorJson, "boxCollider", json::value_t::object))
    {
        if (CheckJsonParameter(rigidActorJson["boxCollider"], "exist", json::value_t::boolean))
        {
            if (rigidActorJson["boxCollider"]["exist"])
            {
                rigidActorData.colliderType = ColliderType::BOX;
                if (CheckJsonParameter(
                        rigidActorJson["boxCollider"], "isTrigger", json::value_t::boolean))
                {
                    rigidActorData.boxColliderData.isTrigger =
                        rigidActorJson["boxCollider"]["isTrigger"];
                }
                rigidActorData.boxColliderData.offset =
                    GetVector3FromJson(rigidActorJson["boxCollider"], "offset");
                rigidActorData.boxColliderData.size =
                    GetVector3FromJson(rigidActorJson["boxCollider"], "size");
            }
        }
    }
    if (CheckJsonParameter(rigidActorJson, "physicsMaterial", json::value_t::object))
    {
        if (CheckJsonNumber(rigidActorJson["physicsMaterial"], "bouciness"))
        {
            rigidActorData.material.bouciness =
                rigidActorJson["physicsMaterial"]["bouciness"];
        }
        if (CheckJsonNumber(rigidActorJson["physicsMaterial"], "staticFriction"))
        {
            rigidActorData.material.staticFriction =
                rigidActorJson["physicsMaterial"]["staticFriction"];
        }
        if (CheckJsonNumber(rigidActorJson["physicsMaterial"], "dynamicFriction"))
        {
            rigidActorData.material.dynamicFriction =
                rigidActorJson["physicsMaterial"]["dynamicFriction"];
        }
    }
    return rigidActorData;
}

RigidActorData RigidActorViewer::DrawImGuiRigidActor(
    const RigidActorData& rigidActorData)
{
    RigidActorData newRigidActorData = rigidActorData;
    switch (newRigidActorData.colliderType)
    {
        case neko::physics::ColliderType::INVALID: break;
        case neko::physics::ColliderType::BOX:
        {
            neko::physics::BoxColliderData boxColliderData = newRigidActorData.boxColliderData;
            if (ImGui::CollapsingHeader("BoxCollider", true))
            {
                if (ImGui::DragFloat3("offset", boxColliderData.offset.coord)) {}
                if (ImGui::DragFloat3("size", boxColliderData.size.coord, 0.1f, 0.0f)) {}
                ImGui::Checkbox("isTrigger", &boxColliderData.isTrigger);
                newRigidActorData.boxColliderData = boxColliderData;
            }
            break;
        }
        case neko::physics::ColliderType::SPHERE:
        {
            neko::physics::SphereColliderData sphereColliderData =
                newRigidActorData.sphereColliderData;
            if (ImGui::CollapsingHeader("SphereCollider", true))
            {
                ImGui::DragFloat3("offset", sphereColliderData.offset.coord, 0);
                ImGui::DragFloat("radius", &sphereColliderData.radius);
                ImGui::Checkbox("isTrigger", &sphereColliderData.isTrigger);
                newRigidActorData.sphereColliderData = sphereColliderData;
            }
            break;
        }
        default:; break;
    }
    if (ImGui::CollapsingHeader("Material", true))
    {
        ImGui::DragFloat("bouciness", &newRigidActorData.material.bouciness, 0.1f, 0.0f, 1.0f);
        ImGui::DragFloat(
            "staticFriction", &newRigidActorData.material.staticFriction, 0.1f, 0.0f, 1.0f);
        ImGui::DragFloat(
            "dynamicFriction", &newRigidActorData.material.dynamicFriction, 0.1f, 0.0f, 1.0f);
    }
    return newRigidActorData;
}

void RigidStatic::Init(physx::PxPhysics* physics,
    const RigidStaticData& rigidStatic,
    const Vec3f& position,
    const EulerAngles& eulerAngle)
{
    physx::PxTransform transform = physx::PxTransform(ConvertToPxVec(position),
        ConvertToPxQuat(Quaternion::FromEuler(eulerAngle)));
    rigidActor_ = physics->createRigidStatic(transform);
    if (!rigidActor_) std::cerr << "create actor failed!";
    material_ = InitMaterial(physics, rigidStatic.material);
    if (!material_) std::cerr << "createMaterial failed!";
    switch (rigidStatic.colliderType) {
        case ColliderType::INVALID: 
        case ColliderType::BOX: shape_ = InitBoxShape(physics,
                                    material_,
                                    rigidStatic.boxColliderData);
            break;
        case ColliderType::SPHERE: shape_ = InitSphereShape(physics,
                                       material_,
                                       rigidStatic.sphereColliderData);
            break;
        default: ;
    }
    if (!shape_)
    {
        std::cerr << "createShape failed!";
        return;
    }
    SetFiltering(shape_, rigidStatic.filterGroup);
    rigidActor_->attachShape(*shape_);
    SetRigidStaticData(rigidStatic);
}

void RigidStatic::SetRigidStaticData(
    const RigidStaticData& rigidStaticData) const
{
    if (!rigidActor_) {
        logDebug("No rigidActor");
        return;
    }
    SetMaterial(rigidStaticData.material);
    switch (rigidStaticData.colliderType) {
        case ColliderType::INVALID: break;
        case ColliderType::BOX: rigidActor_->detachShape(*shape_);
            SetBoxColliderData(rigidStaticData.boxColliderData);
            rigidActor_->attachShape(*shape_);
            break;
        case ColliderType::SPHERE: rigidActor_->detachShape(*shape_);
            SetSphereColliderData(rigidStaticData.sphereColliderData);
            rigidActor_->attachShape(*shape_);
            break;
        default: ;
    }
}

RigidStaticData RigidStatic::GetRigidStaticData() const
{
    RigidStaticData rigidStaticData;
    if (!rigidActor_) {
        logDebug("No rigidActor");
        return rigidStaticData;
    }
    rigidStaticData.material = GetPhysicsMaterial();
    rigidStaticData.colliderType = GetColliderType();
    switch (rigidStaticData.colliderType) {
        case ColliderType::INVALID: break;
        case ColliderType::BOX: rigidActor_->detachShape(*shape_);
            rigidStaticData.boxColliderData = GetBoxColliderData();
            rigidActor_->attachShape(*shape_);
            break;
        case ColliderType::SPHERE: rigidActor_->detachShape(*shape_);
            rigidStaticData.sphereColliderData = GetSphereColliderData();
            rigidActor_->attachShape(*shape_);
            break;
        default: break;
    }
    return rigidStaticData;
}

void RigidDynamic::Init(physx::PxPhysics* physics,
    const RigidDynamicData& rigidDynamic,
    const Vec3f& position,
    const EulerAngles& eulerAngle)
{
    physx::PxTransform transform = physx::PxTransform(ConvertToPxVec(position),
        ConvertToPxQuat(Quaternion::FromEuler(eulerAngle)));
    rigidActor_ = physics->createRigidDynamic(transform);
    if (!rigidActor_) {
        std::cerr << "create actor failed!";
        return;
    }
    material_ = InitMaterial(physics, rigidDynamic.material);
    if (!material_) {
        std::cerr << "createMaterial failed!";
        return;
    }
    switch (rigidDynamic.colliderType) {
        case ColliderType::INVALID: break;
        case ColliderType::BOX: shape_ = InitBoxShape(physics,
                                    material_,
                                    rigidDynamic.boxColliderData);
            break;
        case ColliderType::SPHERE: shape_ = InitSphereShape(physics,
                                       material_,
                                       rigidDynamic.sphereColliderData);
            break;
        default: ;
    }
    if (!shape_) {
        std::cerr << "createShpae failed!";
        return;
    }
    rigidActor_->attachShape(*shape_);
    //rigidActor_->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
    physx::PxU32 posIt = physx::PxU32(10);
    physx::PxU32 velIt = physx::PxU32(1);
    rigidActor_->setSolverIterationCounts(posIt, velIt);
    SetRigidDynamicData(rigidDynamic);
}

void RigidDynamic::SetRigidDynamicData(
    const RigidDynamicData& rigidDynamicData) const
{
    if (!rigidActor_) {
        logDebug("No rigidActor");
        return;
    }
    rigidActor_->setLinearDamping(rigidDynamicData.linearDamping);
    rigidActor_->setAngularDamping(rigidDynamicData.angularDamping);
    rigidActor_->setMass(rigidDynamicData.mass);
    rigidActor_->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, !rigidDynamicData.useGravity);
    rigidActor_->setRigidBodyFlag(physx::PxRigidBodyFlag::eKINEMATIC, rigidDynamicData.isKinematic);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X,
        rigidDynamicData.freezeRotation.x);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y,
        rigidDynamicData.freezeRotation.y);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z,
        rigidDynamicData.freezeRotation.z);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X,
        rigidDynamicData.freezePosition.x);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y,
        rigidDynamicData.freezePosition.y);
    rigidActor_->setRigidDynamicLockFlag(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z,
        rigidDynamicData.freezePosition.z);
    SetMaterial(rigidDynamicData.material);
    switch (rigidDynamicData.colliderType) {
        case ColliderType::INVALID: break;
        case ColliderType::BOX: rigidActor_->detachShape(*shape_);
            SetBoxColliderData(rigidDynamicData.boxColliderData);
            rigidActor_->attachShape(*shape_);
            break;
        case ColliderType::SPHERE: rigidActor_->detachShape(*shape_);
            SetSphereColliderData(rigidDynamicData.sphereColliderData);
            rigidActor_->attachShape(*shape_);
            break;
        default: break;
    }
    rigidActor_->setRigidBodyFlag(physx::PxRigidBodyFlag::eENABLE_CCD, true);
}

physics::RigidDynamicData RigidDynamic::GetRigidDynamicData() const
{
    RigidDynamicData rigidDynamicData;
    if (!rigidActor_) {
        logDebug("No rigidActor");
        return rigidDynamicData;
    }
    rigidDynamicData.linearDamping  = rigidActor_->getLinearDamping();
    rigidDynamicData.angularDamping = rigidActor_->getAngularDamping();
    rigidDynamicData.mass           = rigidActor_->getMass();
    rigidDynamicData.useGravity     = (rigidActor_->getActorFlags() &
                                       physx::PxActorFlags(physx::PxActorFlag::eDISABLE_GRAVITY)) !=
                                      physx::PxActorFlags(physx::PxActorFlag::eDISABLE_GRAVITY);
    rigidDynamicData.isKinematic = (rigidActor_->getRigidBodyFlags() &
                                    physx::PxRigidBodyFlags(physx::PxRigidBodyFlag::eKINEMATIC)) ==
                                   physx::PxRigidBodyFlags(physx::PxRigidBodyFlag::eKINEMATIC);
    rigidDynamicData.freezeRotation = {
        (rigidActor_->getRigidDynamicLockFlags() &
         physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X)) ==
        physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_X),
        (rigidActor_->getRigidDynamicLockFlags() &
         physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y)) ==
        physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Y),
        (rigidActor_->getRigidDynamicLockFlags() &
         physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z)) ==
        physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_ANGULAR_Z)};
    rigidDynamicData.freezePosition = {
        (rigidActor_->getRigidDynamicLockFlags() &
         physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X)) ==
        physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_X),
        (rigidActor_->getRigidDynamicLockFlags() &
         physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y)) ==
        physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Y),
        (rigidActor_->getRigidDynamicLockFlags() &
         physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z)) ==
            physx::PxRigidDynamicLockFlags(physx::PxRigidDynamicLockFlag::eLOCK_LINEAR_Z)};
    rigidDynamicData.material     = GetPhysicsMaterial();
    rigidDynamicData.colliderType = GetColliderType();
    switch (rigidDynamicData.colliderType) {
        case ColliderType::INVALID: break;
        case ColliderType::BOX: rigidActor_->detachShape(*shape_);
            rigidDynamicData.boxColliderData = GetBoxColliderData();
            rigidActor_->attachShape(*shape_);
            break;
        case ColliderType::SPHERE: rigidActor_->detachShape(*shape_);
            rigidDynamicData.sphereColliderData = GetSphereColliderData();
            rigidActor_->attachShape(*shape_);
            break;
        default: ;
    }

    return rigidDynamicData;
}

const physics::DynamicData RigidDynamic::GetDynamicData() const
{
    DynamicData dynamicData;
    if (!rigidActor_) {
        logDebug("No rigidActor");
        return dynamicData;
    }
    dynamicData.linearVelocity  = ConvertFromPxVec(rigidActor_->getLinearVelocity());
    dynamicData.angularVelocity = ConvertFromPxVec(rigidActor_->getAngularVelocity());
    return dynamicData;
}

void RigidDynamic::AddForceAtPosition(const Vec3f& force, const Vec3f& position) const
{
    if (!rigidActor_) {
        logDebug("No rigidActor");
        return;
    }
    physx::PxRigidBodyExt::addForceAtLocalPos(*rigidActor_,
        ConvertToPxVec(force),
        ConvertToPxVec(position));
}

void RigidDynamic::AddForce(const Vec3f& force, physx::PxForceMode::Enum forceMode) const
{
    if (!rigidActor_) {
        logDebug("No rigidActor");
        return;
    }
    rigidActor_->addForce(ConvertToPxVec(force), forceMode);
}

void RigidDynamic::
AddRelativeTorque(const Vec3f& torque, physx::PxForceMode::Enum forceMode) const
{
    if (!rigidActor_)
    {
        logDebug("No rigidActor");
        return;
    }
    rigidActor_->addTorque(ConvertToPxVec(torque), forceMode);
}

void RigidDynamic::MoveRotation(const Quaternion& rot) const
{
    if (!rigidActor_)
    {
        logDebug("No rigidActor");
        return;
    }
    physx::PxTransform transform = rigidActor_->getGlobalPose();
    transform.q                  = ConvertToPxQuat(rot);
    rigidActor_->setGlobalPose(transform);
}

RigidStaticManager::RigidStaticManager(
    EntityManager& entityManager,
    Transform3dManager& transform3dManager,
    PhysicsEngine& physicsEngine)
    : ComponentManager<RigidStatic, EntityMask(ComponentType::RIGID_STATIC)>(entityManager),
      transform3dManager_(transform3dManager),
      physicsEngine_(physicsEngine) {}

void RigidStaticManager::FixedUpdate(seconds dt) { }

void RigidStaticManager::AddRigidStatic(Entity entity, const RigidStaticData& rigidStaticData)
{
    AddComponent(entity);
    Vec3f position                       = transform3dManager_.GetRelativePosition(entity);
    EulerAngles euler                    = transform3dManager_.GetRelativeRotation(entity);
    Vec3f scale                          = transform3dManager_.GetRelativeScale(entity);
    RigidStaticData newRigidStaticData   = rigidStaticData;
    newRigidStaticData.boxColliderData.size =
        Vec3f(newRigidStaticData.boxColliderData.size.x * scale.x,
            newRigidStaticData.boxColliderData.size.y * scale.y,
            newRigidStaticData.boxColliderData.size.z * scale.z);
    newRigidStaticData.sphereColliderData.radius =
        newRigidStaticData.sphereColliderData.radius * scale.x;
    RigidStatic rigidStatic = GetComponent(entity);
    rigidStatic.Init(physicsEngine_.GetPhysx(), newRigidStaticData, position, euler);
    physicsEngine_.GetScene()->addActor(*rigidStatic.GetPxRigidStatic());
    SetComponent(entity, rigidStatic);
}
const neko::physics::RigidStaticData& neko::physics::RigidStaticManager::GetRigidStaticData(
    Entity entity) const
{
    return GetComponent(entity).GetRigidStaticData();
}

void neko::physics::RigidStaticManager::SetRigidStaticData(
    Entity entity, const RigidStaticData& rigidStaticData) const
{
    if (!physicsEngine_.IsPhysicRunning()) {
        GetComponent(entity).SetRigidStaticData(rigidStaticData);
    }
}

void RigidStaticManager::DestroyComponent(Entity entity)
{
    if (entity < components_.size())
    {
        if (GetComponent(entity).GetPxRigidStatic())
        { physicsEngine_.GetScene()->removeActor(*GetComponent(entity).GetPxRigidStatic()); }
        SetComponent(entity, RigidStatic());
    }
    ComponentManager::DestroyComponent(entity);
}

Entity RigidStaticManager::FindEntityFromActor(physx::PxActor* actor)
{
    auto entityIt = std::find_if(
        components_.begin(),
        components_.end(),
        [actor](RigidStatic rigidStatic) {
            return actor == rigidStatic.GetPxRigidStatic();
        });
    if (entityIt == components_.end())
        return INVALID_ENTITY;

    return std::distance(components_.begin(), entityIt);
}

RigidStaticViewer::RigidStaticViewer(
    Transform3dManager& transform3dManager,EntityManager& entityManager,
    PhysicsEngine& physicsEngine,
    RigidStaticManager& rigidStaticManager)
    : ComponentViewer(entityManager),
      physicsEngine_(physicsEngine),
     rigidStaticManager_(rigidStaticManager),
     transform3dManager_(transform3dManager)
{}

void RigidStaticViewer::SetSelectedEntity(Entity selectedEntity)
{
    selectedEntity_ = selectedEntity;
    if (selectedEntity_ == INVALID_ENTITY) return;
    rigidStaticData_ =
        rigidStaticManager_.GetRigidStaticData(selectedEntity_);
}

void RigidStaticViewer::FixedUpdate(seconds dt) {}
json RigidStaticViewer::GetJsonFromComponent(Entity entity) const
{
    json rigidDynamicViewer = json::object();
    if (entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_STATIC)))
    {
        if (entity != INVALID_ENTITY && entityManager_.GetEntitiesSize() > entity)
        {
            RigidStaticData rigidStaticData    = rigidStaticManager_.GetRigidStaticData(entity);
            rigidStaticData.boxColliderData.size =
                Vec3f(rigidStaticData.boxColliderData.size.x /
                          transform3dManager_.GetGlobalScale(entity).x,
                    rigidStaticData.boxColliderData.size.y /
                        transform3dManager_.GetGlobalScale(entity).y,
                    rigidStaticData.boxColliderData.size.z /
                        transform3dManager_.GetGlobalScale(entity).z);
            rigidStaticData.sphereColliderData.radius =
                rigidStaticData.sphereColliderData.radius /
                transform3dManager_.GetGlobalScale(entity).x;
            RigidDynamicData rigidDynamicData;
            rigidDynamicViewer["useGravity"]     = rigidDynamicData.useGravity;
            rigidDynamicViewer["isKinematic"]    = rigidDynamicData.isKinematic;
            rigidDynamicViewer["isStatic"]       = true;
            rigidDynamicViewer["mass"]           = rigidDynamicData.mass;
            rigidDynamicViewer["linearDamping"]  = rigidDynamicData.linearDamping;
            rigidDynamicViewer["angularDamping"] = rigidDynamicData.angularDamping;
            rigidDynamicViewer["rotationLock"]   = json::object();
            rigidDynamicViewer["rotationLock"]["x"] = rigidDynamicData.freezeRotation.x;
            rigidDynamicViewer["rotationLock"]["y"] = rigidDynamicData.freezeRotation.y;
            rigidDynamicViewer["rotationLock"]["z"] = rigidDynamicData.freezeRotation.z;
            rigidDynamicViewer["positionLock"]      = json::object();
            rigidDynamicViewer["positionLock"]["x"] = rigidDynamicData.freezePosition.x;
            rigidDynamicViewer["positionLock"]["y"] = rigidDynamicData.freezePosition.y;
            rigidDynamicViewer["positionLock"]["z"] = rigidDynamicData.freezePosition.z;
            rigidDynamicViewer["boxCollider"]       = GetJsonFromBoxCollider(rigidStaticData);
            rigidDynamicViewer["sphereCollider"]    = GetJsonFromSphereCollider(rigidStaticData);
            rigidDynamicViewer["physicsMaterial"]   = GetJsonFromMaterial(rigidStaticData);
        }
    }    // namespace neko
    return rigidDynamicViewer;
    ;
}

void RigidStaticViewer::SetComponentFromJson(Entity entity, const json& componentJson)
{
    RigidStaticData rigidStaticData;
    RigidActorData rigidActorData        = GetRigidActorFromJson(componentJson);
    rigidStaticData.colliderType         = rigidActorData.colliderType;
    rigidStaticData.boxColliderData      = rigidActorData.boxColliderData;
    rigidStaticData.sphereColliderData  = rigidActorData.sphereColliderData;
    rigidStaticData.material             = rigidActorData.material;
    std::string layer = aer::TagLocator::get().GetEntityLayer(entity);
    if (layer == "Ground") {
        rigidStaticData.filterGroup = FilterGroup::GROUND;
    }
    else if (layer == "Ship") {
        rigidStaticData.filterGroup = FilterGroup::SHIP;
    }
    else if (layer == "Wall") {
        rigidStaticData.filterGroup = FilterGroup::WALL;
    }
    else {
        rigidStaticData.filterGroup = FilterGroup::DEFAULT;
    }
    rigidStaticManager_.AddRigidStatic(entity, rigidStaticData);
}

void RigidStaticViewer::DrawImGui(Entity entity)
{
    if (entity == INVALID_ENTITY) return;
    if (entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_STATIC))) {
        if (ImGui::TreeNode("Rigid Static")) {
            SetSelectedEntity(entity);
            RigidStaticData rigidStaticData     = rigidStaticData_;
            RigidActorData rigidActorData       = DrawImGuiRigidActor(rigidStaticData);
            rigidStaticData.material            = rigidActorData.material;
            rigidStaticData.boxColliderData     = rigidActorData.boxColliderData;
            rigidStaticData.sphereColliderData  = rigidActorData.sphereColliderData;
            if (!physicsEngine_.IsPhysicRunning()) {
                rigidStaticManager_.SetRigidStaticData(selectedEntity_, rigidStaticData);
                rigidStaticData_ =
                    rigidStaticManager_.GetRigidStaticData(selectedEntity_);
            }
            ImGui::TreePop();
        }
    }
}

RigidDynamicManager::RigidDynamicManager(
    EntityManager& entityManager,
    Transform3dManager& transform3dManager,
    PhysicsEngine& physicsEngine)
    : ComponentManager<RigidDynamic, EntityMask(ComponentType::RIGID_DYNAMIC)>(entityManager),
      transform3dManager_(transform3dManager),
      physicsEngine_(physicsEngine) {}

void RigidDynamicManager::FixedUpdate(seconds dt)
{
    for (neko::Entity entity = 0.0f; entity < entityManager_.get().GetEntitiesSize(); entity++) {
        if (!entityManager_.get().HasComponent(entity,
            neko::EntityMask(neko::ComponentType::RIGID_DYNAMIC))) { continue; }
        physx::PxTransform transform;
        transform = GetComponent(entity).GetPxRigidDynamic()->getGlobalPose();
        transform3dManager_.SetGlobalPosition(entity, ConvertFromPxVec(transform.p));
        physx::PxVec3 x = transform.q.getBasisVector0();
        physx::PxVec3 y = transform.q.getBasisVector1();
        physx::PxVec3 z = transform.q.getBasisVector2();
        transform3dManager_.SetGlobalRotation(entity,
            Quaternion::ToEulerAngles(ConvertFromPxQuat(transform.q)));
    }
}

void RigidDynamicManager::AddRigidDynamic(Entity entity, const RigidDynamicData& rigidDynamicData)
{
    AddComponent(entity);
    Vec3f position                        = transform3dManager_.GetGlobalPosition(entity);
    EulerAngles euler                     = transform3dManager_.GetGlobalRotation(entity);
    Vec3f scale                           = transform3dManager_.GetGlobalScale(entity);
    RigidDynamicData newRigidDynamicData  = rigidDynamicData;
    newRigidDynamicData.boxColliderData.size =
        Vec3f(newRigidDynamicData.boxColliderData.size.x * scale.x,
            newRigidDynamicData.boxColliderData.size.y * scale.y,
            newRigidDynamicData.boxColliderData.size.z * scale.z);
    newRigidDynamicData.sphereColliderData.radius =
        newRigidDynamicData.sphereColliderData.radius * scale.x;
    RigidDynamic rigidDynamic = GetComponent(entity);
    rigidDynamic.Init(physicsEngine_.GetPhysx(), newRigidDynamicData, position, euler);
    physicsEngine_.GetScene()->addActor(*rigidDynamic.GetPxRigidDynamic());
    SetComponent(entity, rigidDynamic);
}

const RigidDynamicData& RigidDynamicManager::GetRigidDynamicData(Entity entity) const
{
    return GetComponent(entity).GetRigidDynamicData();
}

const DynamicData RigidDynamicManager::GetDynamicData(Entity entity) const
{
    return GetComponent(entity).GetDynamicData();
}

void RigidDynamicManager::SetRigidDynamicData(
    Entity entity,
    const RigidDynamicData& rigidDynamicData) const
{
    if (!physicsEngine_.IsPhysicRunning()) {
        GetComponent(entity).SetRigidDynamicData(rigidDynamicData);
    }
}

void RigidDynamicManager::AddForceAtPosition(
    Entity entity,
    const Vec3f& force,
    const Vec3f& position) const { GetComponent(entity).AddForceAtPosition(force, position); }

void RigidDynamicManager::AddForce(
    Entity entity, const Vec3f& force, physx::PxForceMode::Enum forceMode) const
{
    GetComponent(entity).AddForce(force);
}

void RigidDynamicManager::AddRelativeTorque(Entity entity,
    const Vec3f& torque,
    physx::PxForceMode::Enum forceMode) const
{
    GetComponent(entity).AddRelativeTorque(torque, forceMode);
}

void RigidDynamicManager::MoveRotation(Entity entity, const Quaternion& rot) const
{
    GetComponent(entity).MoveRotation(rot);
}

void RigidDynamicManager::SetLinearVelocity(Entity entity, const Vec3f& linearVelocity) const
{
    GetComponent(entity).GetPxRigidDynamic()->setLinearVelocity(ConvertToPxVec(linearVelocity));
}

void RigidDynamicManager::SetAngularVelocity(Entity entity, const Vec3f& angularVelocity) const
{
    GetComponent(entity).GetPxRigidDynamic()->setAngularVelocity(ConvertToPxVec(angularVelocity));
}

void RigidDynamicManager::DestroyComponent(Entity entity)
{
    if (entity < components_.size())
    {
        if (GetComponent(entity).GetPxRigidDynamic())
        { physicsEngine_.GetScene()->removeActor(*GetComponent(entity).GetPxRigidDynamic()); }
        SetComponent(entity, RigidDynamic());
    }
    ComponentManager::DestroyComponent(entity);
}

Entity RigidDynamicManager::FindEntityFromActor(physx::PxActor* actor)
{
    auto entityIt = std::find_if(
        components_.begin(),
        components_.end(),
        [actor](RigidDynamic rigidDynamic) {
            return actor == rigidDynamic.GetPxRigidDynamic();
        });
    if (entityIt == components_.end())
        return INVALID_ENTITY;

    return std::distance(components_.begin(), entityIt);
}

RigidDynamicViewer::RigidDynamicViewer(Transform3dManager& transform3dManager,
                                       EntityManager& entityManager,
                                       PhysicsEngine& physicsEngine,
                                       RigidDynamicManager& rigidDynamicManager)
    : ComponentViewer(entityManager),
      physicsEngine_(physicsEngine),
      rigidDynamicManager_(rigidDynamicManager),
        transform3dManager_(transform3dManager){ }


void RigidDynamicViewer::SetSelectedEntity(Entity selectedEntity)
{
    selectedEntity_ = selectedEntity;
    if (selectedEntity_ == INVALID_ENTITY) return;
    if (!physicsEngine_.IsPhysicRunning())
    {
        lastSelectedEntity_ = selectedEntity_;
        rigidDynamicData_ = rigidDynamicManager_.GetRigidDynamicData(lastSelectedEntity_);
        dynamicData_        = rigidDynamicManager_.GetDynamicData(lastSelectedEntity_);
        lastSelectedEntity_ = selectedEntity_;
    }
}


void RigidDynamicViewer::FixedUpdate(seconds dt)
{
    lastSelectedEntity_ = selectedEntity_;
    if (lastSelectedEntity_ == INVALID_ENTITY) return;
    if (entityManager_.HasComponent(
        lastSelectedEntity_,
        static_cast<EntityMask>(ComponentType::RIGID_DYNAMIC))) {
        dynamicData_      = rigidDynamicManager_.GetDynamicData(lastSelectedEntity_);
        rigidDynamicData_ = rigidDynamicManager_.GetRigidDynamicData(lastSelectedEntity_);

    }
}

json RigidDynamicViewer::GetJsonFromComponent(Entity entity) const
{
    json rigidDynamicViewer = json::object();
    if (entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_DYNAMIC)))
    {
        if (entity != INVALID_ENTITY && entityManager_.GetEntitiesSize() > entity)
        {
            RigidDynamicData rigidDynamicData = rigidDynamicManager_.GetRigidDynamicData(entity);
            rigidDynamicData.boxColliderData.size =
                Vec3f(rigidDynamicData.boxColliderData.size.x /
                          transform3dManager_.GetGlobalScale(entity).x,
                    rigidDynamicData.boxColliderData.size.y /
                        transform3dManager_.GetGlobalScale(entity).y,
                    rigidDynamicData.boxColliderData.size.z /
                        transform3dManager_.GetGlobalScale(entity).z);
            rigidDynamicData.sphereColliderData.radius =
                rigidDynamicData.sphereColliderData.radius /
                transform3dManager_.GetGlobalScale(entity).x;
            rigidDynamicViewer["useGravity"]  = rigidDynamicData.useGravity;
            rigidDynamicViewer["isKinematic"] = rigidDynamicData.isKinematic;
            rigidDynamicViewer["isStatic"]    = false;
            rigidDynamicViewer["mass"]          = rigidDynamicData.mass;
            rigidDynamicViewer["linearDamping"] = rigidDynamicData.linearDamping;
            rigidDynamicViewer["angularDamping"] = rigidDynamicData.angularDamping;
            rigidDynamicViewer["rotationLock"] = json::object();
            rigidDynamicViewer["rotationLock"]["x"]    = rigidDynamicData.freezeRotation.x;
            rigidDynamicViewer["rotationLock"]["y"]    = rigidDynamicData.freezeRotation.y;
            rigidDynamicViewer["rotationLock"]["z"]    = rigidDynamicData.freezeRotation.z;
            rigidDynamicViewer["positionLock"]         = json::object();
            rigidDynamicViewer["positionLock"]["x"]    = rigidDynamicData.freezePosition.x;
            rigidDynamicViewer["positionLock"]["y"]    = rigidDynamicData.freezePosition.y;
            rigidDynamicViewer["positionLock"]["z"]    = rigidDynamicData.freezePosition.z;
            rigidDynamicViewer["boxCollider"]       = GetJsonFromBoxCollider(rigidDynamicData);
            rigidDynamicViewer["sphereCollider"]       = GetJsonFromSphereCollider(rigidDynamicData);
            rigidDynamicViewer["physicsMaterial"] = GetJsonFromMaterial(rigidDynamicData);
        }

    }    // namespace neko
    return rigidDynamicViewer;
    ;
}

void RigidDynamicViewer::SetComponentFromJson(Entity entity, const json& componentJson)
{
    RigidDynamicData rigidDynamicData;
    if (CheckJsonParameter(componentJson, "useGravity", json::value_t::boolean)) {
        rigidDynamicData.useGravity = componentJson["useGravity"];
    }
    if (CheckJsonParameter(componentJson, "isKinematic", json::value_t::boolean)) { rigidDynamicData.isKinematic = componentJson["isKinematic"];
    }
    if (CheckJsonNumber(componentJson, "mass")) { rigidDynamicData.mass = componentJson["mass"];
    }
    if (CheckJsonNumber(componentJson, "linearDamping"))
    { rigidDynamicData.linearDamping = componentJson["linearDamping"];
    }
    if (CheckJsonNumber(componentJson, "angularDamping"))
    { rigidDynamicData.angularDamping = componentJson["angularDamping"];
    }
    if (CheckJsonParameter(componentJson, "rotationLock", json::value_t::object)) {
        if (CheckJsonParameter(componentJson["rotationLock"], "x", json::value_t::boolean)) {
            rigidDynamicData.freezeRotation.x = componentJson["rotationLock"]["x"];
        }
        if (CheckJsonParameter(componentJson["rotationLock"], "y", json::value_t::boolean)) {
            rigidDynamicData.freezeRotation.y = componentJson["rotationLock"]["y"];
        }
        if (CheckJsonParameter(componentJson["rotationLock"], "z", json::value_t::boolean)) {
            rigidDynamicData.freezeRotation.z = componentJson["rotationLock"]["z"];
        }
    }
    if (CheckJsonParameter(componentJson, "positionLock", json::value_t::object)) {
        if (CheckJsonParameter(componentJson["positionLock"], "x", json::value_t::boolean)) {
            rigidDynamicData.freezePosition.x = componentJson["positionLock"]["x"];
        }
        if (CheckJsonParameter(componentJson["positionLock"], "y", json::value_t::boolean)) {
            rigidDynamicData.freezePosition.y = componentJson["positionLock"]["y"];
        }
        if (CheckJsonParameter(componentJson["positionLock"], "z", json::value_t::boolean)) {
            rigidDynamicData.freezePosition.z = componentJson["positionLock"]["z"];
        }
    }
    RigidActorData rigidActorData       = GetRigidActorFromJson(componentJson);
    rigidDynamicData.colliderType    = rigidActorData.colliderType;
    rigidDynamicData.boxColliderData    = rigidActorData.boxColliderData;
    rigidDynamicData.sphereColliderData = rigidActorData.sphereColliderData;
    rigidDynamicData.material             = rigidActorData.material;
    std::string layer = aer::TagLocator::get().GetEntityLayer(entity);
    if (layer == "Ground") {
        rigidDynamicData.filterGroup = FilterGroup::GROUND;
    }
    else if (layer == "Ship") {
        rigidDynamicData.filterGroup = FilterGroup::SHIP;
    }
    else if (layer == "Wall") {
        rigidDynamicData.filterGroup = FilterGroup::WALL;
    }
    else {
        rigidDynamicData.filterGroup = FilterGroup::DEFAULT;
    }
    rigidDynamicManager_.AddRigidDynamic(entity, rigidDynamicData);
}

void RigidDynamicViewer::DrawImGui(Entity entity)
{
    if (entity == INVALID_ENTITY) return;
    if (entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_DYNAMIC))) {
        SetSelectedEntity(entity);
        if (lastSelectedEntity_ == INVALID_ENTITY) return;
        if (entityManager_.HasComponent(
                lastSelectedEntity_, EntityMask(ComponentType::RIGID_DYNAMIC)))
        {
            if (ImGui::TreeNode("Rigid Dynamic"))
            {
                RigidDynamicData rigidDynamicData = rigidDynamicData_;
                neko::Vec3f linearVelocity        = dynamicData_.linearVelocity;
                ImGui::DragFloat3("linearVelocity", linearVelocity.coord, 0);
                ImGui::DragFloat("linearDamping", &rigidDynamicData.linearDamping);
                if (rigidDynamicData.linearDamping < 0) { rigidDynamicData.linearDamping = 0; }
                neko::Vec3f angularVelocity = dynamicData_.angularVelocity;
                ImGui::DragFloat3("angularVelocity", angularVelocity.coord, 0);
                ImGui::DragFloat("angularDamping", &rigidDynamicData.angularDamping);
                if (rigidDynamicData.angularDamping < 0) { rigidDynamicData.angularDamping = 0; }
                ImGui::DragFloat("mass", &rigidDynamicData.mass, 0.5f);
                if (rigidDynamicData.mass < 0) { rigidDynamicData.mass = 0; }
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
                RigidActorData rigidActorData       = DrawImGuiRigidActor(rigidDynamicData);
                rigidDynamicData.material           = rigidActorData.material;
                rigidDynamicData.boxColliderData    = rigidActorData.boxColliderData;
                rigidDynamicData.sphereColliderData = rigidActorData.sphereColliderData;
                ImGui::TreePop();
                if (!physicsEngine_.IsPhysicRunning())
                {
                    rigidDynamicManager_.SetRigidDynamicData(lastSelectedEntity_, rigidDynamicData);
                    rigidDynamicData_ =
                        rigidDynamicManager_.GetRigidDynamicData(lastSelectedEntity_);
                    dynamicData_ = rigidDynamicManager_.GetDynamicData(lastSelectedEntity_);
                    lastSelectedEntity_ = selectedEntity_;
                }
            }
        }

    }
}
}
