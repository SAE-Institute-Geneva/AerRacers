#pragma once
#include "px/physics_engine.h"
#include "px/rigidbody.h"
#include "engine/component.h"
#include "engine/transform.h"
#include "ship_input_manager.h"

namespace neko::aer
{
class AerEngine;
class PlayerManager;
class CameraControllerViewer;
    /**
     * \brief Component used to control the camera movements.
     */
    struct CameraController {
        float angularLateralMovement;
        float angularBackwardMovement;
        float angularForwardTarget;
        float linearForwardTarget;
        float forwardTarget;
        float linearUpwardMovement;
        float linearBackwardMovement;
        Vec3f angularAddVector;
        Vec3f addTargetVector;
        Vec3f linearAddVector;
        Vec3f newPosition;
        float fallAddition;
        float fallTargetAddition;
        float dotProductPosVelo;
        float dotProductPosVeloMult;
        Vec3f targetPos;
        Vec3f cameraPos = Vec3f(0, 0, 5);
        physics::DynamicData dynamicData;
    };
    /**
     * \brief Component used to control the camera movements.
     */
    struct CameraParameter {
        const Vec3f kTargetPosition = Vec3f(0, 0, 5);
        const float kMaxTargetPos = 50.0f;
        const Vec3f kCameraPosition = Vec3f(0, 5, -10);

        const float kAngularLateralMult = 3.0f;
        const float kAngularBackwardMult = 1.0f;
        const float kAngularForwardTargetMult = 80.0f;

        const float kLinearUpwardMult = 0.01f;
        const float kLinerarBackwardMult = -0.01f;
        const float kLinerarBackwardDiv = -0.0015f;
        const float kLinearForwardTargetMult = 0.1f;
        const float kFallMultiplicator = 0.01f;
        const float kFallTargetMultiplicator = -0.05f;

        const float kAngularLerp = 0.1f;
        const float kAngularTargetLerp = 0.1f;

        const float kLinearLerp = 1.0f;
        const float kLerpPosition = 0.5f;
    };

    /**
     * \brief System that manages ShipControllers
     */
    class CameraControllerManager final :
        public SystemInterface,
        public physics::FixedUpdateInterface,
        public RenderCommandInterface
    {
    public:
        explicit CameraControllerManager(
            EntityManager& entityManager,
            Transform3dManager& transform3DManager,
            physics::RigidDynamicManager& rigidDynamicManager,
            physics::PhysicsEngine& physicsEngine,
            PlayerManager& playerManager,
            AerEngine& aerEngine);

        void Init() override;
        void Update(seconds dt) override;
        void FixedUpdate(seconds dt) override;
        void Render() override;
        void Destroy() override;

        CameraController GetComponent(PlayerId playerId) { return cameraControllers_[playerId]; }
    private:
        std::vector<CameraController> cameraControllers_;
        PlayerManager& playerManager_;

        AerEngine& engine_;
        Transform3dManager& transformManager_;
        physics::RigidDynamicManager& rigidDynamicManager_;
        physics::PhysicsEngine& physicsEngine_;
    	
        const CameraParameter kCameraParameter_;

    };

    /**
     * \brief The Component Manager use to serialize to json and imgui components
     */
    class CameraControllerViewer : public ComponentViewer
    {
    public:
        explicit CameraControllerViewer(EntityManager& entityManager, PlayerManager& playerManager, CameraControllerManager& cameraControllerManager);

        virtual ~CameraControllerViewer() = default;

        /**
         * \brief Get a json object of the component of an entity
         * \return json object with component parameter
         */
        json GetJsonFromComponent(Entity entity) const override;

        /**
         * \brief Set a component of an entity from a json of the component
         * \componentJson json object with component parameter
         */
        void SetComponentFromJson(Entity entity, const json& componentJson) override;

        /**
         * \brief Draw the Imgui with the component parameter of an entity
         */
        void DrawImGui(Entity entity) override;

    private:
        PlayerManager& playerManager_;
        CameraControllerManager& cameraControllerManager_;
    };
}
