#include "aer/editor/editor_tool_manager.h"

#include "aer/aer_engine.h"
#include "aer/editor/tool/hierarchy.h"
#include "aer/editor/tool/inspector.h"
#include "aer/editor/tool/logger.h"
#include "aer/editor/tool/scene_loader.h"

namespace neko::aer
{
EditorToolManager::EditorToolManager(AerEngine& engine)
   : engine_(engine), cContainer_(engine.GetComponentManagerContainer())
{}

void EditorToolManager::Init()
{
    const ModeEnum mode = engine_.GetMode();
	if (mode == ModeEnum::EDITOR)
	{
		AddEditorTool<Logger, EditorToolInterface::ToolType::LOGGER>();
		AddEditorTool<Hierarchy, EditorToolInterface::ToolType::HIERARCHY>();
		AddEditorTool<Inspector, EditorToolInterface::ToolType::INSPECTOR>();
		AddEditorTool<SceneLoader, EditorToolInterface::ToolType::SCENE_LOADER>();
	}
}

void EditorToolManager::Update(const seconds dt)
{
	dt_ = dt;

	for (auto& tool : tools_) tool->Update(dt);

	Transform3dManager& transform3dManager            = cContainer_.transform3dManager;
	EntityManager& entityManager                      = cContainer_.entityManager;
	physics::RigidDynamicManager& rigidDynamicManager = cContainer_.rigidDynamicManager;
	physics::RigidStaticManager& rigidStaticManager   = cContainer_.rigidStaticManager;
	IGizmoRenderer& gizmosLocator               = GizmosLocator::get();
	if (selectedEntity_ != INVALID_ENTITY)
	{
		gizmosLocator.DrawCube(transform3dManager.GetGlobalPosition(selectedEntity_),
			transform3dManager.GetGlobalScale(selectedEntity_),
			transform3dManager.GetGlobalRotation(selectedEntity_),
			Color::blue,
			5.0f);
	}

	//Display Gizmo
	for (Entity entity = 0.0f; entity < entityManager.GetEntitiesSize(); entity++)
	{
		const physics::RigidActor* rigidActor = nullptr;
		if (entityManager.HasComponent(
				entity, EntityMask(ComponentType::RIGID_DYNAMIC)))
		{
			rigidActor = &rigidDynamicManager.GetComponent(entity);
		}
		else if (entityManager.HasComponent(
					 entity, EntityMask(ComponentType::RIGID_STATIC)))
		{
			rigidActor = &rigidStaticManager.GetComponent(entity);
		}
		else
		{
			continue;
		}

        const physics::ColliderType colliderType = rigidActor->GetColliderType();
        switch (colliderType)
        {
            case physics::ColliderType::INVALID: break;
            case physics::ColliderType::BOX:
                {
                    physics::BoxColliderData boxColliderData = rigidActor->GetBoxColliderData();
                    gizmosLocator.DrawCube(
                        transform3dManager.GetGlobalPosition(entity) + boxColliderData.offset,
                        boxColliderData.size,
                        transform3dManager.GetGlobalRotation(entity),
                        boxColliderData.isTrigger ? Color::yellow : Color::green,
                        2.0f);
                }
                break;
            case physics::ColliderType::SPHERE:
                {
                    physics::SphereColliderData sphereColliderData =
                        rigidActor->GetSphereColliderData();
                    gizmosLocator.DrawSphere(
                        transform3dManager.GetGlobalPosition(entity) + sphereColliderData.offset,
                        sphereColliderData.radius,
                        transform3dManager.GetGlobalRotation(entity),
                        sphereColliderData.isTrigger ? Color::yellow : Color::green,
                        2.0f);
                    break;
                }
            default: break;
        }
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
	ImGuiIO io = GetIO();

	ImGuiWindowFlags windowFlags =
		ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoBackground;

	ImGuiViewport* viewport = GetMainViewport();
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

        const ImGuiID dockspaceId = GetID("ShowroomDockSpace");
		DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), kDockspaceFlags);

		//Editor Menu
		BeginMenuBar();
		{
			if (BeginMenu("Settings"))
			{
				if (MenuItem("Show Demo")) showDemo_ = true;
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
			Text("%s", sceneName.c_str());

			if (engine_.GetPhysicsEngine().IsPhysicRunning())
			{
				PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));
				if (Button("Physics active")) engine_.GetPhysicsEngine().StopPhysic();
			}
			else
			{
				PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));
				if (Button("Physics inactive")) engine_.GetPhysicsEngine().StartPhysic();
			}
			PopStyleColor();
			
            const auto fpsText = fmt::format("{:.0f} FPS", 1.0f / dt_.count());
			const float spacing = GetStyle().ItemSpacing.x + GetStyle().FramePadding.x;
			const float nextPos = GetWindowWidth() - CalcTextSize(fpsText.c_str()).x - spacing;
			SetCursorPosX(nextPos);
			Text("%s", fpsText.c_str());

			EndMenuBar();
		}

		//ImGuizmo::SetDrawlist();

		End();
	}

	if (showDemo_) ShowDemoWindow(&showDemo_);

	for (auto& tool : tools_) tool->DrawImGui();
}

void EditorToolManager::DrawList()
{
	for (auto& tool : tools_)
	{
		std::string name = std::string(tool->GetName()) + " " + std::to_string(tool->GetId());
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

Entity EditorToolManager::GetSelectedEntity() const { return selectedEntity_; }

void EditorToolManager::SetSelectedEntity(const Entity selectedEntity)
{
	selectedEntity_ = selectedEntity;
}
}    // namespace aer
