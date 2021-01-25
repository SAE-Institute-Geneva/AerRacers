#include "aer/editor/editor_tool_manager.h"

#include "aer/aer_engine.h"
#include "aer/editor/tool/logger.h"
#include "aer/editor/tool/hierarchy.h"
#include "aer/editor/tool/inspector.h"
#include "aer/editor/tool/scene_loader.h"

namespace neko::aer
{
EditorToolManager::EditorToolManager(AerEngine& engine) : engine_(engine) {}

void EditorToolManager::Init()
{
	ModeEnum mode = engine_.GetMode();
	if (mode == ModeEnum::EDITOR)
	{
		AddEditorTool<Logger, EditorToolInterface::ToolType::LOGGER>();
		AddEditorTool<Hierarchy, EditorToolInterface::ToolType::HIERARCHY>();
        AddEditorTool<Inspector, EditorToolInterface::ToolType::INSPECTOR>();
        AddEditorTool<SceneLoader, EditorToolInterface::ToolType::SCENE_LOADER>();
	}
}

void EditorToolManager::Update(seconds dt)
{
	for (auto& tool : tools_) tool->Update(dt);
    Transform3dManager& transform3dManager =
        engine_.GetComponentManagerContainer().transform3dManager;
    if (selectedEntity_ != INVALID_ENTITY)
    {
        GizmosLocator::get().DrawCube(transform3dManager.GetGlobalPosition(selectedEntity_),
            transform3dManager.GetGlobalScale(selectedEntity_),
            EulerAngles(degree_t(0)),
            Color::blue,
            2.0f);
    }
}

void EditorToolManager::Destroy()
{
	for (auto& tool : tools_) tool->Destroy();

	tools_.clear();
}

void EditorToolManager::DrawImGui()
{
	using namespace ImGui;
    ImGuiIO io = ImGui::GetIO();

	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	SetNextWindowPos(viewport->GetWorkPos());
	SetNextWindowSize(viewport->GetWorkSize());
	SetNextWindowViewport(viewport->ID);
	PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
	               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	Begin("Showroom", reinterpret_cast<bool*>(true), windowFlags);
	{
		PopStyleVar();
		PopStyleVar(2);

		ImGuiID dockspaceId = GetID("ShowroomDockSpace");
		DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags_);

		//Editor Menu
		BeginMenuBar();
		{
			if (BeginMenu("Settings"))
			{
				DrawList();
                ImGui::EndMenu();
			}

			if (BeginMenu("Tools"))
			{
				DrawList();
                ImGui::EndMenu();
			}

            SceneManager& sceneManager = engine_.GetComponentManagerContainer().sceneManager;
            std::string sceneName      = sceneManager.GetCurrentScene().sceneName;
            if (!sceneManager.GetCurrentScene().saved) { sceneName += "*"; }
            ImGui::Text(sceneName.c_str());

			EndMenuBar();
		}

		//ImGuizmo::SetDrawlist();

		End();
	}

	for (auto& tool : tools_) tool->DrawImGui();
}


void EditorToolManager::DrawList()
{
	for (auto& tool : tools_)
	{
		std::string name = tool->GetName() + " " + std::to_string(tool->GetId());
		if (ImGui::MenuItem((name).c_str())) tool->isVisible = true;
	}
}

void EditorToolManager::OnEvent(const SDL_Event& event)
{
	for (auto& tool : tools_) tool->OnEvent(event);
}

template<typename T, EditorToolInterface::ToolType Type>
void EditorToolManager::AddEditorTool()
{
	auto newTool =
		std::make_unique<T>(engine_, Type, tools_.size(), toolNames_[static_cast<int>(Type)]);
	newTool->Init();
	tools_.push_back(std::move(newTool));
}

int EditorToolManager::GetNumberTools() const { return tools_.size(); }

Entity EditorToolManager::GetSelectedEntity() const
{ return selectedEntity_; }

void EditorToolManager::SetSelectedEntity(const Entity selectedEntity)
{
    selectedEntity_ = selectedEntity;
}
}    // namespace neko::aer
