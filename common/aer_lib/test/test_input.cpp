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

 Author : Floreau Luca
 Co-Author :
 Date : 23.10.2020
---------------------------------------------------------- */
#include <gtest/gtest.h>

#ifdef NEKO_GLES3
	#include "gl/gles3_window.h"
	#include "gl/graphics.h"
	#include "sdl_engine/sdl_engine.h"
	#include "sdl_engine/sdl_input.h"

	#include "aer/aer_engine.h"
	#include "aer/managers/inputs_binding_manager.h"

namespace neko::aer
{
class SimulateInput : public SystemInterface
{
public:
	SimulateInput(AerEngine& engine) : engine_(engine) {}

	void Init() override {}

	void Update(seconds dt) override
	{
		auto& inputLocator = sdl::InputLocator::get();
		SimulateKeyDown(sdl::KeyCodeType::A);
		SimulateKeyUp(sdl::KeyCodeType::B);
		auto controllerIdVector = inputLocator.GetControllerIdVector();
		for (size_t controllerIndex = 0; controllerIndex < controllerIdVector.size();
			 controllerIndex++)
		{
			const sdl::ControllerId controllerId = controllerIdVector[controllerIndex];
			SimulateControllerDown(controllerId, sdl::ControllerButtonType::BUTTON_A);
			SimulateControllerUp(controllerId, sdl::ControllerButtonType::BUTTON_B);
		}

		if (inputLocator.GetKeyState(sdl::KeyCodeType::A) == sdl::ButtonState::DOWN)
		{
			keyDown_ = true;
		}

		if (inputLocator.GetKeyState(sdl::KeyCodeType::B) == sdl::ButtonState::UP)
		{
			keyUp_ = true;
		}

		for (size_t controllerIndex = 0; controllerIndex < controllerIdVector.size();
			 controllerIndex++)
		{
			const sdl::ControllerId controllerId = controllerIdVector[controllerIndex];
			if (inputLocator.GetControllerButtonState(
					controllerId, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
			{
				controllerDown_ = true;
			}
			if (inputLocator.GetControllerButtonState(
					controllerId, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::UP)
			{
				controllerUp_ = true;
			}
		}

		updateCount_ += dt.count();
		if (updateCount_ > kEngineDuration_) { engine_.Stop(); }
	}

	void Destroy() override {}

	void HasSucceed() const
	{
		// TODO(@Luca) Fix empty controller
		EXPECT_TRUE(keyDown_);
		EXPECT_TRUE(keyUp_);
		//EXPECT_TRUE(controllerDown_);
		//EXPECT_TRUE(controllerUp_);
	}

	void SimulateKeyDown(const sdl::KeyCodeType key)
	{
		SDL_Event sdlEvent           = {};
		sdlEvent.type                = SDL_KEYDOWN;
		sdlEvent.key.keysym.scancode = static_cast<SDL_Scancode>(key);
		if (SDL_PushEvent(&sdlEvent) < 1)
		{
			logDebug("SDL_Init failed: " + std::string(SDL_GetError()));
		}
	}

	void SimulateKeyUp(const sdl::KeyCodeType key)
	{
		SDL_Event sdlEvent           = {};
		sdlEvent.type                = SDL_KEYUP;
		sdlEvent.key.keysym.scancode = static_cast<SDL_Scancode>(key);
		if (SDL_PushEvent(&sdlEvent) < 1)
		{
			logDebug("SDL_Init failed: " + std::string(SDL_GetError()));
		}
	}

	void SimulateControllerDown(unsigned controllerId, const sdl::ControllerButtonType key)
	{
		SDL_Event sdlEvent      = {};
		sdlEvent.type           = SDL_JOYBUTTONDOWN;
		sdlEvent.jbutton.button = static_cast<Uint8>(key);
		sdlEvent.jbutton.which  = static_cast<SDL_JoystickID>(controllerId);
		if (SDL_PushEvent(&sdlEvent) < 1)
		{
			logDebug("SDL_Init failed: " + std::string(SDL_GetError()));
		}
	}

	void SimulateControllerUp(unsigned controllerId, const sdl::ControllerButtonType key)
	{
		SDL_Event sdlEvent      = {};
		sdlEvent.type           = SDL_JOYBUTTONUP;
		sdlEvent.jbutton.button = static_cast<Uint8>(key);
		sdlEvent.jbutton.which  = static_cast<SDL_JoystickID>(controllerId);
		if (SDL_PushEvent(&sdlEvent) < 1)
		{
			logDebug("SDL_Init failed: " + std::string(SDL_GetError()));
		}
	}

private:
	bool keyDown_        = false;
	bool keyUp_          = false;
	bool controllerDown_ = false;
	bool controllerUp_   = false;

	float updateCount_           = 0;
    const float kEngineDuration_ = 0.5f;

	AerEngine& engine_;
};

TEST(Input, TestSimulateInput)
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

	SimulateInput simulateInput(engine);
	engine.RegisterSystem(simulateInput);

	engine.Init();

	engine.EngineLoop();

	simulateInput.HasSucceed();
}

//class SimulateBindedInput : public SystemInterface
//{
//public:
//	SimulateBindedInput(AerEngine& engine) : engine_(engine) {}
//
//	void Init() override
//	{
//		BindingInputs bindingInputs;
//		auto& inputManager      = sdl::InputLocator::get();
//		bindingInputs.playerId  = 0;
//		auto controllerIdVector = inputManager.GetControllerIdVector();
//		if (!controllerIdVector.empty())
//		{
//			bindingInputs.bindedControllerId = controllerIdVector[0];
//		}
//		bindingInputs.controllerBindingButtons[static_cast<size_t>(ActionButtonType::MAIN_SHOOT)] =
//			sdl::ControllerButtonType::LEFT_TRIGGER;
//		bindingInputs.controllerBindingButtons[static_cast<size_t>(ActionButtonType::LEFT)] =
//			sdl::ControllerButtonType::BUTTON_X;
//		bindingInputs.controllerBindingButtons[static_cast<size_t>(ActionButtonType::RIGHT)] =
//			sdl::ControllerButtonType::BUTTON_B;
//		bindingInputs.controllerBindingButtons[static_cast<size_t>(ActionButtonType::FORWARD)] =
//			sdl::ControllerButtonType::BUTTON_Y;
//		bindingInputs.controllerBindingButtons[static_cast<size_t>(ActionButtonType::BACKWARD)] =
//			sdl::ControllerButtonType::BUTTON_A;
//		bindingInputs.pcBindingButtons[static_cast<size_t>(ActionButtonType::MAIN_SHOOT)] =
//			sdl::KeyCodeType::SPACE;
//		bindingInputs.pcBindingButtons[static_cast<size_t>(ActionButtonType::LEFT)] =
//			sdl::KeyCodeType::LEFT;
//		bindingInputs.pcBindingButtons[static_cast<size_t>(ActionButtonType::RIGHT)] =
//			sdl::KeyCodeType::RIGHT;
//		bindingInputs.pcBindingButtons[static_cast<size_t>(ActionButtonType::FORWARD)] =
//			sdl::KeyCodeType::UP;
//		bindingInputs.pcBindingButtons[static_cast<size_t>(ActionButtonType::BACKWARD)] =
//			sdl::KeyCodeType::DOWN;
//		auto& bindedInputManager = BindedInputLocator::get();
//		bindedInputManager.SetPlayerActions(bindingInputs);
//	}
//
//	void Update(seconds dt) override
//	{
//		auto& bindedInputManager = BindedInputLocator::get();
//		auto& inputLocator       = sdl::InputLocator::get();
//		auto controllerIdVector  = inputLocator.GetControllerIdVector();
//		if (!controllerIdVector.empty())
//		{
//			auto bindingInputs               = bindedInputManager.GetPlayerActions(0);
//			bindingInputs.bindedControllerId = controllerIdVector[0];
//			bindedInputManager.SetPlayerActions(bindingInputs);
//		}
//		SimulateKeyDown(sdl::KeyCodeType::SPACE);
//		SimulateKeyUp(sdl::KeyCodeType::UP);
//		for (int controllerIndex = 0; controllerIndex < controllerIdVector.size();
//			 controllerIndex++)
//		{
//			const sdl::ControllerId controllerId = controllerIdVector[controllerIndex];
//			SimulateControllerDown(controllerId, sdl::ControllerButtonType::BUTTON_A);
//			SimulateControllerUp(controllerId, sdl::ControllerButtonType::BUTTON_B);
//			SimulateControllerDown(controllerId, sdl::ControllerButtonType::BUTTON_X);
//		}
//
//		if (bindedInputManager.GetActionButtonState(0, ActionButtonType::BACKWARD) ==
//			sdl::ButtonState::DOWN)
//		{
//			controllerBackwardDown_ = true;
//		}
//		if (bindedInputManager.GetActionButtonState(0, ActionButtonType::RIGHT) ==
//			sdl::ButtonState::UP)
//		{
//			controllerRightUp_ = true;
//		}
//		if (bindedInputManager.GetActionButtonState(0, ActionButtonType::LEFT) ==
//			sdl::ButtonState::DOWN)
//		{
//			controllerLeftDown_ = true;
//		}
//		if (bindedInputManager.GetActionButtonState(0, ActionButtonType::MAIN_SHOOT) ==
//			sdl::ButtonState::DOWN)
//		{
//			keyboardShootDown_ = true;
//		}
//		if (bindedInputManager.GetActionButtonState(0, ActionButtonType::FORWARD) ==
//			sdl::ButtonState::UP)
//		{
//			keyboardForwardUp_ = true;
//		}
//
//		updateCount_ += dt.count();
//		if (updateCount_ > kEngineDuration_) { engine_.Stop(); }
//	}
//
//	void Destroy() override {}
//
//	void HasSucceed() const
//	{
//		// TODO(@Luca) Fix empty controller
//		//EXPECT_TRUE(controllerBackwardDown_);
//		//EXPECT_TRUE(controllerRightUp_);
//		//EXPECT_TRUE(controllerLeftDown_);
//		EXPECT_TRUE(keyboardShootDown_);
//		EXPECT_TRUE(keyboardForwardUp_);
//	}
//
//	void SimulateKeyDown(const sdl::KeyCodeType key)
//	{
//		SDL_Event sdlEvent           = {};
//		sdlEvent.type                = SDL_KEYDOWN;
//		sdlEvent.key.keysym.scancode = static_cast<SDL_Scancode>(key);
//		if (SDL_PushEvent(&sdlEvent) < 1)
//		{
//			logDebug("SDL_Init failed: " + std::string(SDL_GetError()));
//		}
//	}
//
//	void SimulateKeyUp(const sdl::KeyCodeType key)
//	{
//		SDL_Event sdlEvent           = {};
//		sdlEvent.type                = SDL_KEYUP;
//		sdlEvent.key.keysym.scancode = static_cast<SDL_Scancode>(key);
//		if (SDL_PushEvent(&sdlEvent) < 1)
//		{
//			logDebug("SDL_Init failed: " + std::string(SDL_GetError()));
//		}
//	}
//
//	void SimulateControllerDown(unsigned controllerId, const sdl::ControllerButtonType key)
//	{
//		SDL_Event sdlEvent      = {};
//		sdlEvent.type           = SDL_JOYBUTTONDOWN;
//		sdlEvent.jbutton.button = static_cast<Uint8>(key);
//		sdlEvent.jbutton.which  = static_cast<SDL_JoystickID>(controllerId);
//		if (SDL_PushEvent(&sdlEvent) < 1)
//		{
//			logDebug("SDL_Init failed: " + std::string(SDL_GetError()));
//		}
//	}
//
//	void SimulateControllerUp(unsigned controllerId, const sdl::ControllerButtonType key)
//	{
//		SDL_Event sdlEvent      = {};
//		sdlEvent.type           = SDL_JOYBUTTONUP;
//		sdlEvent.jbutton.button = static_cast<Uint8>(key);
//		sdlEvent.jbutton.which  = static_cast<SDL_JoystickID>(controllerId);
//		if (SDL_PushEvent(&sdlEvent) < 1)
//		{
//			logDebug("SDL_Init failed: " + std::string(SDL_GetError()));
//		}
//	}
//
//private:
//	bool controllerBackwardDown_ = false;
//	bool controllerRightUp_      = false;
//	bool controllerLeftDown_     = false;
//	bool keyboardShootDown_      = false;
//	bool keyboardForwardUp_      = false;
//
//	float updateCount_           = 0;
//	const float kEngineDuration_ = 0.5f;
//
//	AerEngine& engine_;
//};
//
//TEST(Input, TestSimulateBindedInput)
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
//	AerEngine engine(filesystem, &config, ModeEnum::TEST);
//
//	engine.SetWindowAndRenderer(&window, &renderer);
//
//	InputBindingManager bindedInputManager;
//	SimulateBindedInput simulateInput(engine);
//	engine.RegisterSystem(simulateInput);
//
//	engine.Init();
//
//	engine.EngineLoop();
//
//	simulateInput.HasSucceed();
//}

class InteractiveInput : public SystemInterface,
						 public sdl::SdlEventSystemInterface,
						 public DrawImGuiInterface
{
public:
	InteractiveInput(AerEngine& engine, bool interactive)
	   : engine_(engine), interactive_(interactive)
	{}

	void Init() override {}

	void Update(seconds dt) override
	{
		auto& inputLocator = sdl::InputLocator::get();
		if (inputLocator.GetKeyState(sdl::KeyCodeType::A) == sdl::ButtonState::DOWN)
		{
			keyDown_ = true;
		}
		if (inputLocator.GetKeyState(sdl::KeyCodeType::B) == sdl::ButtonState::UP)
		{
			keyUp_ = true;
		}
		auto controllerIdVector = inputLocator.GetControllerIdVector();
		for (int controllerIndex = 0; controllerIndex < controllerIdVector.size();
			 controllerIndex++)
		{
			const sdl::ControllerId controllerId = controllerIdVector[controllerIndex];
			if (inputLocator.GetControllerButtonState(
					controllerId, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::DOWN)
			{
				controllerDown_ = true;
			}
			if (inputLocator.GetControllerButtonState(
					controllerId, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::UP)
			{
				controllerUp_ = true;
			}
		}

		updateCount_ += dt.count();
		if (updateCount_ > kEngineDuration_ && !interactive_) { engine_.Stop(); }
	}

	void Destroy() override {}
	void OnEvent(const SDL_Event& event) override {}

	void DrawImGui() override
	{
		auto& inputLocator = sdl::InputLocator::get();
		ImGui::Begin("Input");
		ImGui::Text("-----------");
		if (ImGui::CollapsingHeader("KeyBoard", ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (size_t keyIndex = 0;
				 keyIndex != static_cast<size_t>(sdl::KeyCodeType::KEYBOARD_SIZE);
				 keyIndex++)
			{
				sdl::ButtonState buttonState =
					inputLocator.GetKeyState(static_cast<sdl::KeyCodeType>(keyIndex));
				std::string buttonStateText;
				switch (buttonState)
				{
					case sdl::ButtonState::NONE: continue; break;
					case sdl::ButtonState::DOWN: buttonStateText = " IsKeyDown"; break;
					case sdl::ButtonState::HELD: buttonStateText = " IsKeyHeld"; break;
					case sdl::ButtonState::UP: buttonStateText = " IsKeyUp"; break;
					default: continue;
				}
				std::string text =
					inputLocator.PcInputsEnumToString(static_cast<sdl::KeyCodeType>(keyIndex)) +
					buttonStateText;
				ImGui::Text(text.c_str());
			}
		}
		ImGui::Text("-----------");
		auto controllerIdVector = inputLocator.GetControllerIdVector();
		for (int controllerIndex = 0; controllerIndex < controllerIdVector.size();
			 controllerIndex++)
		{
			const sdl::ControllerId controllerId = controllerIdVector[controllerIndex];
			std::string controllerTitle          = "Controller " + std::to_string(controllerId);
			if (ImGui::CollapsingHeader(controllerTitle.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::Text("Controller Input");
				for (size_t controllerIndex = 0;
					 controllerIndex != static_cast<size_t>(sdl::ControllerButtonType::LENGTH);
					 controllerIndex++)
				{
					sdl::ButtonState buttonState = inputLocator.GetControllerButtonState(
						controllerId, static_cast<sdl::ControllerButtonType>(controllerIndex));
					std::string buttonStateText;
					switch (buttonState)
					{
						case sdl::ButtonState::NONE: continue; break;
						case sdl::ButtonState::DOWN: buttonStateText = " Is Down"; break;
						case sdl::ButtonState::HELD: buttonStateText = " Is Held"; break;
						case sdl::ButtonState::UP: buttonStateText = " Is Up"; break;
						default: continue;
					}
					std::string text =
						inputLocator.ControllerInputsEnumToString(
							static_cast<sdl::ControllerButtonType>(controllerIndex)) +
						buttonStateText;
					ImGui::Text(text.c_str());
				}
				ImGui::Text("Controller Axis");
				for (size_t controllerAxisIndex = 0;
					 controllerAxisIndex != static_cast<size_t>(sdl::ControllerAxisType::LENGTH);
					 controllerAxisIndex++)
				{
					std::string text =
						inputLocator.ControllerInputsEnumToString(
							static_cast<sdl::ControllerAxisType>(controllerAxisIndex)) +
						" : " +
						std::to_string(inputLocator.GetControllerAxis(controllerId,
							static_cast<sdl::ControllerAxisType>(controllerAxisIndex)));
					ImGui::Text(text.c_str());
				}
			}
		} /*
        ImGui::Text("-----------");
        for (unsigned playerId = 0; playerId < 4; playerId++) {
            std::string playerTitle = "InputAction Player " + std::to_string(
                playerId);
            if (ImGui::CollapsingHeader(
                playerTitle.c_str(),
                ImGuiTreeNodeFlags_DefaultOpen)) {
                for (unsigned int inputIndex = 0;
                    inputIndex != static_cast<unsigned>(
                        sdl::ActionButton::LENGTH)
                    ;
                    inputIndex++) {
                    if (inputLocator.IsActionDown(
                        playerId,
                        static_cast<sdl::ActionButton>(inputIndex))) {
                        std::string text =
                            inputLocator.ActionEnumToString(
                                static_cast<sdl::ActionButton>(inputIndex)) +
                            " IsActionDown";
                        ImGui::Text(text.c_str());
                    }
                    if (inputLocator.IsActionHeld(
                        playerId,
                        static_cast<sdl::ActionButton>(inputIndex))) {
                        std::string text =
                            inputLocator.ActionEnumToString(
                                static_cast<sdl::ActionButton>(inputIndex)) +
                            " IsActionHeld";
                        ImGui::Text(text.c_str());
                    }
                    if (inputLocator.IsActionUp(
                        playerId,
                        static_cast<sdl::ActionButton>(inputIndex))) {
                        std::string text =
                            inputLocator.ActionEnumToString(
                                static_cast<sdl::ActionButton>(inputIndex)) +
                            " IsActionUp";
                        ImGui::Text(text.c_str());
                    }
                }
            }
        }*/
		ImGui::End();

		ImGui::Begin("TestInput");
		if (keyDown_) { ImGui::Text("Key A has been pressed down"); }
		else
		{
			ImGui::Text("Press down key A");
		}
		if (keyUp_) { ImGui::Text("Key B has been released up"); }
		else
		{
			ImGui::Text("Release up key B");
		}
		if (controllerDown_) { ImGui::Text("Controller button A has been pressed down"); }
		else
		{
			ImGui::Text("Press down controller button A");
		}
		if (controllerUp_) { ImGui::Text("Controller button B has been released up"); }
		else
		{
			ImGui::Text("Release up controller button B");
		}
		ImGui::End();
	}

	void HasSucceed() const
	{
		EXPECT_TRUE(keyDown_);
		EXPECT_TRUE(keyUp_);
		EXPECT_TRUE(controllerDown_);
		EXPECT_TRUE(controllerUp_);
	}

private:
	bool keyDown_        = false;
	bool keyUp_          = false;
	bool controllerDown_ = false;
	bool controllerUp_   = false;
	bool actionDown_     = false;
	bool actionUp_       = false;

	float updateCount_           = 0;
    const float kEngineDuration_ = 1.0f;
    bool interactive_            = false;

	AerEngine& engine_;
};

//class InteractiveBindingInput : public SystemInterface,
//								public sdl::SdlEventSystemInterface,
//								public DrawImGuiInterface
//{
//public:
//	InteractiveBindingInput(AerEngine& engine, bool interactive)
//	   : engine_(engine), interactive_(interactive)
//	{}
//
//	void Init() override
//	{
//		BindingInputs bindingInputs;
//		auto& inputManager      = sdl::InputLocator::get();
//		bindingInputs.playerId  = 0;
//		auto controllerIdVector = inputManager.GetControllerIdVector();
//		if (!controllerIdVector.empty())
//		{
//			bindingInputs.bindedControllerId = controllerIdVector[0];
//		}
//
//		bindingInputs.controllerBindingButtons[static_cast<size_t>(ActionButtonType::MAIN_SHOOT)] =
//			sdl::ControllerButtonType::LEFT_TRIGGER;
//		bindingInputs.controllerBindingButtons[static_cast<size_t>(ActionButtonType::LEFT)] =
//			sdl::ControllerButtonType::BUTTON_X;
//		bindingInputs.controllerBindingButtons[static_cast<size_t>(ActionButtonType::RIGHT)] =
//			sdl::ControllerButtonType::BUTTON_B;
//		bindingInputs.controllerBindingButtons[static_cast<size_t>(ActionButtonType::FORWARD)] =
//			sdl::ControllerButtonType::BUTTON_Y;
//		bindingInputs.controllerBindingButtons[static_cast<size_t>(ActionButtonType::BACKWARD)] =
//			sdl::ControllerButtonType::BUTTON_A;
//		bindingInputs.pcBindingButtons[static_cast<size_t>(ActionButtonType::MAIN_SHOOT)] =
//			sdl::KeyCodeType::SPACE;
//		bindingInputs.pcBindingButtons[static_cast<size_t>(ActionButtonType::LEFT)] =
//			sdl::KeyCodeType::LEFT;
//		bindingInputs.pcBindingButtons[static_cast<size_t>(ActionButtonType::RIGHT)] =
//			sdl::KeyCodeType::RIGHT;
//		bindingInputs.pcBindingButtons[static_cast<size_t>(ActionButtonType::FORWARD)] =
//			sdl::KeyCodeType::UP;
//		bindingInputs.pcBindingButtons[static_cast<size_t>(ActionButtonType::BACKWARD)] =
//			sdl::KeyCodeType::DOWN;
//		auto& bindedInputManager = BindedInputLocator::get();
//		bindedInputManager.SetPlayerActions(bindingInputs);
//	}
//
//	void Update(seconds dt) override
//	{
//		auto& bindedInputManager = BindedInputLocator::get();
//		auto& inputManager       = sdl::InputLocator::get();
//		auto controllerIdVector  = inputManager.GetControllerIdVector();
//		if (!controllerIdVector.empty())
//		{
//			auto bindingInputs               = bindedInputManager.GetPlayerActions(0);
//			bindingInputs.bindedControllerId = controllerIdVector[0];
//			bindedInputManager.SetPlayerActions(bindingInputs);
//		}
//
//		if (bindedInputManager.GetActionButtonState(0, ActionButtonType::MAIN_SHOOT) ==
//			sdl::ButtonState::DOWN)
//		{
//			actionDown_ = true;
//		}
//
//		if (bindedInputManager.GetActionButtonState(0, ActionButtonType::BACKWARD) ==
//			sdl::ButtonState::UP)
//		{
//			actionUp_ = true;
//		}
//
//		updateCount_ += dt.count();
//		if (updateCount_ > kEngineDuration_ && !interactive_) { engine_.Stop(); }
//	}
//
//	void Destroy() override {}
//	void OnEvent(const SDL_Event&) override {}
//
//	void DrawImGui() override
//	{
//		auto& bindedInputManager = BindedInputLocator::get();
//		auto& inputManager       = sdl::InputLocator::get();
//		ImGui::Begin("ActionInput");
//		ImGui::Text("-----------");
//		if (ImGui::CollapsingHeader("ActionBinded", ImGuiTreeNodeFlags_DefaultOpen))
//		{
//			auto bindingInputs = bindedInputManager.GetPlayerActions(0);
//			if (ImGui::CollapsingHeader("Controller", ImGuiTreeNodeFlags_DefaultOpen))
//			{
//				std::string text =
//					"ControllerID bind : " + std::to_string(bindingInputs.bindedControllerId);
//				ImGui::Text("%s", text.c_str());
//				for (size_t actionIndex = 0;
//					 actionIndex != static_cast<size_t>(ActionButtonType::LENGTH);
//					 actionIndex++)
//				{
//					text = bindedInputManager.ActionEnumToString(
//							   static_cast<ActionButtonType>(actionIndex)) +
//					       " bind with " +
//					       inputManager.ControllerInputsEnumToString(
//							   bindingInputs.controllerBindingButtons[actionIndex]);
//					ImGui::Text("%s", text.c_str());
//				}
//			}
//
//			if (ImGui::CollapsingHeader("KeyBoard", ImGuiTreeNodeFlags_DefaultOpen))
//			{
//				for (size_t actionIndex = 0;
//					 actionIndex != static_cast<size_t>(ActionButtonType::LENGTH);
//					 actionIndex++)
//				{
//					std::string text = bindedInputManager.ActionEnumToString(
//										   static_cast<ActionButtonType>(actionIndex)) +
//					                   " bind with " +
//					                   inputManager.PcInputsEnumToString(
//										   bindingInputs.pcBindingButtons[actionIndex]);
//					ImGui::Text("%s", text.c_str());
//				}
//			}
//
//			if (ImGui::CollapsingHeader("Switch", ImGuiTreeNodeFlags_DefaultOpen))
//			{
//				std::string text =
//					"SwitchID bind : " + std::to_string(bindingInputs.bindedSwitchJoyId);
//				ImGui::Text("%s", text.c_str());
//				for (size_t actionIndex = 0;
//					 actionIndex != static_cast<size_t>(ActionButtonType::LENGTH);
//					 actionIndex++)
//				{
//					text = bindedInputManager.ActionEnumToString(
//							   static_cast<ActionButtonType>(actionIndex)) +
//					       " bind with " +
//					       inputManager.SwitchInputsEnumToString(
//							   bindingInputs.switchBindingButtons[actionIndex]);
//					ImGui::Text("%s", text.c_str());
//				}
//			}
//		}
//
//		if (ImGui::CollapsingHeader("ActionPress", ImGuiTreeNodeFlags_DefaultOpen))
//		{
//			for (size_t actionIndex = 0;
//				 actionIndex != static_cast<size_t>(ActionButtonType::LENGTH);
//				 actionIndex++)
//			{
//				sdl::ButtonState buttonState = bindedInputManager.GetActionButtonState(
//					0, static_cast<ActionButtonType>(actionIndex));
//				std::string buttonStateText;
//				switch (buttonState)
//				{
//					case sdl::ButtonState::NONE: continue; break;
//					case sdl::ButtonState::DOWN: buttonStateText = " IsActionDown"; break;
//					case sdl::ButtonState::HELD: buttonStateText = " IsActionHeld"; break;
//					case sdl::ButtonState::UP: buttonStateText = " IsActionUp"; break;
//					default: continue;
//				}
//				std::string text = bindedInputManager.ActionEnumToString(
//									   static_cast<ActionButtonType>(actionIndex)) +
//				                   buttonStateText;
//				ImGui::Text("%s", text.c_str());
//			}
//		}
//		ImGui::End();
//	}
//
//	void HasSucceed() const
//	{
//		EXPECT_TRUE(actionDown_);
//		EXPECT_TRUE(actionUp_);
//	}
//
//private:
//	AerEngine& engine_;
//
//	bool actionDown_ = false;
//	bool actionUp_   = false;
//
//	float updateCount_           = 0;
//    const float kEngineDuration_ = 0.5f;
//	bool interactive_            = false;
//};

TEST(Input, TestInteractiveInput)
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

	//InputBindingManager bindedInputManager;
	//InteractiveBindingInput interactiveBindedInput(engine, false);
	//engine.RegisterSystem(interactiveBindedInput);
	//engine.RegisterOnDrawUi(interactiveBindedInput);
	//engine.RegisterOnEvent(interactiveBindedInput);
	InteractiveInput interactiveInput(engine, false);
	engine.RegisterSystem(interactiveInput);
	engine.RegisterOnDrawUi(interactiveInput);
	engine.RegisterOnEvent(interactiveInput);

	engine.Init();

	engine.EngineLoop();
    logDebug("Test without check");
}
}    // namespace neko::aer
#endif