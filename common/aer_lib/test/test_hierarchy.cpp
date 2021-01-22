#include <gtest/gtest.h>
#include <sdl_engine/sdl_engine.h>
#include <sdl_engine/sdl_input.h>
#include <gl/gles3_window.h>
#include <gl/graphics.h>

#include "aer/aer_engine.h"
#include "aer/editor/tool/logger.h"
#include "aer/log.h"
#include "engine/entity.h"
#include "aer/editor/tool/hierarchy.h"
#include "aer/editor/tool/inspector.h"

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
            toolManager_->AddEditorTool<neko::aer::Hierarchy,
                neko::aer::EditorToolInterface::ToolType::
                    HIERARCHY>();    // Create tool like it is in editor
			toolManager_->AddEditorTool<neko::aer::Inspector, neko::aer::EditorToolInterface::ToolType::INSPECTOR>(); // Create tool like it is in editor
		}
	}

	void Init() override // Where we create entities
	{
        auto& entityManager = engine_.GetComponentManagerContainer().entityManager;
		entityManager.CreateEntity();
		entityManager.CreateEntity();
		entityManager.CreateEntity();
		entityManager.CreateEntity();
		entityManager.CreateEntity();
		entityManager.CreateEntity();
		entityManager.CreateEntity();
		entityManager.CreateEntity();
		entityManager.SetEntityParent(0, 1);
		entityManager.SetEntityParent(1, 2);
		entityManager.SetEntityParent(2, 3);
		entityManager.SetEntityParent(4, 3);
        entityManager.SetEntityParent(5, 6);
        auto& transform3dManager = engine_.GetComponentManagerContainer().transform3dManager;
		transform3dManager.AddComponent(1);
        transform3dManager.SetPosition(1,neko::Vec3f(1.0f, 2.0f, 3.0f));
        transform3dManager.AddComponent(2);
        transform3dManager.SetScale(2, neko::Vec3f(1.0f, 2.0f, 3.0f));
        transform3dManager.AddComponent(0);
        transform3dManager.SetRotation(0, neko::EulerAngles(1.0f, 2.0f, 3.0f));
		transform3dManager.AddComponent(4);
	}

	void Update(neko::seconds dt) override // Where we simulate tests
	{
        //updateCount_++;
        if (updateCount_ == kEngineDuration_) { engine_.Stop(); }
	}

	void Destroy() override
	{
	}

	void HasSucceed() const
	{
	    EXPECT_TRUE(testSuccess_);
	}

private:
	std::unique_ptr<neko::aer::EditorToolManager> toolManager_;

    int updateCount_           = 0;
    const int kEngineDuration_ = 200;

	bool testSuccess_ = true;

	neko::aer::AerEngine& engine_;
};


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
    neko::Filesystem filesystem;
    neko::aer::AerEngine engine(filesystem , &config, neko::aer::ModeEnum::TEST);

	engine.SetWindowAndRenderer(&window, &renderer);

	SimulateHierarchy simulateHierarchy(engine);
	engine.RegisterSystem(simulateHierarchy);

	engine.Init();

	engine.EngineLoop();

	simulateHierarchy.HasSucceed();
}
