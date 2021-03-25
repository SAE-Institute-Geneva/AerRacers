#pragma once
/* ----------------------------------------------------
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author : Floreau Luca
 Co-Author :
 Date : 22.11.2020
---------------------------------------------------------- */
#include "aer/managers/render_manager.h"
#include "engine/transform.h"

#include "px/collider.h"
#include "px/filter_group.h"
#include "px/physics_callbacks.h"

#ifdef NEKO_GLES3
#include "gl/model_manager.h"
#else
#include "vk/models/model_manager.h"
#endif

namespace neko::physics
{
class PhysicsEngine;
/**
 * \brief Data updated when physics is running
 */
struct DynamicData
{
	Vec3f linearVelocity  = Vec3f::zero;
	Vec3f angularVelocity = Vec3f::zero;
};

#pragma region RigidActor
/**
 * \brief All data used by RigidActor
 */
struct RigidActorData
{
	PhysicsMaterial material;
	ColliderType colliderType = ColliderType::INVALID;
	BoxColliderData boxColliderData;
	SphereColliderData sphereColliderData;
	CapsuleColliderData capsuleColliderData;
	MeshColliderData meshColliderData;
	FilterGroup::Enum filterGroup = FilterGroup::DEFAULT;
};

/**
 * \brief Struct containing pointers of PxMaterial and PxShape and functions to read physics info
 */
struct RigidActor
{
public:
	[[nodiscard]] physx::PxShape* GetPxShape() const { return shape_; }
	[[nodiscard]] physx::PxMaterial* GetPxMaterial() const { return material_; }
	[[nodiscard]] ColliderType GetColliderType() const;

	/**
	 * \brief Use to get parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 */
	[[nodiscard]] SphereColliderData GetSphereColliderData() const;

	/**
	 * \brief Use to get parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 */
	[[nodiscard]] BoxColliderData GetBoxColliderData() const;

	/**
	 * \brief Use to get parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 */
	[[nodiscard]] CapsuleColliderData GetCapsuleColliderData() const;

	/**
	 * \brief Use to get parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 */
	[[nodiscard]] PhysicsMaterial GetPhysicsMaterial() const;

	/**
	 * \brief Use to get parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 */
	void SetSphereColliderData(const SphereColliderData& sphereColliderData) const;

	/**
	 * \brief Use to get parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 */
	void SetBoxColliderData(const BoxColliderData& boxColliderData) const;

	/**
	 * \brief Use to get parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 */
	void SetCapsuleColliderData(const CapsuleColliderData& capsuleColliderData) const;

	/**
	 * \brief Use to get parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 */
	void SetMeshColliderData(const MeshColliderData& meshColliderData) const;

	/**
	 * \brief Use to get parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 */
	void SetMaterial(const PhysicsMaterial& physicsMaterial) const;

protected:
	static physx::PxMaterial* InitMaterial(
		physx::PxPhysics* physics, const PhysicsMaterial& material) ;
	static physx::PxShape* InitBoxShape(physx::PxPhysics* physics,
		physx::PxMaterial* material,
		const BoxColliderData& boxCollider);
	static physx::PxShape* InitCapsuleShape(physx::PxPhysics* physics,
		physx::PxMaterial* material,
		const CapsuleColliderData& capsuleCollider) ;
	static physx::PxShape* InitSphereShape(physx::PxPhysics* physics,
		physx::PxMaterial* material,
		const SphereColliderData& sphereCollider) ;
	static physx::PxShape* InitMeshCollider(const PhysicsEngine& physics,
		physx::PxMaterial* material,
#ifdef NEKO_GLES3
		const gl::Mesh& mesh,
#else
		const vk::Mesh& mesh,
#endif
		const physx::PxMeshScale& scale) ;
	static void SetFiltering(physx::PxShape* shape, physx::PxU32 filterGroup);

	physx::PxMaterial* material_ = nullptr;
	physx::PxShape* shape_       = nullptr;
};

/**
 * \brief Class use to serialize to/from json and imgui colliders and materials
 */
class RigidActorViewer
{
protected:
	/**
	 * \brief Get a json object of the boxCollider of a RigidActorData
	 * \return json object with the boxCollider parameter
	 */
	[[nodiscard]] static json GetJsonFromBoxCollider(const RigidActorData& rigidActorData) ;

