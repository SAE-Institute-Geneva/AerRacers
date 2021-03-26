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
#include <gtest/gtest.h>

#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

#include "aer/aer_engine.h"

#ifdef NEKO_GLES3
#include "gl/gles3_window.h"
#include "gl/graphics.h"
#else
#include "vk/graphics.h"
#include "vk/renderers/renderer_editor.h"
#include "vk/vulkan_window.h"
#endif

namespace neko::physics
{
#pragma region Interface
class SceneInterface : public DrawImGuiInterface,
					   public OnCollisionInterface,
					   public OnTriggerInterface,
					   public FixedUpdateInterface
{
public:
	explicit SceneInterface(aer::AerEngine& aerEngine) : aerEngine_(aerEngine) {}

public:
	virtual void InitActors(PhysicsEngine& physicsEngine) = 0;
	virtual void HasSucceed()                             = 0;
	virtual void Update(seconds) {}
	void DrawImGui() override {}
	virtual void FixedUpdate(seconds) override {}
	Entity viewedEntity  = INVALID_ENTITY;
	float engineDuration = 10.0f;

	void OnCollisionEnter(const physx::PxContactPairHeader&) override {}
	void OnCollisionStay(const physx::PxContactPairHeader&) override {}
	void OnCollisionExit(const physx::PxContactPairHeader&) override {}

	void OnTriggerEnter(physx::PxTriggerPair*) override {}
	void OnTriggerExit(physx::PxTriggerPair*) override {}

protected:
	aer::AerEngine& aerEngine_;
	EntityManager* entityManager_             = nullptr;
	Transform3dManager* transform3dManager_   = nullptr;
	RigidDynamicManager* rigidDynamicManager_ = nullptr;
	RigidStaticManager* rigidStaticManager_   = nullptr;
	aer::RenderManager* renderManager_        = nullptr;
	PhysicsEngine* physicsEngine_             = nullptr;
};

class TestPhysX : public SystemInterface, public DrawImGuiInterface
{
public:
	TestPhysX(aer::AerEngine& engine, SceneInterface& sceneInterface)
	   : engine_(engine),
		 entityManager_(engine.GetComponentManagerContainer().entityManager),
		 transform3dManager_(engine.GetComponentManagerContainer().transform3dManager),
		 renderManager_(engine.GetComponentManagerContainer().renderManager),
		 physicsEngine_(engine.GetPhysicsEngine()),
		 sceneInterface_(sceneInterface)
	{
		physicsEngine_.RegisterTriggerListener(sceneInterface_);
		physicsEngine_.RegisterCollisionListener(sceneInterface_);
		physicsEngine_.RegisterFixedUpdateListener(sceneInterface_);
		engine_.RegisterOnDrawUi(sceneInterface_);
	}

	void Init() override { sceneInterface_.InitActors(physicsEngine_); }

	void Update(seconds dt) override
	{
		const auto& config = BasicEngine::GetInstance()->GetConfig();
		sceneInterface_.Update(dt);

		updateCount_ += dt.count();
		if (updateCount_ >= sceneInterface_.engineDuration)
		{
			HasSucceed();
			engine_.Stop();
		}
	}

	void Destroy() override {}

	void DrawImGui() override
	{
		ImGui::Begin("Physics");
		ImGui::Text("RigidBody");
		if (physicsEngine_.IsPhysicRunning())
		{
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Physics is active");
			if (ImGui::Button("Stop")) { physicsEngine_.StopPhysic(); }
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Physics is not active");
			if (ImGui::Button("Play")) { physicsEngine_.StartPhysic(); }
		}
		ImGui::End();
	}

	void HasSucceed() { sceneInterface_.HasSucceed(); }

private:
	float updateCount_ = 0;

	aer::AerEngine& engine_;
	EntityManager& entityManager_;
	PhysicsEngine& physicsEngine_;
	Transform3dManager& transform3dManager_;
	aer::RenderManager& renderManager_;

	SceneInterface& sceneInterface_;
};
#pragma endregion

#ifdef NEKO_GLES3
#pragma region CubeFall
class SceneCubeFall final : public SceneInterface
{
public:
	explicit SceneCubeFall(aer::AerEngine& aerEngine) : SceneInterface(aerEngine) {}

