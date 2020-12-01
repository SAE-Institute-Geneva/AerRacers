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

 Author : Dylan von Arx
 Co-Author :
 Date : 11.11.2020
---------------------------------------------------------- */
#include <gtest/gtest.h>
#include <sdl_engine/sdl_engine.h>
#include <sdl_engine/sdl_input.h>
#include <gl/gles3_window.h>
#include <gl/graphics.h>

#include "aer_engine.h"
#include "gl/shader.h"
#include "gl/shape.h"

class TestPerformance : public neko::RenderCommandInterface , public neko::SystemInterface {
public:

    TestPerformance(neko::aer::AerEngine& engine)
        : engine_(engine)
    {
    }


    void Init() override {
        const auto& config = neko::BasicEngine::GetInstance()->config;
    }

    void Update(neko::seconds dt) override
    {
        neko::RendererLocator::get().Render(this);

        counterTime_ += dt.count();
        if (counterTime_ >= kTimeToWait_) {
            testSuccess_ = true;
            //engine_.Stop();
        }
    }

    void Destroy() override {
        cube_.Destroy();
    }

    void HasSucceed() const
    {
        EXPECT_TRUE(testSuccess_);
    }

    void Render() override {
        const auto& config = neko::BasicEngine::GetInstance()->config;

        cube_.Init();
        for(int i = 0; i<5500; i++)
            cube_.Draw();

    }
private:
    bool testSuccess_ = false;
    neko::aer::AerEngine& engine_;
    float counterTime_ = 0.0f;
    const float kTimeToWait_ = 2.0f;

    //Test
    float dt_ = 0.0f;

    neko::gl::RenderCuboid cube_{neko::Vec3f::zero, neko::Vec3f::one };

};

TEST(Performance, Cube)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("TRAVIS_DEACTIVATE_GUI");
    if (env != nullptr) {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }
    neko::Configuration config;
    config.windowName = "AerEditor";
    config.windowSize = neko::Vec2u(1400, 900);

    neko::sdl::Gles3Window window;
    neko::gl::Gles3Renderer renderer;
    neko::aer::AerEngine engine(&config, neko::aer::ModeEnum::TEST);
    engine.SetWindowAndRenderer(&window, &renderer);
    TestPerformance testPerformance(engine);
    engine.RegisterSystem(testPerformance);


    engine.Init();

    engine.EngineLoop();

    testPerformance.HasSucceed();
}