	/**
	 * \brief Get a json object of the sphereCollider of a RigidActorData
	 * \return json object with the sphereCollider parameter
	 */
	[[nodiscard]] static json GetJsonFromSphereCollider(const RigidActorData& rigidActorData) ;

	/**
	 * \brief Get a json object of the sphereCollider of a RigidActorData
	 * \return json object with the sphereCollider parameter
	 */
	[[nodiscard]] static json GetJsonFromCapsuleCollider(const RigidActorData& rigidActorData) ;

	/**
	 * \brief Get a json object of the material of a RigidActorData
	 * \return json object with the material parameter
	 */
	[[nodiscard]] static json GetJsonFromMaterial(const RigidActorData& rigidActorData) ;

	/**
	 * \brief Create a RigidActor from the json of a rigidActor
	 */
	static RigidActorData GetRigidActorFromJson(const json& rigidActorJson);

	/**
	 * \brief Draw with Imgui rigidActor parameter of a rigidActor
	 */
	static RigidActorData DrawImGuiRigidActor(const RigidActorData& rigidActorData);
};
#pragma endregion

#pragma region RigidStatic
/**
 * \brief Data used by RigidStatic
 */
struct RigidStaticData : public RigidActorData
{
	RigidStaticData()  = default;
	~RigidStaticData() = default;
};

/**
 * \brief Struct containing pointers of PxRigidStatic and functions to read physics info and create RigidStaticData
 */
struct RigidStatic : RigidActor
{
	/**
	 * \brief Init a rigidStatic actor in physx scene
	 * \param physics the PxPhysics pointer of the physicsEngine
	 * \param rigidStatic the data used to generate the rigidStatic
	 * \param position initial position of the actor
	 * \param eulerAngle initial rotation of the actor
	 */
	void Init(const PhysicsEngine& physics,
		const RigidStaticData& rigidStatic,
		const Vec3f& position,
		const EulerAngles& eulerAngle);

	[[nodiscard]] physx::PxRigidStatic* GetPxRigidStatic() const { return rigidActor_; }

	/**
	 * \brief Use to modify parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 * \param rigidStaticData parameter to update
	 */
	void SetRigidStaticData(const RigidStaticData& rigidStaticData) const;

	/**
	 * \brief Use to get parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 */
	[[nodiscard]] RigidStaticData GetRigidStaticData() const;

private:
	physx::PxRigidStatic* rigidActor_ = nullptr;
};

/**
 * \brief Manager for component of static objects
 * RigidStatic are used for physics object without motion
 */
class RigidStaticManager final
   : public ComponentManager<RigidStatic, EntityMask(ComponentType::RIGID_STATIC)>,
	 public FixedUpdateInterface
{
public:
	RigidStaticManager(EntityManager& entityManager,
		Transform3dManager& transform3dManager,
		aer::RenderManager& renderManager,
		PhysicsEngine& physicsEngine);

	void FixedUpdate(seconds dt) override;

	/**
	 * \brief Create a rigidStatic Actor in the PxScene
	 * \param body initial rigidstatic data
	 */
	void AddRigidStatic(Entity entity, const RigidStaticData& body);

	/**
	 * \brief Create a meshCollider rigidStatic Actor in the PxScene
	 * \param modelName of the meshCollider
	 */
	void AddMeshColliderStatic(Entity entity, const std::string& modelName);

	/**
	 * \brief Use to get parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 */
	[[nodiscard]] const RigidStaticData& GetRigidStaticData(Entity entity) const;

	/**
	 * \brief Use to modify parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 * \param rigidStaticData parameter to update
	 */
	void SetRigidStaticData(Entity entity, const RigidStaticData& rigidStaticData) const;

	/**
	 * \brief Remove the actor from the scene
	 */
	void DestroyComponent(Entity entity) override;

	/**
	 * \brief Find the Entity index from an actor
	 */
	Entity FindEntityFromActor(physx::PxActor* actor);

protected:
	Transform3dManager& transform3dManager_;
	PhysicsEngine& physicsEngine_;
	aer::RenderManager& renderManager_;
	std::vector<std::pair<Entity, gl::ModelId>> meshColliderToCreate_;
};

/**
 * \brief Class use to serialize to/from json and imgui rigid static data
 */
class RigidStaticViewer final : public ComponentViewer,
								public FixedUpdateInterface,
								public RigidActorViewer
{
public:
	RigidStaticViewer(Transform3dManager& transform3dManager,
		EntityManager& entityManager,
		PhysicsEngine& physicsEngine,
		RigidStaticManager& rigidStaticManager);

	/**
	 * \brief Entity to read at the next fixed update
	 */
	void SetSelectedEntity(Entity selectedEntity);

	/**
	 * \brief Fixed Update when the data is up-to-date when the physics is running
	 */
	void FixedUpdate(seconds dt) override;

	/**
	 * \brief Get a json object of the component of an entity
	 * \return json object with component parameter
	 */
	[[nodiscard]] json GetJsonFromComponent(Entity entity) const override;

	/**
	 * \brief Set a component of an entity from a json of the component
	 * \componentJson json object with component parameter
	 */
	void SetComponentFromJson(Entity entity, const json& jsonComponent) override;

	/**
	 * \brief Draw the Imgui with the component parameter of an entity
	 */
	void DrawImGui(Entity entity) override;

protected:
	Entity selectedEntity_ = INVALID_ENTITY;
	Transform3dManager& transform3dManager_;
	RigidStaticData rigidStaticData_;
	PhysicsEngine& physicsEngine_;
	RigidStaticManager& rigidStaticManager_;
};
#pragma endregion

#pragma region RigidDynamic
/**
 * \brief Data used by RigidDynamic
 */
struct RigidDynamicData : public RigidActorData
{
	RigidDynamicData()        = default;
	~RigidDynamicData()       = default;
	float linearDamping       = 0.0f;
	float angularDamping      = 0.0f;
	float mass                = 1.0f;
	bool useGravity           = true;
	bool isKinematic          = false;
	Vec3<bool> freezePosition = Vec3<bool>(false);
	Vec3<bool> freezeRotation = Vec3<bool>(false);
};

struct RigidDynamic : RigidActor
{
public:
	/**
	 * \brief Init a RigidDynamic actor in physx scene
	 * \param physics the PxPhysics pointer of the physicsEngine
	 * \param rigidDynamic the data used to generate the rigidDynamic
	 * \param position initial position of the actor
	 * \param eulerAngle initial rotation of the actor
	 */
	void Init(const PhysicsEngine& physics,
		const RigidDynamicData& rigidDynamic,
		const Vec3f& position,
		const EulerAngles& eulerAngle);

