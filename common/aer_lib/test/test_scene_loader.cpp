#include <gtest/gtest.h>
#include <sdl_engine/sdl_engine.h>
#include <sdl_engine/sdl_input.h>
#include <gl/gles3_window.h>
#include <gl/graphics.h>

#include "aer/aer_engine.h"
#include "aer/editor/tool/logger.h"
#include "aer/log.h"
#include "engine/entity.h"
#include "aer/editor/tool/scene_loader.h"

class SimulateSceneLoader : public neko::SystemInterface
{
public:

	SimulateSceneLoader(neko::aer::AerEngine& engine)
		: engine_(engine)
	{
		if (engine.GetMode() != neko::aer::ModeEnum::GAME) // If engine mode is not game (to not have tools in game)
		{
			toolManager_ = std::make_unique<neko::aer::EditorToolManager>(engine_); // Create tool manager
			engine_.RegisterSystem(*toolManager_); // Register in system
			engine_.RegisterOnDrawUi(*toolManager_); // Register in DrawUI
			engine_.RegisterOnEvent(*toolManager_); // Register in Event
            toolManager_->AddEditorTool<neko::aer::SceneLoader,
                neko::aer::EditorToolInterface::ToolType::
                    SCENE_LOADER>();    // Create tool like it is in editor
		}
	}

	void Init() override // Where we create entities
	{
	}

	void Update(neko::seconds dt) override // Where we simulate tests
	{
        updateCount_+= dt.count();
        if (updateCount_ > kEngineDuration_) { engine_.Stop(); }
	}

	void Destroy() override
	{
	}

	void HasSucceed() const
	{
        EXPECT_TRUE(testSuccess_);
        logDebug("Test without check");
	}

private:
	std::unique_ptr<neko::aer::EditorToolManager> toolManager_;

    float updateCount_           = 0;
    const float kEngineDuration_ = 0.5f;

	bool testSuccess_ = true;

	neko::aer::AerEngine& engine_;
};


TEST(Tool, TestSceneLoader)
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
    neko::Filesystem filesystem;
    neko::aer::AerEngine engine(filesystem , &config, neko::aer::ModeEnum::TEST);

	engine.SetWindowAndRenderer(&window, &renderer);

	SimulateSceneLoader simulateSceneLoader(engine);
    engine.RegisterSystem(simulateSceneLoader);

	engine.Init();

	engine.EngineLoop();

	simulateSceneLoader.HasSucceed();
}
