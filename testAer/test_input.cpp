
#include <gtest/gtest.h>
#include <sdl_engine/sdl_engine.h>
#include <sdl_engine/sdl_input.h>

#include "gl/gles3_window.h"
#include "gl/graphics.h"

#include "aer_engine.h"
class SimulateInput : public neko::SystemInterface {
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

TEST(Engine, TestSimulateInput)
{
	char* env = getenv("TRAVIS_GUI");
	std::cout << env << std::endl;
	if (env == "false") {
		std::cout << "poisson" << std::endl;
	}

	neko::Configuration config;
	config.windowName = "AerEditor";
	config.windowSize = neko::Vec2u(1400, 900);

	neko::sdl::Gles3Window window;
	neko::gl::Gles3Renderer renderer;
	neko::aer::AerEngine engine(&config);

	engine.SetWindowAndRenderer(&window, &renderer);

	engine.Init();

	SimulateInput simulateInput(engine);
	engine.RegisterSystem(simulateInput);

	engine.EngineLoop();

	simulateInput.HasSucceed();

}

class InteractiveInput : public neko::SystemInterface,
	public neko::sdl::SdlEventSystemInterface,
	public neko::DrawImGuiInterface {
public:
	InteractiveInput(neko::aer::AerEngine& engine) :
		engine_(engine)
	{

	}

	void Init() override
	{
		
	}
	void Update(neko::seconds dt) override
	{
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
        if (count_ == 10) { engine_.Stop(); }
	}
	void Destroy() override
	{

	}
	void OnEvent(const SDL_Event& event) override
	{

	}

	void DrawImGui() override
	{
		ImGui::Begin("Input");

		ImGui::Text("-----------");
		ImGui::Text("KeyBoard");
		for (unsigned int keyIndex = 0;
			keyIndex != static_cast<unsigned>(neko::sdl::KeyCode::KEYBOARD_SIZE);
			keyIndex++)
		{
			if (neko::sdl::InputLocator::get().IsKeyDown(static_cast<neko::sdl::KeyCode>(keyIndex)))
			{
				std::string text = neko::sdl::InputLocator::get().PcInputsEnumToString(
					static_cast<neko::sdl::KeyCode>(keyIndex)) + " IsKeyDown";
				ImGui::Text(text.c_str());
			}
			if (neko::sdl::InputLocator::get().IsKeyHeld(static_cast<neko::sdl::KeyCode>(keyIndex)))
			{
				std::string text = neko::sdl::InputLocator::get().PcInputsEnumToString(
					static_cast<neko::sdl::KeyCode>(keyIndex)) + " IsKeyHeld";
				ImGui::Text(text.c_str());
			}
			if (neko::sdl::InputLocator::get().IsKeyUp(static_cast<neko::sdl::KeyCode>(keyIndex)))
			{
				std::string text = neko::sdl::InputLocator::get().PcInputsEnumToString(
					static_cast<neko::sdl::KeyCode>(keyIndex)) + " IsKeyUp";
				ImGui::Text(text.c_str());
			}
		}
		ImGui::Text("-----------");
		ImGui::Text("Mouse");
		for (unsigned int mouseIndex = 0;
			mouseIndex != static_cast<unsigned>(neko::sdl::MouseButtonCode::LENGTH);
			mouseIndex++)
		{
			if (neko::sdl::InputLocator::get().IsMouseButtonDown(
				static_cast<neko::sdl::MouseButtonCode>(mouseIndex)))
			{
				std::string text = neko::sdl::InputLocator::get().PcInputsEnumToString(
					static_cast<neko::sdl::MouseButtonCode>(mouseIndex)) + " IsMouseButtonDown";
				ImGui::Text(text.c_str());
			}
			if (neko::sdl::InputLocator::get().IsMouseButtonHeld(
				static_cast<neko::sdl::MouseButtonCode>(mouseIndex)))
			{
				std::string text = neko::sdl::InputLocator::get().PcInputsEnumToString(
					static_cast<neko::sdl::MouseButtonCode>(mouseIndex)) + " IsMouseButtonHeld";
				ImGui::Text(text.c_str());
			}
			if (neko::sdl::InputLocator::get().IsMouseButtonUp(static_cast<neko::sdl::MouseButtonCode>(mouseIndex)))
			{
				std::string text = neko::sdl::InputLocator::get().PcInputsEnumToString(
					static_cast<neko::sdl::MouseButtonCode>(mouseIndex)) + " IsMouseButtonUp";
				ImGui::Text(text.c_str());
			}
		}
		std::string mouseText = "Mouse Position : " + std::
			to_string((neko::sdl::InputLocator::get().GetMousePosition().x)) + ", " + std::to_string(
				(neko::sdl::InputLocator::get().GetMousePosition().y)) + ")";
		ImGui::Text(mouseText.c_str());
		mouseText = "Mouse Relative Position : " + std::
			to_string((neko::sdl::InputLocator::get().GetRelativeMousePosition().x)) + ", " + std::to_string(
				(neko::sdl::InputLocator::get().GetRelativeMousePosition().y)) + ")";
		ImGui::Text(mouseText.c_str());
		mouseText = "Mouse Scroll : " + std::to_string((neko::sdl::InputLocator::get().GetMouseScroll().x)) +
			", " + std::to_string((neko::sdl::InputLocator::get().GetMouseScroll().y)) + ")";
		ImGui::Text(mouseText.c_str());
		ImGui::Text("-----------");
		ImGui::Text("Controller");
		for (unsigned int controllerIndex = 0;
			controllerIndex != static_cast<unsigned>(neko::sdl::ControllerInputs::LENGTH);
			controllerIndex++)
		{
			if (neko::sdl::InputLocator::get().IsControllerDown(
				static_cast<neko::sdl::ControllerInputs>(controllerIndex)))
			{
				std::string text = neko::sdl::InputLocator::get().ControllerInputsEnumToString(
					static_cast<neko::sdl::ControllerInputs>(controllerIndex)) + " IsControllerDown";
				ImGui::Text(text.c_str());
			}
			if (neko::sdl::InputLocator::get().IsControllerHeld(
				static_cast<neko::sdl::ControllerInputs>(controllerIndex)))
			{
				std::string text = neko::sdl::InputLocator::get().ControllerInputsEnumToString(
					static_cast<neko::sdl::ControllerInputs>(controllerIndex)) + " IsControllerHeld";
				ImGui::Text(text.c_str());
			}
			if (neko::sdl::InputLocator::get().IsControllerUp(
				static_cast<neko::sdl::ControllerInputs>(controllerIndex)))
			{
				std::string text = neko::sdl::InputLocator::get().ControllerInputsEnumToString(
					static_cast<neko::sdl::ControllerInputs>(controllerIndex)) + " IsControllerUp";
				ImGui::Text(text.c_str());
			}
		}
		for (unsigned int controllerAxisIndex = 0;
			controllerAxisIndex != static_cast<unsigned>(neko::sdl::ControllerAxis::LENGTH);
			controllerAxisIndex++)
		{
			std::string text = neko::sdl::InputLocator::get().
				ControllerAxisEnumToString(static_cast<neko::sdl::ControllerAxis>(controllerAxisIndex)) +
				" : " + std::to_string(
					neko::sdl::InputLocator::get().GetJoystickAxis(
						static_cast<neko::sdl::ControllerAxis>(controllerAxisIndex)));
			ImGui::Text(text.c_str());
		}
		ImGui::Text("-----------");
		ImGui::Text("InputAction");
		for (unsigned int inputIndex = 0;
			inputIndex != static_cast<unsigned>(neko::sdl::InputAction::LENGTH);
			inputIndex++)
		{
			if (neko::sdl::InputLocator::get().IsActionDown(static_cast<neko::sdl::InputAction>(inputIndex)))
			{
				std::string text = neko::sdl::InputLocator::get().ActionEnumToString(
					static_cast<neko::sdl::InputAction>(inputIndex)) + " IsActionDown";
				ImGui::Text(text.c_str());
			}
			if (neko::sdl::InputLocator::get().IsActionHeld(static_cast<neko::sdl::InputAction>(inputIndex)))
			{
				std::string text = neko::sdl::InputLocator::get().ActionEnumToString(
					static_cast<neko::sdl::InputAction>(inputIndex)) + " IsActionHeld";
				ImGui::Text(text.c_str());
			}
			if (neko::sdl::InputLocator::get().IsActionUp(static_cast<neko::sdl::InputAction>(inputIndex)))
			{
				std::string text = neko::sdl::InputLocator::get().ActionEnumToString(
					static_cast<neko::sdl::InputAction>(inputIndex)) + " IsActionUp";
				ImGui::Text(text.c_str());
			}
		}
		ImGui::End();

		ImGui::Begin("TestInput");
		if (keyDown_) {
			ImGui::Text("Key A has been pressed down");
		}
		else {
			ImGui::Text("Press down key A");
		}
		if (keyUp_) {
			ImGui::Text("Key B has been released up");
		}
		else {
			ImGui::Text("Release up key B");
		}
		if (mouseDown_) {
			ImGui::Text("Mouse Left has been pressed down");
		}
		else {
			ImGui::Text("Press down mouse Left");
		}
		if (mouseUp_) {
			ImGui::Text("Mouse right has been released up");
		}
		else {
			ImGui::Text("Release up mouse Right");
		}
		if (controllerDown_) {
			ImGui::Text("Controller button A has been pressed down");
		}
		else {
			ImGui::Text("Press down controller button A");
		}
		if (controllerUp_) {
			ImGui::Text("Controller button B has been released up");
		}
		else {
			ImGui::Text("Release up controller button B");
		}
		if (actionDown_) {
			ImGui::Text("Action forward has been pressed down");
		}
		else {
			ImGui::Text("Press down action forward");
		}
		if (actionUp_) {
			ImGui::Text("Action backward has been released up");
		}
		else {
			ImGui::Text("Release up action backward");
		}
		ImGui::End();


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

TEST(Engine, TestInteractiveInput)
{
	char* env = getenv("TRAVIS_GUI");
	std::cout << env << std::endl;
	if (env == "false") {
		std::cout << "poisson" << std::endl;
	}
	
	neko::Configuration config;
	config.windowName = "AerEditor";
	config.windowSize = neko::Vec2u(1400, 900);

	neko::sdl::Gles3Window window;
	neko::gl::Gles3Renderer renderer;
	neko::aer::AerEngine engine(&config);

	engine.SetWindowAndRenderer(&window, &renderer);

	engine.Init();

	InteractiveInput interactiveInput(engine);
	engine.RegisterSystem(interactiveInput);
	engine.RegisterOnDrawUi(interactiveInput);
	engine.RegisterOnEvent(interactiveInput);

	engine.EngineLoop();

	//interactiveInput.HasSucceed();

}
