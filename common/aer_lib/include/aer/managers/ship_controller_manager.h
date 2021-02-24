#pragma once
#include "px/physics_engine.h"
#include "px/rigidbody.h"
#include "engine/component.h"
#include "engine/transform.h"
#include "ship_input_manager.h"

namespace neko::aer
{
class PID
{
public:
    float Seek(float seekValue, float currentValue, float deltaTime);

     float pCoeff = 0.8f;
     float iCoeff = 0.002f;
     float dCoeff = 0.05f;
     float minimum = -1;
     float maximum = 1;

private:
    float integral;
    float lastProportional;
};

class ShipControllerViewer;
/**
 * \brief Component used to control the ship movements.
 */
struct ShipParameter {
    //Drive
    const float kForwardForce = 200.0f;
    const float kSlowingVelFactor = 0.99f;
    const float kBrakingVelFactor = 0.95f;
    const float kAngleOfRoll = 10.0f;
    const float kAngleOfPitch = 15.0f;
    const float kRotationMultiplicator = 2.0f;
    const float kPropultionMultiplicator = 1.2f;
    const float kRotorMaxAngle = 30.0f;
    //Hover
    const float kHoverHeight = 8.0f;
    const float kMaxGroundDist = 10.0f;
    const float kHoverForce = 300.0f;
    const std::string kLayerName = "Ground";
    //Physics
    const float kBounceForce = 100.0f;
    const float kTerminalVelocity = 1000.0f;
    const float kHoverGravity = 20.0f;
    const float kFallGravity = 300.0f;
};
/**
 * \brief Component used to control the ship movements.
 */
struct ShipController {
    //Hover
    PID hoverPid;
    //Physics
    float drag = 0.0f;
    bool isOnGround = false;
    float startHoverHeight = 0.0f;
    Entity shipModel = INVALID_ENTITY;
};

/**
 * \brief System that manages ShipControllers
 */
class ShipControllerManager:
					  public SystemInterface,
					  public physics::FixedUpdateInterface,
					  public physics::OnCollisionInterface,
                      public ComponentManager<ShipController, EntityMask(ComponentType::SHIP_CONTROLLER)>
{
public:
    explicit ShipControllerManager(
        EntityManager& entityManager,
        Transform3dManager& transform3DManager,
        physics::RigidDynamicManager& rigidDynamicManager,
        physics::RigidStaticManager& rigidStaticManager,
        physics::PhysicsEngine& physicsEngine);

	void Init() override;
	void Update(seconds dt) override;
	void FixedUpdate(seconds dt) override;
	void Destroy() override;
    void AddComponent(Entity entity) override;
    void CalculateHover(Entity entity, seconds dt);
    void CalculateThrust(Entity entity, seconds dt);
    void OnCollisionEnter(
        const physx::PxContactPairHeader& pairHeader) override;
protected:
    ShipInputManager shipInputManager_;
	Transform3dManager& transformManager_;
    physics::RigidDynamicManager& rigidDynamicManager_;
    physics::RigidStaticManager& rigidStaticManager_;
    physics::PhysicsEngine& physicsEngine_;

    ShipParameter shipParameter_;
};

/**
 * \brief The Component Manager use to serialize to json and imgui components
 */
class ShipControllerViewer : public ComponentViewer
{
public:
    explicit ShipControllerViewer(EntityManager& entityManager, ShipControllerManager& shipControllerManager);

    virtual ~ShipControllerViewer() = default;

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
    ShipControllerManager& shipControllerManager_;
};
}
