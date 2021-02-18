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
	#include <gl/gles3_window.h>
	#include <gl/graphics.h>
	#include <sdl_engine/sdl_engine.h>
	#include <sdl_engine/sdl_input.h>

	#include "aer/aer_engine.h"
	#include "aer/editor/tool/logger.h"
	#include "aer/log.h"

namespace neko::aer
{
class SimulateLogger : public SystemInterface
{
public:
	SimulateLogger(AerEngine& engine) : engine_(engine)
	{
		if (engine.GetMode() != ModeEnum::GAME)
		{
			toolManager_ = std::make_unique<EditorToolManager>(engine_);
			engine_.RegisterSystem(*toolManager_);
			engine_.RegisterOnDrawUi(*toolManager_);
			engine_.RegisterOnEvent(*toolManager_);
			toolManager_->AddEditorTool<Logger,
				EditorToolInterface::ToolType::LOGGER>();
		}
	}

	void Init() override {}

	void Update(seconds dt) override
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
					LogDebug("[TEST] All tests were validated");
					engine_.Stop();
					return;
				}
				LogDebug(msgTest_[numberTest_]);
				nextTest_ = false;
			}
			switch (numberTest_)
			{
				case 0:    //TEST 1
				{
					int nbr = Log::get().GetLogs().size();
					if (nbr <= std::pow(2, 10))
					{
						for (size_t i = 0; i < 100; i++)
						{
							int rdm = rand() % 5;
							switch (rdm)
							{
								case 0: LogDebug(msgTest_[numberTest_]); break;
								case 1: LogInfo(msgTest_[numberTest_]); break;
								case 2: LogWarning(msgTest_[numberTest_]); break;
								case 3: LogError(msgTest_[numberTest_]); break;
								case 4: LogCritical(msgTest_[numberTest_]); break;
							}
						}
					}
					else
					{
						LogDebug("Maximum of logs: Success");
						capacityMax_ = true;
						nextTest_    = true;
					}
				}
				break;

				case 1:    //TEST 2
				{
					Log::get().ClearLogs();
					int nbr = Log::get().GetLogs().size();
					if (nbr <= 0)
					{
						LogDebug("Erasing logs: Success");
						capacityClear_ = true;
						nextTest_      = true;
					}
				}
				break;
			}
		}
	}

	void Destroy() override {}

	void HasSucceed() const
	{
		EXPECT_TRUE(capacityMax_);
		EXPECT_TRUE(capacityClear_);
		EXPECT_TRUE(testSuccess_);
	}

private:
	std::unique_ptr<EditorToolManager> toolManager_;
	bool capacityMax_   = false;
	bool capacityClear_ = false;
	bool testSuccess_   = false;

	bool nextTest_  = true;
	int numberTest_ = -1;

	std::string msgTest_[2] = {"[Action] Please Wait", "[Action] Please clear the logs"};

	AerEngine& engine_;
};

//TEST(Tool, TestLoggerGame)
//{
//	//Travis Fix because Windows can't open a window
//	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
//	if (env != nullptr)
//	{
//		std::cout << "Test skip for travis windows" << std::endl;
//		return;
//	}
//
//	Configuration config;
//	config.windowName = "AerEditor";
//	config.windowSize = Vec2u(1400, 900);
//
//	sdl::Gles3Window window;
//	gl::Gles3Renderer renderer;
//	Filesystem filesystem;
//	AerEngine engine(filesystem, &config, ModeEnum::GAME);
//
//	engine.SetWindowAndRenderer(&window, &renderer);
//
//	SimulateLogger simulateLogger(engine);
//	engine.RegisterSystem(simulateLogger);
//
//	engine.Init();
//
//	engine.EngineLoop();
//
//	simulateLogger.HasSucceed();
//}

TEST(Tool, TestLoggerEditor)
{
	//Travis Fix because Windows can't open a window
	char* env = getenv("TRAVIS_DEACTIVATE_GUI");
	if (env != nullptr)
	{
		std::cout << "Test skip for travis windows" << std::endl;
		return;
	}

	Configuration config;
	config.windowName = "AerEditor";
	config.windowSize = Vec2u(1400, 900);

	sdl::Gles3Window window;
	gl::Gles3Renderer renderer;
	Filesystem filesystem;
	AerEngine engine(filesystem, &config, ModeEnum::TEST);

	engine.SetWindowAndRenderer(&window, &renderer);

	SimulateLogger simulateLogger(engine);
	engine.RegisterSystem(simulateLogger);

	engine.Init();

	engine.EngineLoop();

	simulateLogger.HasSucceed();
}
}    // namespace neko::aer
#endif