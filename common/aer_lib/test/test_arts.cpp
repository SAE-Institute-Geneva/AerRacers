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
    #include "engine/system.h"
    #include "engine/transform.h"
    #include "gl/gles3_window.h"
    #include "gl/graphics.h"
    #include "gl/shader.h"
    #include "gl/shape.h"

namespace neko::aer
{

class TestLevelDesign : public SystemInterface,
    public RenderCommandInterface,
    public DrawImGuiInterface
{
public:
    explicit TestLevelDesign(AerEngine& engine)
        : engine_(engine),
        rContainer_(engine.GetResourceManagerContainer()),
        cContainer_(engine.GetComponentManagerContainer())
    {
    }

    void Init() override
    {
        gizmosRenderer_ = &GizmosLocator::get();
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Init", profiler::colors::Green);
#endif
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        testEntity_ = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            //testEntity_, config.dataRootPath + "models/leveldesign/big_terrain_01.obj");
            testEntity_, config.dataRootPath + "models/leveldesign/aer_racer_circuit_v37.obj");
        engine_.GetCameras().moveSpeed = 50.0f;
    }

    void Update(seconds dt) override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
        const auto modelId = cContainer_.renderManager.GetComponent(testEntity_).modelId;
        updateCount_ += dt.count();
        if (updateCount_ > kEngineDuration_ || rContainer_.modelManager.IsLoaded(modelId))
        {
            loaded_ = rContainer_.modelManager.IsLoaded(modelId);
            engine_.Stop();
        }
        if (!rContainer_.modelManager.IsLoaded(modelId)) return;

        //const auto& model = rContainer_.modelManager.GetModel(modelId);
        //for (size_t i = 0; i < model->GetMeshCount(); ++i)
        //{
        //    const auto& meshAabb = model->GetMesh(i).aabb;
        //    gizmosRenderer_->DrawCube(meshAabb.CalculateCenter(), meshAabb.CalculateExtends());
        //}
    }

    void Render() override {}

    void Destroy() override
    {
        EXPECT_TRUE(loaded_);
    }

    void DrawImGui() override
    {
        ImGui::Begin("Test parameter");
        {
            float speed = engine_.GetCameras().moveSpeed;
            if (ImGui::DragFloat("CameraSpeed", &speed)) {
                engine_.GetCameras().moveSpeed = speed;
            }
        }
        ImGui::End();
    }

private:
    float updateCount_ = 0;
    const float kEngineDuration_ = 30.0f;
    bool loaded_ = false;
    AerEngine& engine_;

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;

    IGizmoRenderer* gizmosRenderer_;

    Entity testEntity_;
};

TEST(Arts, LevelDesign)
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
    AerEngine engine(filesystem, &config, ModeEnum::EDITOR);

    engine.SetWindowAndRenderer(&window, &renderer);

    TestLevelDesign testRenderer(engine);

    engine.RegisterSystem(testRenderer);
    engine.RegisterOnDrawUi(testRenderer);
    engine.Init();
    engine.EngineLoop();
    logDebug("Test without check");

}
#pragma region Ship
class TestShip : public SystemInterface,
    public RenderCommandInterface,
    public DrawImGuiInterface
{
public:
    explicit TestShip(AerEngine& engine)
        : engine_(engine),
        rContainer_(engine.GetResourceManagerContainer()),
        cContainer_(engine.GetComponentManagerContainer())
    {
    }

    void Init() override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Init", profiler::colors::Green);
#endif
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        testEntity_ = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            testEntity_, config.dataRootPath + "models/leveldesign/aer_racer_circuit_v37.obj");
        engine_.GetCameras().moveSpeed = 50.0f;
    }

    void Update(seconds dt) override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
        const auto modelId = cContainer_.renderManager.GetComponent(testEntity_).modelId;
        updateCount_ += dt.count();
        if (updateCount_ > kEngineDuration_ || rContainer_.modelManager.IsLoaded(modelId))
        {
            loaded_ = rContainer_.modelManager.IsLoaded(modelId);
            engine_.Stop();
        }
        if (!rContainer_.modelManager.IsLoaded(modelId)) return;
    }

    void Render() override {}

    void Destroy() override
    {
        EXPECT_TRUE(loaded_);
    }

    void DrawImGui() override
    {
        ImGui::Begin("Test parameter");
        {
            float speed = engine_.GetCameras().moveSpeed;
            if (ImGui::DragFloat("CameraSpeed", &speed)) {
                engine_.GetCameras().moveSpeed = speed;
            }
        }
        ImGui::End();
    }

private:
    float updateCount_ = 0;
    const float kEngineDuration_ = 30.0f;
    bool loaded_ = false;
    AerEngine& engine_;

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;

    Entity testEntity_;
};

