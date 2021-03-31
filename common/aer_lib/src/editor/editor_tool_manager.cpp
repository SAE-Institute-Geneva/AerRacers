#include <imgui_internal.h>

#include "engine/resource_locations.h"

#include "aer/aer_engine.h"
#include "aer/editor/tool/hierarchy.h"
#include "aer/editor/tool/inspector.h"
#include "aer/editor/tool/light_controller.h"
#include "aer/editor/tool/logger.h"
#include "aer/editor/tool/scene_loader.h"
#include "aer/gizmos_renderer.h"

namespace neko::aer
{
EditorToolManager::EditorToolManager(AerEngine& engine)
	: engine_(engine), cContainer_(engine.GetComponentManagerContainer())
{
	const ModeEnum mode = engine_.GetMode();
	if (mode == ModeEnum::EDITOR)
	{
		AddEditorTool<Logger>();
		AddEditorTool<Hierarchy>();
		AddEditorTool<Inspector>();
		AddEditorTool<SceneLoader>();
		AddEditorTool<LightController>();
	}
}

void EditorToolManager::Init()
{
#ifdef NEKO_GLES3
	const float fontSizeInPixels = 16.0f;
	const std::string path       = GetFontsFolderPath() + "droid_sans.ttf";
	ImGui::GetIO().Fonts->AddFontFromFileTTF(path.c_str(), fontSizeInPixels);
#endif

	for (auto& tool : tools_) tool->Init();
}

void EditorToolManager::Update(const seconds dt)
{
	dt_ = dt;

	for (auto& tool : tools_) tool->Update(dt);

	auto& gizmosLocator             = GizmosLocator::get();
	const auto& transform3DManager  = cContainer_.transform3dManager;
	const auto& entityManager       = cContainer_.entityManager;
	const auto& rigidDynamicManager = cContainer_.rigidDynamicManager;
	const auto& rigidStaticManager  = cContainer_.rigidStaticManager;
	if (selectedEntity_ != INVALID_ENTITY)
	{
		gizmosLocator.DrawCube(transform3DManager.GetGlobalPosition(selectedEntity_),
			transform3DManager.GetGlobalScale(selectedEntity_),
			transform3DManager.GetGlobalRotation(selectedEntity_),
			Color::blue,
			5.0f);
	}

	//Display Gizmo
	for (Entity entity = 0.0f; entity < entityManager.GetEntitiesSize(); entity++)
	{
		const physics::RigidActor* rigidActor;
		if (entityManager.HasComponent(entity, EntityMask(ComponentType::RIGID_DYNAMIC)))
			rigidActor = &rigidDynamicManager.GetComponent(entity);
		else if (entityManager.HasComponent(entity, EntityMask(ComponentType::RIGID_STATIC)))
			rigidActor = &rigidStaticManager.GetComponent(entity);
		else
			continue;

		const physics::ColliderType colliderType = rigidActor->GetColliderType();
		switch (colliderType)
		{
			case physics::ColliderType::INVALID: break;
			case physics::ColliderType::BOX:
			{
				physics::BoxColliderData boxColliderData = rigidActor->GetBoxColliderData();
				gizmosLocator.DrawCube(
					transform3DManager.GetGlobalPosition(entity) + boxColliderData.offset,
					boxColliderData.size,
					transform3DManager.GetGlobalRotation(entity),
					boxColliderData.isTrigger ? Color::yellow : Color::green,
					2.0f);
			}
			break;
			case physics::ColliderType::SPHERE:
			{
				physics::SphereColliderData sphereColliderData =
					rigidActor->GetSphereColliderData();
				gizmosLocator.DrawSphere(
					transform3DManager.GetGlobalPosition(entity) + sphereColliderData.offset,
					sphereColliderData.radius,
					transform3DManager.GetGlobalRotation(entity),
					sphereColliderData.isTrigger ? Color::yellow : Color::green,
					2.0f);
				break;
			}
			case physics::ColliderType::CAPSULE:
			{
				physics::CapsuleColliderData capsuleColliderData =
					rigidActor->GetCapsuleColliderData();
				Quaternion rot =
					Quaternion::FromEuler(transform3DManager.GetGlobalRotation(entity));
				gizmosLocator.DrawSphere(
					transform3DManager.GetGlobalPosition(entity) + capsuleColliderData.offset +
						(capsuleColliderData.height / 2.0f) * (rot * Vec3f::right),
					capsuleColliderData.radius,
					transform3DManager.GetGlobalRotation(entity),
					capsuleColliderData.isTrigger ? Color::yellow : Color::green,
					2.0f);
				gizmosLocator.DrawSphere(
					transform3DManager.GetGlobalPosition(entity) + capsuleColliderData.offset +
						(capsuleColliderData.height / 2.0f) * (rot * Vec3f::left),
					capsuleColliderData.radius,
					transform3DManager.GetGlobalRotation(entity),
					capsuleColliderData.isTrigger ? Color::yellow : Color::green,
					2.0f);
				gizmosLocator.DrawCube(
					transform3DManager.GetGlobalPosition(entity) + capsuleColliderData.offset,
					Vec3f(capsuleColliderData.height,
						capsuleColliderData.radius * 1.5f,
						capsuleColliderData.radius * 1.5f),
					transform3DManager.GetGlobalRotation(entity),
					capsuleColliderData.isTrigger ? Color::yellow : Color::green,
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

	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->GetWorkPos());
	ImGui::SetNextWindowSize(viewport->GetWorkSize());
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
	               ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
	windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	Begin("Editor", reinterpret_cast<bool*>(true), windowFlags);
	{
		ImGui::PopStyleVar();
		ImGui::PopStyleVar(2);

        const ImGuiID dockspaceId = ImGui::GetID("ShowroomDockSpace");
		ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), kDockspaceFlags);

		//Editor Menu
		ImGui::BeginMenuBar();
		{
			if (ImGui::BeginMenu("Settings"))
			{
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Tools"))
			{
				DrawList();
				ImGui::EndMenu();
			}

			SceneManager& sceneManager = engine_.GetComponentManagerContainer().sceneManager;
			std::string sceneName      = sceneManager.GetCurrentScene().sceneName;
			if (!sceneManager.GetCurrentScene().saved) { sceneName += "*"; }
			ImGui::Text("%s", sceneName.c_str());

			if (engine_.GetPhysicsEngine().IsPhysicRunning())
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 1.0f, 0.0f, 0.5f));
				if (ImGui::Button("Physics active")) engine_.GetPhysicsEngine().StopPhysic();
			}
			else
			{
				ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(1.0f, 0.0f, 0.0f, 0.5f));
				if (ImGui::Button("Physics inactive")) engine_.GetPhysicsEngine().StartPhysic();
			}
			ImGui::PopStyleColor();
			
            const auto fpsText = fmt::format("{:.0f} FPS", 1.0f / dt_.count());
			const float spacing = ImGui::GetStyle().ItemSpacing.x + ImGui::GetStyle().FramePadding.x;
			const float nextPos = ImGui::GetWindowWidth() - ImGui::CalcTextSize(fpsText.c_str()).x - spacing;
			ImGui::SetCursorPosX(nextPos);
			ImGui::Text("%s", fpsText.c_str());

			ImGui::EndMenuBar();
		}

