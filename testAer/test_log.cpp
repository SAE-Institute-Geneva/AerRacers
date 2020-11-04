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
//TODOCR(Luca@Dylan) Apply Syntax Style
class SimulateLogger : public neko::SystemInterface {
public:
    SimulateLogger(neko::aer::AerEngine& engine) : engine_(engine) {
        logger_ = engine_.GetTool(neko::aer::TypeTool::LOG);
        if (logger_ == nullptr) {
            neko::LogDebug("Logger not found");
            engine_.Stop();
        }
    }

    void Init() override { 
        
    }

    void Update(neko::seconds dt) override
    {
        
        if (!testSucces_) {
            if (nextTest_) {
             
                numberTest_++;
                if (numberTest_ == 4) {
                    //TEST SUCCESS
                    testSucces_ = true;
                    return;
                }
                neko::LogDebug(msgTest_[numberTest_]);
                nextTest_ = false;
            }
            switch (numberTest_)
            {
            case 0: //TEST 1
            {

                if (logger_->isVisible) {
                    neko::LogDebug("[TEST] Opening the Logger: OK");
                    loggerOpen_ = true;
                    nextTest_ = true;
                }
                else {
                    logger_->isVisible = true;
                }
            }
                break;

            case 1://TEST 2
            {
                int nbr = neko::aer::Log::get().GetLogs().size();
                if (nbr <= std::pow(2, 20)) {
                    for (size_t i = 0; i < 5000; i++)
                    {
                        int rdm = rand() % 5;
                        switch (rdm)
                        {
                        case 0:
                            neko::aer::DebugLog(msgTest_[numberTest_]);
                            break;
                        case 1:
                            neko::aer::InfoLog(msgTest_[numberTest_]);
                            break;
                        case 2:
                            neko::aer::WarningLog(msgTest_[numberTest_]);
                            break;
                        case 3:
                            neko::aer::ErrorLog(msgTest_[numberTest_]);
                            break;
                        case 4:
                            neko::aer::CriticalLog(msgTest_[numberTest_]);
                            break;
                        }

                    }
                }
                else {
                    neko::LogDebug("[TEST] Maximum of logs: OK");
                    capacityMax_ = true;
                    nextTest_ = true;
                }
            }
                break;

            case 2://TEST 3
            {
                int nbr = neko::aer::Log::get().GetLogs().size();
                if (nbr <= 0) {
                    neko::LogDebug("[TEST] Erasing logs: OK");
                    capacityClear_ = true;
                    nextTest_ = true;
                }
                else {
                    neko::aer::Log::get().ClearLogs();
                }
            }
                break;

            case 3://TEST 4
            {
                if (!engine_.GetTool(neko::aer::TypeTool::LOG)->isVisible) {
                    neko::LogDebug("[TEST] Closing the Logger: OK");
                    loggerClose_ = true;
                    nextTest_ = true;
                }
                else {
                    logger_->isVisible = false;
                }
            }
                break;
            }
        }
        else {
            neko::LogDebug("[TEST] All tests were validated");
            engine_.Stop();
        }
       
    }

    void Destroy() override { }

    void HasSucceed() const
    {
        EXPECT_TRUE(loggerOpen_);
        EXPECT_TRUE(capacityMax_);
        EXPECT_TRUE(capacityClear_);
        EXPECT_TRUE(loggerClose_);
        EXPECT_TRUE(testSucces_);
    }

private:
    neko::aer::Tool* logger_;
    bool loggerOpen_ = false;
    bool loggerClose_ = false;
    bool capacityMax_ = false;
    bool capacityClear_ = false;
    bool testSucces_ = false;

    bool nextTest_ = true;
    int numberTest_ = -1;

    std::string msgTest_[4] = {
        "[Action] Please open the logger",
        "[Action] Please Wait",
        "[Action] Please clear the logs",
        "[Action] Please Close the Logger"
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
    neko::aer::AerEngine engine(&config, neko::aer::ModeEnum::EDITOR);

    engine.SetWindowAndRenderer(&window, &renderer);

    engine.Init();

    SimulateLogger simulateLogger(engine);
    engine.RegisterSystem(simulateLogger);

    engine.EngineLoop();

    simulateLogger.HasSucceed();
}