	/**
	 * \brief Add force at a relative position of the object
	 * \param force Force to apply
	 * \param position Relative position from the center
	 */
	void AddForceAtPosition(const Vec3f& force, const Vec3f& position) const;

	/**
	 * \brief Add force at the center of the object
	 * \param force Force to apply
	 * \param forceMode how force is apply
	 */
	void AddForce(
		const Vec3f& force, physx::PxForceMode::Enum forceMode = physx::PxForceMode::eFORCE) const;

	/**
	 * \brief Add Torque at the center of the object
	 * \param torque Torque to apply
	 * \param forceMode how the force will be apply
	 */
	void AddRelativeTorque(
		const Vec3f& torque, physx::PxForceMode::Enum forceMode = physx::PxForceMode::eFORCE) const;

	/**
	 * \brief Change rotation of the rigidbody
	 * \param rot The new rotation for the Rigidbody
	 */
	void MoveRotation(const Quaternion& rot) const;

	/**
	 * \brief Change position of the rigidbody
	 * \param pos The new position for the Rigidbody
	 */
	void MovePosition(const Vec3f& pos) const;

	/**
	 * \brief Use to modify parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 * \param rigidDynamicData parameter to update
	 */
	void SetRigidDynamicData(const RigidDynamicData& rigidDynamicData) const;

	/**
	 * \brief Use to get parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 */
	[[nodiscard]] RigidDynamicData GetRigidDynamicData() const;

	/**
	 * \brief Use to get dynamic parameter of an actor
	 */
	[[nodiscard]] DynamicData GetDynamicData() const;

