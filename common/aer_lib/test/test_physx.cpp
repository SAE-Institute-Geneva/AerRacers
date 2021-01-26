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
#include "PxPhysicsAPI.h"
#include "PxConfig.h"


#include "aer/aer_engine.h"
#include "aer/log.h"
#include "aer/gizmos_renderer.h"
#include "aer/managers/render_manager.h"
#include "px/physics_engine.h"
#include "px/physics_callbacks.h"
#include "engine/engine.h"
#include "engine/system.h"
#include "engine/transform.h"
#include "gl/gles3_window.h"
#include "gl/graphics.h"
#include "gl/shader.h"
#include "gl/shape.h"

class SceneInterface :
    public neko::DrawImGuiInterface,
    public neko::physics::OnCollisionInterface,
    public neko::physics::OnTriggerInterface,
    public neko::physics::FixedUpdateInterface {
public:
    explicit SceneInterface(neko::aer::AerEngine& aerEngine)
        : aerEngine_(aerEngine) {}

protected:
    ~SceneInterface() = default;
public:
    virtual void InitActors(
        neko::physics::PhysicsEngine& physicsEngine) = 0;
    virtual void HasSucceed() = 0;
    virtual void Update() {}
    virtual void DrawImGui() override {}
    virtual void FixedUpdate(neko::seconds dt) override {}
    neko::Entity viewedEntity = neko::INVALID_ENTITY;
    float engineDuration = 10.0f;
protected:
    neko::aer::AerEngine& aerEngine_;
    neko::EntityManager* entityManager_                      = nullptr;
    neko::Transform3dManager* transform3dManager_            = nullptr;
    neko::physics::RigidDynamicManager* rigidDynamicManager_ = nullptr;
    neko::physics::RigidStaticManager* rigidStaticManager_   = nullptr;
    neko::aer::RenderManager* renderManager_                 = nullptr;
    neko::physics::PhysicsEngine* physicsEngine_             = nullptr;
};

class TestPhysX : public neko::SystemInterface, public neko::DrawImGuiInterface
{
public:
    TestPhysX(neko::aer::AerEngine& engine, SceneInterface& sceneInterface)
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

    void Update(neko::seconds dt) override
    {
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        sceneInterface_.Update();

        updateCount_+= dt.count();
        if (updateCount_ >= sceneInterface_.engineDuration) { engine_.Stop(); }
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

    neko::aer::AerEngine& engine_;
    neko::EntityManager& entityManager_;
    neko::physics::PhysicsEngine& physicsEngine_;
    neko::Transform3dManager& transform3dManager_;
    neko::aer::RenderManager& renderManager_;

    SceneInterface& sceneInterface_;
};

class SceneCubeFall final : public SceneInterface {
public :
    explicit SceneCubeFall(neko::aer::AerEngine& aerEngine)
        : SceneInterface(aerEngine) {}

