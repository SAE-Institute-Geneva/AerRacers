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


#include "aer/ui/ui_element.h"
#include "aer/ui/ui_manager.h"
#include <aer\aer_engine.h>

#ifdef NEKO_GLES3
#include <gl/gles3_window.h>
#include <gl/graphics.h>

namespace neko::aer
{
const static uint8_t kHotBarSize = 9;
const static Vec2u kTileSize = Vec2u(80u);

struct PlayerUi
{
    //Crosshair
    UiElement crossHair{ Vec3f::zero, Vec2u(64, 64) };

    //HotBar
    UiElement hotBar{ Vec3f::zero, Vec2u(728, 88) };
    std::array<size_t, kHotBarSize> hotBarBlocks{};
    std::array<UiElement, kHotBarSize> hotBarPreviews{};

    //Block Select
    UiElement blockSelect{ Vec3f::zero, Vec2u(96, 96) };
    short selectIndex = 0;
};

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
        const Vec2f toolBarSize = Vec2f(ui_.hotBar.size) / Vec2f(config.windowSize);
        ui_.hotBar.position.y = toolBarSize.y / 2 - 1.0;

        const Vec2f tileSize = Vec2f(kTileSize) / Vec2f(config.windowSize);

        ui_.blockSelect.position.y = ui_.hotBar.position.y;
        ui_.blockSelect.position.x = ui_.hotBar.position.x + (ui_.selectIndex - 4) * tileSize.x;

        ui_.crossHair.texturePath = config.dataRootPath + "sprites/grass.png";
        ui_.hotBar.texturePath = config.dataRootPath + "sprites/grass.png";
        ui_.blockSelect.texturePath = config.dataRootPath + "sprites/grass.png";


        Entity testEntity = cContainer_.entityManager.CreateEntity();
        cContainer_.transform3dManager.AddComponent(testEntity);
        cContainer_.transform3dManager.SetRelativePosition(testEntity, Vec3f(0.0f, 0.0f, 0.0f));
        cContainer_.renderManager.AddComponent(testEntity);
        cContainer_.renderManager.SetModel(
            testEntity, config.dataRootPath + "models/cube/cube.obj");
    }

    void InitRender()
    {
        UiManagerLocator::get().AddUiElement(&ui_.crossHair);
        UiManagerLocator::get().AddUiElement(&ui_.hotBar);
        UiManagerLocator::get().AddUiElement(&ui_.blockSelect);
        //for (int i = 0; i < ui_.hotBarPreviews.size(); ++i)
        //{
        //    ui_.hotBarPreviews[i].position.x = ui_.hotBar.position.x + (i - 4) * tileSize.x;
        //    ui_.hotBarPreviews[i].position.y = ui_.hotBar.position.y;
        //    ui_.hotBarPreviews[i].size = kTileSize;

        //    if (ui_.hotBarBlocks[i] > 0)
        //        ui_.hotBarPreviews[i].textureId = blockManager_.GetBlock(ui_.hotBarBlocks[i]).previewTexture;
        //    else
        //        ui_.hotBarPreviews[i].texturePath = config.dataRootPath + "sprites/empty.png";

        //    uiManager_.AddUiElement(&ui_.hotBarPreviews[i]);
        //}
    }

    void Update(seconds dt) override
    {
        //updateCount_ += dt.count();
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
    PlayerUi ui_;

    ResourceManagerContainer& rContainer_;
    ComponentManagerContainer& cContainer_;
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
    Job initTestJob{ [&testUiManager]() { testUiManager.InitRender(); } };
    BasicEngine::GetInstance()->ScheduleJob(&initTestJob, JobThreadType::RENDER_THREAD);
    engine.EngineLoop();
    #ifdef EASY_PROFILE_USE
    profiler::dumpBlocksToFile("Renderer_Neko_Profile.prof");
    #endif
}
}    // namespace neko::aer
#endif