	[[nodiscard]] physx::PxRigidDynamic* GetPxRigidDynamic() const { return rigidActor_; }

private:
	physx::PxRigidDynamic* rigidActor_ = nullptr;
};

/**
 * \brief Manager for component of dynamic objects
 */
class RigidDynamicManager final
   : public ComponentManager<RigidDynamic, EntityMask(ComponentType::RIGID_DYNAMIC)>,
	 public FixedUpdateInterface
{
public:
	RigidDynamicManager(EntityManager& entityManager,
		Transform3dManager& transform3dManager,
		PhysicsEngine& physicsEngine);

	/**
	 * \brief Use to update transform position based on physics motion
	 */
	void FixedUpdate(seconds dt) override;

	/**
	 * \brief Create a rigiDynamic Actor in the PxScene
	 * \param body initial rigidDynamic data
	 */
	void AddRigidDynamic(Entity entity, const RigidDynamicData& body);

	/**
	 * \brief Add force at a relative position of the object
	 * \param force Force to apply
	 * \param position Relative position from the center
	 */
	void AddForceAtPosition(Entity entity, const Vec3f& force, const Vec3f& position) const;

	/**
	 * \brief Add force at the center of the object
	 * \param force Force to apply
	 * \param forceMode how the force will be apply
	 */
	void AddForce(Entity entity,
		const Vec3f& force,
		physx::PxForceMode::Enum forceMode = physx::PxForceMode::eFORCE) const;

	/**
	 * \brief Add Torque at the center of the object
	 * \param torque Torque to apply
	 * \param forceMode how the force will be apply
	 */
	void AddRelativeTorque(Entity entity,
		const Vec3f& torque,
		physx::PxForceMode::Enum forceMode = physx::PxForceMode::eFORCE) const;

	/**
	 * \brief Change rotation of the rigidbody
	 * \param rot The new rotation for the Rigidbody
	 */
	void MoveRotation(Entity entity, const Quaternion& rot) const;

	/**
	 * \brief Change position of the rigidbody
	 * \param pos The new position for the Rigidbody
	 */
	void MovePosition(Entity entity, const Vec3f& pos) const;

	/**
	 * \brief Change linear velocity of the object
	 * \param linearVelocity new linear velocity
	 */
	void SetLinearVelocity(Entity entity, const Vec3f& linearVelocity) const;

	/**
	 * \brief Change angular velocity of the object
	 * \param angularVelocity new angular velocity
	 */
	void SetAngularVelocity(Entity entity, const Vec3f& angularVelocity) const;

	/**
	 * \brief Use to get parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 */
	[[nodiscard]] const RigidDynamicData GetRigidDynamicData(Entity entity) const;

	/**
	 * \brief Use to get dynamicData of an actor
	 */
	[[nodiscard]] const DynamicData GetDynamicData(Entity entity) const;

	/**
	 * \brief Use to modify parameter of an actor
	 * Warning must be call in FixedUpdate or if physics is not running
	 * \param rigidDynamicData parameter to update
	 */
	void SetRigidDynamicData(Entity entity, const RigidDynamicData& rigidDynamicData) const;

	/**
	 * \brief Remove the actor from the scene
	 */
	void DestroyComponent(Entity entity) override;

	/**
	 * \brief Find the Entity index from an actor
	 */
	Entity FindEntityFromActor(physx::PxActor* actor);

protected:
	Transform3dManager& transform3dManager_;
	PhysicsEngine& physicsEngine_;
};

/**
 * \brief Class use to serialize to/from json and imgui rigid static data
 */
class RigidDynamicViewer final : public ComponentViewer,
								 public FixedUpdateInterface,
								 public RigidActorViewer
{
public:
	RigidDynamicViewer(Transform3dManager& transform3dManager,
		EntityManager& entityManager,
		PhysicsEngine& physicsEngine,
		RigidDynamicManager& rigidDynamicManager);

	/**
	 * \brief Entity to read at the next fixed update
	 */
	void SetSelectedEntity(Entity selectedEntity);

	/**
	 * \brief Fixed Update when the data is up-to-date when the physics is running
	 */
	void FixedUpdate(seconds dt) override;

	/**
	 * \brief Get a json object of the component of an entity
	 * \return json object with component parameter
	 */
	[[nodiscard]] json GetJsonFromComponent(Entity entity) const override;

	/**
	 * \brief Set a component of an entity from a json of the component
	 * \componentJson json object with component parameter
	 */
	void SetComponentFromJson(Entity entity, const json& jsonComponent) override;

	/**
	 * \brief Draw the Imgui with the component parameter of an entity
	 */
	void DrawImGui(Entity entity) override;

protected:
	Entity selectedEntity_     = INVALID_ENTITY;
	Entity lastSelectedEntity_ = INVALID_ENTITY;
	Transform3dManager& transform3dManager_;
	PhysicsEngine& physicsEngine_;
	DynamicData dynamicData_;
	RigidDynamicData rigidDynamicData_;
	RigidDynamicManager& rigidDynamicManager_;
};
#pragma endregion
}    // namespace neko::physics
