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

#ifdef NEKO_GLES3
#include <sdl_engine/sdl_engine.h>
#include <sdl_engine/sdl_input.h>
#include <gl/gles3_window.h>
#include <gl/graphics.h>

#include "aer_engine.h"
#include "editor/tool/logger.h"
#include "log.h"

class SimulateLogger : public neko::SystemInterface
{
public:

	SimulateLogger(neko::aer::AerEngine& engine)
		: engine_(engine)
	{
		if (engine.GetMode() != neko::aer::ModeEnum::GAME)
		{
			toolManager_ = std::make_unique<neko::aer::EditorToolManager>(engine_);
			engine_.RegisterSystem(*toolManager_);
			engine_.RegisterOnDrawUi(*toolManager_);
			engine_.RegisterOnEvent(*toolManager_);
			toolManager_->AddEditorTool<neko::aer::Logger, neko::aer::EditorToolInterface::ToolType::LOGGER>();
		}
	}

	void Init() override
	{
	}

	void Update(neko::seconds dt) override
	{
		if (!testSuccess_)
		{
			if (nextTest_)
			{
				numberTest_++;
				if (numberTest_ == 2)
				{
					//TEST SUCCESS
					testSuccess_ = true;
					neko::LogDebug("[TEST] All tests were validated");
					engine_.Stop();
					return;
				}
				neko::LogDebug(msgTest_[numberTest_]);
				nextTest_ = false;
			}
			switch (numberTest_)
			{
				case 0: //TEST 1
					{
						int nbr = neko::Log::get().GetLogs().size();
						if (nbr <= std::pow(2, 10))
						{
							for (size_t i = 0; i < 100; i++)
							{
								int rdm = rand() % 5;
								switch (rdm)
								{
									case 0:
										neko::LogDebug(
											msgTest_[numberTest_]);
										break;
									case 1:
										neko::LogInfo(
											msgTest_[numberTest_]);
										break;
									case 2:
										neko::LogWarning(
											msgTest_[numberTest_]);
										break;
									case 3:
										neko::LogError(
											msgTest_[numberTest_]);
										break;
									case 4:
										neko::LogCritical(
											msgTest_[numberTest_]);
										break;
								}
							}
						}
						else
						{
							neko::LogDebug("Maximum of logs: Success");
							capacityMax_ = true;
							nextTest_ = true;
						}
					}
					break;

				case 1: //TEST 2
					{
						neko::Log::get().ClearLogs();
						int nbr = neko::Log::get().GetLogs().size();
						if (nbr <= 0)
						{
							neko::LogDebug("Erasing logs: Success");
							capacityClear_ = true;
							nextTest_ = true;
						}
					}
					break;
			}
		}
	}

	void Destroy() override
	{
	}

	void HasSucceed() const
	{
		EXPECT_TRUE(capacityMax_);
		EXPECT_TRUE(capacityClear_);
		EXPECT_TRUE(testSuccess_);
	}

private:
	std::unique_ptr<neko::aer::EditorToolManager> toolManager_;
	bool capacityMax_ = false;
	bool capacityClear_ = false;
	bool testSuccess_ = false;

	bool nextTest_ = true;
	int numberTest_ = -1;

	std::string msgTest_[2] = {
		"[Action] Please Wait",
		"[Action] Please clear the logs"
	};

	neko::aer::AerEngine& engine_;
};

TEST(Tool, TestLoggerGame)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	neko::Configuration config;
	config.windowName = "AerEditor";
	config.windowSize = neko::Vec2u(1400, 900);

	neko::sdl::Gles3Window window;
	neko::gl::Gles3Renderer renderer;
	neko::aer::AerEngine engine(&config, neko::aer::ModeEnum::GAME);

	engine.SetWindowAndRenderer(&window, &renderer);

	SimulateLogger simulateLogger(engine);
	engine.RegisterSystem(simulateLogger);

	engine.Init();

	engine.EngineLoop();

	simulateLogger.HasSucceed();
}


TEST(Tool, TestLoggerEditor)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
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

	SimulateLogger simulateLogger(engine);
	engine.RegisterSystem(simulateLogger);

	engine.Init();

	engine.EngineLoop();

	simulateLogger.HasSucceed();
}
#endif