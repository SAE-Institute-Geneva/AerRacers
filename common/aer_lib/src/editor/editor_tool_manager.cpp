#include "aer_engine.h"
#include "editor/editor_tool_manager.h"
#include "editor/tool/logger.h"
#include "editor/tool/inspector.h"
#include "editor/tool/hierarchy.h"
#include "editor/tool/inspector.h"

namespace neko::aer
{
EditorToolManager::EditorToolManager(AerEngine& engine) : engine_(engine)
{
}

void EditorToolManager::Init()
{
	ModeEnum mode = engine_.GetMode();
	if (mode == ModeEnum::EDITOR)
	{
		AddEditorTool<Logger, EditorToolInterface::ToolType::LOGGER>();
		AddEditorTool<Inspector, EditorToolInterface::ToolType::INSPECTOR>();
		AddEditorTool<Hierarchy, EditorToolInterface::ToolType::HIERARCHY>();
		AddEditorTool<Inspector, EditorToolInterface::ToolType::INSPECTOR>();
	}
}


void EditorToolManager::Update(seconds dt)
{
	for (auto& tool : tools_)
	{
		tool->Update(dt);
	}
}

void EditorToolManager::Destroy()
{
	for (auto& tool : tools_)
	{
		tool->Destroy();
	}

	tools_.clear();
}

void EditorToolManager::DrawImGui()
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

		ImGui::EndMainMenuBar();
	}

	for (auto& tool : tools_)
	{
		tool->DrawImGui();
	}
}

void EditorToolManager::DrawList()
{
	for (auto& tool : tools_)
	{
		std::string name = tool->GetName() + " " +
			std::to_string(tool->GetId());
		if (ImGui::MenuItem((name).c_str()))
		{
			tool->isVisible = true;
		}
	}
}

void EditorToolManager::OnEvent(const SDL_Event& event)
{
	for (auto& tool : tools_)
	{
		tool->OnEvent(event);
	}
}

template <typename T, EditorToolInterface::ToolType Type>
void EditorToolManager::AddEditorTool()
{
    auto newTool = std::make_unique<T>(
        engine_,
        Type,
        tools_.size(),
        toolNames_[static_cast<int>(Type)]
    );
    newTool->Init();
    tools_.push_back(std::move(newTool));
}

int EditorToolManager::GetNumberTools() const
{
	return tools_.size();
}
}
