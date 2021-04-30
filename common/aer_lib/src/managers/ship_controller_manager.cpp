#include "aer/managers/ship_controller_manager.h"

#include "aer/tag.h"
#include <aer/log.h>

namespace neko::aer
{
    ShipControllerManager::ShipControllerManager(
        physics::PhysicsEngine& physicsEngine,
        ComponentManagerContainer& cContainer)
        : physicsEngine_(physicsEngine),
          cContainer_(cContainer)
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
   
   PlaySound(player, Sound::Engine);
}

void ShipControllerManager::RotorRotation(PlayerId playerId) {
    PlayerComponent playerComponent = cContainer_.playerManager.GetPlayerComponent(playerId);
    Entity& rightRotor = playerComponent.rightRotorModel;
    Entity& leftRotor = playerComponent.leftRotorModel;

    Vec3f currentRightRotorRotation = Vec3f(cContainer_.transform3dManager.GetRelativeRotation(rightRotor));
    cContainer_.transform3dManager.SetRelativeRotation(rightRotor, EulerAngles(Vec3f(currentRightRotorRotation.x, currentRightRotorRotation.y-shipParameter_.kRotorRotationSpeed, currentRightRotorRotation.z)));

    Vec3f currentLeftRotorRotation = Vec3f(cContainer_.transform3dManager.GetRelativeRotation(leftRotor));
    cContainer_.transform3dManager.SetRelativeRotation(leftRotor, EulerAngles(Vec3f(currentLeftRotorRotation.x, currentLeftRotorRotation.y - shipParameter_.kRotorRotationSpeed, currentLeftRotorRotation.z)));
}


void ShipControllerManager::Init()
{
}

void ShipControllerManager::FixedUpdate(seconds dt) {
    for (PlayerId playerId = 0; playerId < cContainer_.playerManager.GetPlayerCount(); ++playerId) {
        Entity shipEntity = cContainer_.playerManager.GetShipEntity(playerId);
        if (shipEntity == INVALID_ENTITY) continue;
        if (!cContainer_.entityManager.HasComponent(shipEntity, EntityMask(ComponentType::RIGID_DYNAMIC))) continue;
        CalculateHover(playerId, dt);
        CalculateThrust(playerId, dt);
        RotorMovement(playerId);
        RotorRotation(playerId);
    }
}


void ShipControllerManager::Update(seconds dt)
{
    cContainer_.shipInputManager.Update(dt);
}

