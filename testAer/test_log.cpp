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
 Date : 03.11.2020
---------------------------------------------------------- */
#include <gtest/gtest.h>

#include "sdl_engine/sdl_engine.h"
#include "sdl_engine/sdl_input.h"
#include "gl/gles3_window.h"
#include "gl/graphics.h"

#include "aer_engine.h"
#include "editor/tool/logger.h"
#include "log.h"

class SimulateLogger : public neko::SystemInterface {
public:
    SimulateLogger(neko::aer::AerEngine& engine) : engine_(engine) {

    }

    void Init() override { 
        
    }

    void Update(neko::seconds dt) override
    {
        int nbr; 
        if (nextTest) {
            if (numberTest == 4) {
                
            }
            numberTest++;
            neko::LogDebug(msgTest[numberTest]);
            nextTest = false;
        }
        switch (numberTest)
        {
        case 0:
            if (engine_.GetTool(neko::aer::TypeTool::LOG).isVisible) {
                neko::LogDebug("[TEST] Logger Opened: OK");
                loggerOpen_ = true;
                EXPECT_TRUE(loggerOpen_);
                nextTest = true;
            }
            break;

        case 1:
            nbr = static_cast<neko::aer::Logger&>(engine_.GetTool(neko::aer::TypeTool::LOG)).GetLogs().size();
            if (nbr <= std::pow(2, 23)) {
                for (size_t i = 0; i < 5000; i++)
                {
                    int rdm = rand() % 5;
                    switch (rdm)
                    {
                    case 0:
                        neko::aer::DebugLog(msgTest[numberTest]);
                        break;
                    case 1:
                        neko::aer::InfoLog(msgTest[numberTest]);
                        break;
                    case 2:
                        neko::aer::WarningLog(msgTest[numberTest]);
                        break;
                    case 3:
                        neko::aer::ErrorLog(msgTest[numberTest]);
                        break;
                    case 4:
                        neko::aer::CriticalLog(msgTest[numberTest]);
                        break;
                    }
                   
                }
            }
            else {
                neko::LogDebug("[TEST] Logs Max: OK");
                capacityMax_ = true;
                nextTest = true;
            }
            break;

        case 2:
            nbr = static_cast<neko::aer::Logger&>(engine_.GetTool(neko::aer::TypeTool::LOG)).GetLogs().size();
            if (nbr <= 0) {
                neko::LogDebug("[TEST] Logs Cleared: OK");
                capacityClear_ = true;
                nextTest = true;
            }
            break;
        case 3:
            if (!engine_.GetTool(neko::aer::TypeTool::LOG).isVisible) {
                neko::LogDebug("[TEST] Logger Closed: OK");
                loggerClose_ = true;
                nextTest = true;
            }
            break;
        }
       
       
    }

    void Destroy() override { }

    void HasSucceed() const
    {
        
        EXPECT_TRUE(loggerClose_);
        EXPECT_TRUE(capacityMax_);
        EXPECT_TRUE(capacityClear_);
    }

private:
    bool loggerOpen_ = false;
    bool loggerClose_ = false;
    bool capacityMax_ = false;
    bool capacityClear_ = false;

    bool nextTest = true;
    int numberTest = -1;

    std::string msgTest[4] = {
        "Please Open Logger",
        "Please Waiting",
        "Please Clear Logger",
        "Please Close Logger"
    };

    neko::aer::AerEngine& engine_;
};

TEST(Tool, TestLogger)
{
    //Travis Fix because Windows can't open a window
    char* env = getenv("WIN_TRAVIS");
    if (env != nullptr) {
        std::cout << "Test skip for travis windows" << std::endl;
        return;
    }

    neko::Configuration config;
    config.windowName = "AerEditor";
    config.windowSize = neko::Vec2u(1400, 900);

    neko::sdl::Gles3Window window;
    neko::gl::Gles3Renderer renderer;
    neko::aer::AerEngine engine(&config);

    engine.SetWindowAndRenderer(&window, &renderer);

    engine.Init();

    SimulateLogger simulateLogger(engine);
    engine.RegisterSystem(simulateLogger);

    engine.EngineLoop();

    simulateLogger.HasSucceed();
}