	void InitActors(PhysicsEngine& physicsEngine) override
	{
		engineDuration       = 5.0f;
		entityManager_       = &aerEngine_.GetComponentManagerContainer().entityManager;
		transform3dManager_  = &aerEngine_.GetComponentManagerContainer().transform3dManager;
		renderManager_       = &aerEngine_.GetComponentManagerContainer().renderManager;
		rigidStaticManager_  = &aerEngine_.GetComponentManagerContainer().rigidStaticManager;
		rigidDynamicManager_ = &aerEngine_.GetComponentManagerContainer().rigidDynamicManager;
		physicsEngine_       = &physicsEngine;
		//Plane
		{
			planeEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(planeEntity_);
			transform3dManager_->SetRelativePosition(planeEntity_, planePosition_);
			transform3dManager_->SetRelativeScale(planeEntity_, Vec3f(5, 1, 5));
			RigidStaticData rigidStatic;
			rigidStatic.colliderType = ColliderType::BOX;
			rigidStatic.material     = PhysicsMaterial {0.5f, 0.5f, 0.1f};
			rigidStaticManager_->AddRigidStatic(planeEntity_, rigidStatic);
			renderManager_->AddComponent(planeEntity_);
			renderManager_->SetModel(
				planeEntity_, aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
		}
		//Cube
		for (int i = 0; i < kCubeNumbers; ++i)
		{
			cubeEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(cubeEntity_);
			transform3dManager_->SetRelativePosition(cubeEntity_, cubePosition_ + Vec3f(0, i, 0));
			transform3dManager_->SetRelativeScale(cubeEntity_, Vec3f::one / 4.0f);
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType = ColliderType::BOX;
			rigidDynamic.material     = PhysicsMaterial {0.5f, 0.5f, 0.1f};
			rigidDynamicManager_->AddRigidDynamic(cubeEntity_, rigidDynamic);
			renderManager_->AddComponent(cubeEntity_);
			renderManager_->SetModel(
				cubeEntity_, aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
		}
		//Sphere
		for (int i = 0; i < kSphereNumbers; ++i)
		{
			Entity sphereEntity = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(sphereEntity);
			transform3dManager_->SetRelativePosition(
				sphereEntity, cubePosition_ + Vec3f(0, i + kCubeNumbers, 0));
			transform3dManager_->SetRelativeScale(sphereEntity, Vec3f::one / 4.0f);
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType = ColliderType::SPHERE;
			rigidDynamic.material     = PhysicsMaterial {0.5f, 0.5f, 0.1f};
			rigidDynamicManager_->AddRigidDynamic(sphereEntity, rigidDynamic);
			renderManager_->AddComponent(sphereEntity);
			renderManager_->SetModel(
				sphereEntity, aerEngine_.GetConfig().dataRootPath + "models/sphere/sphere.obj");
		}
		//Capsule
		for (int i = 0; i < kCapsuleNumbers; ++i)
		{
			Entity capsuleEntity = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(capsuleEntity);
			transform3dManager_->SetRelativePosition(
				capsuleEntity, cubePosition_ + Vec3f(0, i + kCubeNumbers + kSphereNumbers, 0));
			transform3dManager_->SetRelativeScale(capsuleEntity, Vec3f::one / 4.0f);
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType = ColliderType::CAPSULE;
			rigidDynamic.material     = PhysicsMaterial {0.5f, 0.5f, 0.1f};
			rigidDynamicManager_->AddRigidDynamic(capsuleEntity, rigidDynamic);
			renderManager_->AddComponent(capsuleEntity);
			renderManager_->SetModel(
				capsuleEntity, aerEngine_.GetConfig().dataRootPath + "models/capsule/capsule.obj");
		}
		{
			plateformEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(plateformEntity_);
			transform3dManager_->SetRelativePosition(plateformEntity_, Vec3f(-10.0f, 0.0f, 0.0f));
			Quaternion quaternion =
				Quaternion::FromEuler(EulerAngles(degree_t(0), degree_t(0), degree_t(70)));
			LogDebug(Vec4f(quaternion).ToString());
			EulerAngles euler = Quaternion::ToEulerAngles(quaternion);
			LogDebug(Vec3f(euler.x.value(), euler.y.value(), euler.z.value()).ToString());
			transform3dManager_->SetRelativeRotation(
				plateformEntity_, Quaternion::ToEulerAngles(quaternion));
			transform3dManager_->SetRelativeScale(plateformEntity_, Vec3f(1.0f, 20.0f, 50.0f));
			RigidStaticData rigidStatic;
			rigidStatic.colliderType = ColliderType::BOX;
			rigidStatic.material     = PhysicsMaterial {0.5f, 0.5f, 0.1f};
			rigidStaticManager_->AddRigidStatic(plateformEntity_, rigidStatic);
			renderManager_->AddComponent(plateformEntity_);
			renderManager_->SetModel(
				plateformEntity_, aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
		}
		viewedEntity = plateformEntity_;
	}

	void HasSucceed() override { logDebug("Test without check"); }

	void Update(seconds dt) override
	{
		auto& inputLocator    = sdl::InputLocator::get();
		Entity selectedEntity = aerEngine_.GetEditorToolManager().GetSelectedEntity();
		if (selectedEntity != INVALID_ENTITY)
		{
			if (entityManager_->HasComponent(
					selectedEntity, EntityMask(ComponentType::RIGID_DYNAMIC)))
			{
				if (inputLocator.GetKeyState(sdl::KeyCodeType::KEY_LEFT_ALT) ==
					sdl::ButtonState::DOWN)
				{
					rigidDynamicManager_->AddForce(selectedEntity, Vec3f::up * 200.0f);
				}
				if (inputLocator.GetKeyState(sdl::KeyCodeType::KEY_LEFT_CTRL) ==
					sdl::ButtonState::DOWN)
				{
					rigidDynamicManager_->AddForceAtPosition(
						selectedEntity, Vec3f::up * 200.0f, Vec3f::one);
				}
				if (inputLocator.GetKeyState(sdl::KeyCodeType::KEY_0) == sdl::ButtonState::DOWN)
				{
					rigidDynamicManager_->SetLinearVelocity(selectedEntity, Vec3f::up);
				}
				if (inputLocator.GetKeyState(sdl::KeyCodeType::KEY_1) == sdl::ButtonState::DOWN)
				{
					rigidDynamicManager_->SetAngularVelocity(selectedEntity, Vec3f::up);
				}
			}
		}
	}

	void FixedUpdate(seconds dt) override {}

	void DrawImGui() override {}

private:
	const static size_t kCubeNumbers    = 10;
	const static size_t kSphereNumbers  = 10;
	const static size_t kCapsuleNumbers = 10;
	Vec3f cubePosition_                 = Vec3f(0.0f, 5.0f, -5.0f);
	Vec3f planePosition_                = Vec3f(0.0f, -3.0f, -5.0f);
	Vec3f cubePositions_[kCubeNumbers]  = {Vec3f(0.0f, 0.0f, 0.0f),
        Vec3f(2.0f, 5.0f, -15.0f),
        Vec3f(-1.5f, -2.2f, -2.5f),
        Vec3f(-3.8f, -2.0f, -12.3f),
        Vec3f(2.4f, -0.4f, -3.5f),
        Vec3f(-1.7f, 3.0f, -7.5f),
        Vec3f(1.3f, -2.0f, -2.5f),
        Vec3f(1.5f, 2.0f, -2.5f),
        Vec3f(1.5f, 0.2f, -1.5f),
        Vec3f(-1.3f, 1.0f, -1.5f)};

	Entity cubeEntity_;
	Entity planeEntity_;
	Entity plateformEntity_;
};

TEST(PhysX, TestCubeFall)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	//config.dataRootPath = "../data/";
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	aer::AerEngine engine(filesystem, &config, aer::ModeEnum::EDITOR);

	engine.SetWindowAndRenderer(&window, &renderer);

	SceneCubeFall sceneCubeFall = SceneCubeFall(engine);
	TestPhysX testPhysX(engine, sceneCubeFall);

	engine.RegisterOnDrawUi(testPhysX);
	engine.Init();
	testPhysX.Init();
	engine.RegisterSystem(testPhysX);
	engine.EngineLoop();
}

#pragma endregion
#pragma region ForceVelocity
class SceneForceVelocity final : public SceneInterface
{
public:
	explicit SceneForceVelocity(aer::AerEngine& aerEngine) : SceneInterface(aerEngine) {}
	void InitActors(PhysicsEngine& physicsEngine) override
	{
		engineDuration       = 5.0f;
		entityManager_       = &aerEngine_.GetComponentManagerContainer().entityManager;
		transform3dManager_  = &aerEngine_.GetComponentManagerContainer().transform3dManager;
		renderManager_       = &aerEngine_.GetComponentManagerContainer().renderManager;
		rigidStaticManager_  = &aerEngine_.GetComponentManagerContainer().rigidStaticManager;
		rigidDynamicManager_ = &aerEngine_.GetComponentManagerContainer().rigidDynamicManager;
		physicsEngine_       = &physicsEngine;

		Camera3D* camera = GizmosLocator::get().GetCamera();
		camera->position = Vec3f(1.5f, 2.5f, 20.0f);

		//Sphere Velocity
		{
			sphereVelocityEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(sphereVelocityEntity_);
			transform3dManager_->SetRelativePosition(
				sphereVelocityEntity_, cubePosition_ + Vec3f(0, 2.5f, 0));
			transform3dManager_->SetRelativeScale(sphereVelocityEntity_, Vec3f::one);
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType = ColliderType::SPHERE;
			rigidDynamic.useGravity   = false;
			rigidDynamicManager_->AddRigidDynamic(sphereVelocityEntity_, rigidDynamic);
			renderManager_->AddComponent(sphereVelocityEntity_);
			renderManager_->SetModel(sphereVelocityEntity_,
				aerEngine_.GetConfig().dataRootPath + "models/sphere/sphere.obj");
		}
		//Sphere Force
		{
			sphereForceEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(sphereForceEntity_);
			transform3dManager_->SetRelativePosition(
				sphereForceEntity_, cubePosition_ + Vec3f(0, 5.0f, 0));
			transform3dManager_->SetRelativeScale(sphereForceEntity_, Vec3f::one);
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType = ColliderType::SPHERE;
			rigidDynamic.useGravity   = false;
			rigidDynamicManager_->AddRigidDynamic(sphereForceEntity_, rigidDynamic);
			renderManager_->AddComponent(sphereForceEntity_);
			renderManager_->SetModel(sphereForceEntity_,
				aerEngine_.GetConfig().dataRootPath + "models/sphere/sphere.obj");
		}
		//Cube Force At Position
		{
			cubeForceAtPositionEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(cubeForceAtPositionEntity_);
			transform3dManager_->SetRelativePosition(
				cubeForceAtPositionEntity_, cubePosition_ + Vec3f(0, -2.5f, 0));
			transform3dManager_->SetRelativeScale(cubeForceAtPositionEntity_, Vec3f::one);
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType = ColliderType::BOX;
			rigidDynamic.useGravity   = false;
			rigidDynamicManager_->AddRigidDynamic(cubeForceAtPositionEntity_, rigidDynamic);
			renderManager_->AddComponent(cubeForceAtPositionEntity_);
			renderManager_->SetModel(cubeForceAtPositionEntity_,
				aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
		}
		//Cube Torque
		{
			cubeTorqueEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(cubeTorqueEntity_);
			transform3dManager_->SetRelativePosition(
				cubeTorqueEntity_, cubePosition_ + Vec3f(2.5f, -5.0f, 0));
			transform3dManager_->SetRelativeScale(cubeTorqueEntity_, Vec3f::one);
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType = ColliderType::BOX;
			rigidDynamic.useGravity   = false;
			rigidDynamicManager_->AddRigidDynamic(cubeTorqueEntity_, rigidDynamic);
			renderManager_->AddComponent(cubeTorqueEntity_);
			renderManager_->SetModel(
				cubeTorqueEntity_, aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
		}
		//Cube MoveRotation
		{
			cubeMoveRotationEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(cubeMoveRotationEntity_);
			transform3dManager_->SetRelativePosition(
				cubeMoveRotationEntity_, cubePosition_ + Vec3f(5.0f, -5.0f, 0));
			transform3dManager_->SetRelativeScale(cubeMoveRotationEntity_, Vec3f::one);
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType = ColliderType::BOX;
			rigidDynamic.useGravity   = false;
			rigidDynamicManager_->AddRigidDynamic(cubeMoveRotationEntity_, rigidDynamic);
			renderManager_->AddComponent(cubeMoveRotationEntity_);
			renderManager_->SetModel(cubeMoveRotationEntity_,
				aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
		}
		//Cube AngularVelocity
		{
			cubeAngularVelocityEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(cubeAngularVelocityEntity_);
			transform3dManager_->SetRelativePosition(
				cubeAngularVelocityEntity_, cubePosition_ + Vec3f(0, -5.0f, 0));
			transform3dManager_->SetRelativeScale(cubeAngularVelocityEntity_, Vec3f::one);
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType = ColliderType::BOX;
			rigidDynamic.useGravity   = false;
			rigidDynamicManager_->AddRigidDynamic(cubeAngularVelocityEntity_, rigidDynamic);
			renderManager_->AddComponent(cubeAngularVelocityEntity_);
			renderManager_->SetModel(cubeAngularVelocityEntity_,
				aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
		}
		rigidDynamicManager_->AddForce(sphereForceEntity_, sphereForce_);
		rigidDynamicManager_->AddForceAtPosition(
			cubeForceAtPositionEntity_, cubeForce_, cubeForcePos_);
		rigidDynamicManager_->AddRelativeTorque(cubeTorqueEntity_, cubeTorque_);
	}

	void PostHasSucceed()
	{
		float precision = 0.1f;
		//Sphere Velocity
		EXPECT_NEAR(transform3dManager_->GetRelativePosition(sphereVelocityEntity_).x,
			sphereVelocity_.x * kPhysicsDuration_,
			precision);
		EXPECT_NEAR(rigidDynamicManager_->GetDynamicData(sphereVelocityEntity_).linearVelocity.x,
			sphereVelocity_.x,
			precision);
		//Sphere Force
		/*EXPECT_NEAR(transform3dManager_->GetRelativePosition(sphereForceEntity_).x,
            sphereForce_.x * 1.0f/(kPhysicsDuration_*kPhysicsDuration_),
            precision);
        EXPECT_NEAR(
                         rigidDynamicManager_->GetDynamicData(sphereForceEntity_)
                        .linearVelocity.x,
            sphereForce_.x * 1.0f/kPhysicsDuration_,
            precision);*/
		//Cube Angular Velocity
		EXPECT_NEAR(radian_t(transform3dManager_->GetRelativeRotation(cubeAngularVelocityEntity_).z)
						.value(),
			cubeAngularVelocity_.z * kPhysicsDuration_,
			precision);
		EXPECT_NEAR(
			rigidDynamicManager_->GetDynamicData(cubeAngularVelocityEntity_).angularVelocity.z,
			cubeAngularVelocity_.z,
			precision);
		//Cube Torque
		//EXPECT_NEAR(transform3dManager_->GetRelativeRotation(cubeTorqueEntity_).x
		//        .value(),
		//    cubeTorque_.x * kPhysicsDuration_,
		//    precision);
		float modulo = 360.f;
		EXPECT_NEAR(
			std::fmod(degree_t(radian_t(rigidDynamicManager_->GetDynamicData(cubeTorqueEntity_)
											.angularVelocity.x))
						  .value(),
				modulo),
			std::fmod(cubeTorque_.x, modulo),
			10.0f);
		//Cube Rotation
		EXPECT_NEAR(transform3dManager_->GetRelativeRotation(cubeMoveRotationEntity_).x.value(),
			cubeRotation_.x.value(),
			precision);
		EXPECT_NEAR(transform3dManager_->GetRelativePosition(cubeMoveRotationEntity_).x,
			cubeNewPosition_.x,
			precision);
		//Cube Force At Position
		//EXPECT_NEAR(transform3dManager_->GetRelativeRotation(cubeAngularVelocityEntity_).z.value(),
		//    cubeAngularVelocity_.z * kPhysicsDuration_,
		//    0.1f);
	}

	void FixedUpdate(seconds dt) override
	{
		rigidDynamicManager_->SetAngularVelocity(cubeAngularVelocityEntity_, cubeAngularVelocity_);
		rigidDynamicManager_->SetLinearVelocity(sphereVelocityEntity_, sphereVelocity_);
		rigidDynamicManager_->MoveRotation(
			cubeMoveRotationEntity_, Quaternion::FromEuler(cubeRotation_));
		rigidDynamicManager_->MovePosition(cubeMoveRotationEntity_, cubeNewPosition_);
		if (physicsTimer_ >= kPhysicsDuration_)
		{
			physicsEngine_->StopPhysic();
			PostHasSucceed();
		}
		else
		{
			physicsTimer_ += dt.count();
		}
	}

	void DrawImGui() override {}
	void HasSucceed() override {}

private:
	Vec3f cubePosition_        = Vec3f(0.0f, 0.0f, 0.0f);
	Vec3f sphereVelocity_      = Vec3f(2.0f, 0.0f, 0.0f);
	Vec3f sphereForce_         = Vec3f(200.0f, 0.0f, 1.0f);
	Vec3f cubeForce_           = Vec3f(100.0f, 0.0f, 0.0f);
	Vec3f cubeForcePos_        = Vec3f(1.0f, 0.0f, 1.0f);
	Vec3f cubeTorque_          = Vec3f(100.0f, 0.0f, 0.0f);
	EulerAngles cubeRotation_  = EulerAngles(degree_t(100.0f), degree_t(0.0f), degree_t(0.0f));
	Vec3f cubeNewPosition_     = Vec3f(5.0f, 0.0f, 0.0f);
	Vec3f cubeAngularVelocity_ = Vec3f(0.0f, 0.0f, 1.0f);

	Entity sphereVelocityEntity_;
	Entity sphereForceEntity_;
	Entity cubeForceAtPositionEntity_;
	Entity cubeTorqueEntity_;
	Entity cubeMoveRotationEntity_;
	Entity cubeAngularVelocityEntity_;

	const float kPhysicsDuration_ = 2.0f;
	float physicsTimer_           = 0.0f;
};
TEST(PhysX, TestRotation)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	//config.dataRootPath = "../data/";
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	aer::AerEngine engine(filesystem, &config, aer::ModeEnum::EDITOR);

	engine.SetWindowAndRenderer(&window, &renderer);

	SceneForceVelocity sceneRotation = SceneForceVelocity(engine);
	TestPhysX testPhysX(engine, sceneRotation);

	engine.RegisterOnDrawUi(testPhysX);
	engine.Init();
	testPhysX.Init();
	engine.RegisterSystem(testPhysX);
	engine.EngineLoop();
}
#pragma endregion
#pragma region Raycast
class SceneRaycast final : public SceneInterface
{
public:
	explicit SceneRaycast(aer::AerEngine& aerEngine) : SceneInterface(aerEngine) {}
	void InitActors(PhysicsEngine& physicsEngine) override
	{
		engineDuration       = 0.5f;
		entityManager_       = &aerEngine_.GetComponentManagerContainer().entityManager;
		transform3dManager_  = &aerEngine_.GetComponentManagerContainer().transform3dManager;
		renderManager_       = &aerEngine_.GetComponentManagerContainer().renderManager;
		rigidStaticManager_  = &aerEngine_.GetComponentManagerContainer().rigidStaticManager;
		rigidDynamicManager_ = &aerEngine_.GetComponentManagerContainer().rigidDynamicManager;
		physicsEngine_       = &physicsEngine;
		//Plane
		{
			planeEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(planeEntity_);
			transform3dManager_->SetRelativePosition(planeEntity_, planePosition_);
			transform3dManager_->SetRelativeScale(planeEntity_, Vec3f(4, 1, 4));
			RigidStaticData rigidStatic;
			rigidStatic.colliderType              = ColliderType::BOX;
			rigidStatic.material                  = PhysicsMaterial {0.5f, 0.5f, 0.1f};
			rigidStatic.boxColliderData.isTrigger = false;
			rigidStatic.filterGroup               = FilterGroup::GROUND;
			rigidStaticManager_->AddRigidStatic(planeEntity_, rigidStatic);

			renderManager_->AddComponent(planeEntity_);
			renderManager_->SetModel(
				planeEntity_, aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
		}
		viewedEntity = planeEntity_;
	}

	void HasSucceed() override
	{
		EXPECT_TRUE(rayOneTouch_);
		EXPECT_FALSE(rayTwoTouch_);
		EXPECT_FALSE(rayThreeTouch_);
		EXPECT_FALSE(rayFourTouch_);
		EXPECT_TRUE(rayFilteredTouch_);
		EXPECT_FALSE(rayFilteredNotTouch_);
	}

	void Update(seconds dt) override
	{
		IGizmoRenderer& gizmosLocator = GizmosLocator::get();
		{
			//Raycast
			RaycastInfo raycastInfo = physicsEngine_->Raycast(rayOrigin_, rayDirection_, 50.0f);
			//std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
			//    " Distance : " << raycastInfo.GetDistance() <<
			//    " Position : " << raycastInfo.GetPoint() <<
			//    " Normal : " << raycastInfo.GetNormal() << std::endl;
			rayOneTouch_ = raycastInfo.touch;

			//Display Gizmo
			gizmosLocator.DrawLine(rayOrigin_,
				rayOrigin_ + rayDirection_ * 50.0f,
				raycastInfo.touch ? Color::green : Color::red,
				2.0f);
		}
		{
			//Raycast
			RaycastInfo raycastInfo =
				physicsEngine_->Raycast(rayOrigin_ + Vec3f::right * 2, rayDirection_, 2.0f);
			//std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
			//    " Distance : " << raycastInfo.GetDistance() <<
			//    " Position : " << raycastInfo.GetPoint() <<
			//    " Normal : " << raycastInfo.GetNormal() << std::endl;
			rayTwoTouch_ = raycastInfo.touch;

			//Display Gizmo
			gizmosLocator.DrawLine(rayOrigin_ + Vec3f::right * 2,
				rayOrigin_ + Vec3f::right * 2 + rayDirection_ * 2.0f,
				raycastInfo.touch ? Color::green : Color::red,
				2.0f);
		}
		{
			//Raycast
			RaycastInfo raycastInfo =
				physicsEngine_->Raycast(rayOrigin_ + Vec3f::left * 4, rayDirection_, 50.0f);
			//std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
			//    " Distance : " << raycastInfo.GetDistance() <<
			//    " Position : " << raycastInfo.GetPoint() <<
			//    " Normal : " << raycastInfo.GetNormal() << std::endl;
			rayThreeTouch_ = raycastInfo.touch;

			//Display Gizmo
			gizmosLocator.DrawLine(rayOrigin_ + Vec3f::left * 4,
				rayOrigin_ + Vec3f::left * 4 + rayDirection_ * 50.0f,
				raycastInfo.touch ? Color::green : Color::red,
				2.0f);
		}
		{
			//Raycast
			RaycastInfo raycastInfo = physicsEngine_->Raycast(
				rayOrigin_ + Vec3f::left * 2, rayOrigin_ + Vec3f::left * 2 + Vec3f::up, 50.0f);
			//std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
			//    " Distance : " << raycastInfo.GetDistance() <<
			//    " Position : " << raycastInfo.GetPoint() <<
			//    " Normal : " << raycastInfo.GetNormal() << std::endl;
			rayFourTouch_ = raycastInfo.touch;

			//Display Gizmo
			gizmosLocator.DrawLine(rayOrigin_ + Vec3f::left * 2,
				rayOrigin_ + Vec3f::left * 2 + Vec3f::up * 50.0f,
				raycastInfo.touch ? Color::green : Color::red,
				2.0f);
		}
		//FilterTouch ok
		{
			//Raycast
			RaycastInfo raycastInfo = physicsEngine_->Raycast(
				rayOrigin_ + Vec3f::right * 1, rayDirection_, 50.0f, FilterGroup::GROUND);
			//std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
			//    " Distance : " << raycastInfo.GetDistance() <<
			//    " Position : " << raycastInfo.GetPoint() <<
			//    " Normal : " << raycastInfo.GetNormal() << std::endl;
			rayFilteredTouch_ = raycastInfo.touch;

			//Display Gizmo
			gizmosLocator.DrawLine(rayOrigin_ + Vec3f::right * 1,
				rayOrigin_ + Vec3f::right * 1 + rayDirection_ * 50.0f,
				raycastInfo.touch ? Color::green : Color::red,
				2.0f);
		}
		//FilterTouch wrong
		{
			//Raycast
			RaycastInfo raycastInfo = physicsEngine_->Raycast(
				rayOrigin_ + Vec3f::right * -1, rayDirection_, 50.0f, FilterGroup::SHIP);
			//std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
			//    " Distance : " << raycastInfo.GetDistance() <<
			//    " Position : " << raycastInfo.GetPoint() <<
			//    " Normal : " << raycastInfo.GetNormal() << std::endl;
			rayFilteredNotTouch_ = raycastInfo.touch;

			//Display Gizmo
			gizmosLocator.DrawLine(rayOrigin_ + Vec3f::right * -1,
				rayOrigin_ + Vec3f::right * -1 + rayDirection_ * 50.0f,
				raycastInfo.touch ? Color::green : Color::red,
				2.0f);
		}
		//std::cout << std::endl;
	}

	void DrawImGui() override {}

private:
	Vec3f planePosition_ = Vec3f(0.0f, -3.0f, -5.0f);
	Entity planeEntity_;

	Vec3f rayOrigin_    = Vec3f(0.0f, 0.0f, -5.0f);
	Vec3f rayDirection_ = Vec3f::down;

	bool rayOneTouch_         = false;
	bool rayTwoTouch_         = false;
	bool rayThreeTouch_       = false;
	bool rayFourTouch_        = false;
	bool rayFilteredTouch_    = false;
	bool rayFilteredNotTouch_ = false;
};
TEST(PhysX, TestRaycast)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	//config.dataRootPath = "../data/";
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	aer::AerEngine engine(filesystem, &config, aer::ModeEnum::EDITOR);

	engine.SetWindowAndRenderer(&window, &renderer);

	SceneRaycast sceneRaycast = SceneRaycast(engine);
	TestPhysX testPhysX(engine, sceneRaycast);

	engine.RegisterOnDrawUi(testPhysX);
	engine.Init();
	testPhysX.Init();
	engine.RegisterSystem(testPhysX);
	engine.EngineLoop();
}
#pragma endregion
#pragma region Collision
class SceneCollision final : public SceneInterface
{
public:
	explicit SceneCollision(aer::AerEngine& aerEngine) : SceneInterface(aerEngine) {}
	void InitActors(PhysicsEngine& physicsEngine) override
	{
		engineDuration       = 2.0f;
		entityManager_       = &aerEngine_.GetComponentManagerContainer().entityManager;
		transform3dManager_  = &aerEngine_.GetComponentManagerContainer().transform3dManager;
		renderManager_       = &aerEngine_.GetComponentManagerContainer().renderManager;
		rigidStaticManager_  = &aerEngine_.GetComponentManagerContainer().rigidStaticManager;
		rigidDynamicManager_ = &aerEngine_.GetComponentManagerContainer().rigidDynamicManager;
		physicsEngine_       = &physicsEngine;
		//Plane
		{
			planeEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(planeEntity_);
			transform3dManager_->SetRelativePosition(planeEntity_, planePosition_);
			transform3dManager_->SetRelativeScale(planeEntity_, Vec3f(5, 0.5f, 5));
			RigidStaticData rigidStatic;
			rigidStatic.colliderType = ColliderType::BOX;
			rigidStaticManager_->AddRigidStatic(planeEntity_, rigidStatic);
			renderManager_->AddComponent(planeEntity_);
			renderManager_->SetModel(
				planeEntity_, aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
		}
		//Cube Touch
		{
			cubeTouchEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(cubeTouchEntity_);
			transform3dManager_->SetRelativePosition(
				cubeTouchEntity_, cubePosition_ + Vec3f::left * 1.0f);
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType = ColliderType::BOX;
			rigidDynamicManager_->AddRigidDynamic(cubeTouchEntity_, rigidDynamic);
			renderManager_->AddComponent(cubeTouchEntity_);
			renderManager_->SetModel(
				cubeTouchEntity_, aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
		}
		//Cube Trigger
		{
			cubeTriggerEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(cubeTriggerEntity_);
			transform3dManager_->SetRelativePosition(
				cubeTriggerEntity_, cubePosition_ + Vec3f::right * 1.0f);
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType              = ColliderType::BOX;
			rigidDynamic.boxColliderData.isTrigger = true;
			rigidDynamicManager_->AddRigidDynamic(cubeTriggerEntity_, rigidDynamic);
			renderManager_->AddComponent(cubeTriggerEntity_);
			renderManager_->SetModel(
				cubeTriggerEntity_, aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
		}
		//Cube Not Touch
		{
			cubeNotTouchEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(cubeNotTouchEntity_);
			transform3dManager_->SetRelativePosition(
				cubeNotTouchEntity_, cubePosition_ + Vec3f::right * 5.0f);
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType = ColliderType::BOX;
			rigidDynamicManager_->AddRigidDynamic(cubeNotTouchEntity_, rigidDynamic);
			renderManager_->AddComponent(cubeNotTouchEntity_);
			renderManager_->SetModel(
				cubeNotTouchEntity_, aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
		}
		//Cube Not Trigger
		{
			cubeNotTriggerEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(cubeNotTriggerEntity_);
			transform3dManager_->SetRelativePosition(
				cubeNotTriggerEntity_, cubePosition_ + Vec3f::left * 5.0f);
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType              = ColliderType::BOX;
			rigidDynamic.boxColliderData.isTrigger = true;
			rigidDynamicManager_->AddRigidDynamic(cubeNotTriggerEntity_, rigidDynamic);
			renderManager_->AddComponent(cubeNotTriggerEntity_);
			renderManager_->SetModel(cubeNotTriggerEntity_,
				aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
		}
		viewedEntity = cubeTouchEntity_;
	}

	void HasSucceed() override
	{
		EXPECT_TRUE(cubeTouch_);
		EXPECT_FALSE(cubeNotTouch_);
		EXPECT_TRUE(cubeTrigger_);
		EXPECT_TRUE(cubeTriggerExit_);
		EXPECT_FALSE(cubeNotTrigger_);
	}

	void Update(seconds dt) override {}

	void OnCollisionEnter(const physx::PxContactPairHeader& pairHeader) override
	{
		if (pairHeader.actors[0] ==
				rigidDynamicManager_->GetComponent(cubeTouchEntity_).GetPxRigidDynamic() ||
			pairHeader.actors[1] ==
				rigidDynamicManager_->GetComponent(cubeTouchEntity_).GetPxRigidDynamic())
		{
			cubeTouch_ = true;
		}
		if (pairHeader.actors[0] ==
				rigidDynamicManager_->GetComponent(cubeNotTouchEntity_).GetPxRigidDynamic() ||
			pairHeader.actors[1] ==
				rigidDynamicManager_->GetComponent(cubeNotTouchEntity_).GetPxRigidDynamic())
		{
			cubeNotTouchEntity_ = true;
		}
	}

	void OnTriggerEnter(physx::PxTriggerPair* pairs) override
	{
		if (pairs->triggerActor ==
			rigidDynamicManager_->GetComponent(cubeTriggerEntity_).GetPxRigidDynamic())
		{
			cubeTrigger_ = true;
		}
		if (pairs->triggerActor ==
			rigidDynamicManager_->GetComponent(cubeNotTriggerEntity_).GetPxRigidDynamic())
		{
			cubeNotTrigger_ = true;
		}
	}

	void OnTriggerExit(physx::PxTriggerPair* pairs) override
	{
		if (pairs->triggerActor ==
			rigidDynamicManager_->GetComponent(cubeTriggerEntity_).GetPxRigidDynamic())
		{
			cubeTriggerExit_ = true;
		}
	}

	void FixedUpdate(seconds dt) override {}

	void DrawImGui() override {}

private:
	const static size_t kCubeNumbers = 25;
	Vec3f cubePosition_              = Vec3f(0.0f, 5.0f, -5.0f);
	Vec3f planePosition_             = Vec3f(0.0f, 0.0f, -5.0f);

	Entity cubeTouchEntity_      = INVALID_ENTITY;
	Entity cubeNotTouchEntity_   = INVALID_ENTITY;
	Entity cubeTriggerEntity_    = INVALID_ENTITY;
	Entity cubeNotTriggerEntity_ = INVALID_ENTITY;

	Entity planeEntity_ = INVALID_ENTITY;

	bool cubeTouch_       = false;
	bool cubeNotTouch_    = false;
	bool cubeTrigger_     = false;
	bool cubeNotTrigger_  = false;
	bool cubeTriggerExit_ = false;
};
TEST(PhysX, TestTriggerCollision)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	//config.dataRootPath = "../data/";
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	aer::AerEngine engine(filesystem, &config, aer::ModeEnum::EDITOR);

	engine.SetWindowAndRenderer(&window, &renderer);

	SceneCollision sceneCollision = SceneCollision(engine);
	TestPhysX testPhysX(engine, sceneCollision);

	engine.RegisterOnDrawUi(testPhysX);
	engine.Init();
	testPhysX.Init();
	engine.RegisterSystem(testPhysX);
	engine.EngineLoop();
}
#pragma endregion
#pragma region MeshCollider
class SceneMeshCollider final : public SceneInterface
{
public:
	explicit SceneMeshCollider(aer::AerEngine& aerEngine) : SceneInterface(aerEngine) {}
	void InitActors(PhysicsEngine& physicsEngine) override
	{
		engineDuration       = 5.0f;
		entityManager_       = &aerEngine_.GetComponentManagerContainer().entityManager;
		transform3dManager_  = &aerEngine_.GetComponentManagerContainer().transform3dManager;
		renderManager_       = &aerEngine_.GetComponentManagerContainer().renderManager;
		rigidStaticManager_  = &aerEngine_.GetComponentManagerContainer().rigidStaticManager;
		rigidDynamicManager_ = &aerEngine_.GetComponentManagerContainer().rigidDynamicManager;
		physicsEngine_       = &physicsEngine;
		//Plane
		{
			planeEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(planeEntity_);
			transform3dManager_->SetRelativePosition(planeEntity_, planePosition_);
			renderManager_->AddComponent(planeEntity_);
			renderManager_->SetModel(
				planeEntity_, aerEngine_.GetConfig().dataRootPath + "models/test/test.obj");
		}
		{
			sphereEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(sphereEntity_);
			transform3dManager_->SetRelativePosition(sphereEntity_, objPosition_);
			renderManager_->AddComponent(sphereEntity_);
			renderManager_->SetModel(
				sphereEntity_, aerEngine_.GetConfig().dataRootPath + "models/capsule/capsule.obj");
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType = ColliderType::CAPSULE;
			rigidDynamicManager_->AddRigidDynamic(sphereEntity_, rigidDynamic);
		}
		viewedEntity = sphereEntity_;
	}

	void HasSucceed() override {}

	void Update(seconds dt) override
	{
		if (gl::ModelManagerLocator::get().IsLoaded(
				renderManager_->GetComponent(planeEntity_).modelId) &&
			!entityManager_->HasComponent(planeEntity_, EntityMask(ComponentType::RIGID_STATIC)))
		{
			RigidStaticData rigidStatic;
			rigidStatic.colliderType = ColliderType::MESH;
			rigidStatic.meshColliderData.modelId =
				renderManager_->GetComponent(planeEntity_).modelId;
			rigidStaticManager_->AddRigidStatic(planeEntity_, rigidStatic);
		}
	}

	void FixedUpdate(seconds dt) override {}

	void DrawImGui() override {}

private:
	const static size_t kCubeNumbers = 25;
	Vec3f objPosition_               = Vec3f(0.1f, 7.0f, -5.0f);
	Vec3f planePosition_             = Vec3f(0.0f, -2.0f, -5.0f);

	Entity sphereEntity_ = INVALID_ENTITY;

	Entity planeEntity_ = INVALID_ENTITY;
};
TEST(PhysX, TestMeshCollider)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	//config.dataRootPath = "../data/";
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	aer::AerEngine engine(filesystem, &config, aer::ModeEnum::EDITOR);

	engine.SetWindowAndRenderer(&window, &renderer);

	SceneMeshCollider sceneMeshCollider = SceneMeshCollider(engine);
	TestPhysX testPhysX(engine, sceneMeshCollider);

	engine.RegisterOnDrawUi(testPhysX);
	engine.Init();
	testPhysX.Init();
	engine.RegisterSystem(testPhysX);
	engine.EngineLoop();
}
#pragma endregion
#pragma region GroundMeshCollider
class SceneGroundMeshCollider final : public SceneInterface
{
public:
	explicit SceneGroundMeshCollider(aer::AerEngine& aerEngine) : SceneInterface(aerEngine) {}
	void InitActors(PhysicsEngine& physicsEngine) override
	{
		engineDuration       = 10.0f;
		entityManager_       = &aerEngine_.GetComponentManagerContainer().entityManager;
		transform3dManager_  = &aerEngine_.GetComponentManagerContainer().transform3dManager;
		renderManager_       = &aerEngine_.GetComponentManagerContainer().renderManager;
		rigidStaticManager_  = &aerEngine_.GetComponentManagerContainer().rigidStaticManager;
		rigidDynamicManager_ = &aerEngine_.GetComponentManagerContainer().rigidDynamicManager;
		physicsEngine_       = &physicsEngine;
		Camera3D* camera     = GizmosLocator::get().GetCamera();
		camera->position     = cameraPosition_;
		//Plane
		{
			planeEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(planeEntity_);
			transform3dManager_->SetRelativePosition(planeEntity_, planePosition_);
			renderManager_->AddComponent(planeEntity_);
			renderManager_->SetModel(planeEntity_,
				aerEngine_.GetConfig().dataRootPath + "models/leveldesign/big_terrain_03.obj");
		}
		{
			sphereEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(sphereEntity_);
			transform3dManager_->SetRelativePosition(sphereEntity_, objPosition_);
			renderManager_->AddComponent(sphereEntity_);
			renderManager_->SetModel(
				sphereEntity_, aerEngine_.GetConfig().dataRootPath + "models/sphere/sphere.obj");
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType = ColliderType::SPHERE;
			rigidDynamicManager_->AddRigidDynamic(sphereEntity_, rigidDynamic);
		}
		viewedEntity = sphereEntity_;
	}

	void HasSucceed() override {}

	void Update(seconds dt) override
	{
		Camera3D* camera = GizmosLocator::get().GetCamera();
		camera->WorldLookAt(transform3dManager_->GetRelativePosition(sphereEntity_));
	}

	void FixedUpdate(seconds dt) override
	{
		if (gl::ModelManagerLocator::get().IsLoaded(
				renderManager_->GetComponent(planeEntity_).modelId) &&
			!entityManager_->HasComponent(planeEntity_, EntityMask(ComponentType::RIGID_STATIC)) &&
			gl::ModelManagerLocator::get().IsLoaded(
				renderManager_->GetComponent(sphereEntity_).modelId))
		{
#ifdef EASY_PROFILE_USE
			EASY_BLOCK("Load Mesh Collider", profiler::colors::Green);
#endif
			RigidStaticData rigidStatic;
			rigidStatic.colliderType = ColliderType::MESH;
			rigidStatic.meshColliderData.modelId =
				renderManager_->GetComponent(planeEntity_).modelId;
			rigidStatic.meshColliderData.size = 1.0f;
			rigidStaticManager_->AddRigidStatic(planeEntity_, rigidStatic);
		}
	}

	void DrawImGui() override {}

private:
	const static size_t kCubeNumbers = 25;
	//Vec3f objPosition_ = Vec3f(1300.0f, 150.0f, -1135.0f);
	//Vec3f cameraPosition_ = Vec3f(1300.0f, 157.0f, -1145.0f);
	Vec3f objPosition_    = Vec3f(60.0f, 10.0f, 15.0f);
	Vec3f cameraPosition_ = Vec3f(45.0f, 10.0f, 12.0f);
	Vec3f planePosition_  = Vec3f(0.0f, 0.0f, -5.0f);

	Entity sphereEntity_ = INVALID_ENTITY;

	Entity planeEntity_ = INVALID_ENTITY;
};
TEST(PhysX, TestGroundMeshCollider)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	//config.dataRootPath = "../data/";
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	aer::AerEngine engine(filesystem, &config, aer::ModeEnum::EDITOR);

	engine.SetWindowAndRenderer(&window, &renderer);

	SceneGroundMeshCollider sceneMeshCollider = SceneGroundMeshCollider(engine);
	TestPhysX testPhysX(engine, sceneMeshCollider);

	engine.RegisterOnDrawUi(testPhysX);
	engine.Init();
	testPhysX.Init();
	engine.RegisterSystem(testPhysX);
	engine.EngineLoop();
}
#pragma endregion
#pragma region SceneTest
#pragma region InterfaceScene
class TestSceneInterface
{
public:
	virtual void Init(neko::aer::AerEngine&) {}
	virtual void HasSucceed(neko::aer::ComponentManagerContainer& cContainer) = 0;
	std::string sceneName;
	float engineDuration = 10.0f;
};

class PhysXSceneImporterTester : public SystemInterface, public DrawImGuiInterface
{
public:
	explicit PhysXSceneImporterTester(neko::aer::AerEngine& engine, TestSceneInterface& testScene)
	   : engine_(engine), testScene_(testScene)
	{}

	void Init() override
	{
#ifdef EASY_PROFILE_USE
		EASY_BLOCK("Test Init", profiler::colors::Green);
#endif
		const Configuration config = BasicEngine::GetInstance()->GetConfig();
		engine_.GetComponentManagerContainer().sceneManager.LoadScene(
			config.dataRootPath + testScene_.sceneName);
		testScene_.Init(engine_);
		engineDuration_ = testScene_.engineDuration;
	}

	void Update(seconds dt) override
	{
#ifdef EASY_PROFILE_USE
		EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
		updateCount_ += dt.count();
		if (updateCount_ >= engineDuration_) { engine_.Stop(); }
	}

	void Destroy() override {}

	void HasSucceed() { testScene_.HasSucceed(engine_.GetComponentManagerContainer()); }

	void DrawImGui() override
	{
		Camera3D* camera = GizmosLocator::get().GetCamera();
		ImGui::Begin("Camera");
		{
			std::string position = camera->position.ToString();
			ImGui::Text(position.c_str());
			std::string rotation = camera->reverseDirection.ToString();
			ImGui::Text(rotation.c_str());
		}
		ImGui::End();
		ImGui::Begin("Physics");
		ImGui::Text("RigidBody");
		if (engine_.GetPhysicsEngine().IsPhysicRunning())
		{
			ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "Physics is active");
			if (ImGui::Button("Stop")) { engine_.GetPhysicsEngine().StopPhysic(); }
		}
		else
		{
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Physics is not active");
			if (ImGui::Button("Play")) { engine_.GetPhysicsEngine().StartPhysic(); }
		}
		ImGui::End();
	}

private:
	float updateCount_    = 0;
	float engineDuration_ = 5.0f;

	neko::aer::AerEngine& engine_;

	TestSceneInterface& testScene_;
};

#pragma endregion
#pragma region ExampleScene
class TestPhysXExampleScene : public TestSceneInterface
{
public:
	explicit TestPhysXExampleScene()
	{
		sceneName = "scenes/PlaygroundTest2021-02-03-11-20-04.aerscene";
	}

	void HasSucceed(neko::aer::ComponentManagerContainer& cContainer) override {}

	void Init(neko::aer::AerEngine& aerengine) override
	{
		Camera3D* camera = GizmosLocator::get().GetCamera();
		camera->position = Vec3f(7.0f, 45.0f, 0.0f);
		camera->Rotate(EulerAngles(degree_t(90.0f), degree_t(-90.0f), degree_t(0.0f)));
		//aerengine.GetPhysicsEngine().StopPhysic();
		engineDuration = 1.0f;
	}
};
TEST(PhysX, TestExampleSceneImporteur)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	neko::aer::AerEngine engine(filesystem, &config, neko::aer::ModeEnum::EDITOR);

	engine.SetWindowAndRenderer(&window, &renderer);
	TestPhysXExampleScene testExample;
	PhysXSceneImporterTester testSceneImporteur(engine, testExample);
	engine.RegisterSystem(testSceneImporteur);
	engine.RegisterOnDrawUi(testSceneImporteur);

	engine.Init();

	engine.EngineLoop();

	testSceneImporteur.HasSucceed();
}

#pragma endregion
#pragma region BounceScene
class TestPhysXBounceScene : public TestSceneInterface
{
public:
	explicit TestPhysXBounceScene() { sceneName = "scenes/PhysicsScene1-02-02.aerscene"; }

	void HasSucceed(neko::aer::ComponentManagerContainer& cContainer) override {}
	void Init(neko::aer::AerEngine& aerengine) override
	{
		Camera3D* camera = GizmosLocator::get().GetCamera();
		camera->position = Vec3f(0.0f, 3.0f, 10.0f);
		camera->fovY     = degree_t(60);
		engineDuration   = 5.0f;
		//aerengine.GetPhysicsEngine().StopPhysic();
	}
};
TEST(PhysX, TestPhysXBounceScene)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	neko::aer::AerEngine engine(filesystem, &config, neko::aer::ModeEnum::EDITOR);

	engine.SetWindowAndRenderer(&window, &renderer);
	TestPhysXBounceScene testExample;
	PhysXSceneImporterTester testSceneImporteur(engine, testExample);
	engine.RegisterSystem(testSceneImporteur);
	engine.RegisterOnDrawUi(testSceneImporteur);

	engine.Init();

	engine.EngineLoop();

	testSceneImporteur.HasSucceed();
}

#pragma endregion
#pragma region ColliderScene
class TestPhysXColliderScene : public TestSceneInterface
{
public:
	explicit TestPhysXColliderScene() { sceneName = "scenes/PhysicsScene2-02-02.aerscene"; }

	void HasSucceed(neko::aer::ComponentManagerContainer& cContainer) override {}
	void Init(neko::aer::AerEngine& aerengine) override
	{
		Camera3D* camera = GizmosLocator::get().GetCamera();
		camera->position = Vec3f(0.0f, 1.0f, -10.0f);
		camera->Rotate(EulerAngles(degree_t(0), degree_t(180), degree_t(0)));
		engineDuration = 7.0f;
		//aerengine.GetPhysicsEngine().StopPhysic();
	}
};
TEST(PhysX, TestPhysXColliderScene)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	aer::AerEngine engine(filesystem, &config, neko::aer::ModeEnum::EDITOR);

	engine.SetWindowAndRenderer(&window, &renderer);
	TestPhysXColliderScene testExample;
	PhysXSceneImporterTester testSceneImporteur(engine, testExample);
	engine.RegisterSystem(testSceneImporteur);
	engine.RegisterOnDrawUi(testSceneImporteur);

	engine.Init();

	engine.EngineLoop();

	testSceneImporteur.HasSucceed();
}
#pragma endregion
#pragma endregion

#elif NEKO_VULKAN
#pragma region GroundMeshCollider
class SceneGroundMeshCollider final : public SceneInterface
{
public:
	explicit SceneGroundMeshCollider(aer::AerEngine& aerEngine) : SceneInterface(aerEngine) {}
	void InitActors(PhysicsEngine& physicsEngine) override
	{
		engineDuration       = 5.0f;
		entityManager_       = &aerEngine_.GetComponentManagerContainer().entityManager;
		transform3dManager_  = &aerEngine_.GetComponentManagerContainer().transform3dManager;
		renderManager_       = &aerEngine_.GetComponentManagerContainer().renderManager;
		rigidStaticManager_  = &aerEngine_.GetComponentManagerContainer().rigidStaticManager;
		rigidDynamicManager_ = &aerEngine_.GetComponentManagerContainer().rigidDynamicManager;
		physicsEngine_       = &physicsEngine;
		Camera3D* camera     = GizmosLocator::get().GetCamera();
		camera->position     = cameraPosition_;
		//Plane
		{
			planeEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(planeEntity_);
			transform3dManager_->SetRelativePosition(planeEntity_, planePosition_);
			renderManager_->AddComponent(planeEntity_);
			renderManager_->SetModel(planeEntity_,
				aerEngine_.GetConfig().dataRootPath + "models/leveldesign/big_terrain_03.obj");
		}
		{
			sphereEntity_ = entityManager_->CreateEntity();
			transform3dManager_->AddComponent(sphereEntity_);
			transform3dManager_->SetRelativePosition(sphereEntity_, objPosition_);
			renderManager_->AddComponent(sphereEntity_);
			renderManager_->SetModel(
				sphereEntity_, aerEngine_.GetConfig().dataRootPath + "models/sphere/sphere.obj");
			RigidDynamicData rigidDynamic;
			rigidDynamic.colliderType = ColliderType::SPHERE;
			rigidDynamicManager_->AddRigidDynamic(sphereEntity_, rigidDynamic);
		}
		viewedEntity = sphereEntity_;
	}

	void HasSucceed() override {}

	void Update(seconds dt) override
	{
		Camera3D* camera = GizmosLocator::get().GetCamera();
		camera->WorldLookAt(transform3dManager_->GetRelativePosition(sphereEntity_));
	}

	void FixedUpdate(seconds dt) override
	{
		if (vk::ModelManagerLocator::get().IsLoaded(
				renderManager_->GetComponent(planeEntity_).modelId) &&
			!entityManager_->HasComponent(planeEntity_, EntityMask(ComponentType::RIGID_STATIC)))
		{
			RigidStaticData rigidStatic;
			rigidStatic.colliderType = ColliderType::MESH;
			rigidStatic.meshColliderData.modelId =
				renderManager_->GetComponent(planeEntity_).modelId;
			rigidStatic.meshColliderData.size = 1.0f;
			rigidStaticManager_->AddRigidStatic(planeEntity_, rigidStatic);
		}
	}

	void DrawImGui() override {}

private:
	const static size_t kCubeNumbers = 25;
	//Vec3f objPosition_ = Vec3f(1300.0f, 150.0f, -1135.0f);
	//Vec3f cameraPosition_ = Vec3f(1300.0f, 157.0f, -1145.0f);
	Vec3f objPosition_    = Vec3f(60.0f, 10.0f, 15.0f);
	Vec3f cameraPosition_ = Vec3f(45.0f, 10.0f, 12.0f);
	Vec3f planePosition_  = Vec3f(0.0f, 0.0f, -5.0f);

	Entity sphereEntity_ = INVALID_ENTITY;

	Entity planeEntity_ = INVALID_ENTITY;
};

TEST(PhysX, TestGroundMeshCollider)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	//config.dataRootPath = "../data/";
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1400, 900);

	Filesystem filesystem;
	aer::AerEngine engine(filesystem, &config, aer::ModeEnum::EDITOR);
	sdl::VulkanWindow window;
	vk::VkRenderer renderer(&window);
	renderer.SetRenderer(std::make_unique<vk::RendererEditor>());

	engine.SetWindowAndRenderer(&window, &renderer);

	SceneGroundMeshCollider sceneMeshCollider = SceneGroundMeshCollider(engine);
	TestPhysX testPhysX(engine, sceneMeshCollider);

	engine.RegisterOnDrawUi(testPhysX);
	engine.Init();
	testPhysX.Init();
	engine.RegisterSystem(testPhysX);
	engine.EngineLoop();
}
#pragma endregion
#endif
}