void ShipControllerManager::CalculateHover(PlayerId playerId, seconds dt)
{
    ShipController& shipController = shipControllers_[playerId];

    Entity shipEntity = cContainer_.playerManager.GetShipEntity(playerId);
    if (shipEntity == INVALID_ENTITY) return;

    physics::RigidDynamic rigidDynamic = cContainer_.rigidDynamicManager.GetComponent(shipEntity);

    //Raycast to ground
    Vec3f groundNormal = Vec3f::zero;
    Vec3f shipPosition = cContainer_.transform3dManager.GetGlobalPosition(shipEntity);
    Vec3f rayPos =  shipPosition;
    Quaternion shipRotation = Quaternion::FromEuler(cContainer_.transform3dManager.GetGlobalRotation(shipEntity));
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
    float rollAngle = 0; 
    float pitchAngle = 0; 

        if (shipController.canMove)
        {
            //Roll angle
            float rudder = -cContainer_.shipInputManager.GetRudder(playerId);
            
            if(rudder > shipParameter_.kAngleDeadzoneLimit){
                shipController.rollMultiplicator += shipParameter_.kRollChangeSpeed;
            }
            else if(rudder < -shipParameter_.kAngleDeadzoneLimit) {
                shipController.rollMultiplicator -= shipParameter_.kRollChangeSpeed;
            }
            else if(rudder > -shipParameter_.kAngleDeadzoneLimit && rudder < shipParameter_.kAngleDeadzoneLimit) {
                if (shipController.rollMultiplicator < -shipParameter_.kAngleDeadzoneLimit) {
                    shipController.rollMultiplicator += shipParameter_.kRollChangeSpeed;
                }
                else if (shipController.rollMultiplicator > shipParameter_.kAngleDeadzoneLimit) {
                    shipController.rollMultiplicator -= shipParameter_.kRollChangeSpeed;
                }
                else {
                    shipController.rollMultiplicator = 0;
                }
            }

            if (shipController.rollMultiplicator >= 1) {
                shipController.rollMultiplicator = 1;
            }
            if (shipController.rollMultiplicator <= -1) {
                shipController.rollMultiplicator = -1;
            }

            //Automated roll
            rollAngle = shipParameter_.kAngleOfRoll * shipController.rollMultiplicator;

            //Roll with user input
            //rollAngle = shipParameter_.kAngleOfRoll * -cContainer_.shipInputManager.GetRudder(playerId) * cContainer_.shipInputManager.GetIntensity(playerId);

            //Pitch angle
            float thrust = cContainer_.shipInputManager.GetThruster(playerId);

            if(thrust > shipParameter_.kAngleDeadzoneLimit) {
                shipController.pitchMultiplicator += shipParameter_.kPitchChangeSpeed;
            }
            else if(thrust < -shipParameter_.kAngleDeadzoneLimit){
                shipController.pitchMultiplicator -= shipParameter_.kPitchChangeSpeed;
            }
            else if(thrust > -shipParameter_.kAngleDeadzoneLimit && thrust < shipParameter_.kAngleDeadzoneLimit) {
                if(shipController.pitchMultiplicator < -shipParameter_.kAngleDeadzoneLimit) {
                    shipController.pitchMultiplicator += shipParameter_.kPitchChangeSpeed;
                }
                else if(shipController.pitchMultiplicator > shipParameter_.kAngleDeadzoneLimit) {
                    shipController.pitchMultiplicator -= shipParameter_.kPitchChangeSpeed;
                }
                else {
                    shipController.pitchMultiplicator = 0;
                }
            }

            if(shipController.pitchMultiplicator >= 1) {
                shipController.pitchMultiplicator = 1;
            }
            if(shipController.pitchMultiplicator <= -1) {
                shipController.pitchMultiplicator = -1;
            }

            //Automated pitch
            pitchAngle = shipParameter_.kAngleOfPitch * shipController.pitchMultiplicator;

            //Pitch with user input
            //pitchAngle = shipParameter_.kAngleOfPitch *cContainer_.shipInputManager.GetThruster(playerId) * cContainer_.shipInputManager.GetIntensity(playerId);
        }

    Entity shipModelEntity = cContainer_.playerManager.GetPlayerComponent(playerId).shipModelEntity;
    Quaternion bodyRotation = Quaternion::FromEuler(EulerAngles(pitchAngle, 0.0f, rollAngle));

    cContainer_.transform3dManager.SetRelativeRotation(shipModelEntity, Quaternion::ToEulerAngles(bodyRotation));
}

