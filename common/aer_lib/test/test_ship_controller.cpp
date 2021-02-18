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
 Date : 22.01.2020
---------------------------------------------------------- */
#include <gtest/gtest.h>
#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

#include "aer/gizmos_renderer.h"
#ifdef NEKO_GLES3
#include "aer/aer_engine.h"
#include "engine/engine.h"

namespace neko::aer {
class TestShipController
    : public SystemInterface,
      public RenderCommandInterface,
      public DrawImGuiInterface {
public:
    TestShipController(
        AerEngine& engine)
        : engine_(engine),
          rContainer_(engine.GetResourceManagerContainer()),
          cContainer_(engine.GetComponentManagerContainer()) { }

    void Init() override
    {
#ifdef EASY_PROFILE_USE
            EASY_BLOCK("Test Init", profiler::colors::Green);
#endif
        Camera3D* camera = GizmosLocator::get().GetCamera();
        camera->position = Vec3f(0.0f, 25.0f, -25.0f);
        camera->Rotate(EulerAngles(degree_t(25), degree_t(180), degree_t(0)));
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        {
            shipEntity_ = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(shipEntity_);
            cContainer_.transform3dManager.SetRelativePosition(
                shipEntity_,
                Vec3f(0.0f, 2.0f, 0.0f));
            cContainer_.renderManager.AddComponent(shipEntity_);
            cContainer_.renderManager.SetModel(
                shipEntity_,
                config.dataRootPath + "models/cube/cube.obj");
            physics::RigidDynamicData rigidDynamic;
            rigidDynamic.useGravity = false;
            rigidDynamic.colliderType = physics::ColliderType::BOX;
            cContainer_.rigidDynamicManager.AddRigidDynamic(
                shipEntity_,
                rigidDynamic);
            cContainer_.shipControllerManager.AddComponent(shipEntity_);
        }
        {
            groundEntity_ = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(groundEntity_);
            cContainer_.transform3dManager.SetRelativeScale(
                groundEntity_,
                Vec3f(10000.0f, 0.5f, 10000.0f));
            cContainer_.sceneManager.AddLayer("Ground");
            TagLocator::get().SetEntityLayer(groundEntity_, "Ground");
            cContainer_.renderManager.AddComponent(groundEntity_);
            cContainer_.renderManager.SetModel(
                groundEntity_,
                config.dataRootPath + "models/cube/cube.obj");
            physics::RigidStaticData rigidStaticData;
            rigidStaticData.colliderType = physics::ColliderType::BOX;
            rigidStaticData.filterGroup = physics::FilterGroup::GROUND;
            cContainer_.rigidStaticManager.AddRigidStatic(
                groundEntity_,
                rigidStaticData);
        }
        {
            Entity grund2Entity = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(grund2Entity);
            cContainer_.transform3dManager.SetRelativePosition(
                grund2Entity,
                Vec3f(0.0f, 0.0f, 15.0f));
            cContainer_.transform3dManager.SetRelativeScale(
                grund2Entity,
                Vec3f(50.0f, 0.5f, 50.0f));
            cContainer_.transform3dManager.SetRelativeRotation(
                grund2Entity,
                ConvertVec3fToEulerAngles(Vec3f(-15.0f, 0.0f, 0.0f)));
            cContainer_.sceneManager.AddLayer("Ground");
            TagLocator::get().SetEntityLayer(grund2Entity, "Ground");
            cContainer_.renderManager.AddComponent(grund2Entity);
            cContainer_.renderManager.SetModel(
                grund2Entity,
                config.dataRootPath + "models/cube/cube.obj");
            physics::RigidStaticData rigidStaticData;
            rigidStaticData.colliderType = physics::ColliderType::BOX;
            rigidStaticData.filterGroup = physics::FilterGroup::GROUND;
            cContainer_.rigidStaticManager.AddRigidStatic(
                grund2Entity,
                rigidStaticData);
        }
    }

    void Update(seconds dt) override
    {
#ifdef EASY_PROFILE_USE
            EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
        updateCount_ += dt.count();
        if (updateCount_ > kEngineDuration_) { engine_.Stop(); }
    }

    void Render() override { }

    void Destroy() override { }

    void DrawImGui() override {}

private:
    float updateCount_ = 0;
    const float kEngineDuration_ = 0.5f;

    AerEngine& engine_;

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;


    Entity shipEntity_;
    Entity groundEntity_;
};

TEST(Game, TestShipController)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr) {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    Configuration config;
    // config.dataRootPath = "../data/";
    config.windowName = "AerEditor";
    config.windowSize = Vec2u(1400, 900);

    sdl::Gles3Window window;
    gl::Gles3Renderer renderer;
    Filesystem filesystem;
    AerEngine engine(filesystem, &config, ModeEnum::EDITOR);

    engine.SetWindowAndRenderer(&window, &renderer);

    TestShipController testShipController(engine);

    engine.RegisterSystem(testShipController);
    engine.RegisterOnDrawUi(testShipController);
    engine.Init();
    engine.EngineLoop();
#ifdef EASY_PROFILE_USE
        profiler::dumpBlocksToFile("Renderer_Neko_Profile.prof");
#endif
}

