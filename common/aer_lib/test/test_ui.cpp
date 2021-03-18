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
 Date : 13.03.2021
---------------------------------------------------------- */
#include <gtest/gtest.h>

#include "graphics/color.h"

#include "aer/ui/ui_element.h"
#include "aer/ui/ui_manager.h"
#include <aer\aer_engine.h>

#ifdef NEKO_GLES3
#include <gl/gles3_window.h>
#include <gl/graphics.h>

namespace neko::aer
{
class TestUiManager : public SystemInterface, public RenderCommandInterface, public DrawImGuiInterface
{
public:
    TestUiManager(
        AerEngine& engine)
        : engine_(engine),
          rContainer_(engine.GetResourceManagerContainer()),
          cContainer_(engine.GetComponentManagerContainer()) { }
    void Init() override
    {
        const auto& config = BasicEngine::GetInstance()->GetConfig();

        Entity testEntity = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity);
        cContainer_.transform3dManager.SetRelativePosition(testEntity, Vec3f(0.0f, 0.0f, 0.0f));
        cContainer_.renderManager.AddComponent(testEntity);
        cContainer_.renderManager.SetModel(
            testEntity, config.dataRootPath + "models/cube/cube.obj");

        anchorTR = UiImage{ config.dataRootPath + "sprites/water/Water01.jpg",Vec3f::zero , Vec2u::one * 150.0f, UiAnchor::TOP_RIGHT, 0, Color::white };
        anchorTL = UiImage{ config.dataRootPath + "sprites/water/Water01.jpg",Vec3f::zero , Vec2u::one * 150.0f, UiAnchor::TOP_LEFT , 0, Color::white };
        anchorBL = UiImage{ config.dataRootPath + "sprites/water/Water01.jpg",Vec3f::zero , Vec2u::one * 150.0f, UiAnchor::BOTTOM_LEFT, 0, Color::white };
        anchorBR = UiImage{ config.dataRootPath + "sprites/water/Water01.jpg",Vec3f::zero , Vec2u::one * 150.0f, UiAnchor::BOTTOM_RIGHT, 0, Color::white };
        movingImage = UiImage{ config.dataRootPath + "sprites/wall.jpg", Vec3f::zero, Vec2u{2, 1} *150.0f, UiAnchor::CENTER, 0, Color::white };
        auto& uiManager = UiManagerLocator::get();
        textBL = UiText{ FontLoaded::LOBSTER, "BL", Vec3f(1.0f, 1.0f, 0.0f) * 0.1f, UiAnchor::BOTTOM_LEFT, 0, 1.0f, Color::cyan };
        textBR = UiText{ FontLoaded::LOBSTER, "BR", Vec3f(-1.0f, 1.0f, 0.0f) * 0.1f, UiAnchor::BOTTOM_RIGHT, 0, 1.0f, Color::yellow };
        textTL = UiText{ FontLoaded::LOBSTER, "TL", Vec3f(1.0f, -1.0f, 0.0f) * 0.1f, UiAnchor::TOP_LEFT, 0, 1.0f, Color::red };
        textTR = UiText{ FontLoaded::LOBSTER, "TR", Vec3f(-1.0f, -1.0f, 0.0f) * 0.1f, UiAnchor::TOP_RIGHT, 0, 1.0f, Color::green };
        movingText = UiText{ FontLoaded::LOBSTER, std::to_string(updateCount_), (Vec3f::up * 0.5f), UiAnchor::CENTER, 0, 1.0f, Color::blue };
        uiManager.AddUiImage(&anchorTR);
        uiManager.AddUiImage(&anchorTL);
        uiManager.AddUiImage(&anchorBL);
        uiManager.AddUiImage(&anchorBR);
        uiManager.AddUiImage(&movingImage);
        uiManager.AddUiText(&textBL);
        uiManager.AddUiText(&textBR);
        uiManager.AddUiText(&textTL);
        uiManager.AddUiText(&textTR);
        uiManager.AddUiText(&movingText);

    }