void ShipControllerManager::CalculateThrust(PlayerId playerId, seconds dt)
{
    ShipController& shipController = shipControllers_[playerId];
    Entity shipEntity = cContainer_.playerManager.GetShipEntity(playerId);
    if (shipEntity == INVALID_ENTITY) return;
    physics::RigidDynamic rigidDynamic = cContainer_.rigidDynamicManager.GetComponent(shipEntity);

    float rotationTorque = 0; 

    if (shipController.canMove)
    {
        rotationTorque = (cContainer_.shipInputManager.GetRudder(playerId) * shipParameter_.kRotationMultiplicator) - rigidDynamic.GetDynamicData().angularVelocity.y;
    }

    rigidDynamic.AddRelativeTorque(Vec3f(0.0f, rotationTorque, 0.0f), physx::PxForceMode::eVELOCITY_CHANGE);

    Vec3f right = Quaternion::FromEuler(cContainer_.transform3dManager.GetGlobalRotation(shipEntity)) * Vec3f::right;
    float sidewaySpeed = Vec3f::Dot(rigidDynamic.GetDynamicData().linearVelocity, right);

    Vec3f sideFriction = -right * (sidewaySpeed / dt.count());

    rigidDynamic.AddForce(sideFriction, physx::PxForceMode::eACCELERATION);

    float currentThruster = 0;
    bool currentBreaking = false;
        if (shipController.canMove)
        {
            currentThruster = cContainer_.shipInputManager.GetThruster(playerId);
            currentBreaking = cContainer_.shipInputManager.IsBreaking(playerId);
        }

    if(currentThruster <= 0.0f) {
        cContainer_.rigidDynamicManager.SetLinearVelocity(shipEntity, rigidDynamic.GetDynamicData().linearVelocity * shipParameter_.kSlowingVelFactor);
    }

    if(!shipController.isOnGround) {
        return;
    }
    if(currentBreaking) {
        cContainer_.rigidDynamicManager.SetLinearVelocity(shipEntity, rigidDynamic.GetDynamicData().linearVelocity * shipParameter_.kBrakingVelFactor);
    }

    Vec3f forward = Quaternion::FromEuler(cContainer_.transform3dManager.GetGlobalRotation(shipEntity)) * Vec3f::forward;
    float propulsion = (shipParameter_.kForwardForce * currentThruster * shipParameter_.kPropultionMultiplicator) -
        (shipController.drag * Clamp(Vec3f::Dot(rigidDynamic.GetDynamicData().linearVelocity, forward), 0.0f, shipParameter_.kTerminalVelocity) *
            shipParameter_.kPropultionMultiplicator);
    rigidDynamic.AddForce(forward * propulsion, physx::PxForceMode::eACCELERATION);

    SetEngineSpeedSound(playerId, std::clamp(Abs(cContainer_.shipInputManager.GetThruster(playerId)) * 100.0f, 0.1f, 100.0f));
}

