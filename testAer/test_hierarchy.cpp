#include <gtest/gtest.h>
#include <sdl_engine/sdl_engine.h>
#include <sdl_engine/sdl_input.h>
#include <gl/gles3_window.h>
#include <gl/graphics.h>

#include "aer_engine.h"
#include "editor/tool/logger.h"
#include "log.h"
#include "engine/entity.h"
#include "editor/tool/hierarchy.h"

class SimulateHierarchy : public neko::SystemInterface
{
public:

	SimulateHierarchy(neko::aer::AerEngine& engine)
		: engine_(engine)
	{
		if (engine.GetMode() != neko::aer::ModeEnum::GAME) // If engine mode is not game (to not have tools in game)
		{
			toolManager_ = std::make_unique<neko::aer::EditorToolManager>(engine_); // Create tool manager
			engine_.RegisterSystem(*toolManager_); // Register in system
			engine_.RegisterOnDrawUi(*toolManager_); // Register in DrawUI
			engine_.RegisterOnEvent(*toolManager_); // Register in Event
			toolManager_->AddEditorTool<neko::aer::Hierarchy, neko::aer::EditorToolInterface::ToolType::HIERARCHY>(); // Create tool like it is in editor
		}
	}

	void Init() override // Where we create entities
	{
		engine_.GetEntityManager().CreateEntity();
		engine_.GetEntityManager().CreateEntity();
		engine_.GetEntityManager().CreateEntity();
		engine_.GetEntityManager().CreateEntity();
		engine_.GetEntityManager().SetEntityParent(0, 1);
		engine_.GetEntityManager().SetEntityParent(1, 2);
		engine_.GetEntityManager().SetEntityParent(2, 3);
	}

	void Update(neko::seconds dt) override // Where we simulate tests
	{

		if (!testSuccess_)
		{
			//if (nextTest_)
			//{
			//	numberTest_++;
			//	if (numberTest_ == 2)
			//	{
			//		//TEST SUCCESS
			//		testSuccess_ = true;
			//		neko::LogDebug("[TEST] All tests were validated");
			//		engine_.Stop();
			//		return;
			//	}
			//	neko::LogDebug(msgTest_[numberTest_]);
			//	nextTest_ = false;
			//}
			//switch (numberTest_)
			//{
			//case 0: //TEST 1
			//{
			//	int nbr = neko::Log::get().GetLogs().size();
			//	if (nbr <= std::pow(2, 10))
			//	{
			//		for (size_t i = 0; i < 100; i++)
			//		{
			//			int rdm = rand() % 5;
			//			switch (rdm)
			//			{
			//			case 0:
			//				neko::LogDebug(
			//					msgTest_[numberTest_]);
			//				break;
			//			case 1:
			//				neko::LogInfo(
			//					msgTest_[numberTest_]);
			//				break;
			//			case 2:
			//				neko::LogWarning(
			//					msgTest_[numberTest_]);
			//				break;
			//			case 3:
			//				neko::LogError(
			//					msgTest_[numberTest_]);
			//				break;
			//			case 4:
			//				neko::LogCritical(
			//					msgTest_[numberTest_]);
			//				break;
			//			}
			//		}
			//	}
			//	else
			//	{
			//		neko::LogDebug("Maximum of logs: Success");
			//		capacityMax_ = true;
			//		nextTest_ = true;
			//	}
			//}
			//break;

			//case 1: //TEST 2
			//{
			//	neko::Log::get().ClearLogs();
			//	int nbr = neko::Log::get().GetLogs().size();
			//	if (nbr <= 0)
			//	{
			//		neko::LogDebug("Erasing logs: Success");
			//		capacityClear_ = true;
			//		nextTest_ = true;
			//	}
			//}
			//break;
			//}
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
//	neko::Configuration config;
//	config.windowName = "AerEditor";
//	config.windowSize = neko::Vec2u(1400, 900);
//
//	neko::sdl::Gles3Window window;
//	neko::gl::Gles3Renderer renderer;
//	neko::aer::AerEngine engine(&config, neko::aer::ModeEnum::GAME);
//
//	engine.SetWindowAndRenderer(&window, &renderer);
//
//	SimulateHierarchy simulateLogger(engine);
//	engine.RegisterSystem(simulateLogger);
//
//	engine.Init();
//
//	engine.EngineLoop();
//
//	simulateLogger.HasSucceed();
//}


TEST(Tool, TestHierarchy)
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

	SimulateHierarchy simulateHierarchy(engine);
	engine.RegisterSystem(simulateHierarchy);

	engine.Init();

	engine.EngineLoop();

	simulateHierarchy.HasSucceed();
}