    void InitActors(
        neko::physics::PhysicsEngine& physicsEngine) override
    {
        engineDuration      = 10.0f;
        entityManager_      = &aerEngine_.GetComponentManagerContainer().entityManager;
        transform3dManager_ = &aerEngine_.GetComponentManagerContainer().transform3dManager;
        renderManager_      = &aerEngine_.GetComponentManagerContainer().renderManager;
        rigidStaticManager_      = &aerEngine_.GetComponentManagerContainer().rigidStaticManager;
        rigidDynamicManager_      = &aerEngine_.GetComponentManagerContainer().rigidDynamicManager;
        physicsEngine_      = &physicsEngine;
        //Plane
        {
            planeEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(planeEntity_);
            transform3dManager_->SetRelativePosition(planeEntity_, planePosition_);
            transform3dManager_->SetRelativeScale(planeEntity_, neko::Vec3f(5, 1, 5));
            neko::physics::RigidStaticData rigidStatic;
            rigidStatic.colliderType = neko::physics::ColliderType::BOX;
            rigidStatic.material = neko::physics::PhysicsMaterial{
                0.5f,
                0.5f,
                0.1f
            };
            rigidStaticManager_->AddRigidStatic(
                planeEntity_,
                rigidStatic);
            renderManager_->AddComponent(planeEntity_);
            renderManager_->SetModel(
                planeEntity_, aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
        }
        //Cube
        for (int i = 0; i < kCubeNumbers; ++i) {
            cubeEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(cubeEntity_);
            transform3dManager_->SetRelativePosition(
                cubeEntity_,
                cubePosition_ + neko::Vec3f(0, i, 0));
            transform3dManager_->SetRelativeScale(cubeEntity_, neko::Vec3f::one / 4.0f);
            neko::physics::RigidDynamicData rigidDynamic;
            rigidDynamic.mass = 1.0f;
            rigidDynamic.useGravity = true;
            rigidDynamic.freezeRotation = neko::Vec3<bool>(false);
            rigidDynamic.colliderType = neko::physics::ColliderType::BOX;
            rigidDynamic.material = neko::physics::PhysicsMaterial{
                0.5f,
                0.5f,
                0.1f
            };
            rigidDynamicManager_->AddRigidDynamic(
                cubeEntity_, rigidDynamic);
            renderManager_->AddComponent(cubeEntity_);
            renderManager_->SetModel(
                cubeEntity_, aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
        }
        {
            plateformEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(plateformEntity_);
            transform3dManager_->SetRelativePosition(plateformEntity_, neko::Vec3f(-10.0f, 0.0f, 0.0f));
            neko::Quaternion quaternion = neko::Quaternion::FromEuler(
                neko::EulerAngles(neko::degree_t(0), neko::degree_t(0), neko::degree_t(70)));
            neko::LogDebug(neko::Vec4f(quaternion).ToString());
            neko::EulerAngles euler = neko::Quaternion::ToEulerAngles(quaternion);
            neko::LogDebug(
                neko::Vec3f(euler.x.value(), euler.y.value(), euler.z.value()).ToString());
            transform3dManager_->SetRelativeRotation(
                plateformEntity_, neko::Quaternion::ToEulerAngles(quaternion));
            transform3dManager_->SetRelativeScale(plateformEntity_, neko::Vec3f(1.0f, 20.0f, 50.0f));
            neko::physics::RigidStaticData rigidStatic;
            rigidStatic.colliderType = neko::physics::ColliderType::BOX;
            rigidStatic.material     = neko::physics::PhysicsMaterial {0.5f, 0.5f, 0.1f};
            rigidStaticManager_->AddRigidStatic(plateformEntity_, rigidStatic);
            renderManager_->AddComponent(plateformEntity_);
            renderManager_->SetModel(
                plateformEntity_, aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
        }
        viewedEntity = plateformEntity_;
    }

    void HasSucceed() override
    {
        EXPECT_TRUE(true);
        EXPECT_TRUE(true);
    }

    void Update() override
    {
        auto& inputLocator = neko::sdl::InputLocator::get();
        if (inputLocator.GetKeyState(neko::sdl::KeyCodeType::SPACE) ==
            neko::sdl::ButtonState::DOWN) {
            rigidDynamicManager_->AddForce(
                cubeEntity_,
                neko::Vec3f::up * 200.0f);
        }
        if (inputLocator.GetKeyState(neko::sdl::KeyCodeType::KEY_LEFT_CTRL) ==
            neko::sdl::ButtonState::DOWN) {
            rigidDynamicManager_->AddForceAtPosition(
                cubeEntity_,
                neko::Vec3f::up * 200.0f,
                neko::Vec3f::one);
        }

    }


    void OnCollisionEnter(const physx::PxContactPair& pairHeader) override
    {
        //logDebug(std::to_string(pairHeader.actors[0]->getNbShapes()));
    }

    void OnTriggerEnter(physx::PxTriggerPair* pairs) override
    {
        //logDebug(std::to_string(pairs->otherActor->getNbShapes()));
    }

    void FixedUpdate(neko::seconds dt) override { }

    void DrawImGui() override { }
private:
    const static size_t kCubeNumbers = 25;
    neko::Vec3f cubePosition_ = neko::Vec3f(0.0f, 5.0f, -5.0f);
    neko::Vec3f planePosition_ = neko::Vec3f(0.0f, -3.0f, -5.0f);
    neko::Vec3f cubePositions_[kCubeNumbers] =
    {
        neko::Vec3f(0.0f, 0.0f, 0.0f),
        neko::Vec3f(2.0f, 5.0f, -15.0f),
        neko::Vec3f(-1.5f, -2.2f, -2.5f),
        neko::Vec3f(-3.8f, -2.0f, -12.3f),
        neko::Vec3f(2.4f, -0.4f, -3.5f),
        neko::Vec3f(-1.7f, 3.0f, -7.5f),
        neko::Vec3f(1.3f, -2.0f, -2.5f),
        neko::Vec3f(1.5f, 2.0f, -2.5f),
        neko::Vec3f(1.5f, 0.2f, -1.5f),
        neko::Vec3f(-1.3f, 1.0f, -1.5f)
    };

    neko::Entity cubeEntity_;
    neko::Entity planeEntity_;
    neko::Entity plateformEntity_;

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

    neko::Configuration config;
    //config.dataRootPath = "../data/";
    config.windowName = "AerEditor";
    config.windowSize = neko::Vec2u(1400, 900);

    neko::sdl::Gles3Window window;
    neko::gl::Gles3Renderer renderer;
    neko::Filesystem filesystem;
    neko::aer::AerEngine engine(filesystem, &config, neko::aer::ModeEnum::EDITOR);

    engine.SetWindowAndRenderer(&window, &renderer);

    SceneCubeFall sceneCubeFall = SceneCubeFall(engine);
    TestPhysX testPhysX(engine, sceneCubeFall);

    engine.RegisterOnDrawUi(testPhysX);
    engine.Init();
    testPhysX.Init();
    engine.RegisterSystem(testPhysX);
    engine.EngineLoop();
}

class SceneRotation final : public SceneInterface {
public:
    explicit SceneRotation(neko::aer::AerEngine& aerEngine) : SceneInterface(aerEngine) {}
    void InitActors(
        neko::physics::PhysicsEngine& physicsEngine) override
    {
        engineDuration      = 10.0f;
        entityManager_      = &aerEngine_.GetComponentManagerContainer().entityManager;
        transform3dManager_ = &aerEngine_.GetComponentManagerContainer().transform3dManager;
        renderManager_      = &aerEngine_.GetComponentManagerContainer().renderManager;
        rigidStaticManager_      = &aerEngine_.GetComponentManagerContainer().rigidStaticManager;
        rigidDynamicManager_      = &aerEngine_.GetComponentManagerContainer().rigidDynamicManager;
        physicsEngine_      = &physicsEngine;

        //Cube
        cubeEntity_ = entityManager_->CreateEntity();
        transform3dManager_->AddComponent(cubeEntity_);
        transform3dManager_->SetRelativePosition(
            cubeEntity_,
            cubePosition_);
        transform3dManager_->SetRelativeScale(cubeEntity_, cubeSize_);
        neko::physics::RigidDynamicData rigidDynamic;
        rigidDynamic.colliderType = neko::physics::ColliderType::BOX;
        rigidDynamic.material = neko::physics::PhysicsMaterial{
            0.5f,
            0.5f,
            0.1f
        };
        rigidDynamic.mass = 1.0f;
        rigidDynamic.useGravity = false;
        rigidDynamic.freezeRotation = neko::Vec3<bool>(false);
        rigidDynamic.freezePosition = neko::Vec3<bool>(true);
        rigidDynamicManager_->AddRigidDynamic(
            cubeEntity_,
            rigidDynamic);
        rigidDynamicManager_->AddForceAtPosition(
            cubeEntity_,
            neko::Vec3f::up * 200.0f, neko::Vec3f::one);
        renderManager_->AddComponent(cubeEntity_);
        renderManager_->SetModel(
            cubeEntity_, aerEngine_.GetConfig().dataRootPath + "models/cube/cube.obj");
        viewedEntity = cubeEntity_;
    }

    void HasSucceed() override
    {
        EXPECT_TRUE(true);
        EXPECT_TRUE(true);
    }

    void Update() override
    {
        auto& inputLocator = neko::sdl::InputLocator::get();
        if (inputLocator.GetKeyState(neko::sdl::KeyCodeType::SPACE) ==
            neko::sdl::ButtonState::DOWN) {
        }
    }

    void DrawImGui() override { }
private:
    neko::Vec3f cubePosition_ = neko::Vec3f(0.0f, 0.0f, 0.0f);
    neko::Vec3f cubeSize_ = neko::Vec3f(1.0f, 1.0f, 1.0f);

    neko::Entity cubeEntity_;
    neko::Entity planeEntity_;

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

    neko::Configuration config;
    //config.dataRootPath = "../data/";
    config.windowName = "AerEditor";
    config.windowSize = neko::Vec2u(1400, 900);

    neko::sdl::Gles3Window window;
    neko::gl::Gles3Renderer renderer;
    neko::Filesystem filesystem;
    neko::aer::AerEngine engine(filesystem, &config, neko::aer::ModeEnum::EDITOR);

    engine.SetWindowAndRenderer(&window, &renderer);

    SceneRotation sceneRotation = SceneRotation(engine);
    TestPhysX testPhysX(engine, sceneRotation);

    engine.RegisterOnDrawUi(testPhysX);
    engine.Init();
    testPhysX.Init();
    engine.RegisterSystem(testPhysX);
    engine.EngineLoop();
}

/*
class SceneRaycast final : public SceneInterface {
public:
    void InitActors(
        neko::EntityManager& entityManager,
        neko::Transform3dManager& transform3dManager,
        neko::physics::PhysicsEngine& physicsEngine) override
    {
        engineDuration = 50;
        entityManager_ = &entityManager;
        transform3dManager_ = &transform3dManager;
        physicsEngine_ = &physicsEngine;
        //Plane
        {
            planeEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(planeEntity_);
            transform3dManager_->SetRelativePosition(planeEntity_, planePosition_);
            transform3dManager_->SetRelativeScale(planeEntity_, neko::Vec3f(4, 1, 4));
            neko::physics::RigidStaticData rigidStatic;
            rigidStatic.colliderType = neko::physics::ColliderType::BOX;
            rigidStatic.material = neko::physics::PhysicsMaterial{
                0.5f,
                0.5f,
                0.1f
            };
            rigidStatic.boxColliderData.isTrigger = false;
            physicsEngine_->AddRigidStatic(
                planeEntity_,
                rigidStatic);
        }
        viewedEntity = planeEntity_;
    }

    void HasSucceed() override
    {
        EXPECT_TRUE(rayOneTouch_);
        EXPECT_FALSE(rayTwoTouch_);
        EXPECT_FALSE(rayThreeTouch_);
        EXPECT_FALSE(rayFourTouch_);
    }

    void Update() override
    {
        neko::IGizmoRenderer& gizmosLocator = neko::GizmosLocator::get();
        {
            //Raycast
            neko::physics::PxRaycastInfo raycastInfo = physicsEngine_->Raycast(
                rayOrigin_,
                rayDirection_,
                50.0f);
            //std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
            //    " Distance : " << raycastInfo.GetDistance() <<
            //    " Position : " << raycastInfo.GetPoint() <<
            //    " Normal : " << raycastInfo.GetNormal() << std::endl;
            rayOneTouch_ = raycastInfo.touch;
            //Display Gizmo
            gizmosLocator.DrawLine(
                rayOrigin_,
                rayOrigin_ + rayDirection_* 50.0f,
                raycastInfo.touch ?
                neko::Color::green :
                neko::Color::red,2.0f);
        }
        {
            //Raycast
            neko::physics::PxRaycastInfo raycastInfo = physicsEngine_->Raycast(
                rayOrigin_ + neko::Vec3f::right * 2,
                rayDirection_,
                2.0f);
            //std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
            //    " Distance : " << raycastInfo.GetDistance() <<
            //    " Position : " << raycastInfo.GetPoint() <<
            //    " Normal : " << raycastInfo.GetNormal() << std::endl;
            rayTwoTouch_ = raycastInfo.touch;

            //Display Gizmo
            gizmosLocator.DrawLine(
                rayOrigin_ + neko::Vec3f::right * 2,
                rayOrigin_ + neko::Vec3f::right * 2 + rayDirection_ * 2.0f,
                raycastInfo.touch ?
                neko::Color::green :
                neko::Color::red,
                2.0f);
        }
        {
            //Raycast
            neko::physics::PxRaycastInfo raycastInfo = physicsEngine_->Raycast(
                rayOrigin_ + neko::Vec3f::left * 4,
                rayDirection_,
                50.0f);
            //std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
            //    " Distance : " << raycastInfo.GetDistance() <<
            //    " Position : " << raycastInfo.GetPoint() <<
            //    " Normal : " << raycastInfo.GetNormal() << std::endl;
            rayThreeTouch_ = raycastInfo.touch;

            //Display Gizmo
            gizmosLocator.DrawLine(
                rayOrigin_ + neko::Vec3f::left * 4,
                rayOrigin_ + neko::Vec3f::left * 4 + rayDirection_ * 50.0f,
                raycastInfo.touch ?
                neko::Color::green :
                neko::Color::red,
                2.0f);
        }
        {
            //Raycast
            neko::physics::PxRaycastInfo raycastInfo = physicsEngine_->Raycast(
                rayOrigin_ + neko::Vec3f::left * 2,
                rayOrigin_ + neko::Vec3f::left * 2 + neko::Vec3f::up,
                50.0f);
            //std::cout << "Raycast " << (raycastInfo.touch ? "hit" : "not hit") <<
            //    " Distance : " << raycastInfo.GetDistance() <<
            //    " Position : " << raycastInfo.GetPoint() <<
            //    " Normal : " << raycastInfo.GetNormal() << std::endl;
            rayFourTouch_ = raycastInfo.touch;

            //Display Gizmo
            gizmosLocator.DrawLine(
                rayOrigin_ + neko::Vec3f::left * 2,
                rayOrigin_ + neko::Vec3f::left * 2 + neko::Vec3f::up * 50.0f,
                raycastInfo.touch ?
                neko::Color::green :
                neko::Color::red,
                2.0f);
        }
        //std::cout << std::endl;
    }

    void DrawImGui() override { }
private:
    neko::Vec3f planePosition_ = neko::Vec3f(0.0f, -3.0f, -5.0f);
    neko::Entity planeEntity_;

    neko::Vec3f rayOrigin_ = neko::Vec3f(0.0f, 0.0f, -5.0f);
    neko::Vec3f rayDirection_ = neko::Vec3f::down;

    bool rayOneTouch_ = false;
    bool rayTwoTouch_ = false;
    bool rayThreeTouch_ = false;
    bool rayFourTouch_ = false;
};

class SceneCollision final : public SceneInterface {
public:
    void InitActors(
        neko::EntityManager& entityManager,
        neko::Transform3dManager& transform3dManager,
        neko::physics::PhysicsEngine& physicsEngine) override
    {
        engineDuration = 100;
        entityManager_ = &entityManager;
        transform3dManager_ = &transform3dManager;
        physicsEngine_ = &physicsEngine;
        //Plane
        {
            planeEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(planeEntity_);
            transform3dManager_->SetRelativePosition(planeEntity_, planePosition_);
            transform3dManager_->SetRelativeScale(planeEntity_, neko::Vec3f(5, 0.5f, 5));
            neko::physics::RigidStaticData rigidStatic;
            rigidStatic.colliderType = neko::physics::ColliderType::BOX;
            physicsEngine_->AddRigidStatic(
                planeEntity_,
                rigidStatic);
        }
        //Cube Touch
        {
            cubeTouchEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(cubeTouchEntity_);
            transform3dManager_->SetRelativePosition(
                cubeTouchEntity_,
                cubePosition_ + neko::Vec3f::left * 1.0f);
            neko::physics::RigidDynamicData rigidDynamic;
            rigidDynamic.colliderType = neko::physics::ColliderType::BOX;
            physicsEngine_->AddRigidDynamic(
                cubeTouchEntity_,
                rigidDynamic);
        }
        //Cube Trigger
        {
            cubeTriggerEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(cubeTriggerEntity_);
            transform3dManager_->SetRelativePosition(
                cubeTriggerEntity_,
                cubePosition_ + neko::Vec3f::right * 1.0f);
            neko::physics::RigidDynamicData rigidDynamic;
            rigidDynamic.colliderType = neko::physics::ColliderType::BOX;
            rigidDynamic.boxColliderData.isTrigger = true;
            physicsEngine_->AddRigidDynamic(
                cubeTriggerEntity_,
                rigidDynamic);
        }
        //Cube Not Touch
        {
            cubeNotTouchEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(cubeNotTouchEntity_);
            transform3dManager_->SetRelativePosition(
                cubeNotTouchEntity_,
                cubePosition_ + neko::Vec3f::right * 5.0f);
            neko::physics::RigidDynamicData rigidDynamic;
            rigidDynamic.colliderType = neko::physics::ColliderType::BOX;
            physicsEngine_->AddRigidDynamic(
                cubeNotTouchEntity_,
                rigidDynamic);
        }
        //Cube Not Trigger
        {
            cubeNotTriggerEntity_ = entityManager_->CreateEntity();
            transform3dManager_->AddComponent(cubeNotTriggerEntity_);
            transform3dManager_->SetRelativePosition(
                cubeNotTriggerEntity_,
                cubePosition_ + neko::Vec3f::left * 5.0f);
            neko::physics::RigidDynamicData rigidDynamic;
            rigidDynamic.colliderType = neko::physics::ColliderType::BOX;
            rigidDynamic.boxColliderData.isTrigger = true;
            physicsEngine_->AddRigidDynamic(
                cubeNotTriggerEntity_,
                rigidDynamic);
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

    void Update() override
    {
        //Display Gizmo
        neko::IGizmoRenderer& gizmosLocator = neko::GizmosLocator::get();
        for (neko::Entity entity = 0.0f;
            entity < entityManager_->GetEntitiesSize(); entity++) {
            if (!entityManager_->HasComponent(
                entity,
                neko::EntityMask(neko::ComponentType::RIGID_DYNAMIC))) {
                continue;
            }

            neko::physics::ColliderType colliderType = physicsEngine_->
                GetColliderType(entity);
            switch (colliderType) {
            case neko::physics::ColliderType::INVALID:
                break;
            case neko::physics::ColliderType::BOX: {
                neko::physics::BoxColliderData boxColliderData = physicsEngine_
                    ->
                    GetBoxColliderData(entity);
                gizmosLocator.DrawCube(
                    transform3dManager_->GetRelativePosition(entity) + boxColliderData.
                    offset,
                    boxColliderData.size,
                    transform3dManager_->GetRelativeRotation(entity),
                    boxColliderData.isTrigger ?
                    neko::Color::yellow :
                    neko::Color::green,
                    2.0f);
            }
            break;
            }
        }
    }


    void OnCollisionEnter(const physx::PxContactPair& pairHeader) override
    {
        if (pairHeader.shapes[0]->getActor() == physicsEngine_->GetRigidDynamic(cubeTouchEntity_).GetPxRigidDynamic() ||
            pairHeader.shapes[1]->getActor() == physicsEngine_->GetRigidDynamic(cubeTouchEntity_).GetPxRigidDynamic()) {
            cubeTouch_ = true;
        }
        if (pairHeader.shapes[0]->getActor() == physicsEngine_->GetRigidDynamic(cubeNotTouchEntity_).GetPxRigidDynamic() ||
            pairHeader.shapes[1]->getActor() == physicsEngine_->GetRigidDynamic(cubeNotTouchEntity_).GetPxRigidDynamic()) {
            cubeNotTouchEntity_ = true;
        }
    }

    void OnTriggerEnter(physx::PxTriggerPair* pairs) override
    {
        if (pairs->triggerActor == physicsEngine_->GetRigidDynamic(cubeTriggerEntity_).GetPxRigidDynamic()) {
            cubeTrigger_ = true;
        }
        if (pairs->triggerActor == physicsEngine_->GetRigidDynamic(cubeNotTriggerEntity_).GetPxRigidDynamic()) {
            cubeNotTrigger_ = true;
        }
    }

    void OnTriggerExit(physx::PxTriggerPair* pairs) override
    {
        if (pairs->triggerActor == physicsEngine_->GetRigidDynamic(cubeTriggerEntity_).GetPxRigidDynamic()) {
            cubeTriggerExit_ = true;
        }
    }

    void FixedUpdate(neko::seconds dt) override { }

    void DrawImGui() override { }
private:
    const static size_t kCubeNumbers = 25;
    neko::Vec3f cubePosition_ = neko::Vec3f(0.0f, 5.0f, -5.0f);
    neko::Vec3f planePosition_ = neko::Vec3f(0.0f, 0.0f, -5.0f);

    neko::Entity cubeTouchEntity_ = neko::INVALID_ENTITY;
    neko::Entity cubeNotTouchEntity_ = neko::INVALID_ENTITY;
    neko::Entity cubeTriggerEntity_ = neko::INVALID_ENTITY;
    neko::Entity cubeNotTriggerEntity_ = neko::INVALID_ENTITY;

    neko::Entity planeEntity_ = neko::INVALID_ENTITY;

    bool cubeTouch_ = false;
    bool cubeNotTouch_ = false;
    bool cubeTrigger_ = false;
    bool cubeNotTrigger_ = false;
    bool cubeTriggerExit_ = false;

};
*/
/*
TEST(PhysX, TestRaycast)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr) {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    neko::Configuration config;
    //config.dataRootPath = "../data/";
    config.windowName = "AerEditor";
    config.windowSize = neko::Vec2u(1400, 900);

    neko::sdl::Gles3Window window;
    neko::gl::Gles3Renderer renderer;
    neko::Filesystem filesystem;
    neko::aer::AerEngine engine(filesystem, &config, neko::aer::ModeEnum::TEST);

    engine.SetWindowAndRenderer(&window, &renderer);

    neko::EntityManager entityManager;
    neko::Transform3dManager transform3dManager = neko::Transform3dManager(
        entityManager);
    SceneRaycast sceneRaycast;
    TestPhysX testPhysX(engine, entityManager, transform3dManager, sceneRaycast);

    engine.RegisterSystem(testPhysX);
    engine.RegisterOnDrawUi(testPhysX);
    engine.Init();
    neko::Job initJob{ [&testPhysX]() { testPhysX.InitRenderer(); } };
    neko::BasicEngine::GetInstance()->ScheduleJob(
        &initJob,
        neko::JobThreadType::RENDER_THREAD);
    engine.EngineLoop();
}

TEST(PhysX, TestTriggerCollision)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr) {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    neko::Configuration config;
    //config.dataRootPath = "../data/";
    config.windowName = "AerEditor";
    config.windowSize = neko::Vec2u(1400, 900);

    neko::sdl::Gles3Window window;
    neko::gl::Gles3Renderer renderer;
    neko::Filesystem filesystem;
    neko::aer::AerEngine engine(filesystem, &config, neko::aer::ModeEnum::TEST);

    engine.SetWindowAndRenderer(&window, &renderer);

    neko::EntityManager entityManager;
    neko::Transform3dManager transform3dManager = neko::Transform3dManager(
        entityManager);
    SceneCollision sceneCollision;
    TestPhysX testPhysX(engine, entityManager, transform3dManager, sceneCollision);

    engine.RegisterSystem(testPhysX);
    engine.RegisterOnDrawUi(testPhysX);
    engine.Init();
    neko::Job initJob{ [&testPhysX]() { testPhysX.InitRenderer(); } };
    neko::BasicEngine::GetInstance()->ScheduleJob(
        &initJob,
        neko::JobThreadType::RENDER_THREAD);
    engine.EngineLoop();
}
*/