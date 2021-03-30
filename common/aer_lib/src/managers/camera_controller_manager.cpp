#include "aer/managers/camera_controller_manager.h"

#include <imgui.h>

#include "aer/managers/player_manager.h"
#include "aer/aer_engine.h"

namespace neko::aer
{
CameraControllerManager::CameraControllerManager(
    EntityManager& entityManager,
    Transform3dManager& transform3DManager,
    physics::RigidDynamicManager& rigidDynamicManager,
    physics::PhysicsEngine& physicsEngine,
    PlayerManager& playerManager,
    AerEngine& aerEngine)
    : transformManager_(transform3DManager),
      rigidDynamicManager_(rigidDynamicManager),
      physicsEngine_(physicsEngine),
      playerManager_(playerManager),
      engine_(aerEngine)
{
    cameraControllers_.resize(INIT_PLAYER_NMB, CameraController());
    physicsEngine.RegisterFixedUpdateListener(*this);
}
void CameraControllerManager::Init()
{

}
void CameraControllerManager::FixedUpdate(seconds dt) {
    for (PlayerId playerId = 0; playerId < playerManager_.GetPlayerCount(); ++playerId)
    {
        auto& cameraComponent = cameraControllers_[playerId];
        Entity shipEntity = playerManager_.GetShipEntity(playerId);
        if (shipEntity == INVALID_ENTITY) continue;
        auto shipBody = rigidDynamicManager_.GetDynamicData(shipEntity);
        auto shipPosition = transformManager_.GetGlobalPosition(shipEntity);
        auto shipRotation = Quaternion::FromEuler(transformManager_.GetGlobalRotation(shipEntity));
        auto angularVelocity = shipBody.angularVelocity;
        auto velocity = shipBody.linearVelocity;

        cameraComponent.angularLateralMovement = kCameraParameter_.kAngularLateralMult * angularVelocity.y;
        cameraComponent.angularBackwardMovement = kCameraParameter_.kAngularBackwardMult * angularVelocity.y;
        cameraComponent.linearUpwardMovement = kCameraParameter_.kLinearUpwardMult * velocity.Magnitude();
        cameraComponent.linearBackwardMovement = kCameraParameter_.kLinerarBackwardMult * velocity.Magnitude();
        cameraComponent.dotProductPosVelo = Vec3f::Dot((cameraComponent.cameraPos - shipPosition).Normalized(), velocity.Normalized());
        if (cameraComponent.dotProductPosVelo > 0.1f)
        {
            cameraComponent.dotProductPosVeloMult = cameraComponent.dotProductPosVelo * kCameraParameter_.kLinerarBackwardDiv;
            cameraComponent.linearBackwardMovement = cameraComponent.linearBackwardMovement * cameraComponent.dotProductPosVeloMult;
        }
        cameraComponent.newPosition = shipPosition;
        cameraComponent.angularAddVector = Vec3f::Lerp(cameraComponent.angularAddVector, Vec3f::right * cameraComponent.angularLateralMovement + Vec3f::back * Abs(cameraComponent.angularBackwardMovement), kCameraParameter_.kAngularLerp);
        cameraComponent.linearAddVector = Vec3f::Lerp(cameraComponent.linearAddVector, Vec3f::up * cameraComponent.linearUpwardMovement + Vec3f::back * cameraComponent.linearBackwardMovement, kCameraParameter_.kLinearLerp);
        cameraComponent.newPosition += shipRotation * (kCameraParameter_.kCameraPosition + cameraComponent.angularAddVector + cameraComponent.linearAddVector);
        cameraComponent.fallAddition = (kCameraParameter_.kFallMultiplicator * -Abs(shipBody.linearVelocity.y));
        cameraComponent.newPosition += Vec3f::up * cameraComponent.fallAddition;
        //transformManager_.SetGlobalPosition(cameraEntity, Vec3f::Lerp(cameraPosition, cameraComponent.newPosition, kCameraParameter_.kLerpPosition));
        cameraComponent.cameraPos = Vec3f::Lerp(cameraComponent.cameraPos, cameraComponent.newPosition, kCameraParameter_.kLerpPosition);

        cameraComponent.angularForwardTarget = kCameraParameter_.kAngularForwardTargetMult * Abs(angularVelocity.y);
        cameraComponent.linearForwardTarget = kCameraParameter_.kLinearForwardTargetMult * velocity.Magnitude();
        cameraComponent.forwardTarget = cameraComponent.linearForwardTarget + cameraComponent.angularForwardTarget;
        cameraComponent.fallTargetAddition = (kCameraParameter_.kFallTargetMultiplicator * -Abs(velocity.y));
        cameraComponent.forwardTarget = Clamp(cameraComponent.forwardTarget, -2.0f, kCameraParameter_.kMaxTargetPos);
        cameraComponent.addTargetVector = Vec3f::Lerp(cameraComponent.addTargetVector, Vec3f::forward * Abs(cameraComponent.forwardTarget) + Vec3f::down * Abs(cameraComponent.fallTargetAddition), kCameraParameter_.kAngularTargetLerp);
        Vec3f targetPos = shipPosition + shipRotation * (kCameraParameter_.kTargetPosition + cameraComponent.addTargetVector);
        //transformManager_.SetRelativeRotation(entity, Quaternion::ToEulerAngles(Quaternion::LookRotation(targetPos - cameraPosition, Vec3f::up)));
        cameraComponent.targetPos = targetPos;
    }
}

void CameraControllerManager::Render()
{
    for (PlayerId playerId = 0; playerId < playerManager_.GetPlayerCount(); ++playerId) {
        auto& camera          = engine_.GetCameras().GetCamera(playerId);
        auto& cameraComponent = cameraControllers_[playerId];
        camera.position       = cameraComponent.cameraPos;
        camera.WorldLookAt(cameraComponent.targetPos);
    }
}

void CameraControllerManager::Update(seconds dt)
{
    RendererLocator::get().Render(this);
}

void CameraControllerManager::Destroy()
{

}

CameraControllerViewer::CameraControllerViewer(EntityManager& entityManager,
    PlayerManager& playerManager,
    CameraControllerManager& cameraControllerManager)
    : ComponentViewer(entityManager),
      cameraControllerManager_(cameraControllerManager),
    playerManager_(playerManager)
{
}

json CameraControllerViewer::GetJsonFromComponent(Entity entity) const
{
    json rendererComponent = json::object();
    return rendererComponent;
}

void CameraControllerViewer::SetComponentFromJson(Entity entity, const json& componentJson)
{

}

void CameraControllerViewer::DrawImGui(Entity entity)
{
    for (PlayerId playerId = 0; playerId < playerManager_.GetPlayerCount(); ++playerId) {
        if (playerManager_.GetShipEntity(playerId) != entity) continue;
        if (ImGui::TreeNode("ShipCamera")) {
            //auto cameraComponent = cameraControllerManager_.GetComponent(entity);
            auto cameraComponent = cameraControllerManager_.GetComponent(playerId);
            ImGui::Text("angularLateralMovement");
            ImGui::SameLine();
            ImGui::Text(std::to_string(cameraComponent.angularLateralMovement).c_str());
            ImGui::Text("angularBackwardMovement");
            ImGui::SameLine();
            ImGui::Text(std::to_string(cameraComponent.angularBackwardMovement).c_str());
            ImGui::Text("angularForwardTarget");
            ImGui::SameLine();
            ImGui::Text(std::to_string(cameraComponent.angularForwardTarget).c_str());
            ImGui::Text("linearForwardTarget");
            ImGui::SameLine();
            ImGui::Text(std::to_string(cameraComponent.linearForwardTarget).c_str());
            ImGui::Text("forwardTarget");
            ImGui::SameLine();
            ImGui::Text(std::to_string(cameraComponent.forwardTarget).c_str());
            ImGui::Text("linearUpwardMovement");
            ImGui::SameLine();
            ImGui::Text(std::to_string(cameraComponent.linearUpwardMovement).c_str());
            ImGui::Text("linearBackwardMovement");
            ImGui::SameLine();
            ImGui::Text(std::to_string(cameraComponent.linearBackwardMovement).c_str());
            ImGui::Text("angularAddVector");
            ImGui::SameLine();
            ImGui::Text(cameraComponent.angularAddVector.ToString().c_str());
            ImGui::Text("addTargetVector");
            ImGui::SameLine();
            ImGui::Text(cameraComponent.addTargetVector.ToString().c_str());
            ImGui::Text("linearAddVector");
            ImGui::SameLine();
            ImGui::Text(cameraComponent.linearAddVector.ToString().c_str());
            ImGui::Text("newPosition");
            ImGui::SameLine();
            ImGui::Text(cameraComponent.newPosition.ToString().c_str());
            ImGui::Text("fallAddition");
            ImGui::SameLine();
            ImGui::Text(std::to_string(cameraComponent.fallAddition).c_str());
            ImGui::Text("fallTargetAddition");
            ImGui::SameLine();
            ImGui::Text(std::to_string(cameraComponent.fallTargetAddition).c_str());
            ImGui::Text("dotProductPosVelo");
            ImGui::SameLine();
            ImGui::Text(std::to_string(cameraComponent.dotProductPosVelo).c_str());
            ImGui::Text("dotProductPosVeloMult");
            ImGui::SameLine();
            ImGui::Text(std::to_string(cameraComponent.dotProductPosVeloMult).c_str());
            ImGui::TreePop();
        }
    }
}
}    // namespace neko::aer