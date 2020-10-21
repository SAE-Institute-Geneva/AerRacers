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
        tools_.push_back(new Logger(TypeTool::LOG));
        engine_.RegisterSystem(*tools_.back());
        engine_.RegisterOnEvent(*tools_.back());
        engine_.RegisterOnDrawUi(*tools_.back());
      

        tools_.push_back(new Inspector(TypeTool::INSP));
        engine_.RegisterSystem(*tools_.back());
        engine_.RegisterOnEvent(*tools_.back());
        engine_.RegisterOnDrawUi(*tools_.back());
    }

    void Editor::Update(seconds dt)
    {
     /*   if (logger_ != nullptr) 
        {
            for (size_t i = 0; i < 20; i++)
            {
                switch (std::rand() % 5) {
                case 0:
                    DebugLog("This is a DEBUG message");
                    break;
                case 1:
                    InfoLog("This is a INFO message");
                    break;
                case 2:
                    WarningLog("This is a WARNING message");
                    break;
                case 3:
                    ErrorLog("This is a ERROR message");
                    break;
                case 4:
                    CriticalLog("This is a CRITICAL message");
                    break;
                }
            }
        }*/
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
            if (ImGui::BeginMenu("Tools"))
            {
                if (ImGui::MenuItem("Inspector", "Ctrl+I")) {
                    GetTool(TypeTool::INSP)->isVisible = true;
                }
                if (ImGui::MenuItem("Logger", "Ctrl+L")) { 
                    GetTool(TypeTool::LOG)->isVisible = true;
                   
                }
                if (ImGui::MenuItem("Profiler", "Ctrl+P")) { /* Do stuff */ }
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("About Neko")) {
                    about_IsVisible = true;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (about_IsVisible) {
            ImGui::Begin("About Neko", &about_IsVisible);
            ImGui::Text("Welcome to Neko Engine");
            ImGui::Text((engine_.config.windowName).c_str());
            
           //TOUDOU
            ImGui::Text("Neko is a 3D game engine based on SDL2 and OpenGL ES 3.0 that works on Desktop,");
            ImGui::Text("WebGL2 and Nintendo Switch (port to Android and iOS possible) used at SAE Institute Geneva.");
            ImGui::Text("");
            ImGui::Text("Copyright(c) 2020 SAE Institute Switzerland AG");
            ImGui::End();
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


    Tool* Editor::GetTool(TypeTool type) {
        for each (Tool * tool in tools_)
        {
            if (tool->type == type) {
                return tool;
            }
        }
        ErrorLog("Tool not found");
        return nullptr;
    }
} 