void ShipControllerManager::RotorMovement(PlayerId playerId) {

    PlayerComponent playerComponent = cContainer_.playerManager.GetPlayerComponent(playerId);

    ShipController& shipController = shipControllers_[playerId];

    Vec3f rightRotation = Vec3f::zero;
    if (shipController.canMove)
    {
        rightRotation.x = cContainer_.shipInputManager.GetJoystickAxis(playerId, ShipInputManager::Joystick::Right, ShipInputManager::Axis::Vertical) * shipParameter_.kRotorMaxAngle;
        rightRotation.z = cContainer_.shipInputManager.GetJoystickAxis(playerId, ShipInputManager::Joystick::Right, ShipInputManager::Axis::Horizontal) * shipParameter_.kRotorMaxAngle;
    }

    Vec3f leftRotation = Vec3f::zero;
    if (shipController.canMove)
    {
        leftRotation.x = cContainer_.shipInputManager.GetJoystickAxis(playerId, ShipInputManager::Joystick::Left, ShipInputManager::Axis::Vertical) * shipParameter_.kRotorMaxAngle;
        leftRotation.z = cContainer_.shipInputManager.GetJoystickAxis(playerId, ShipInputManager::Joystick::Left, ShipInputManager::Axis::Horizontal) * shipParameter_.kRotorMaxAngle;
    }

    cContainer_.transform3dManager.SetRelativeRotation(playerComponent.rightRotorAnchor, EulerAngles(rightRotation));
    cContainer_.transform3dManager.SetRelativeRotation(playerComponent.leftRotorAnchor, EulerAngles(leftRotation));
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
    const Entity entity1 = cContainer_.rigidDynamicManager.FindEntityFromActor(pairHeader.actors[0]);
    if (entity1 != INVALID_ENTITY) {
        if (cContainer_.entityManager.HasComponent(entity1, EntityMask(ComponentType::PLAYER_COMPONENT))) {
            for (PlayerId playerId = 0; playerId < cContainer_.playerManager.GetPlayerCount(); ++playerId) {
                Entity currentEntity = cContainer_.playerManager.GetShipEntity(playerId);
                if (currentEntity == INVALID_ENTITY) continue;
                if (!cContainer_.entityManager.HasComponent(currentEntity, EntityMask(ComponentType::RIGID_DYNAMIC))) continue;
                if(currentEntity == entity1) {
                    PlaySound(playerId, Sound::Collision);
                    break;
                }
            }
        }
    }


    //physx::PxContactPairPoint contactPointBuffer[16];
    //int32_t numContactPoints = pairHeader.pairs->extractContacts(contactPointBuffer, 16);
    //LogDebug(std::to_string(numContactPoints));
    //const Entity entity1 = cContainer_.rigidDynamicManager.FindEntityFromActor(pairHeader.actors[0]);
    //if (entity1 != INVALID_ENTITY)
    //{
    //    if (cContainer_.entityManager.get().HasComponent(entity1, EntityMask(ComponentType::SHIP_CONTROLLER))) {
    //        physx::PxContactPairPoint contactPointBuffer[16];
    //        int32_t numContactPoints = pairHeader.pairs->extractContacts(contactPointBuffer, 16);
    //        LogDebug(std::to_string(numContactPoints));
    //        if (Vec3f::Angle(Vec3f::up, physics::ConvertFromPxVec(contactPointBuffer->normal)) > degree_t(80.0f))
    //        {
    //            physics::RigidDynamic rigidDynamic = cContainer_.rigidDynamicManager.GetComponent(entity1);
    //            rigidDynamic.AddForce(physics::ConvertFromPxVec(contactPointBuffer->normal) * shipParameter_.kBounceForce, physx::PxForceMode::eIMPULSE);
    //        }
    //    }
    //}
    //const Entity entity2 = cContainer_.rigidDynamicManager.FindEntityFromActor(pairHeader.actors[0]);
    //if (entity2 == INVALID_ENTITY) return;
    //if (cContainer_.entityManager.get().HasComponent(entity2, EntityMask(ComponentType::SHIP_CONTROLLER))) {
    //    physx::PxContactPairPoint contactPointBuffer[16];
    //    int32_t numContactPoints = pairHeader.pairs->extractContacts(contactPointBuffer, 16);
    //    LogDebug(std::to_string(numContactPoints));
    //    if (Vec3f::Angle(Vec3f::up, physics::ConvertFromPxVec(contactPointBuffer->normal)) > degree_t(80.0f))
    //    {
    //        physics::RigidDynamic rigidDynamic = cContainer_.rigidDynamicManager.GetComponent(entity1);
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

void ShipControllerManager::PlaySound(PlayerId playerId, Sound sound) {
    PlayerComponent playerComponent = cContainer_.playerManager.GetPlayerComponent(playerId);

    switch(sound) {
        case Sound::Engine:
            cContainer_.audioManager.Play(playerComponent.engineAudioEntity);
            break;
        case Sound::Collision:
        {
            float random = RandomRange(0.0f, 1.0f);
            if (random > 0.5f)
            {
                cContainer_.audioManager.SetEventName(playerComponent.audioEntity, "sfx/crash_short1");
            }
            else
            {
                cContainer_.audioManager.SetEventName(playerComponent.audioEntity, "sfx/crash_short2");

            }
            cContainer_.audioManager.Play(playerComponent.audioEntity);
        }
            break;
        default: ;
    }
    
}

void ShipControllerManager::SetEngineSpeedSound(PlayerId playerId, float speed) {
    PlayerComponent playerComponent = cContainer_.playerManager.GetPlayerComponent(playerId);
    cContainer_.audioManager.SetParameter(playerComponent.engineAudioEntity, "Speed", speed);
}



}    // namespace neko::aer