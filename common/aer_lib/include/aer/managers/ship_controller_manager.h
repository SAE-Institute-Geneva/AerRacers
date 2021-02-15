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
     float iCoeff = 0.0002f;
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
struct ShipController {
    //Drive
    float forwardForce_ = 150.0f;
    float slowingVelFactor_ = 0.99f;
    float brakingVelFactor_ = 0.95f;
    float angleOfRoll_ = 10.0f;
    float angleOfPitch_ = 15.0f;
    float rotationMultiplicator_ = 2.0f;
    float propultionMultiplicator_ = 1.2f;
    float rotorMaxAngle_ = 30.0f;

    //Hover
    float hoverHeight_ = 8.0f;
    float maxGroundDist_ = 10.0f;
    float hoverForce_ = 300.0f;
    std::string layerName_ = "Ground";
    PID hoverPID;

    //Physics
    float bounceForce_ = 100.0f;
    float terminalVelocity_ = 100.0f;
    float hoverGravity_ = 20.0f;
    float fallGravity_ = 80.0f;
    float drag_ = 0.0f;
    bool isOnGround_ = false;
    float startHoverHeight_ = 0.0f;
};

/**
 * \brief System that manages ShipControllers
 */
class ShipControllerManager:
					  public SystemInterface,
					  public physics::FixedUpdateInterface,
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
protected:
    ShipInputManager shipInputManager_;
	Transform3dManager& transformManager_;
    physics::RigidDynamicManager& rigidDynamicManager_;
    physics::RigidStaticManager& rigidStaticManager_;
    physics::PhysicsEngine& physicsEngine_;
	
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