TEST(Arts, Ship)
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
    AerEngine engine(filesystem, &config, ModeEnum::EDITOR);

    engine.SetWindowAndRenderer(&window, &renderer);

    TestShip testRenderer(engine);

    engine.RegisterSystem(testRenderer);
    engine.RegisterOnDrawUi(testRenderer);
    engine.Init();
    engine.EngineLoop();
    logDebug("Test without check");

}
#pragma endregion
#pragma region Block
class TestBlock : public SystemInterface,
    public RenderCommandInterface,
    public DrawImGuiInterface
{
public:
    explicit TestBlock(AerEngine& engine)
        : engine_(engine),
        rContainer_(engine.GetResourceManagerContainer()),
        cContainer_(engine.GetComponentManagerContainer())
    {
    }

    void Init() override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Init", profiler::colors::Green);
#endif
        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        testEntity_ = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.transform3dManager.SetRelativeScale(testEntity_, Vec3f::one * 0.1f);
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            testEntity_, config.dataRootPath + "models/gros_block1/gros_block1.obj");
        engine_.GetCameras().moveSpeed = 50.0f;
        engine_.GetCameras().SetPosition(cameraPosition_, 0);
    }

    void Update(seconds dt) override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
        const auto modelId = cContainer_.renderManager.GetComponent(testEntity_).modelId;
        updateCount_ += dt.count();
        if (updateCount_ > kEngineDuration_ || rContainer_.modelManager.IsLoaded(modelId))
        {
            loaded_ = rContainer_.modelManager.IsLoaded(modelId);
            engine_.Stop();
        }
        if (!rContainer_.modelManager.IsLoaded(modelId)) return;
    }

    void Render() override {}

    void Destroy() override
    {
        EXPECT_TRUE(loaded_);
    }

    void DrawImGui() override
    {
        ImGui::Begin("Test parameter");
        {
            float speed = engine_.GetCameras().moveSpeed;
            if (ImGui::DragFloat("CameraSpeed", &speed)) {
                engine_.GetCameras().moveSpeed = speed;
            }
        }
        ImGui::End();
    }

private:
    float updateCount_ = 0;
    const float kEngineDuration_ = 20.0f;
    bool loaded_ = false;
    AerEngine& engine_;

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;

    Entity testEntity_;
    Vec3f cameraPosition_ = Vec3f(162, 498, 753);
};

TEST(Arts, Block)
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
    AerEngine engine(filesystem, &config, ModeEnum::EDITOR);

    engine.SetWindowAndRenderer(&window, &renderer);

    TestBlock testRenderer(engine);

    engine.RegisterSystem(testRenderer);
    engine.RegisterOnDrawUi(testRenderer);
    engine.Init();
    engine.EngineLoop();
    logDebug("Test without check");

}
#pragma endregion 
class LevelDesignViewer : public SystemInterface,
    public DrawImGuiInterface
{
public:
    explicit LevelDesignViewer(AerEngine& engine)
        : engine_(engine)
    {}

    void Init() override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Init", profiler::colors::Green);
#endif
        const Configuration config = BasicEngine::GetInstance()->GetConfig();
        engine_.GetComponentManagerContainer().sceneManager.LoadScene(
            config.dataRootPath + "scenes/LevelDesign24-03.aerscene");
        Camera3D* camera = GizmosLocator::get().GetCamera();
        camera->position = Vec3f(10.0f, 5.0f, 0.0f);
        camera->Rotate(EulerAngles(degree_t(0.0f), degree_t(-90.0f), degree_t(0.0f)));
    }

    void Update(seconds dt) override
    {
#ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
        updateCount_ += dt.count();
        //if (updateCount_ > kEngineDuration_) { engine_.Stop(); }
    }

    void Destroy() override {}

    void HasSucceed() {}

    void DrawImGui() override
    {
        ImGui::Begin("Test parameter");
        {
            float speed = engine_.GetCameras().moveSpeed;
            if (ImGui::DragFloat("CameraSpeed", &speed)) {
                engine_.GetCameras().moveSpeed = speed;
            }
        }
        ImGui::End();
    }

private:
    float updateCount_ = 0;
    const float kEngineDuration_ = 2.0f;

    AerEngine& engine_;
};
TEST(Arts, TestLevelDesignSceneViewer)
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
    AerEngine engine(filesystem, &config, ModeEnum::EDITOR);

    engine.SetWindowAndRenderer(&window, &renderer);
    LevelDesignViewer testSceneImporteur(engine);
    engine.RegisterSystem(testSceneImporteur);
    engine.RegisterOnDrawUi(testSceneImporteur);

    engine.Init();

    engine.EngineLoop();
#ifdef EASY_PROFILE_USE
    profiler::dumpBlocksToFile("Scene_Neko_Profile.prof");
#endif

    //testSceneImporteur.HasSucceed();
    logDebug("Test without check");
}
#endif
}    // namespace neko::aer