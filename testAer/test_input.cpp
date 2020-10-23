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

#include "sdl_engine/sdl_engine.h"
#include "sdl_engine/sdl_input.h"
#include "gl/gles3_window.h"
#include "gl/graphics.h"

#include "aer_engine.h"

class SimulateInput : public neko::SystemInterface {
public:
    SimulateInput(neko::aer::AerEngine& engine) : engine_(engine) { }

    void Init() override { }

    void Update(neko::seconds dt) override
    {
        auto& inputLocator = neko::sdl::InputLocator::get();
        inputLocator.SimulateKeyDown(neko::sdl::KeyCode::A);
        inputLocator.SimulateKeyUp(neko::sdl::KeyCode::B);
        inputLocator.SimulateMouseDown(neko::sdl::MouseButtonCode::LEFT);
        inputLocator.SimulateMouseUp(neko::sdl::MouseButtonCode::RIGHT);
        inputLocator.SimulateControllerDown(0,
                                            neko::sdl::ControllerInputs::BUTTON_A);
        inputLocator.SimulateControllerUp(1,
                                          neko::sdl::ControllerInputs::BUTTON_B);
        inputLocator.SimulateActionDown(2, neko::sdl::ActionInput::FORWARD);
        inputLocator.SimulateActionUp(3, neko::sdl::ActionInput::BACKWARD);

        if (inputLocator.IsKeyDown(neko::sdl::KeyCode::A)) {
            keyDown_ = true;
        }
        if (inputLocator.IsKeyUp(neko::sdl::KeyCode::B)) {
            keyUp_ = true;
        }
        if (inputLocator.IsMouseButtonDown(neko::sdl::MouseButtonCode::LEFT)) {
            mouseDown_ = true;
        }
        if (inputLocator.IsMouseButtonUp(neko::sdl::MouseButtonCode::RIGHT)) {
            mouseUp_ = true;
        }
        if (inputLocator.IsControllerDown(0,
                                          neko::sdl::ControllerInputs::BUTTON_A)
        ) {
            controllerDown_ = true;
        }
        if (inputLocator.IsControllerUp(1,
                                        neko::sdl::ControllerInputs::BUTTON_B)
        ) {
            controllerUp_ = true;
        }
        if (inputLocator.IsActionDown(2, neko::sdl::ActionInput::FORWARD)) {
            actionDown_ = true;
        }
        if (inputLocator.IsActionUp(3, neko::sdl::ActionInput::BACKWARD)) {
            actionUp_ = true;
        }

        updateCount_++;
        if (updateCount_ == kEngineDuration_) {
            engine_.Stop();
        }
    }

    void Destroy() override { }

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

    int updateCount_ = 0;
    const int kEngineDuration_ = 10;

    neko::aer::AerEngine& engine_;
};

TEST(Engine, TestSimulateInput)
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
    neko::aer::AerEngine engine(&config);

    engine.SetWindowAndRenderer(&window, &renderer);

    engine.Init();

    SimulateInput simulateInput(engine);
    engine.RegisterSystem(simulateInput);

    engine.EngineLoop();

    simulateInput.HasSucceed();
}

