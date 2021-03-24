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
 Author : Sebastien Feser
 Co-Author :
 Date : 10.03.2021
---------------------------------------------------------- */

#include <gtest/gtest.h>
#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

#include "aer/gizmos_renderer.h"
#ifdef NEKO_GLES3
#include "aer/aer_engine.h"
#include "engine/engine.h"
#include <engine/system.h>
#include <gl/gles3_window.h>
#include <gl/graphics.h>

namespace neko::aer {

    class TestLevelDesignBlock
        : public SystemInterface
         {
    public:
        TestLevelDesignBlock(
            AerEngine& engine)
            : engine_(engine),
            rContainer_(engine.GetResourceManagerContainer()),
            cContainer_(engine.GetComponentManagerContainer()) { }


        void Init() override {
            Camera3D* camera = GizmosLocator::get().GetCamera();
            camera->fovY = degree_t(80.0f);
            camera->nearPlane = 0.1f;
            camera->farPlane = 1'000'000.0f;
            engine_.GetCameras().SetCameras(*camera);
            const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
            engine_.GetComponentManagerContainer().sceneManager.LoadScene(
                config.dataRootPath +
                "scenes/test_leveldesign.aerscene");
            cContainer_.playerManager.CreatePlayer(Vec3f(-192.0f, 84.0f, 56.0f));
        }

        void Update(neko::seconds dt) override {

        }

        void Destroy() override {
        }

        void HasSucceed() const {
        }

    private:

        ResourceManagerContainer& rContainer_;
        ComponentManagerContainer& cContainer_;

        AerEngine& engine_;
    };

    TEST(Game, TestLevelDesign)
    {
        Configuration config;
        config.windowName = "AerEditor";
        config.windowSize = neko::Vec2u(1400, 900);
        sdl::Gles3Window window;
        gl::Gles3Renderer renderer;
        Filesystem filesystem;
        AerEngine engine(filesystem, &config, ModeEnum::EDITOR);
        engine.SetWindowAndRenderer(&window, &renderer);

        TestLevelDesignBlock testLevelDesign(engine);
        engine.RegisterSystem(testLevelDesign);

        engine.Init();
        engine.EngineLoop();
        testLevelDesign.HasSucceed();

    }
}
#endif
