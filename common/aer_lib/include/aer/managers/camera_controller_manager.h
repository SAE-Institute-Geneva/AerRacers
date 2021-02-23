#pragma once
#include "px/physics_engine.h"
#include "px/rigidbody.h"
#include "engine/component.h"
#include "engine/transform.h"
#include "ship_input_manager.h"

namespace neko::aer
{
    class CameraControllerViewer;
    /**
     * \brief Component used to control the ship movements.
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
        /* Old Camera
        int sprintStiffness = 10;
        float cameraMass = 0.1f;
        float forceLossPercentage = 0.5f;

        float minimumSpeed = 20.0f;
        float lookingDownIntensity = 45.0f;
        float maxYVelocity = 30.0f;
        float lookAngle = 25.0f;
        float cameraFixedY = 2.0f;

        Vec3f cameraVelocity = Vec3f::zero;
        Vec3f lastFrameShipVelocity;

        float lerpLookTurning = 0.025f;
        float cameraLookLerpValue = 0.2f;
        float maxYForce = 1000.0f;
        float cameraPositionLerpValue = 0.9f;
        */
    };

    /**
     * \brief System that manages ShipControllers
     */
    class CameraControllerManager :
        public SystemInterface,
        public physics::FixedUpdateInterface,
        public ComponentManager<CameraController, EntityMask(ComponentType::SHIP_CAMERA)>
    {
    public:
        explicit CameraControllerManager(
            EntityManager& entityManager,
            Transform3dManager& transform3DManager,
            physics::RigidDynamicManager& rigidDynamicManager,
            physics::PhysicsEngine& physicsEngine);

        void Init() override;
        void Update(seconds dt) override;
        void FixedUpdate(seconds dt) override;
        void Destroy() override;
        void AddComponent(Entity entity) override;

        /*
        void CreateTargetsEntity();
        void LookTurning();
        void LookDown();
        Vec2f SpringForceSmooth(Vec2f force);
        Vec3f SpringForceSmooth(Vec3f force);
        Vec3f SpringPosition2D();
        Vec3f SpringPosition();
        Vec3f CameraSmoothSpring(Vec3f springPosition);
        void SpringTransformChild(Entity shipChildEntity);
        void CameraSmoothLerp(Vec3f cameraPosition);

        void LastFrameShipValues();
        void MaximumAngle();
        */
    
    protected:
        ShipInputManager shipInputManager_;
        Transform3dManager& transformManager_;
        physics::RigidDynamicManager& rigidDynamicManager_;
        physics::PhysicsEngine& physicsEngine_;
    	
        Entity cameraEntity_;
        Entity shipEntity_;

        const Vec3f kTargetPosition_ = Vec3f(0, 0, 5);
        const float kMaxTargetPos_ = 25.0f;
        const Vec3f kCameraPosition_ = Vec3f(0, 5, -10);

        const float kAngularLateralMult_ = 3.0f;
        const float kAngularBackwardMult_ = 1.0f;
        const float kAngularForwardTargetMult_ = 80.0f;

        const float kLinearUpwardMult_ = 0.01f;
        const float kLinerarBackwardMult_ = -0.01f;
        const float kLinerarBackwardDiv_ = -0.0015f;
        const float kLinearForwardTargetMult_ = 0.1f;
        const float kFallMultiplicator_ = 0.01f;
        const float kFallTargetMultiplicator_ = -0.1f;

        const float kAngularLerp_ = 0.1f;
        const float kAngularTargetLerp_ = 0.1f;

        const float kLinearLerp_ = 1.0f; 
        const float kLerpPosition_ = 1.0f;

        /*Old Camera
        Entity targetEntity_;
        Entity lookTargetLeft_;
        Entity lookTargetRight_;
        Entity lookTargetNormal_;
        Entity springEntity_;
    	
        Vec3f lookTargetRightPos_ = Vec3f(0, 6.85f, -9.58f);
        Vec3f lookTargetLeftPos_ = Vec3f(-1, 6.85f, -9.58f);
        Vec3f lookTargetNormalPos_ = Vec3f(0, 6.85f, -9.58f);
        Vec3f targetPos_ = Vec3f(0, 2, -10);

        EulerAngles lookTargetRightRot_ = EulerAngles(0, 5, 0);
        EulerAngles lookTargetLeftRot_ = EulerAngles(0, -5, 0);
        */
    };

    /**
     * \brief The Component Manager use to serialize to json and imgui components
     */
    class CameraControllerViewer : public ComponentViewer
    {
    public:
        explicit CameraControllerViewer(EntityManager& entityManager, CameraControllerManager& cameraControllerManager);

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
        CameraControllerManager& cameraControllerManager_;
    };
}
