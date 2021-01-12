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

class TestSceneInterface {
public:
    virtual void HasSucceed(neko::EntityManager& entityManager, neko::Transform3dManager& transform3dManager, neko::SceneManager sceneManager) = 0;
    std::string sceneName;
};

class TestUnityScene : public TestSceneInterface {
public:
    explicit TestUnityScene() {
        sceneName = "scenes/WillToolScene2021-01-11-14-05-34.aerscene";
    }

    virtual void HasSucceed(
        neko::EntityManager& entityManager,
        neko::Transform3dManager& transform3dManager, neko::SceneManager sceneManager) override
    {
        EXPECT_TRUE(sceneManager.GetCurrentScene().sceneName == "WillToolScene");
        EXPECT_TRUE(entityManager.GetEntitiesNmb() == 7);
        EXPECT_TRUE(entityManager.GetEntityParent(1) == 0);
        EXPECT_TRUE(neko::TagLocator::get().CompareEntitiesTag(0, 1));
        EXPECT_TRUE(neko::TagLocator::get().IsEntityLayer(0, "Ground"));
        EXPECT_TRUE(neko::TagLocator::get().IsEntityTag(0, 0));
        EXPECT_TRUE(entityManager.HasComponent(0, neko::EntityMask(neko::ComponentType::TRANSFORM3D)));
        EXPECT_TRUE(entityManager.GetEntityParent(1) == 0);
        EXPECT_TRUE(transform3dManager.GetPosition(0) == neko::Vec3f(1.0, 3.0, 5.0));
    }
};

class TestExampleScene : public TestSceneInterface {
public:
    explicit TestExampleScene() {
        sceneName = "scenes/scene_example.scene";
    }

    void HasSucceed(neko::EntityManager& entityManager, neko::Transform3dManager& transform3dManager, neko::SceneManager sceneManager) override
    {
        EXPECT_TRUE(neko::TagLocator::get().CompareEntitiesTag(1, 2));
        EXPECT_TRUE(neko::TagLocator::get().IsEntityTag(0, "0"));
        EXPECT_TRUE(neko::TagLocator::get().IsEntityLayer(1, "5"));
        EXPECT_TRUE(entityManager.HasComponent(0, neko::EntityMask(neko::ComponentType::TRANSFORM3D)));
        EXPECT_TRUE(entityManager.GetEntityParent(1) == 0);
        EXPECT_TRUE(transform3dManager.GetPosition(0) == neko::Vec3f(960, 540, 0));
    }

};

class SceneImporteurTester : public neko::SystemInterface {
public:
    explicit SceneImporteurTester(neko::aer::AerEngine& engine, TestSceneInterface& testScene) :
          engine_(engine),
          sceneManager_(engine_.GetEntityManager(), engine_.GetFilesystem(), transform3dManager_),
          transform3dManager_(engine_.GetEntityManager()), testScene_(testScene)
    { }

    void Init() override
    {
        const neko::Configuration config = neko::BasicEngine::GetInstance()->GetConfig();
        sceneManager_.LoadScene(config.dataRootPath + testScene_.sceneName);
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
        testScene_.HasSucceed(engine_.GetEntityManager(), transform3dManager_, sceneManager_);
    }



private:

    int updateCount_ = 0;
    const int kEngineDuration_ = 10;

    neko::aer::AerEngine& engine_;

    neko::SceneManager sceneManager_;
    neko::Transform3dManager transform3dManager_;
    TestSceneInterface& testScene_;
};

TEST(Scene, TestExampleSceneImporteur)
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
    TestExampleScene testExample;
    SceneImporteurTester testSceneImporteur(engine, testExample);
    engine.RegisterSystem(testSceneImporteur);

    engine.Init();

    engine.EngineLoop();

    testSceneImporteur.HasSucceed();
}

TEST(Scene, TestUnitySceneImporteur)
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
    neko::aer::AerEngine engine(filesystem, &config, neko::aer::ModeEnum::TEST);

    engine.SetWindowAndRenderer(&window, &renderer);
    TestUnityScene testUnity;
    SceneImporteurTester testSceneImporteur(engine, testUnity);
    engine.RegisterSystem(testSceneImporteur);

    engine.Init();

    engine.EngineLoop();

    testSceneImporteur.HasSucceed();
}
#endif
