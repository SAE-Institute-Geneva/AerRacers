/*
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
 */
#include "editor/tool/editor.h"
#include "aer_engine.h"
#include <editor/tool/logger.h>


namespace neko::aer
{
    #pragma region Editor
    Editor::Editor(AerEngine& engine) : engine_(engine)
    {
    }

    void Editor::Init()
    {
        logger_ = new Logger();
        engine_.RegisterSystem(*logger_);
        engine_.RegisterOnEvent(*logger_);
        engine_.RegisterOnDrawUi(*logger_);

        inspector_ = std::move(std::make_unique<Inspector>());
        engine_.RegisterSystem(*inspector_.get());
        engine_.RegisterOnEvent(*inspector_.get());
        engine_.RegisterOnDrawUi(*inspector_.get());
    }

    void Editor::Update(seconds dt)
    {

        if (logger_ != nullptr) {
            for (size_t i = 0; i < 10000; i++)
            {
                switch (std::rand() % 5) {
                case 0:
                    DebugLog("AHHHHH");
                    break;
                case 1:
                    InfoLog("Pas terrible");
                    break;
                case 2:
                    WarningLog("On se calme!");
                    break;
                case 3:
                    ErrorLog("Rein a declarer");
                    break;
                case 4:
                    CriticalLog("Un dinosaure");
                    break;
                }
            }
        }
    }

    void Editor::Destroy()
    {

    }

    void Editor::OnEvent(const SDL_Event& event)
    {

    }

    void Editor::DrawImGui()
    {
        ImGuiIO io = ImGui::GetIO();
        //Editor Menu
        bool my_tool_active;
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Scenes"))
            {
                if (ImGui::MenuItem("Open..", "Ctrl+O")) {
                    ImGui::OpenPopup("popup");
                }
                if (ImGui::MenuItem("Save", "Ctrl+S")) { /* Do stuff */ }
                if (ImGui::MenuItem("Close", "Ctrl+W")) {}
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem("Inspector", "Ctrl+I")) {
                    inspector_.get()->isVisible = true;
                }
                if (ImGui::MenuItem("Logger", "Ctrl+L")) { 
                    logger_->isVisible = true;
                }
                if (ImGui::MenuItem("Profiler", "Ctrl+P")) { /* Do stuff */ }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("About"))
            {
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        ImGui::SetNextWindowPos(ImVec2(0, 20));
        ImGui::SetNextWindowSize(ImVec2(io.DisplaySize.x , io.DisplaySize.y - 20)); 
        //ImGui::SetNextWindowBgAlpha(0.0f);
        ImGui::Begin("Main", 0, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
        {

        }
        ImGui::End();
    }
    #pragma endregion
} 
