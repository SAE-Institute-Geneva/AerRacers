#include "aer_engine.h"
#include "editor/tool_manager.h"
#include "editor/tool/logger.h"

namespace neko::aer //TODOCR(Luca@Dylan)  Apply Code Style
{
    ToolManager::ToolManager(AerEngine& engine) : engine_(engine)
    {
    }

    void ToolManager::Init()
    {
        //LOGGER
        tools_.push_back(new Logger(TypeTool::LOG));
        engine_.RegisterSystem(*tools_.back());
        engine_.RegisterOnEvent(*tools_.back());
        engine_.RegisterOnDrawUi(*tools_.back());
    }



    void ToolManager::Update(seconds dt)
    {

    }

    void ToolManager::Destroy()
    {
        for (Tool* tool : tools_) {
            tool->Destroy();
        }
    }

    void ToolManager::DrawImGui()
    {
        ImGuiIO io = ImGui::GetIO();

        //Editor Menu
        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("Settings"))
            {
                DrawList();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Tools"))
            {
                DrawList();
                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("Help"))
            {
                if (ImGui::MenuItem("About Neko")) {
                    aboutVisible_ = true;
                }
                ImGui::EndMenu();
            }
            ImGui::EndMainMenuBar();
        }

        if (aboutVisible_) {
            ImGui::Begin("About", &aboutVisible_);
            ImGui::Text(("Welcome to " + (engine_.config.windowName)).c_str());
            ImGui::Text("Best Editor Ever");
            ImGui::Separator();
            ImGui::Text("Neko is a 3D game engine based on SDL2 and OpenGL ES 3.0 that works on Desktop,");
            ImGui::Text("WebGL2 and Nintendo Switch (port to Android and iOS possible) used at SAE Institute Geneva.");
            ImGui::Text("");
            ImGui::Text("Copyright(c) 2020 SAE Institute Switzerland AG");
            ImGui::End();
        }
    }

    void ToolManager::DrawList()
    {
        if (ImGui::MenuItem("Logger", "Ctrl+L")) {
            GetTool(TypeTool::LOG)->isVisible = true;
        }
    }


    Tool* ToolManager::GetTool(TypeTool type) {
        for (Tool* t : tools_) {
            if (t->type == type) {
                return t;
            }
        }
        return nullptr;
    }

    void ToolManager::OnEvent(const SDL_Event& event)
    {

    }

}
