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
        physics::PhysicsEngine& physicsEngine,
        ShipInputManager& shipInputManager,
        PlayerManager& playerManager)
        : playerManager_(playerManager),
          shipInputManager_(shipInputManager),
          transformManager_(transform3DManager),
          entityManager_(entityManager),
          rigidDynamicManager_(rigidDynamicManager),
          rigidStaticManager_(rigidStaticManager),
          physicsEngine_(physicsEngine)
    {
        shipControllers_.resize(INIT_PLAYER_NMB, ShipController());
    }

void ShipControllerManager::InitComponent(PlayerId player)
{
   ShipController& shipController = shipControllers_[player];
    //Set the startHoverHeight base height reference has the hoverHeight value.
   shipController.startHoverHeight = shipParameter_.kHoverHeight;

    //Set the drag to be the forwardForce / terminalVelocity.
   shipController.drag = shipParameter_.kForwardForce / shipParameter_.kTerminalVelocity;
}

void ShipControllerManager::RotorRotation(PlayerId playerId) {
    PlayerComponent playerComponent = playerManager_.GetPlayerComponent(playerId);
    Entity& rightRotor = playerComponent.rightRotorModel;
    Entity& leftRotor = playerComponent.leftRotorModel;

    Vec3f currentRightRotorRotation = Vec3f(transformManager_.GetRelativeRotation(rightRotor));
    transformManager_.SetRelativeRotation(rightRotor, EulerAngles(Vec3f(currentRightRotorRotation.x, currentRightRotorRotation.y-shipParameter_.kRotorRotationSpeed, currentRightRotorRotation.z)));

    Vec3f currentLeftRotorRotation = Vec3f(transformManager_.GetRelativeRotation(leftRotor));
    transformManager_.SetRelativeRotation(leftRotor, EulerAngles(Vec3f(currentLeftRotorRotation.x, currentLeftRotorRotation.y - shipParameter_.kRotorRotationSpeed, currentLeftRotorRotation.z)));
}


void ShipControllerManager::Init()
{

}

void ShipControllerManager::FixedUpdate(seconds dt) {
    for (PlayerId playerId = 0; playerId < playerManager_.GetPlayerCount(); ++playerId) {
        Entity shipEntity = playerManager_.GetShipEntity(playerId);
        if (shipEntity == INVALID_ENTITY) continue;
        if (!entityManager_.HasComponent(shipEntity, EntityMask(ComponentType::RIGID_DYNAMIC))) continue;
        CalculateHover(playerId, dt);
        CalculateThrust(playerId, dt);
        RotorMovement(playerId);
        RotorRotation(playerId);
    }
}


void ShipControllerManager::Update(seconds dt)
{
    shipInputManager_.Update(dt);
}

