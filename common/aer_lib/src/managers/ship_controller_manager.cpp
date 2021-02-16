#include "aer/managers/ship_controller_manager.h"

#include <imgui.h>

#include "aer/tag.h"
#include "px/raycast.h"

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
   shipController.startHoverHeight_ = shipController.hoverHeight_;

    //Set the drag to be the forwardForce / terminalVelocity.
   shipController.drag_ = shipController.forwardForce_ / shipController.terminalVelocity_;
}


void ShipControllerManager::Init()
{

}

void ShipControllerManager::FixedUpdate(seconds dt) {
    const auto& entities =
        entityManager_.get().FilterEntities(static_cast<EntityMask>(ComponentType::SHIP_CONTROLLER));

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
    physics::RigidDynamicData data = rigidDynamic.GetRigidDynamicData();
    data.useGravity = false;

    rigidDynamicManager_.SetRigidDynamicData(entity, data);

    //Raycast to ground
    Vec3f groundNormal = Vec3f::zero;
    Vec3f shipPosition = transformManager_.GetGlobalPosition(entity);
    const physics::RaycastInfo& raycastInfo = physicsEngine_.Raycast(
        shipPosition,
        Vec3f::down,
        10.0f,
        physics::FilterGroup::GROUND);
    shipController.isOnGround_ = raycastInfo.GetDistance() < shipController.maxGroundDist_;

    if(shipController.isOnGround_)
    {
        float height = raycastInfo.GetDistance();
        groundNormal = raycastInfo.GetNormal().Normalized();
        float forcePercent = shipController.hoverPID.Seek(shipController.hoverHeight_, height, dt.count());
        Vec3f force = groundNormal * shipController.hoverForce_ * forcePercent;
        Vec3f gravity = -groundNormal * shipController.hoverGravity_ * height;
       
        rigidDynamic.AddForce(force, physx::PxForceMode::eACCELERATION);
        rigidDynamic.AddForce(gravity, physx::PxForceMode::eACCELERATION);
    }
    else
    {
        groundNormal = transformManager_.GetGlobalPosition(entity).up;
        Vec3f gravity = -groundNormal * shipController.fallGravity_;

        rigidDynamic.AddForce(gravity, physx::PxForceMode::eACCELERATION);
    }

    float rotationSpeed = 0.0f;

    if(shipController.isOnGround_)
    {
        rotationSpeed = 10.0f;
    }
        else
    {
        rotationSpeed = 1.0f;
    }

    Vec3f forward = Quaternion::FromEuler(transformManager_.GetGlobalRotation(entity)) * Vec3f::forward;
    Vec3f projection = Vec3f::ProjectOnPlane(forward, groundNormal);
 
    Quaternion rotation = Quaternion::LookRotation(projection, groundNormal);

    Quaternion shipRotation = Quaternion::FromEuler(transformManager_.GetGlobalRotation(entity));
    rigidDynamic.MoveRotation(Quaternion::Lerp(shipRotation,rotation, dt.count() * shipController.rotationMultiplicator_));

    float angle = shipController.angleOfRoll_ * -shipInputManager_.rudder_ * shipInputManager_.GetIntensity();
    float pitchAngle = shipController.angleOfPitch_ * shipInputManager_.thruster_ * shipInputManager_.GetIntensity();
    Quaternion bodyRotation = Quaternion::FromEuler(transformManager_.GetGlobalRotation(entity)) * Quaternion::FromEuler(EulerAngles(pitchAngle, 0.0f, angle));

    transformManager_.SetGlobalRotation(entity, 
        Quaternion::ToEulerAngles(
            Quaternion::Lerp(
                Quaternion::FromEuler(transformManager_.GetGlobalRotation(entity)), 
                bodyRotation, 
                dt.count() * 10.0f)));

    SetComponent(entity, shipController);
}

void ShipControllerManager::CalculateThrust(Entity entity, seconds dt)
{
    ShipController shipController = GetComponent(entity);
    physics::RigidDynamic rigidDynamic = rigidDynamicManager_.GetComponent(entity);

    float rotationTorque = (shipInputManager_.rudder_ * shipController.rotationMultiplicator_) - rigidDynamic.GetDynamicData().angularVelocity.y;

    rigidDynamic.AddRelativeTorque(Vec3f(0.0f, rotationTorque, 0.0f), physx::PxForceMode::eVELOCITY_CHANGE);

    Vec3f right = Quaternion::FromEuler(transformManager_.GetGlobalRotation(entity)) * Vec3f::right;
    float sidewaySpeed = Vec3f::Dot(rigidDynamic.GetDynamicData().linearVelocity, right);

    Vec3f sideFriction = -right * (sidewaySpeed / dt.count());

    rigidDynamic.AddForce(sideFriction, physx::PxForceMode::eACCELERATION);

    if(shipInputManager_.thruster_ <= 0.0f) {
        rigidDynamicManager_.SetLinearVelocity(entity, rigidDynamic.GetDynamicData().linearVelocity * shipController.slowingVelFactor_);
    }

    if(!shipController.isOnGround_) {
        return;
    }

    if(shipInputManager_.isBreaking_) {
        rigidDynamicManager_.SetLinearVelocity(entity, rigidDynamic.GetDynamicData().linearVelocity * shipController.brakingVelFactor_);
    }

    Vec3f forward = Quaternion::FromEuler(transformManager_.GetGlobalRotation(entity)) * Vec3f::forward;
    float propultion = (shipController.forwardForce_ * shipInputManager_.thruster_ * shipController.propultionMultiplicator_) -
        (shipController.drag_ * Clamp(Vec3f::Dot(rigidDynamic.GetDynamicData().linearVelocity, forward), 0.0f, shipController.terminalVelocity_) *
            shipController.propultionMultiplicator_);
    rigidDynamic.AddForce(forward * propultion, physx::PxForceMode::eACCELERATION);
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