		//ImGuizmo::SetDrawlist();

		ImGui::End();
	}

	for (auto& tool : tools_)
	{
		const std::string id = fmt::format("###{}{}", tool->GetName(), tool->GetId());
		if (!hasInit_)
		{
			const auto* winSettings = ImGui::FindOrCreateWindowSettings(id.c_str());
			tool->isVisible         = winSettings->IsOpen;
		}

		if (!tool->isVisible) continue;
		const std::string winName = std::string(tool->GetName()) + id;
		if (ImGui::Begin(winName, &tool->isVisible))
		{
			// Window Label
			if (ImGui::IsWindowDocked())
			{
				ImGui::Text(tool->GetName().data());
				ImGui::Separator();
			}

			tool->DrawImGui();
		}
		ImGui::End();
	}

	hasInit_ = true;
}

void EditorToolManager::DrawList()
{
	for (auto& tool : tools_)
	{
		if (tool->isVisible) ImGui::Text("ø");
		else ImGui::Text("o");

		const auto test = ImGui::GetCurrentContext()->SettingsHandlers;

		ImGui::SameLine();
		if (ImGui::MenuItem(tool->GetName())) tool->isVisible = !tool->isVisible;
	}
}

void EditorToolManager::OnEvent(const SDL_Event& event)
{
	for (auto& tool : tools_) tool->OnEvent(event);
}

template<typename T>
void EditorToolManager::AddEditorTool()
{
	static_assert(std::is_base_of<EditorToolInterface, T>::value,
		"Tool must derive from EditorToolInterface");

	tools_.emplace_back(std::make_unique<T>(engine_));
}

int EditorToolManager::GetNumberTools() const { return tools_.size(); }

Entity EditorToolManager::GetSelectedEntity() const { return selectedEntity_; }

void EditorToolManager::SetSelectedEntity(const Entity selectedEntity)
{
	selectedEntity_ = selectedEntity;
}
}    // namespace neko::aer