void ShipControllerManager::CalculateHover(PlayerId playerId, seconds dt)
{
    ShipController& shipController = shipControllers_[playerId];

    Entity shipEntity = playerManager_.GetShipEntity(playerId);
    if (shipEntity == INVALID_ENTITY) return;
    physics::RigidDynamic rigidDynamic = rigidDynamicManager_.GetComponent(shipEntity);

    //Raycast to ground
    Vec3f groundNormal = Vec3f::zero;
    Vec3f shipPosition = transformManager_.GetGlobalPosition(shipEntity);
    Vec3f rayPos =  shipPosition;
    Quaternion shipRotation = Quaternion::FromEuler(transformManager_.GetGlobalRotation(shipEntity));
    Vec3f forward = shipRotation * Vec3f::forward;
    //Vec3f rayPos =  forward + shipPosition;
    const physics::RaycastInfo& raycastInfo = physicsEngine_.Raycast(
        rayPos,
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
    Vec3f projection = Vec3f::ProjectOnPlane(forward, groundNormal);
    
    Quaternion rotation = Quaternion::LookRotation(projection, groundNormal);
    //GizmosLocator::get().DrawLine(shipPosition, shipPosition + forward * 10.0f, Color::green, 5.0f);
    //GizmosLocator::get().DrawLine(shipPosition, shipPosition + (shipRotation * Vec3f::up) * 3.0f, Color::green, 5.0f);
    //GizmosLocator::get().DrawLine(shipPosition, shipPosition + (shipRotation * Vec3f::right) * 3.0f, Color::green, 5.0f);
    //GizmosLocator::get().DrawLine(shipPosition, shipPosition + projection * 10.0f, Color::yellow, 5.0f);
    //GizmosLocator::get().DrawLine(shipPosition, shipPosition + groundNormal * 3.0f, Color::yellow, 5.0f);
    //GizmosLocator::get().DrawLine(shipPosition, shipPosition + (rotation * Vec3f::forward) * 10.0f, Color::red, 5.0f);
    //GizmosLocator::get().DrawLine(shipPosition, shipPosition + (rotation * Vec3f::up) * 3.0f, Color::red, 5.0f);
    //GizmosLocator::get().DrawLine(shipPosition, shipPosition + (rotation * Vec3f::right) * 3.0f, Color::red, 5.0f);
    rigidDynamic.MoveRotation(Quaternion::Lerp(shipRotation,rotation, shipParameter_.kAngleRoadLerp));
    float angle = 0; 
    float pitchAngle = 0; 

        if (shipController.canMove)
        {
            angle = shipParameter_.kAngleOfRoll * -shipInputManager_.GetRudder(playerId) * shipInputManager_.GetIntensity(playerId);
            float thrust = shipInputManager_.GetThruster(playerId);

            if(thrust > shipParameter_.kAngleDeadzoneLimit) {
                shipController.angleMultiplicator += shipParameter_.kAngleChangeSpeed;
            }
            else if(thrust < -shipParameter_.kAngleDeadzoneLimit){
                shipController.angleMultiplicator -= shipParameter_.kAngleChangeSpeed;
            }
            else if(thrust > -shipParameter_.kAngleDeadzoneLimit && thrust < shipParameter_.kAngleDeadzoneLimit) {
                if(shipController.angleMultiplicator < -shipParameter_.kAngleDeadzoneLimit) {
                    shipController.angleMultiplicator += shipParameter_.kAngleChangeSpeed;
                }
                else if(shipController.angleMultiplicator > shipParameter_.kAngleDeadzoneLimit) {
                    shipController.angleMultiplicator -= shipParameter_.kAngleChangeSpeed;
                }
                else {
                    shipController.angleMultiplicator = 0;
                }
            }

            if(shipController.angleMultiplicator >= 1) {
                shipController.angleMultiplicator = 1;
            }
            if(shipController.angleMultiplicator <= -1) {
                shipController.angleMultiplicator = -1;
            }

            //Automated pitch
            pitchAngle = shipParameter_.kAngleOfPitch * shipController.angleMultiplicator;

            //Pitch with user input
            //pitchAngle = shipParameter_.kAngleOfPitch *shipInputManager_.GetThruster(playerId) * shipInputManager_.GetIntensity(playerId);
        }

    Entity shipModelEntity = playerManager_.GetPlayerComponent(playerId).shipModelEntity;
    Quaternion bodyRotation = Quaternion::FromEuler(EulerAngles(pitchAngle, 0.0f, angle));

    transformManager_.SetRelativeRotation(shipModelEntity, Quaternion::ToEulerAngles(bodyRotation));
    //transformManager_.SetGlobalRotation(entity, 
    //    Quaternion::ToEulerAngles(
    //        Quaternion::Lerp(
    //            Quaternion::FromEuler(transformManager_.GetGlobalRotation(entity)), 
    //            bodyRotation, 
    //            dt.count() * 10.0f)));

}

void ShipControllerManager::CalculateThrust(PlayerId playerId, seconds dt)
{
    ShipController& shipController = shipControllers_[playerId];
    Entity shipEntity = playerManager_.GetShipEntity(playerId);
    if (shipEntity == INVALID_ENTITY) return;
    physics::RigidDynamic rigidDynamic = rigidDynamicManager_.GetComponent(shipEntity);

    float rotationTorque = 0; 

    if (shipController.canMove)
    {
        rotationTorque = (shipInputManager_.GetRudder(playerId) * shipParameter_.kRotationMultiplicator) - rigidDynamic.GetDynamicData().angularVelocity.y;
    }

    rigidDynamic.AddRelativeTorque(Vec3f(0.0f, rotationTorque, 0.0f), physx::PxForceMode::eVELOCITY_CHANGE);

    Vec3f right = Quaternion::FromEuler(transformManager_.GetGlobalRotation(shipEntity)) * Vec3f::right;
    float sidewaySpeed = Vec3f::Dot(rigidDynamic.GetDynamicData().linearVelocity, right);

    Vec3f sideFriction = -right * (sidewaySpeed / dt.count());

    rigidDynamic.AddForce(sideFriction, physx::PxForceMode::eACCELERATION);

    //TODO
    float currentThruster = 0;
    bool currentBreaking = false;
        if (shipController.canMove)
        {
            currentThruster = shipInputManager_.GetThruster(playerId);
            currentBreaking = shipInputManager_.IsBreaking(playerId);
        }

    if(currentThruster <= 0.0f) {
        rigidDynamicManager_.SetLinearVelocity(shipEntity, rigidDynamic.GetDynamicData().linearVelocity * shipParameter_.kSlowingVelFactor);
    }

    if(!shipController.isOnGround) {
        return;
    }
    if(currentBreaking) {
        rigidDynamicManager_.SetLinearVelocity(shipEntity, rigidDynamic.GetDynamicData().linearVelocity * shipParameter_.kBrakingVelFactor);
    }

    Vec3f forward = Quaternion::FromEuler(transformManager_.GetGlobalRotation(shipEntity)) * Vec3f::forward;
    float propulsion = (shipParameter_.kForwardForce * currentThruster * shipParameter_.kPropultionMultiplicator) -
        (shipController.drag * Clamp(Vec3f::Dot(rigidDynamic.GetDynamicData().linearVelocity, forward), 0.0f, shipParameter_.kTerminalVelocity) *
            shipParameter_.kPropultionMultiplicator);
    rigidDynamic.AddForce(forward * propulsion, physx::PxForceMode::eACCELERATION);
}

void ShipControllerManager::RotorMovement(PlayerId playerId) {

    PlayerComponent playerComponent = playerManager_.GetPlayerComponent(playerId);

    ShipController& shipController = shipControllers_[playerId];

    Vec3f rightRotation = Vec3f::zero;
    if (shipController.canMove)
    {
        rightRotation.x = shipInputManager_.GetJoystickAxis(playerId, ShipInputManager::Joystick::Right, ShipInputManager::Axis::Vertical) * shipParameter_.kRotorMaxAngle;
        rightRotation.z = shipInputManager_.GetJoystickAxis(playerId, ShipInputManager::Joystick::Right, ShipInputManager::Axis::Horizontal) * shipParameter_.kRotorMaxAngle;
    }

    Vec3f leftRotation = Vec3f::zero;
    if (shipController.canMove)
    {
        leftRotation.x = shipInputManager_.GetJoystickAxis(playerId, ShipInputManager::Joystick::Left, ShipInputManager::Axis::Vertical) * shipParameter_.kRotorMaxAngle;
        leftRotation.z = shipInputManager_.GetJoystickAxis(playerId, ShipInputManager::Joystick::Left, ShipInputManager::Axis::Horizontal) * shipParameter_.kRotorMaxAngle;
    }

    transformManager_.SetRelativeRotation(playerComponent.rightRotorAnchor, EulerAngles(rightRotation));
    transformManager_.SetRelativeRotation(playerComponent.leftRotorAnchor, EulerAngles(leftRotation));
    

// <<<<<<< HEAD
//     ShipController& shipController = shipControllers_[playerId];
//     
//     Vec3f rightRotation = Vec3f::zero;
//     if (shipController.canMove)
//     {
//         rightRotation.x = shipInputManager_.GetJoystickAxis(playerId, ShipInputManager::Joystick::Right, ShipInputManager::Axis::Vertical) * shipParameter_.kRotorMaxAngle;
//         rightRotation.z = shipInputManager_.GetJoystickAxis(playerId, ShipInputManager::Joystick::Right, ShipInputManager::Axis::Horizontal) * shipParameter_.kRotorMaxAngle;
//     }
//      
//     Vec3f leftRotation = Vec3f::zero;
//     if (shipController.canMove)
//     {
//         leftRotation.x = shipInputManager_.GetJoystickAxis(playerId, ShipInputManager::Joystick::Left, ShipInputManager::Axis::Vertical) * shipParameter_.kRotorMaxAngle;
//         leftRotation.z = shipInputManager_.GetJoystickAxis(playerId, ShipInputManager::Joystick::Left, ShipInputManager::Axis::Horizontal) * shipParameter_.kRotorMaxAngle;
//     }
//     
//     transformManager_.SetRelativeRotation(shipController.rightRotor, ConvertVec3fToEulerAngles(rightRotation));
//     transformManager_.SetRelativeRotation(shipController.leftRotor, ConvertVec3fToEulerAngles(leftRotation));
// =======
//     transformManager_.SetRelativeRotation(playerComponent.rightRotorAnchor, EulerAngles(rightRotation));
//     transformManager_.SetRelativeRotation(playerComponent.leftRotorAnchor, EulerAngles(leftRotation));
// >>>>>>> ea26681eab9459c5b4b74681e5e6c2e10d7cb858
}

void ShipControllerManager::SetCanMove(PlayerId playerId, bool value)
{
    shipControllers_[playerId].canMove = value;
}

bool ShipControllerManager::GetCanMove(PlayerId playerId)
{
    return shipControllers_[playerId].canMove;
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

ShipControllerViewer::ShipControllerViewer(EntityManager& entityManager,
    PlayerManager& playerManager,
    ShipControllerManager& shipControllerManager)
    : ComponentViewer(entityManager),
        shipControllerManager_(shipControllerManager),
        playerManager_(playerManager) {}

json ShipControllerViewer::GetJsonFromComponent(Entity entity) const
{
    json rendererComponent = json::object();

    return rendererComponent;
}

void ShipControllerViewer::SetComponentFromJson(Entity entity, const json& componentJson)
{
    
}

void ShipControllerViewer::DrawImGui(Entity entity)
{
    for (PlayerId playerId = 0; playerId < playerManager_.GetPlayerCount(); ++playerId) {
        if (playerManager_.GetShipEntity(playerId) != entity) continue;
        if (ImGui::TreeNode("ShipController")) {
            //auto cameraComponent = cameraControllerManager_.GetComponent(entity);
            auto shipController = shipControllerManager_.GetComponent(playerId);
            std::string isGroundText = "IsGround : ";
            isGroundText += (shipController.isOnGround ? "1" : "0");
            ImGui::Text(isGroundText.c_str());
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