    void Update(seconds dt) override
    {

        const auto& config = BasicEngine::GetInstance()->GetConfig();
        movingText.SetPosition(Vec3f((Vec2f::up * 0.5f).Rotate(radian_t(updateCount_))));
        movingText.SetText(std::to_string(updateCount_));
        movingImage.SetPosition(Vec3f((Vec2f::up * 0.5f).Rotate(radian_t(updateCount_))));
        uint8_t screen = (static_cast<int>(updateCount_) / 1) % 5;
        movingText.SetScreenId(screen);
        movingImage.SetScreenId(screen);
        anchorTR.SetScreenId(screen);
        anchorTL.SetScreenId(screen);
        anchorBL.SetScreenId(screen);
        anchorBR.SetScreenId(screen);
        textBL.SetScreenId(screen);
        textBR.SetScreenId(screen);
        textTL.SetScreenId(screen);
        textTR.SetScreenId(screen);
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
    const float kEngineDuration_ = 2.0f;

    AerEngine& engine_;

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;

    //Test anchor

    UiImage anchorBL;
    UiImage anchorBR;
    UiImage anchorTL;
    UiImage anchorTR;
    UiImage movingImage;
    UiText textBL;
    UiText textBR;
    UiText textTL;
    UiText textTR;
    UiText movingText;
};

TEST(UIManager, TestWithEngine)
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

    TestUiManager testUiManager(engine);

    engine.RegisterSystem(testUiManager);
    engine.RegisterOnDrawUi(testUiManager);
    engine.Init();
    engine.EngineLoop();
    #ifdef EASY_PROFILE_USE
    profiler::dumpBlocksToFile("UiManager_Neko_Profile.prof");
    #endif
}

TEST(UIManager, TestInGame)
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
    AerEngine engine(filesystem, &config, ModeEnum::GAME);

    engine.SetWindowAndRenderer(&window, &renderer);

    TestUiManager testUiManager(engine);

    engine.RegisterSystem(testUiManager);
    engine.RegisterOnDrawUi(testUiManager);
    engine.Init();
    engine.EngineLoop();
#ifdef EASY_PROFILE_USE
    profiler::dumpBlocksToFile("UiManager_Neko_Profile.prof");
#endif
}

class TestUiMenu : public SystemInterface, public RenderCommandInterface, public DrawImGuiInterface
{
public:
    TestUiMenu(
        AerEngine& engine)
        : engine_(engine),
        rContainer_(engine.GetResourceManagerContainer()),
        cContainer_(engine.GetComponentManagerContainer()) { }
    void Init() override
    {
        const auto& config = BasicEngine::GetInstance()->GetConfig();

        Entity testEntity = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity);
        cContainer_.transform3dManager.SetRelativePosition(testEntity, Vec3f(0.0f, 0.0f, 0.0f));
        cContainer_.renderManager.AddComponent(testEntity);
        cContainer_.renderManager.SetModel(
            testEntity, config.dataRootPath + "models/cube/cube.obj");

    }

    void Update(seconds dt) override
    {
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
    float updateCount_ = 0;
    const float kEngineDuration_ = 0.5f;

    AerEngine& engine_;

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;

    enum Menu {
        MENUA,
        MENUB,
        MENUC
    };

    Menu currentMenu_;

    //Menu A
    UiImage backGroundA_;
    UiImage buttonPrevA_;
    UiImage buttonNextA_;

    UiImage backGroundB_;
    UiImage buttonPrevB_;
    UiImage buttonNextB_;

    UiImage backGroundC_;
    UiImage buttonPrevC_;
    UiImage buttonNextC_;
};

TEST(UIManager, TestMenu)
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

    TestUiMenu testUiMenu(engine);

    engine.RegisterSystem(testUiMenu);
    engine.RegisterOnDrawUi(testUiMenu);
    engine.Init();
    engine.EngineLoop();
#ifdef EASY_PROFILE_USE
    profiler::dumpBlocksToFile("UiManager_Neko_Profile.prof");
#endif
}
}    // namespace neko::aer
#endif