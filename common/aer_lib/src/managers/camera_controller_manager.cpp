#include "aer/managers/camera_controller_manager.h"

#include <imgui.h>

#include "aer/tag.h"
#include "px/raycast.h"

namespace neko::aer
{
    CameraControllerManager::CameraControllerManager(
        EntityManager& entityManager,
        Transform3dManager& transform3DManager,
        physics::RigidDynamicManager& rigidDynamicManager,
        physics::PhysicsEngine& physicsEngine)
        : ComponentManager<CameraController, EntityMask(ComponentType::SHIP_CAMERA)>(entityManager),
        transformManager_(transform3DManager),
        rigidDynamicManager_(rigidDynamicManager),
        physicsEngine_(physicsEngine)
{
        physicsEngine.RegisterFixedUpdateListener(*this);
}

void CameraControllerManager::AddComponent(Entity entity)
{
    ComponentManager::AddComponent(entity);
    cameraController_ = GetComponent(entity);
    cameraEntity_ = entityManager_.get().FindEntityByName("cameraEntity");
    shipEntity_ = entityManager_.get().FindEntityByName("ship");
    CreateTargetsEntity();

    transformManager_.SetRelativeRotation(lookTargetLeft_, EulerAngles(lookTargetLeftPos_.x, -cameraController_.lookAngle, lookTargetLeftPos_.z));
    transformManager_.SetRelativeRotation(lookTargetRight_, EulerAngles(lookTargetRightPos_.x, cameraController_.lookAngle, lookTargetRightPos_.z));

}

void CameraControllerManager::Init()
{

}

void CameraControllerManager::CreateTargetsEntity() {
    //Create Left Target
    lookTargetLeft_ = entityManager_.get().CreateEntity();
    entityManager_.get().SetEntityName(lookTargetLeft_, "lookTargetLeft");
    transformManager_.AddComponent(lookTargetLeft_);
    entityManager_.get().SetEntityParent(lookTargetLeft_, shipEntity_);
    transformManager_.SetRelativePosition(lookTargetLeft_, lookTargetLeftPos_);
    transformManager_.SetRelativeRotation(lookTargetLeft_, lookTargetLeftRot_);

    //Create Right Target
    lookTargetRight_ = entityManager_.get().CreateEntity();
    entityManager_.get().SetEntityName(lookTargetRight_, "lookTargetRight");
    transformManager_.AddComponent(lookTargetRight_);
    entityManager_.get().SetEntityParent(lookTargetRight_, shipEntity_);
    transformManager_.SetRelativePosition(lookTargetRight_, lookTargetRightPos_);
    transformManager_.SetRelativeRotation(lookTargetRight_, lookTargetRightRot_);

    //Create Center Target
    lookTargetNormal_ = entityManager_.get().CreateEntity();
    entityManager_.get().SetEntityName(lookTargetNormal_, "lookTargetCenter");
    transformManager_.AddComponent(lookTargetNormal_);
    entityManager_.get().SetEntityParent(lookTargetNormal_, shipEntity_);
    transformManager_.SetRelativePosition(lookTargetNormal_, lookTargetNormalPos_);

    //Create Target Entity
    targetEntity_ = entityManager_.get().CreateEntity();
    entityManager_.get().SetEntityName(targetEntity_, "targetEntity");
    transformManager_.AddComponent(targetEntity_);
    entityManager_.get().SetEntityParent(targetEntity_, shipEntity_);

    //Create Spring Entity
    springEntity_ = entityManager_.get().CreateEntity();
    entityManager_.get().SetEntityName(springEntity_, "springEntity");
    entityManager_.get().SetEntityParent(springEntity_, shipEntity_);
}


void CameraControllerManager::FixedUpdate(seconds dt) {
    dt_ = dt;
    const auto& entities =
        entityManager_.get().FilterEntities(static_cast<EntityMask>(ComponentType::SHIP_CAMERA));

    for (auto& entity : entities)
    {
        LookTurning();
        LookDown();

        Vec3f currentSpringCameraPosition = SpringPosition2D();
        currentSpringCameraPosition = CameraSmoothSpring(currentSpringCameraPosition);

        CameraSmoothLerp(currentSpringCameraPosition);

        LastFrameShipValues();

        SpringTransformChild(springEntity_);
        float xPos = transformManager_.GetGlobalPosition(cameraEntity_).x;
        float yPos = transformManager_.GetGlobalPosition(targetEntity_).y;
        float zPos = transformManager_.GetGlobalPosition(cameraEntity_).z;
        transformManager_.SetGlobalPosition(cameraEntity_, Vec3f(xPos, yPos, zPos));
    }
       
}


void CameraControllerManager::Update(seconds dt)
{
    shipInputManager_.Update(dt);
}

void CameraControllerManager::Destroy()
{
	
}

CameraControllerViewer::CameraControllerViewer(EntityManager& entityManager, CameraControllerManager& cameraControllerManager) : ComponentViewer(entityManager), cameraControllerManager_(cameraControllerManager)
{
}

json CameraControllerViewer::GetJsonFromComponent(Entity entity) const
{
    json rendererComponent = json::object();
    if (entityManager_.HasComponent(entity, EntityMask(ComponentType::SHIP_CAMERA)))
    {
        if (entity != INVALID_ENTITY && entityManager_.GetEntitiesSize() > entity)
        {
            
        }
    }
    return rendererComponent;
}

void CameraControllerViewer::SetComponentFromJson(Entity entity, const json& componentJson)
{
    
}

void CameraControllerViewer::DrawImGui(Entity entity)
{
    if (entity == INVALID_ENTITY) return;
    if (entityManager_.HasComponent(entity, EntityMask(ComponentType::SHIP_CAMERA))) {
        if (ImGui::TreeNode("ShipCamera")) {
            ImGui::Text("Param 1");
            ImGui::TreePop();
        }
    }
}

void CameraControllerManager::LookTurning() {
    switch (shipInputManager_.GetCurrentGesture()) {
        case ShipInputManager::Gesture::TurnLeftForward:
            transformManager_.SetGlobalRotation(targetEntity_, Quaternion::ToEulerAngles(
                Quaternion::Lerp(Quaternion::FromEuler(transformManager_.GetGlobalRotation(targetEntity_)), 
                                   Quaternion::FromEuler(transformManager_.GetGlobalRotation(lookTargetLeft_)), 
                                      cameraController_.lerpLookTurning)));
            break;
        case ShipInputManager::Gesture::TurnRightForward:
            transformManager_.SetGlobalRotation(targetEntity_, Quaternion::ToEulerAngles(
                Quaternion::Lerp(Quaternion::FromEuler(transformManager_.GetGlobalRotation(targetEntity_)),
                    Quaternion::FromEuler(transformManager_.GetGlobalRotation(lookTargetRight_)),
                    cameraController_.lerpLookTurning)));
            break;
        default:
            transformManager_.SetGlobalRotation(targetEntity_, Quaternion::ToEulerAngles(
                Quaternion::Lerp(Quaternion::FromEuler(transformManager_.GetGlobalRotation(targetEntity_)),
                    Quaternion::FromEuler(transformManager_.GetGlobalRotation(lookTargetNormal_)),
                    cameraController_.lerpLookTurning)));
            break;
    }
}

void CameraControllerManager::LookDown() {
    float yEulerModifier = 0;
    physics::RigidDynamic shipRigidDynamic = rigidDynamicManager_.GetComponent(shipEntity_);
    if(Abs(shipRigidDynamic.GetDynamicData().linearVelocity.y) > cameraController_.minimumSpeed) {
        yEulerModifier = shipRigidDynamic.GetDynamicData().linearVelocity.Normalized().y * cameraController_.lookingDownIntensity;
    }

    Quaternion targetQuaternion = Quaternion::FromEuler(transformManager_.GetGlobalRotation(targetEntity_));
    transformManager_.SetGlobalRotation(cameraEntity_, Quaternion::ToEulerAngles(Quaternion::Lerp(
                                                                    Quaternion::FromEuler(transformManager_.GetGlobalRotation(cameraEntity_)),
                                                                    targetQuaternion,
                                                                    cameraController_.cameraLookLerpValue)));
}

Vec3f CameraControllerManager::SpringPosition2D() {
    Vec3f targetPosition = transformManager_.GetGlobalPosition(targetEntity_);
    Vec3f cameraPosition = transformManager_.GetGlobalPosition(cameraEntity_);

    Vec2f force = Vec2f(targetPosition.x - cameraPosition.x, targetPosition.z - cameraPosition.z) * cameraController_.sprintStiffness;

    force = SpringForceSmooth(force);

    Vec2f velocity = force * dt_.count() / cameraController_.cameraMass;

    cameraController_.cameraVelocity += Vec3f(velocity.x, 0, velocity.y);

    Vec3f position = cameraPosition + (cameraController_.cameraVelocity * dt_.count());
    position = Vec3f(position.x, targetPosition.y, position.z);
    return position;
}

Vec3f CameraControllerManager::SpringPosition() {
    Vec3f targetPosition = transformManager_.GetGlobalPosition(targetEntity_);
    Vec3f cameraPosition = transformManager_.GetGlobalPosition(cameraEntity_);
    //Calculate SpringForce 2D
    //Translate it in 3D
    Vec3f force = (targetPosition - cameraPosition) * cameraController_.sprintStiffness; //TODO Rework to Vec2f
    force = SpringForceSmooth(force);

    cameraController_.cameraVelocity += force * dt_.count() / cameraController_.cameraMass;
    cameraController_.cameraVelocity *= cameraController_.forceLossPercentage;

    Vec3f position = cameraPosition + (cameraController_.cameraVelocity * dt_.count());

    return position;
}

Vec2f CameraControllerManager::SpringForceSmooth(Vec2f force) {
    Vec3f rigidDynamicVelocity = rigidDynamicManager_.GetComponent(shipEntity_).GetDynamicData().linearVelocity;
    if (Abs(force.x) > cameraController_.maxYForce && Abs(rigidDynamicVelocity.x - cameraController_.lastFrameShipVelocity.x) > cameraController_.maxYVelocity) {
        force.x = Sign(force.x) * cameraController_.maxYForce;
    }
    if (Abs(force.y) > cameraController_.maxYForce && Abs(rigidDynamicVelocity.z - cameraController_.lastFrameShipVelocity.z) > cameraController_.maxYVelocity) {
        force.y = Sign(force.y) * cameraController_.maxYForce;
    }
    return Vec2f(force.x, force.y);
}

Vec3f CameraControllerManager::SpringForceSmooth(Vec3f force) {
    Vec3f rigidDynamicVelocity = rigidDynamicManager_.GetComponent(shipEntity_).GetDynamicData().linearVelocity;
    if (Abs(force.x) > cameraController_.maxYForce && Abs(rigidDynamicVelocity.x - cameraController_.lastFrameShipVelocity.x) > cameraController_.maxYVelocity) {
        force.x = Sign(force.x) * cameraController_.maxYForce;
    }
    if (Abs(force.y) > cameraController_.maxYForce && Abs(rigidDynamicVelocity.y - cameraController_.lastFrameShipVelocity.y) > cameraController_.maxYVelocity) {
        force.y = Sign(force.y) * cameraController_.maxYForce;
    }
    if (Abs(force.z) > cameraController_.maxYForce && Abs(rigidDynamicVelocity.z - cameraController_.lastFrameShipVelocity.z) > cameraController_.maxYVelocity) {
        force.z = Sign(force.z) * cameraController_.maxYForce;
    }
    return force;
}

Vec3f CameraControllerManager::CameraSmoothSpring(Vec3f springPosition) {
    Vec3f targetPosition = transformManager_.GetGlobalPosition(targetEntity_);
    Vec3f rigidDynamicVelocity = rigidDynamicManager_.GetComponent(shipEntity_).GetDynamicData().linearVelocity;

    Vec3f estimatedPosition = targetPosition + (rigidDynamicVelocity * dt_.count());

    if(Abs(Abs(cameraController_.cameraVelocity.x) - Abs(cameraController_.lastFrameShipVelocity.x)) > cameraController_.maxYVelocity) {
        cameraController_.cameraVelocity.x = rigidDynamicVelocity.x;
        springPosition.x = estimatedPosition.x;
    }

    if (Abs(Abs(cameraController_.cameraVelocity.y) - Abs(cameraController_.lastFrameShipVelocity.y)) > cameraController_.maxYVelocity) {
        cameraController_.cameraVelocity.y = rigidDynamicVelocity.y;
        springPosition.y = estimatedPosition.y;
    }

    if (Abs(Abs(cameraController_.cameraVelocity.z) - Abs(cameraController_.lastFrameShipVelocity.z)) > cameraController_.maxYVelocity) {
        cameraController_.cameraVelocity.z = rigidDynamicVelocity.z;
        springPosition.z = estimatedPosition.z;
    }

    return springPosition;
}

void CameraControllerManager::SpringTransformChild(Entity shipChildEntity) {
    //Step 1: Transform Spring = Child of Spaceship

    //Step 2: Give to this transform the value of the camera

    transformManager_.SetGlobalPosition(shipChildEntity, transformManager_.GetGlobalPosition(cameraEntity_));

    //Step 3: Set the Y of this transform position to 0

    transformManager_.SetRelativePosition(shipChildEntity, Vec3f(transformManager_.GetRelativePosition(shipChildEntity).x, cameraController_.cameraFixedY, transformManager_.GetRelativePosition(shipChildEntity).z));

    //Step 4: Get the new value back

    transformManager_.SetGlobalPosition(cameraEntity_, Vec3f::Lerp(transformManager_.GetGlobalPosition(cameraEntity_), transformManager_.GetGlobalPosition(shipChildEntity), 0.1f));
}

void CameraControllerManager::CameraSmoothLerp(Vec3f cameraPosition) {
    transformManager_.SetGlobalPosition(cameraEntity_, Vec3f::Lerp(transformManager_.GetGlobalPosition(cameraEntity_), cameraPosition, cameraController_.cameraPositionLerpValue));
}

void CameraControllerManager::LastFrameShipValues() {
    cameraController_.lastFrameShipVelocity = rigidDynamicManager_.GetComponent(shipEntity_).GetDynamicData().linearVelocity;
}
void CameraControllerManager::MaximumAngle() {
    
}
}    // namespace neko::aer