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
 Date : 01.03.2021
---------------------------------------------------------- */
#include <gtest/gtest.h>
#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

#include "aer/gizmos_renderer.h"
#include "gl/gles3_window.h"
#include "gl/graphics.h"
#ifdef NEKO_GLES3
#include "aer/aer_engine.h"
#include "engine/engine.h"

namespace neko::aer {
    class TestMultiSpawning
        : public SystemInterface,
        public RenderCommandInterface,
        public DrawImGuiInterface {
    public:
        TestMultiSpawning(
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
            camera->fovY = degree_t(80.0f);
            camera->nearPlane = 0.1f;
            camera->farPlane = 1'000'000.0f;
            engine_.GetCameras().SetCameras(*camera);
            const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
            engine_.GetComponentManagerContainer().sceneManager.LoadScene(
                config.dataRootPath +
                "scenes/PlayGroundLuca2021-03-01withoutShip.aerscene");
            cContainer_.playerManager.CreatePlayer(Vec3f(0, 10.0f, 0), true);
            cContainer_.playerManager.CreatePlayer(Vec3f(10.0f, 10.0f, 0), true);
            cContainer_.playerManager.CreatePlayer(Vec3f(-10.0f, 10.0f, 0), false);
            cContainer_.playerManager.CreatePlayer(Vec3f(0.0f, 10.0f, 10.0f), false);
        }

        void Update(seconds dt) override
        {
#ifdef EASY_PROFILE_USE
            EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
            //updateCount_ += dt.count();
            if (updateCount_ > kEngineDuration_) {
                HasSucceed();
                engine_.Stop();
            }
        }

        void Render() override { }

        void Destroy() override { }

        void DrawImGui() override {}

        void HasSucceed()
        {
            EXPECT_TRUE(cContainer_.playerManager.GetPlayerCount() == 4);

        }

    private:
        float updateCount_ = 0;
        const float kEngineDuration_ = 10.0f;

        AerEngine& engine_;

        ResourceManagerContainer& rContainer_;
        ComponentManagerContainer& cContainer_;


        Entity shipEntity_;
        Entity groundEntity_;
    };

    TEST(Game, TestMultiSpawning)
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

        engine.Init();
        TestMultiSpawning testMultiSpawning(engine);
        testMultiSpawning.Init();
        engine.RegisterSystem(testMultiSpawning);
        engine.RegisterOnDrawUi(testMultiSpawning);
        engine.EngineLoop();
    }
    class TestMultiColori
        : public SystemInterface,
        public RenderCommandInterface,
        public DrawImGuiInterface {
    public:
        TestMultiColori(
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
            camera->fovY = degree_t(80.0f);
            camera->nearPlane = 0.1f;
            camera->farPlane = 1'000'000.0f;
            engine_.GetCameras().SetCameras(*camera);
            const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
            engine_.GetComponentManagerContainer().sceneManager.LoadScene(
                config.dataRootPath +
                "scenes/PlayGroundLuca2021-03-01withoutShip.aerscene");
        }

        void Update(seconds dt) override
        {
#ifdef EASY_PROFILE_USE
            EASY_BLOCK("Test Update", profiler::colors::Green);
#endif
            updateCount_ += dt.count();
            if (updateCount_ > kLoadingDuration_) {
                cContainer_.playerManager.CreatePlayer(Vec3f(0, 10.0f, 0), true, 1);
                cContainer_.playerManager.CreatePlayer(Vec3f(10.0f, 10.0f, 0), true, 0);
                cContainer_.playerManager.CreatePlayer(Vec3f(-10.0f, 10.0f, 0), false, 2);
                cContainer_.playerManager.CreatePlayer(Vec3f(0.0f, 10.0f, 10.0f), false, 3);
            }
            if (updateCount_ > kEngineDuration_) {
                HasSucceed();
                //engine_.Stop();
            }
        }

        void Render() override { }

        void Destroy() override { }

        void DrawImGui() override {}

        void HasSucceed()
        {
            EXPECT_TRUE(cContainer_.playerManager.GetPlayerCount() == 4);

        }

    private:
        float updateCount_ = 0;
        const float kLoadingDuration_ = 3.0f;
        const float kEngineDuration_ = 10.0f;

        AerEngine& engine_;

        ResourceManagerContainer& rContainer_;
        ComponentManagerContainer& cContainer_;


        Entity shipEntity_;
        Entity groundEntity_;
    };

    TEST(Game, TestMultiColori)
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

        engine.Init();
        TestMultiColori testMultiSpawning(engine);
        testMultiSpawning.Init();
        engine.RegisterSystem(testMultiSpawning);
        engine.RegisterOnDrawUi(testMultiSpawning);
        engine.EngineLoop();
    }
}
#endif
