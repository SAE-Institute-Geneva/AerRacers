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

        anchorTR = UiImage{ config.dataRootPath + "sprites/water/Water01.jpg", Vec3f::zero, Vec2u::one * 150.0f, UiAnchor::TOP_RIGHT, Color::white };
        anchorTL = UiImage{ config.dataRootPath + "sprites/water/Water01.jpg", Vec3f::zero, Vec2u::one * 150.0f, UiAnchor::TOP_LEFT , Color::white };
        anchorBL = UiImage{ config.dataRootPath + "sprites/water/Water01.jpg", Vec3f::zero, Vec2u::one * 150.0f, UiAnchor::BOTTOM_LEFT, Color::white };
        anchorBR = UiImage{ config.dataRootPath + "sprites/water/Water01.jpg", Vec3f::zero, Vec2u::one * 150.0f, UiAnchor::BOTTOM_RIGHT, Color::white };
        movingImage = UiImage{ config.dataRootPath + "sprites/wall.jpg", Vec3f::zero, Vec2u::one * 150.0f, UiAnchor::CENTER, Color::white };

    }

    void Update(seconds dt) override
    {
        UiManagerLocator::get().RenderUiImage(&anchorTR);
        UiManagerLocator::get().RenderUiImage(&anchorTL);
        UiManagerLocator::get().RenderUiImage(&anchorBL);
        UiManagerLocator::get().RenderUiImage(&anchorBR);
        UiManagerLocator::get().RenderUiText(FontLoaded::LOBSTER, "TL", Vec2f::zero, UiAnchor::TOP_LEFT, 1.0f, Color::red);
        UiManagerLocator::get().RenderUiText(FontLoaded::LOBSTER, "TR", Vec2f::zero, UiAnchor::TOP_RIGHT, 1.0f, Color::green);
        UiManagerLocator::get().RenderUiText(FontLoaded::LOBSTER, "BL", Vec2f::zero, UiAnchor::BOTTOM_LEFT, 1.0f, Color::blue);
        UiManagerLocator::get().RenderUiText(FontLoaded::LOBSTER, "BR", Vec2f::zero, UiAnchor::BOTTOM_RIGHT, 1.0f, Color::yellow);

        const auto& config = BasicEngine::GetInstance()->GetConfig();
        UiManagerLocator::get().RenderUiText(FontLoaded::LOBSTER, std::to_string(updateCount_), (Vec2f::up * 0.5f).Rotate(radian_t(updateCount_)), UiAnchor::CENTER, 1.0f, Color::cyan);
        movingImage.SetPosition(Vec3f((Vec2f::up * 0.5f).Rotate(radian_t(updateCount_))));
        UiManagerLocator::get().RenderUiImage(&movingImage);
        updateCount_ += dt.count();
        //if (updateCount_ > kEngineDuration_) { engine_.Stop(); }
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

    //Test anchor

    UiImage anchorBL;
    UiImage anchorBR;
    UiImage anchorTL;
    UiImage anchorTR;
    UiImage movingImage;
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
        //if (updateCount_ > kEngineDuration_) { engine_.Stop(); }
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