#include "aer/managers/ship_controller_manager.h"

#include <imgui.h>

#include "aer/tag.h"
#include "px/raycast.h"
#include "px/physx_utility.h"
#include <aer\log.h>

namespace neko::aer
{
    ShipControllerManager::ShipControllerManager(
        EntityManager& entityManager,
        Transform3dManager& transform3DManager,
        physics::RigidDynamicManager& rigidDynamicManager,
        physics::RigidStaticManager& rigidStaticManager,
        physics::PhysicsEngine& physicsEngine)
        : ComponentManager<ShipController, EntityMask(ComponentType::SHIP_CONTROLLER)>(entityManager),
        transformManager_(transform3DManager),
        rigidDynamicManager_(rigidDynamicManager),
        rigidStaticManager_(rigidStaticManager),
        physicsEngine_(physicsEngine)
{
}

void ShipControllerManager::AddComponent(Entity entity)
{
   ComponentManager::AddComponent(entity);
   ShipController shipController = GetComponent(entity);
   entityManager_.get().SetEntityName(entity, "ship");
    //Set the startHoverHeight base height reference has the hoverHeight value.
   shipController.startHoverHeight = shipParameter_.kHoverHeight;

    //Set the drag to be the forwardForce / terminalVelocity.
   shipController.drag = shipParameter_.kForwardForce / shipParameter_.kTerminalVelocity;
}


void ShipControllerManager::Init()
{

}

void ShipControllerManager::FixedUpdate(seconds dt) {
    const auto& entities =
        entityManager_.get().FilterEntities(static_cast<EntityMask>(ComponentType::SHIP_CONTROLLER));
    //LogDebug(std::to_string(dt.count()));
    for (auto& entity : entities)
    {
       
        CalculateHover(entity, dt);
        CalculateThrust(entity, dt);
    }
}


void ShipControllerManager::Update(seconds dt)
{
    shipInputManager_.Update(dt);
}

void ShipControllerManager::CalculateHover(Entity entity, seconds dt)
{
    ShipController shipController = GetComponent(entity);
    physics::RigidDynamic rigidDynamic = rigidDynamicManager_.GetComponent(entity);

    //Raycast to ground
    Vec3f groundNormal = Vec3f::zero;
    Vec3f shipPosition = transformManager_.GetGlobalPosition(entity);
    const physics::RaycastInfo& raycastInfo = physicsEngine_.Raycast(
        shipPosition,
        Vec3f::down,
        shipParameter_.kMaxGroundDist,
        physics::FilterGroup::GROUND);

    shipController.isOnGround = raycastInfo.touch;

    if(shipController.isOnGround)
    {
        float height = raycastInfo.GetDistance();
        groundNormal = raycastInfo.GetNormal().Normalized();
        float forcePercent = shipController.hoverPid.Seek(shipParameter_.kHoverHeight, height, dt.count());
        Vec3f force = groundNormal * shipParameter_.kHoverForce * forcePercent;
        Vec3f gravity = -groundNormal * shipParameter_.kHoverGravity * height;
       
        rigidDynamic.AddForce(force, physx::PxForceMode::eACCELERATION);
        rigidDynamic.AddForce(gravity, physx::PxForceMode::eACCELERATION);
    }
    else
    {
        groundNormal = Vec3f::up;
        Vec3f gravity = -groundNormal * shipParameter_.kFallGravity;

        rigidDynamic.AddForce(gravity, physx::PxForceMode::eACCELERATION);
    }

    float rotationSpeed = 0.0f;

    if(shipController.isOnGround)
    {
        rotationSpeed = 10.0f;
    }
        else
    {
        rotationSpeed = 1.0f;
    }
    //LogDebug(groundNormal.ToString());
    Vec3f forward = Quaternion::FromEuler(transformManager_.GetGlobalRotation(entity)) * Vec3f::forward;
    Vec3f projection = Vec3f::ProjectOnPlane(forward, groundNormal);
    
    Quaternion rotation = Quaternion::LookRotation(projection, groundNormal);
    //GizmosLocator::get().DrawLine(shipPosition, shipPosition + forward * 10.0f, Color::green, 5.0f);
    //GizmosLocator::get().DrawLine(shipPosition, shipPosition + projection * 10.0f, Color::yellow, 5.0f);
    //GizmosLocator::get().DrawLine(shipPosition, shipPosition + groundNormal * 3.0f, Color::yellow, 5.0f);
    //Vec3f::OrthoNormalize(projection, groundNormal);
    //GizmosLocator::get().DrawLine(shipPosition, shipPosition + projection * 12.0f, Color::magenta, 5.0f);
    //GizmosLocator::get().DrawLine(shipPosition, shipPosition + groundNormal * 5.0f, Color::magenta, 5.0f);
    //GizmosLocator::get().DrawLine(shipPosition, shipPosition + (rotation * Vec3f::forward) * 10.0f, Color::red, 5.0f);
    //GizmosLocator::get().DrawLine(shipPosition, shipPosition + (rotation * Vec3f::up) * 3.0f, Color::red, 5.0f);
    Quaternion shipRotation = Quaternion::FromEuler(transformManager_.GetGlobalRotation(entity));
    rigidDynamic.MoveRotation(Quaternion::Lerp(shipRotation,rotation, dt.count() * shipParameter_.kRotationMultiplicator));
    float angle = shipParameter_.kAngleOfRoll * -shipInputManager_.rudder_ * shipInputManager_.GetIntensity();
    float pitchAngle = shipParameter_.kAngleOfPitch * shipInputManager_.thruster_ * shipInputManager_.GetIntensity();
    Quaternion bodyRotation = Quaternion::FromEuler(transformManager_.GetGlobalRotation(entity)) * Quaternion::FromEuler(EulerAngles(pitchAngle, 0.0f, angle));

    //transformManager_.SetGlobalRotation(entity, 
    //    Quaternion::ToEulerAngles(
    //        Quaternion::Lerp(
    //            Quaternion::FromEuler(transformManager_.GetGlobalRotation(entity)), 
    //            bodyRotation, 
    //            dt.count() * 10.0f)));

    SetComponent(entity, shipController);
}

void ShipControllerManager::CalculateThrust(Entity entity, seconds dt)
{
    ShipController shipController = GetComponent(entity);
    physics::RigidDynamic rigidDynamic = rigidDynamicManager_.GetComponent(entity);

    float rotationTorque = (shipInputManager_.rudder_ * shipParameter_.kRotationMultiplicator) - rigidDynamic.GetDynamicData().angularVelocity.y;

    rigidDynamic.AddRelativeTorque(Vec3f(0.0f, rotationTorque, 0.0f), physx::PxForceMode::eVELOCITY_CHANGE);

    Vec3f right = Quaternion::FromEuler(transformManager_.GetGlobalRotation(entity)) * Vec3f::right;
    float sidewaySpeed = Vec3f::Dot(rigidDynamic.GetDynamicData().linearVelocity, right);

    Vec3f sideFriction = -right * (sidewaySpeed / dt.count());

    rigidDynamic.AddForce(sideFriction, physx::PxForceMode::eACCELERATION);

    if(shipInputManager_.thruster_ <= 0.0f) {
        rigidDynamicManager_.SetLinearVelocity(entity, rigidDynamic.GetDynamicData().linearVelocity * shipParameter_.kSlowingVelFactor);
    }

    if(!shipController.isOnGround) {
        return;
    }

    if(shipInputManager_.isBreaking_) {
        rigidDynamicManager_.SetLinearVelocity(entity, rigidDynamic.GetDynamicData().linearVelocity * shipParameter_.kBrakingVelFactor);
    }

    Vec3f forward = Quaternion::FromEuler(transformManager_.GetGlobalRotation(entity)) * Vec3f::forward;
    float propultion = (shipParameter_.kForwardForce * shipInputManager_.thruster_ * shipParameter_.kPropultionMultiplicator) -
        (shipController.drag * Clamp(Vec3f::Dot(rigidDynamic.GetDynamicData().linearVelocity, forward), 0.0f, shipParameter_.kTerminalVelocity) *
            shipParameter_.kPropultionMultiplicator);
    rigidDynamic.AddForce(forward * propultion, physx::PxForceMode::eACCELERATION);
}

void ShipControllerManager::OnCollisionEnter(
    const physx::PxContactPairHeader& pairHeader)
{
    //physx::PxContactPairPoint contactPointBuffer[16];
    //int32_t numContactPoints = pairHeader.pairs->extractContacts(contactPointBuffer, 16);
    //LogDebug(std::to_string(numContactPoints));
    //const Entity entity1 = rigidDynamicManager_.FindEntityFromActor(pairHeader.actors[0]);
    //if (entity1 != INVALID_ENTITY)
    //{
    //    if (entityManager_.get().HasComponent(entity1, EntityMask(ComponentType::SHIP_CONTROLLER))) {
    //        physx::PxContactPairPoint contactPointBuffer[16];
    //        int32_t numContactPoints = pairHeader.pairs->extractContacts(contactPointBuffer, 16);
    //        LogDebug(std::to_string(numContactPoints));
    //        if (Vec3f::Angle(Vec3f::up, physics::ConvertFromPxVec(contactPointBuffer->normal)) > degree_t(80.0f))
    //        {
    //            physics::RigidDynamic rigidDynamic = rigidDynamicManager_.GetComponent(entity1);
    //            rigidDynamic.AddForce(physics::ConvertFromPxVec(contactPointBuffer->normal) * shipParameter_.kBounceForce, physx::PxForceMode::eIMPULSE);
    //        }
    //    }
    //}
    //const Entity entity2 = rigidDynamicManager_.FindEntityFromActor(pairHeader.actors[0]);
    //if (entity2 == INVALID_ENTITY) return;
    //if (entityManager_.get().HasComponent(entity2, EntityMask(ComponentType::SHIP_CONTROLLER))) {
    //    physx::PxContactPairPoint contactPointBuffer[16];
    //    int32_t numContactPoints = pairHeader.pairs->extractContacts(contactPointBuffer, 16);
    //    LogDebug(std::to_string(numContactPoints));
    //    if (Vec3f::Angle(Vec3f::up, physics::ConvertFromPxVec(contactPointBuffer->normal)) > degree_t(80.0f))
    //    {
    //        physics::RigidDynamic rigidDynamic = rigidDynamicManager_.GetComponent(entity1);
    //        rigidDynamic.AddForce(physics::ConvertFromPxVec(contactPointBuffer->normal) * shipParameter_.kBounceForce, physx::PxForceMode::eIMPULSE);
    //    }
    //}
}


void ShipControllerManager::Destroy()
{
	
}

ShipControllerViewer::ShipControllerViewer(EntityManager& entityManager, ShipControllerManager& shipControllerManager) : ComponentViewer(entityManager), shipControllerManager_(shipControllerManager)
{
}

json ShipControllerViewer::GetJsonFromComponent(Entity entity) const
{
    json rendererComponent = json::object();
    if (entityManager_.HasComponent(entity, EntityMask(ComponentType::SHIP_CONTROLLER)))
    {
        if (entity != INVALID_ENTITY && entityManager_.GetEntitiesSize() > entity)
        {
            
        }
    }
    return rendererComponent;
}

void ShipControllerViewer::SetComponentFromJson(Entity entity, const json& componentJson)
{
    
}

void ShipControllerViewer::DrawImGui(Entity entity)
{
    if (entity == INVALID_ENTITY) return;
    if (entityManager_.HasComponent(entity, EntityMask(ComponentType::SHIP_CONTROLLER))) {
        if (ImGui::TreeNode("ShipController")) {
            ImGui::Text("Param 1");
            ImGui::TreePop();
        }
    }
}

float PID::Seek(float seekValue, float currentValue, float deltaTime)
{
    float proportional = seekValue - currentValue;

    float derivative = (proportional - lastProportional) / deltaTime;
    integral += proportional * deltaTime;
    lastProportional = proportional;

    float value = pCoeff * proportional + iCoeff * integral + dCoeff * derivative;
    value = Clamp(value, minimum, maximum);

    return value;
}


}    // namespace neko::aer