class InteractiveInput
    : public neko::SystemInterface,
      public neko::sdl::SdlEventSystemInterface,
      public neko::DrawImGuiInterface {
public:
    InteractiveInput(neko::aer::AerEngine& engine, bool interactive) : engine_(engine), interactive_(interactive) { }

    void Init() override {
    }

    void Update(neko::seconds dt) override
    {
        auto& inputLocator = neko::sdl::InputLocator::get();
        if (inputLocator.IsKeyDown(neko::sdl::KeyCode::A)) {
            keyDown_ = true;
        }
        if (inputLocator.IsKeyUp(neko::sdl::KeyCode::B)) {
            keyUp_ = true;
        }
        if (inputLocator.IsMouseButtonDown(neko::sdl::MouseButtonCode::LEFT)) {
            mouseDown_ = true;
        }
        if (inputLocator.IsMouseButtonUp(neko::sdl::MouseButtonCode::RIGHT)) {
            mouseUp_ = true;
        }
        if (inputLocator.IsControllerDown(0, neko::sdl::ControllerInputs::BUTTON_A)) {
            controllerDown_ = true;
        }
        if (inputLocator.IsControllerUp(1, neko::sdl::ControllerInputs::BUTTON_B)) {
            controllerUp_ = true;
        }
        if (inputLocator.IsActionDown(2, neko::sdl::ActionInput::FORWARD)) {
            actionDown_ = true;
        }
        if (inputLocator.IsActionUp(3, neko::sdl::ActionInput::BACKWARD)) {
            actionUp_ = true;
        }

        updateCount_++;
        if (updateCount_ == kEngineDuration_ && !interactive_) { engine_.Stop(); }
    }

    void Destroy() override { }
    void OnEvent(const SDL_Event& event) override { }

    void DrawImGui() override
    {
        auto& inputLocator = neko::sdl::InputLocator::get();
        ImGui::Begin("Input");
        ImGui::Text("-----------");
        if (ImGui::CollapsingHeader("KeyBoard", ImGuiTreeNodeFlags_DefaultOpen)) {
            for (unsigned int keyIndex = 0;
                keyIndex != static_cast<unsigned>(
                    neko::sdl::KeyCode::KEYBOARD_SIZE);
                keyIndex++) {
                if (inputLocator.IsKeyDown(static_cast<neko::sdl::KeyCode>(keyIndex))) {
                    std::string text =
                        inputLocator.PcInputsEnumToString(
                            static_cast<neko::sdl::KeyCode>(keyIndex)) +
                        " IsKeyDown" + std::to_string(updateCount_);
                    ImGui::Text(text.c_str());
                }
                if (inputLocator.IsKeyHeld(static_cast<neko::sdl::KeyCode>(keyIndex))) {
                    std::string text =
                        inputLocator.PcInputsEnumToString(
                            static_cast<neko::sdl::KeyCode>(keyIndex)) +
                        " IsKeyHeld buttonstate" + std::to_string(updateCount_);
                    ImGui::Text(text.c_str());
                }
                if (inputLocator.IsKeyUp(static_cast<neko::sdl::KeyCode>(keyIndex))) {
                    std::string text =
                        inputLocator.PcInputsEnumToString(
                            static_cast<neko::sdl::KeyCode>(keyIndex)) + " IsKeyUp" + std::to_string(updateCount_);
                    ImGui::Text(text.c_str());
                }
            }
        }
        ImGui::Text("-----------");
        if (ImGui::CollapsingHeader("Mouse", ImGuiTreeNodeFlags_DefaultOpen)) {
            for (unsigned int mouseIndex = 0;
                mouseIndex != static_cast<unsigned>(
                    neko::sdl::MouseButtonCode::LENGTH);
                mouseIndex++) {
                if (inputLocator.IsMouseButtonDown(
                    static_cast<neko::sdl::MouseButtonCode>(mouseIndex))) {
                    std::string text =
                        inputLocator.PcInputsEnumToString(
                            static_cast<neko::sdl::MouseButtonCode>(mouseIndex)) +
                        " IsMouseButtonDown";
                    ImGui::Text(text.c_str());
                }
                if (inputLocator.IsMouseButtonHeld(
                    static_cast<neko::sdl::MouseButtonCode>(mouseIndex))) {
                    std::string text =
                        inputLocator.PcInputsEnumToString(
                            static_cast<neko::sdl::MouseButtonCode>(mouseIndex)) +
                        " IsMouseButtonHeld";
                    ImGui::Text(text.c_str());
                }
                if (inputLocator.IsMouseButtonUp(
                    static_cast<neko::sdl::MouseButtonCode>(mouseIndex))) {
                    std::string text =
                        inputLocator.PcInputsEnumToString(
                            static_cast<neko::sdl::MouseButtonCode>(mouseIndex)) +
                        " IsMouseButtonUp";
                    ImGui::Text(text.c_str());
                }
            }
            std::string mouseText = "Mouse Position : " +
                std::to_string(
                    (neko::sdl::InputLocator::get()
                        .GetMousePosition().x)) + ", " +
                std::to_string(
                    (neko::sdl::InputLocator::get()
                        .GetMousePosition().y)) + ")";
            ImGui::Text(mouseText.c_str());
            mouseText = "Mouse Relative Position : " +
                std::to_string(
                    (inputLocator.GetRelativeMousePosition().x)) + ", " +
                std::to_string(
                    (inputLocator.GetRelativeMousePosition().y)) + ")";
            ImGui::Text(mouseText.c_str());
            mouseText = "Mouse Scroll : " +
                std::to_string(
                    (inputLocator.GetMouseScroll().x)) +
                ", " + std::to_string(
                    (inputLocator.GetMouseScroll().y)) +
                ")";
            ImGui::Text(mouseText.c_str());
        }
        ImGui::Text("-----------");
        for (unsigned controllerId = 0; controllerId < 4; controllerId++) {
            std::string controllerTitle = "Controller " + std::to_string(controllerId);
            if (ImGui::CollapsingHeader(controllerTitle.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                ImGui::Text("Controller Input");
                for (unsigned int controllerIndex = 0;
                    controllerIndex != static_cast<unsigned>(
                        neko::sdl::ControllerInputs::LENGTH);
                    controllerIndex++) {
                    if (inputLocator.IsControllerDown(controllerId,
                        static_cast<neko::sdl::ControllerInputs>(controllerIndex))) {
                        std::string text =
                            inputLocator.ControllerInputsEnumToString(
                                static_cast<neko::sdl::ControllerInputs>(controllerIndex
                                    )) + " IsControllerDown";
                        ImGui::Text(text.c_str());
                    }
                    if (inputLocator.IsControllerHeld(controllerId,
                        static_cast<neko::sdl::ControllerInputs>(controllerIndex))) {
                        std::string text =
                            inputLocator.ControllerInputsEnumToString(
                                static_cast<neko::sdl::ControllerInputs>(controllerIndex
                                    )) + " IsControllerHeld";
                        ImGui::Text(text.c_str());
                    }
                    if (inputLocator.IsControllerUp(controllerId,
                        static_cast<neko::sdl::ControllerInputs>(controllerIndex))) {
                        std::string text =
                            inputLocator.ControllerInputsEnumToString(
                                static_cast<neko::sdl::ControllerInputs>(controllerIndex
                                    )) + " IsControllerUp";
                        ImGui::Text(text.c_str());
                    }
                }
                ImGui::Text("Controller Axis");
                for (unsigned int controllerAxisIndex = 0;
                    controllerAxisIndex != static_cast<unsigned>(
                        neko::sdl::ControllerAxis::LENGTH);
                    controllerAxisIndex++) {
                    std::string text = inputLocator.
                        ControllerAxisEnumToString(
                            static_cast<neko::sdl::ControllerAxis>(
                                controllerAxisIndex)) +
                        " : " + std::to_string(
                            inputLocator.
                            GetControllerAxis(controllerId,
                                static_cast<neko::sdl::ControllerAxis>(
                                    controllerAxisIndex)));
                    ImGui::Text(text.c_str());
                }
            }
        }
        ImGui::Text("-----------");
        for (unsigned playerId = 0; playerId < 4; playerId++) {
            std::string playerTitle = "InputAction Player " + std::to_string(
                playerId);
            if (ImGui::CollapsingHeader(
                playerTitle.c_str(),
                ImGuiTreeNodeFlags_DefaultOpen)) {
                for (unsigned int inputIndex = 0;
                    inputIndex != static_cast<unsigned>(
                        neko::sdl::ActionInput::LENGTH)
                    ;
                    inputIndex++) {
                    if (inputLocator.IsActionDown(
                        playerId,
                        static_cast<neko::sdl::ActionInput>(inputIndex))) {
                        std::string text =
                            inputLocator.ActionEnumToString(
                                static_cast<neko::sdl::ActionInput>(inputIndex)) +
                            " IsActionDown";
                        ImGui::Text(text.c_str());
                    }
                    if (inputLocator.IsActionHeld(
                        playerId,
                        static_cast<neko::sdl::ActionInput>(inputIndex))) {
                        std::string text =
                            inputLocator.ActionEnumToString(
                                static_cast<neko::sdl::ActionInput>(inputIndex)) +
                            " IsActionHeld";
                        ImGui::Text(text.c_str());
                    }
                    if (inputLocator.IsActionUp(
                        playerId,
                        static_cast<neko::sdl::ActionInput>(inputIndex))) {
                        std::string text =
                            inputLocator.ActionEnumToString(
                                static_cast<neko::sdl::ActionInput>(inputIndex)) +
                            " IsActionUp";
                        ImGui::Text(text.c_str());
                    }
                }
            }
        }
        ImGui::End();

        ImGui::Begin("TestInput");
        if (keyDown_) { ImGui::Text("Key A has been pressed down"); }
        else {
            ImGui::Text("Press down key A");
        }
        if (keyUp_) { ImGui::Text("Key B has been released up"); }
        else {
            ImGui::Text("Release up key B");
        }
        if (mouseDown_) { ImGui::Text("Mouse Left has been pressed down"); }
        else {
            ImGui::Text("Press down mouse Left");
        }
        if (mouseUp_) { ImGui::Text("Mouse right has been released up"); }
        else {
            ImGui::Text("Release up mouse Right");
        }
        if (controllerDown_) {
            ImGui::Text("Controller button A has been pressed down");
        }
        else { ImGui::Text("Press down controller button A"); }
        if (controllerUp_) {
            ImGui::Text("Controller button B has been released up");
        }
        else { ImGui::Text("Release up controller button B"); }
        if (actionDown_) {
            ImGui::Text("Action forward has been pressed down");
        }
        else { ImGui::Text("Press down action forward"); }
        if (actionUp_) {
            ImGui::Text("Action backward has been released up");
        }
        else { ImGui::Text("Release up action backward"); }
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

    int updateCount_ = 0;
    const int kEngineDuration_ = 10;
    bool interactive_ = false;


    neko::aer::AerEngine& engine_;
};

TEST(Engine, TestInteractiveInput)
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
    neko::aer::AerEngine engine(&config);

    engine.SetWindowAndRenderer(&window, &renderer);

    engine.Init();

    InteractiveInput interactiveInput(engine, false);
    engine.RegisterSystem(interactiveInput);
    engine.RegisterOnDrawUi(interactiveInput);
    engine.RegisterOnEvent(interactiveInput);

    engine.EngineLoop();
}
