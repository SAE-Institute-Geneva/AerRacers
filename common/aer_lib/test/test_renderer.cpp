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
class TestRenderer : public SystemInterface, public RenderCommandInterface, public DrawImGuiInterface
{
public:
    TestRenderer(
        AerEngine& engine)
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
        testEntity_        = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.transform3dManager.SetRelativePosition(testEntity_, Vec3f(-3.0f, -3.0f, -3.0f));
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            testEntity_, config.dataRootPath + "models/cube/cube.fbx");
        testEntity_ = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.transform3dManager.SetRelativePosition(testEntity_, Vec3f(3.0f, -3.0f, -5.0f));
        cContainer_.transform3dManager.SetRelativeScale(testEntity_, Vec3f(1.0f, 3.0f, 1.0f));
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            testEntity_, config.dataRootPath + "models/cube/cube.obj");
        testEntity_ = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.transform3dManager.SetRelativePosition(testEntity_, Vec3f(1.0f, 3.0f, 1.0f));
        cContainer_.transform3dManager.SetRelativeRotation(
            testEntity_, EulerAngles(degree_t(180.0f), degree_t(45.0f), degree_t(265.0f)));
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            testEntity_, config.dataRootPath + "models/cube/cube.obj");
        testEntity_ = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.transform3dManager.SetRelativePosition(testEntity_, Vec3f(0.0f, 0.0f, -10.0f));
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            testEntity_, config.dataRootPath + "models/sphere/sphere.obj");
        testEntity_ = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.transform3dManager.SetRelativePosition(testEntity_, Vec3f(3.0f, 1.0f, 3.0f));
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            testEntity_, config.dataRootPath + "models/sphere/sphere.obj");
        testEntity_ = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.transform3dManager.SetRelativePosition(testEntity_, Vec3f(0.0f, 0.0f, 0.0f));
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            testEntity_, config.dataRootPath + "models/cube/cube.obj");
        cContainer_.entityManager.SetEntityParent(3, testEntity_);
    }

    void Update(seconds dt) override
    {
    #ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
    #endif
        cContainer_.transform3dManager.SetRelativePosition(testEntity_,
            Vec3f(Cos(radian_t(updateCount_)),
                Sin(radian_t(updateCount_)),
                Cos(radian_t(updateCount_))) * 2.0f);
        cContainer_.transform3dManager.SetRelativeRotation(testEntity_,
            EulerAngles(radian_t(updateCount_), radian_t(updateCount_), radian_t(updateCount_)));
        cContainer_.transform3dManager.SetRelativeScale(testEntity_,
            Vec3f(abs(Cos(radian_t(updateCount_))),
                abs(Sin(radian_t(updateCount_))),
                abs(Cos(radian_t(updateCount_)))));
        updateCount_ += dt.count();
        if (updateCount_ > kEngineDuration_) { engine_.Stop(); }
    }

    void Render() override
    {
        
    }

    void Destroy() override
    {

    }

    void DrawImGui() override {}

private:
    float updateCount_           = 0;
    const float kEngineDuration_ = 0.5f;

    AerEngine& engine_;

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;


    Entity testEntity_;
};

TEST(Renderer, Cube_Sphere)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr)
    {
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

    TestRenderer testRenderer(engine);

    engine.RegisterSystem(testRenderer);
    engine.RegisterOnDrawUi(testRenderer);
    engine.Init();
    engine.EngineLoop();
    #ifdef EASY_PROFILE_USE
    profiler::dumpBlocksToFile("Renderer_Neko_Profile.prof");
    #endif
}
class TestNanosuitRenderer : public SystemInterface,
                     public RenderCommandInterface,
                     public DrawImGuiInterface
{
public:
    explicit TestNanosuitRenderer(AerEngine& engine)
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
            testEntity_, config.dataRootPath + "models/nanosuit2/nanosuit.obj");
    }

    void Update(seconds dt) override
    {
    #ifdef EASY_PROFILE_USE
        EASY_BLOCK("Test Update", profiler::colors::Green);
    #endif
        const auto modelId = cContainer_.renderManager.GetComponent(testEntity_).modelId;
        if (!rContainer_.modelManager.IsLoaded(modelId)) return;

        const auto& model = rContainer_.modelManager.GetModel(modelId);
        for (size_t i = 0; i < model->GetMeshCount(); ++i)
        {
        	const auto& meshAabb = model->GetMesh(i).aabb;
        	gizmosRenderer_->DrawCube(meshAabb.CalculateCenter(), meshAabb.CalculateExtends());
        }
        updateCount_ += dt.count();
        if (updateCount_ > kEngineDuration_) { engine_.Stop(); }
    }

    void Render() override {}

    void Destroy() override {}

    void DrawImGui() override {}

private:
    float updateCount_           = 0;
    const float kEngineDuration_ = 0.5f;

    AerEngine& engine_;
    sdl::MovableCamera3D camera_;

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;

    IGizmoRenderer* gizmosRenderer_;

    Entity testEntity_;
};

TEST(Renderer, NanosuitMesh)
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

    TestNanosuitRenderer testRenderer(engine);

    engine.RegisterSystem(testRenderer);
    engine.RegisterOnDrawUi(testRenderer);
    engine.Init();
    engine.EngineLoop();
    logDebug("Test without check");

}
}    // namespace neko::aer
#endif