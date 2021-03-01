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

#ifdef NEKO_GLES3
#include "aer/aer_engine.h"
#include "engine/engine.h"
#include "aer/gizmos_renderer.h"
#include "gl/gles3_window.h"
#include "gl/graphics.h"

namespace neko::aer {
class TestUnityPlayGround
    : public SystemInterface,
      public RenderCommandInterface,
      public DrawImGuiInterface {
public:
    TestUnityPlayGround(
        AerEngine& engine)
        : engine_(engine),
          rContainer_(engine.GetResourceManagerContainer()),
          cContainer_(engine.GetComponentManagerContainer()) { }

    void Init() override
    {
#ifdef EASY_PROFILE_USE
            EASY_BLOCK("Test Init", profiler::colors::Green);
#endif
        const Configuration config = BasicEngine::GetInstance()->GetConfig();
        engine_.GetComponentManagerContainer().sceneManager.LoadScene(
            config.dataRootPath +
            "scenes/SceneForNeko02-18withship.aerscene");
        Camera3D* camera = GizmosLocator::get().GetCamera();
        camera->farPlane = 100'000.0f;
        camera->position = Vec3f(0.0f, 5.0f, -15.0f);
        shipEntity_ = 15;
        for (Entity entity = 0;
             entity < engine_
                      .GetComponentManagerContainer().entityManager.
                      GetEntitiesSize(); ++entity) {
            if (TagLocator::get().IsEntityTag(entity, "Camera")) {
                cameraEntity_ = entity;
                break;
            }
        }
        engine_.GetComponentManagerContainer().shipControllerManager.
            AddComponent(15);
        engine_.GetComponentManagerContainer().entityManager.SetEntityParent(
            cameraEntity_,
            shipEntity_);
        engine_.GetComponentManagerContainer().transform3dManager.
                SetRelativePosition(cameraEntity_, Vec3f(0.0f, 5.0f, -15.0f));
        engine_.GetComponentManagerContainer().transform3dManager.
                SetRelativeRotation(
                    cameraEntity_,
                    EulerAngles(
                        degree_t(0.0f),
                        degree_t(-90.0f),
                        degree_t(0.0f)));
    }

    void Update(seconds dt) override
    {
#ifdef EASY_PROFILE_USE
            EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
        Camera3D* camera = GizmosLocator::get().GetCamera();
        if (cameraFollow) {
            camera->WorldLookAt(
                engine_.GetComponentManagerContainer().transform3dManager.
                GetGlobalPosition(shipEntity_));
            camera->position = engine_
                               .GetComponentManagerContainer().
                               transform3dManager.GetGlobalPosition(
                                   cameraEntity_);
        }
        updateCount_ += dt.count();
        if (updateCount_ > kEngineDuration_) { engine_.Stop(); }
    }

    void Render() override { }

    void Destroy() override { }

    void DrawImGui() override
    {
        Camera3D* camera = GizmosLocator::get().GetCamera();
        ImGui::Begin("Camera");
        {
            std::string position = camera->position.ToString();
            ImGui::Text(position.c_str());
            std::string rotation = ConvertEulerAnglesToVec3f(camera->GetRotation()).ToString();
            ImGui::Text(rotation.c_str());
            std::string right = camera->GetRight().ToString();
            ImGui::Text(right.c_str());
            ImGui::Checkbox("CameraFollow", &cameraFollow);
        }
        ImGui::End();
    }

private:
    bool cameraFollow = true;
    float updateCount_ = 0;
    const float kEngineDuration_ = 0.5f;

    AerEngine& engine_;

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;


    Entity shipEntity_;
    Entity cameraEntity_;
};

TEST(Game, TestUnityPlayGround)
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

    TestUnityPlayGround testUnityPlayGround(engine);

    engine.RegisterSystem(testUnityPlayGround);
    engine.RegisterOnDrawUi(testUnityPlayGround);
    engine.Init();
    engine.EngineLoop();
#ifdef EASY_PROFILE_USE
        profiler::dumpBlocksToFile("Renderer_Neko_Profile.prof");
#endif
}

class TestPlaygroundCamera
    : public SystemInterface,
    public RenderCommandInterface,
    public DrawImGuiInterface {
public:
    TestPlaygroundCamera(
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
        camera_->farPlane = 100'000.0f;
        camera_->fovY = degree_t(80);
        const Configuration config = BasicEngine::GetInstance()->GetConfig();
        engine_.GetComponentManagerContainer().sceneManager.LoadScene(
            config.dataRootPath +
            "scenes/PlayGroundLuca2021-02-23.aerscene");
        shipEntity_ = cContainer_.entityManager.FilterEntities(EntityMask(ComponentType::SHIP_CONTROLLER))[0];
        cContainer_.entityManager.SetEntityName(shipEntity_, "ship");

        for (Entity entity = 0;
            entity < engine_
            .GetComponentManagerContainer().entityManager.
            GetEntitiesSize(); ++entity) {
            if (TagLocator::get().IsEntityTag(entity, "Camera")) {
                cameraEntity_ = entity;
                break;
            }
        }
        cContainer_.entityManager.SetEntityName(cameraEntity_, "cameraEntity");
        cContainer_.cameraControllerManager.AddComponent(cameraEntity_);
    }

    void Update(seconds dt) override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
        if (cameraFollow_ && cameraEntity_ != INVALID_ENTITY && camera_)
        {
            camera_->position = cContainer_.transform3dManager.GetGlobalPosition(cameraEntity_);
            //camera_->reverseDirection = Quaternion::FromEuler(cContainer_.transform3dManager.GetGlobalRotation(cameraEntity_)) * Vec3f::back;
        }
        //updateCount_ += dt.count();
        if (updateCount_ > kEngineDuration_) { engine_.Stop(); }
    }

    void Render() override { }

    void Destroy() override { }

    void DrawImGui() override {
        Camera3D* camera = GizmosLocator::get().GetCamera();
        ImGui::Begin("Camera");
        {
            std::string position = camera->position.ToString();
            ImGui::Text(position.c_str());
            std::string rotation = ConvertEulerAnglesToVec3f(camera->GetRotation()).ToString();
            ImGui::Text(rotation.c_str());
            std::string right = camera->GetRight().ToString();
            ImGui::Text(right.c_str());
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

TEST(Game, TestPlaygroundCamera)
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

    TestPlaygroundCamera testPlaygroundCamera(engine);

    engine.RegisterSystem(testPlaygroundCamera);
    engine.RegisterOnDrawUi(testPlaygroundCamera);
    engine.Init();
    engine.EngineLoop();
#ifdef EASY_PROFILE_USE
    profiler::dumpBlocksToFile("Renderer_Neko_Profile.prof");
#endif
}
}
#endif
