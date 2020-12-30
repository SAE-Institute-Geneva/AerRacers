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
 Date : 28.12.2020
---------------------------------------------------------- */
#include <gtest/gtest.h>

#include "engine/scene.h"
#ifdef NEKO_GLES3
#include <engine\system.h>
#include <aer_engine.h>
#include <gl\gles3_window.h>
#include <gl\graphics.h>

class TestSceneImporteur : public neko::SystemInterface {
public:
    explicit TestSceneImporteur(neko::aer::AerEngine& engine) :
          engine_(engine),
          sceneManager_(engine_.GetEntityManager(), engine_.GetFilesystem()) { }

    void Init() override
    {
        const neko::Configuration config = neko::BasicEngine::GetInstance()->GetConfig();
        sceneManager_.LoadScene(config.dataRootPath + "scenes/scene_example.scene");
    }

    void Update(neko::seconds dt) override
    {
        updateCount_++;
        if (updateCount_ == kEngineDuration_) {
            engine_.Stop();
        }
    }

    void Destroy() override { }

    void HasSucceed()
    {
        EXPECT_TRUE(neko::TagLocator::get().CompareEntitiesTag(1,2));
        EXPECT_TRUE(neko::TagLocator::get().IsEntityTag(0,"0"));
        EXPECT_TRUE(neko::TagLocator::get().IsEntityLayer(1, 5));
        EXPECT_TRUE(engine_.GetEntityManager().HasComponent(0, neko::EntityMask(neko::ComponentType::TRANSFORM3D)));
        //EXPECT_TRUE(entityManager_.GetEntityParent(0) == 1);
    }



private:

    int updateCount_ = 0;
    const int kEngineDuration_ = 10;

    neko::aer::AerEngine& engine_;

    neko::SceneManager sceneManager_;
};

TEST(Scene, TestSceneImporteur)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr) {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    neko::Configuration config;
    config.windowName = "AerEditor";
    config.dataRootPath = "../../data/";
    config.windowSize = neko::Vec2u(1400, 900);

    neko::sdl::Gles3Window window;
    neko::gl::Gles3Renderer renderer;
    neko::Filesystem filesystem;
    neko::aer::AerEngine engine(filesystem , &config, neko::aer::ModeEnum::TEST);

    engine.SetWindowAndRenderer(&window, &renderer);

    TestSceneImporteur testSceneImporteur(engine);
    engine.RegisterSystem(testSceneImporteur);

    engine.Init();

    engine.EngineLoop();

    testSceneImporteur.HasSucceed();
}
#endif