class TestShipCamera
    : public SystemInterface,
    public RenderCommandInterface,
    public DrawImGuiInterface {
public:
    TestShipCamera(
        AerEngine& engine)
        : engine_(engine),
        rContainer_(engine.GetResourceManagerContainer()),
        cContainer_(engine.GetComponentManagerContainer()) { }

    void Init() override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Init", profiler::colors::Green);
#endif
        camera_ = GizmosLocator::get().GetCamera();
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        Entity groundEntity = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(groundEntity);
        cContainer_.renderManager.AddComponent(groundEntity);
        cContainer_.renderManager.SetModel(groundEntity, config.dataRootPath + "models/cube/cube.obj");
        cContainer_.transform3dManager.SetRelativeScale(groundEntity, Vec3f(10000.0f, 0.5f, 10000.0f));
        physics::RigidStaticData rigidStaticData;
        rigidStaticData.colliderType = physics::ColliderType::BOX;
        rigidStaticData.filterGroup = physics::FilterGroup::GROUND;
        cContainer_.rigidStaticManager.AddRigidStatic(
            groundEntity,
            rigidStaticData);

        Entity shipEntity = engine_.GetComponentManagerContainer().entityManager.CreateEntity();
        cContainer_.entityManager.SetEntityName(shipEntity, "ship");
        cContainer_.transform3dManager.AddComponent(shipEntity);
        cContainer_.transform3dManager.SetGlobalPosition(shipEntity, Vec3f(0, 2, 0));
        cContainer_.rigidDynamicManager.AddComponent(shipEntity);
        physics::RigidDynamicData rigidDynamic;
        rigidDynamic.useGravity = false;
        rigidDynamic.colliderType = physics::ColliderType::BOX;
        cContainer_.rigidDynamicManager.AddRigidDynamic(shipEntity, rigidDynamic);
        cContainer_.shipControllerManager.AddComponent(shipEntity);
        cContainer_.renderManager.AddComponent(shipEntity);
        cContainer_.renderManager.SetModel(shipEntity, config.dataRootPath + "models/cube/cube.obj");

        cameraEntity_ = engine_.GetComponentManagerContainer().entityManager.CreateEntity();
        cContainer_.entityManager.SetEntityName(cameraEntity_, "cameraEntity");
        cContainer_.transform3dManager.AddComponent(cameraEntity_);
        cContainer_.cameraControllerManager.AddComponent(cameraEntity_);
        {
            Entity grund2Entity = cContainer_.entityManager.CreateEntity();
            cContainer_.transform3dManager.AddComponent(grund2Entity);
            cContainer_.transform3dManager.SetRelativePosition(
                grund2Entity,
                Vec3f(0.0f, 0.0f, 15.0f));
            cContainer_.transform3dManager.SetRelativeScale(
                grund2Entity,
                Vec3f(50.0f, 0.5f, 50.0f));
            cContainer_.transform3dManager.SetRelativeRotation(
                grund2Entity,
                ConvertVec3fToEulerAngles(Vec3f(-15.0f, 0.0f, 0.0f)));
            cContainer_.sceneManager.AddLayer("Ground");
            TagLocator::get().SetEntityLayer(grund2Entity, "Ground");
            cContainer_.renderManager.AddComponent(grund2Entity);
            cContainer_.renderManager.SetModel(
                grund2Entity,
                config.dataRootPath + "models/cube/cube.obj");
            physics::RigidStaticData rigidStaticData;
            rigidStaticData.colliderType = physics::ColliderType::BOX;
            rigidStaticData.filterGroup = physics::FilterGroup::GROUND;
            cContainer_.rigidStaticManager.AddRigidStatic(
                grund2Entity,
                rigidStaticData);
        }
    }

    void Update(seconds dt) override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
        if (cameraFollow_ && cameraEntity_ != INVALID_ENTITY && camera_)
        {
            camera_->position = cContainer_.transform3dManager.GetGlobalPosition(cameraEntity_);
            camera_->reverseDirection = Quaternion::FromEuler(cContainer_.transform3dManager.GetGlobalRotation(cameraEntity_)) * Vec3f::back;
         }
        updateCount_ += dt.count();
        if (updateCount_ > kEngineDuration_) { engine_.Stop(); }
    }

    void Render() override { }

    void Destroy() override { }

    void DrawImGui() override {
        ImGui::Begin("Camera");
        if (camera_)
        {
            std::string position = camera_->position.ToString();
            ImGui::Text(position.c_str());
            std::string rotation = ConvertEulerAnglesToVec3f(camera_->GetRotation()).ToString();
            ImGui::Text(rotation.c_str());
            ImGui::Checkbox("CameraFollow", &cameraFollow_);
        }
        ImGui::End();
    }

private:
    float updateCount_ = 0;
    const float kEngineDuration_ = 0.5f;

    AerEngine& engine_;

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;

    Camera3D* camera_ = nullptr;
    bool cameraFollow_ = true;


    Entity shipEntity_ = INVALID_ENTITY;
    Entity cameraEntity_ = INVALID_ENTITY;
};

TEST(Game, TestShipCamera)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr) {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    Configuration config;
    // config.dataRootPath = "../data/";
    config.windowName = "AerEditor";
    config.windowSize = Vec2u(1400, 900);

    sdl::Gles3Window window;
    gl::Gles3Renderer renderer;
    Filesystem filesystem;
    AerEngine engine(filesystem, &config, ModeEnum::EDITOR);

    engine.SetWindowAndRenderer(&window, &renderer);

    TestShipCamera testShipCamera(engine);

    engine.RegisterSystem(testShipCamera);
    engine.RegisterOnDrawUi(testShipCamera);
    engine.Init();
    engine.EngineLoop();
#ifdef EASY_PROFILE_USE
    profiler::dumpBlocksToFile("Renderer_Neko_Profile.prof");
#endif
}
}
#endif
