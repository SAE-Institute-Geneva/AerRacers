
#include <gtest/gtest.h>
#include <sdl_engine/sdl_engine.h>
#include <sdl_engine/sdl_input.h>

#include "gl/gles3_window.h"
#include "gl/graphics.h"

#include "aer_engine.h"

class SimulateInput : public neko::SystemInterface,
                      public neko::sdl::SdlEventSystemInterface {
public:
	SimulateInput(neko::aer::AerEngine& engine) :
		engine_(engine)
	{

	}

    void Init() override
	{

	}
    void Update(neko::seconds dt) override
    {
		neko::sdl::InputLocator::get().SimulateKeyDown(neko::sdl::KeyCode::A);
		neko::sdl::InputLocator::get().SimulateKeyUp(neko::sdl::KeyCode::B);
		neko::sdl::InputLocator::get().SimulateMouseDown(neko::sdl::MouseButtonCode::LEFT);
		neko::sdl::InputLocator::get().SimulateMouseUp(neko::sdl::MouseButtonCode::RIGHT);
		neko::sdl::InputLocator::get().SimulateControllerDown(neko::sdl::ControllerInputs::BUTTON_A);
		neko::sdl::InputLocator::get().SimulateControllerUp(neko::sdl::ControllerInputs::BUTTON_B);
		neko::sdl::InputLocator::get().SimulateActionDown(neko::sdl::InputAction::FORWARD);
		neko::sdl::InputLocator::get().SimulateActionUp(neko::sdl::InputAction::BACKWARD);

		if (neko::sdl::InputLocator::get().IsKeyDown(neko::sdl::KeyCode::A)) {
			keyDown_ = true;
		}
		if (neko::sdl::InputLocator::get().IsKeyUp(neko::sdl::KeyCode::B)) {
			keyUp_ = true;
		}
		if (neko::sdl::InputLocator::get().IsMouseButtonDown(neko::sdl::MouseButtonCode::LEFT)) {
			mouseDown_ = true;
		}
		if (neko::sdl::InputLocator::get().IsMouseButtonUp(neko::sdl::MouseButtonCode::RIGHT)) {
			mouseUp_ = true;
		}
		if (neko::sdl::InputLocator::get().IsControllerDown(neko::sdl::ControllerInputs::BUTTON_A)) {
			controllerDown_ = true;
		}
		if (neko::sdl::InputLocator::get().IsControllerUp(neko::sdl::ControllerInputs::BUTTON_B)) {
			controllerUp_ = true;
		}
		if (neko::sdl::InputLocator::get().IsActionDown(neko::sdl::InputAction::FORWARD)) {
			actionDown_ = true;
		}
		if (neko::sdl::InputLocator::get().IsActionUp(neko::sdl::InputAction::BACKWARD)) {
			actionUp_ = true;
		}


		count_++;
		if (count_ == 10) {
			engine_.Stop();
		}
    }
    void Destroy() override
	{

	}
    void OnEvent(const SDL_Event& event) override
	{

	}

	void HasSucceed() const
	{
		EXPECT_TRUE(keyDown_);
		EXPECT_TRUE(keyUp_);
		EXPECT_TRUE(mouseDown_);
		EXPECT_TRUE(mouseUp_);
		EXPECT_TRUE(controllerDown_);
		EXPECT_TRUE(controllerUp_);
		EXPECT_TRUE(actionDown_);
		EXPECT_TRUE(actionUp_);
	}
private:

	bool keyDown_ = false;
	bool keyUp_ = false;
	bool mouseDown_ = false;
	bool mouseUp_ = false;
	bool controllerDown_ = false;
	bool controllerUp_ = false;
	bool actionDown_ = false;
	bool actionUp_ = false;

	int count_ = 0;

	neko::aer::AerEngine& engine_;
};


TEST(Engine, TestInput)
{
    neko::Configuration config;
    config.windowName = "AerEditor";
    config.windowSize = neko::Vec2u(1400, 900);

	neko::sdl::Gles3Window window;
	neko::gl::Gles3Renderer renderer;
	neko::aer::AerEngine engine(&config);

	engine.SetWindowAndRenderer(&window, &renderer);

	engine.Init();

	// -------------------------------- TEST ----------------------------------
	SimulateInput simulateInput(engine);
	engine.RegisterSystem(simulateInput);
	// ------------------------------------------------------------------------

	engine.EngineLoop();

	simulateInput.HasSucceed();

}