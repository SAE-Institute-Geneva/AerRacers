#include "Editor/tools_system.h"

#include "imgui.h"

namespace neko::aer
{
ToolsSystem::ToolsSystem(AerEngine& engine)
	: engine_(engine)
{
}

void ToolsSystem::Init()
{
}

void ToolsSystem::Update(seconds dt)
{
}

void ToolsSystem::Destroy()
{
}

void ToolsSystem::DrawImGui()
{
	ImGui::ShowDemoWindow();
	//Exemple Tool list
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Tools")) {
			for (auto& tool : toolList_) {
				if (ImGui::MenuItem(tool.c_str(), nullptr, false)) {
				}
			}

			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void ToolsSystem::OnEvent(const SDL_Event& event)
{
}
}
