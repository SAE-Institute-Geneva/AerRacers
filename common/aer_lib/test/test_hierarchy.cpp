#include <gtest/gtest.h>

#ifdef NEKO_GLES3
#include <gl/gles3_window.h>
#include <gl/graphics.h>

#include "aer/aer_engine.h"
#include "aer/editor/tool/hierarchy.h"
#include "aer/editor/tool/inspector.h"

namespace neko::aer
{
class SimulateHierarchy : public SystemInterface
{
public:
	SimulateHierarchy(AerEngine& engine) : engine_(engine)
	{
		if (engine_.GetMode() != ModeEnum::GAME)    // If engine mode is not game (to not have tools in game)
		{
			toolManager_ = std::make_unique<EditorToolManager>(engine_);    // Create tool manager
			engine_.RegisterSystem(*toolManager_);                          // Register in system
			engine_.RegisterOnDrawUi(*toolManager_);                        // Register in DrawUI
			engine_.RegisterOnEvent(*toolManager_);                         // Register in Event
			toolManager_->AddEditorTool<Hierarchy>();    // Create tool like it is in editor
			toolManager_->AddEditorTool<Inspector>();    // Create tool like it is in editor
		}
	}

	void Init() override    // Where we create entities
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
		transform3dManager.SetRelativePosition(1, Vec3f(1.0f, 2.0f, 3.0f));
		transform3dManager.AddComponent(2);
		transform3dManager.SetRelativeScale(2, Vec3f(1.0f, 2.0f, 3.0f));
		transform3dManager.AddComponent(0);
		transform3dManager.SetRelativeRotation(0, EulerAngles(1.0f, 2.0f, 3.0f));
		transform3dManager.AddComponent(4);
	}

	void Update(seconds) override    // Where we simulate tests
	{
		updateCount_++;
		if (updateCount_ == kEngineDuration_) { engine_.Stop(); }
	}

	void Destroy() override
	{
	    
	}

	void HasSucceed() const
	{
		logDebug("Test without check");
		//EXPECT_TRUE(testSuccess_);
	}

private:
	std::unique_ptr<EditorToolManager> toolManager_;

	int updateCount_           = 0;
	const int kEngineDuration_ = 50;

	bool testSuccess_ = true;

	AerEngine& engine_;
};

TEST(Editor, TestHierarchy)
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

	SimulateHierarchy simulateHierarchy(engine);
	engine.RegisterSystem(simulateHierarchy);

	engine.Init();

	engine.EngineLoop();

	simulateHierarchy.HasSucceed();
}
}    // namespace neko::aer
#endif
