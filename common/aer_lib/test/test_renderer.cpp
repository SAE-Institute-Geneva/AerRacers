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
        engine.RegisterSystem(camera_);
        engine.RegisterOnEvent(camera_);

        #ifdef NEKO_GLES3
        gizmosRenderer_ = std::make_unique<GizmoRenderer>(&camera_);
        #endif

        engine.RegisterSystem(*gizmosRenderer_);
    }

    void Init() override
    {
        camera_.position         = Vec3f::forward * 2.0f;
        camera_.reverseDirection = Vec3f::forward;
        camera_.fovY             = degree_t(45.0f);
        camera_.nearPlane        = 0.1f;
        camera_.farPlane         = 100.0f;

        gizmosRenderer_->SetCamera(&camera_);

        const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
        testEntity_        = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.transform3dManager.SetPosition(testEntity_, Vec3f(-3.0f, -3.0f, -3.0f));
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            testEntity_, config.dataRootPath + "models/cube/cube.obj");
        testEntity_ = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.transform3dManager.SetPosition(testEntity_, Vec3f(3.0f, -3.0f, -5.0f));
        cContainer_.transform3dManager.SetScale(testEntity_, Vec3f(1.0f, 3.0f, 1.0f));
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            testEntity_, config.dataRootPath + "models/cube/cube.obj");
        testEntity_ = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.transform3dManager.SetPosition(testEntity_, Vec3f(1.0f, 3.0f, 1.0f));
        cContainer_.transform3dManager.SetRotation(
            testEntity_, EulerAngles(degree_t(180.0f), degree_t(45.0f), degree_t(265.0f)));
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            testEntity_, config.dataRootPath + "models/cube/cube.obj");
        testEntity_ = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.transform3dManager.SetPosition(testEntity_, Vec3f(0.0f, 0.0f, -10.0f));
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            testEntity_, config.dataRootPath + "models/sphere/sphere.obj");
        testEntity_ = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.transform3dManager.SetPosition(testEntity_, Vec3f(3.0f, 1.0f, 3.0f));
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            testEntity_, config.dataRootPath + "models/sphere/sphere.obj");
        testEntity_ = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity_);
        cContainer_.transform3dManager.SetPosition(testEntity_, Vec3f(0.0f, 0.0f, 0.0f));
        cContainer_.renderManager.AddComponent(testEntity_);
        cContainer_.renderManager.SetModel(
            testEntity_, config.dataRootPath + "models/cube/cube.obj");
        cContainer_.entityManager.SetEntityParent(5, testEntity_);
    }

    void Update(seconds dt) override
    {
        cContainer_.transform3dManager.SetPosition(testEntity_,
            Vec3f(Cos(radian_t(updateCount_)),
                Sin(radian_t(updateCount_)),
                Cos(radian_t(updateCount_))) * 2.0f);
        cContainer_.transform3dManager.SetRotation(testEntity_,
            EulerAngles(radian_t(updateCount_), radian_t(updateCount_), radian_t(updateCount_)));
        cContainer_.transform3dManager.SetScale(testEntity_,
            Vec3f(abs(Cos(radian_t(updateCount_))),
                abs(Sin(radian_t(updateCount_))),
                abs(Cos(radian_t(updateCount_)))));
        updateCount_ += dt.count();
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
    sdl::MovableCamera3D camera_;

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;

    #ifdef NEKO_GLES3
    std::unique_ptr<GizmoRenderer> gizmosRenderer_;
    #endif

    Entity testEntity_;
};

TEST(Engine, TestRenderer)
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
}
}    // namespace neko::aer
#endif