#include "vk_draw_system.h"

#include "mathematics/transform.h"

namespace neko::vk
{
VkDrawSystem::VkDrawSystem()
   : transformManager_(entityManager_),
	 renderManager_(entityManager_, modelManager_, transformManager_, rendererViewer_),
	 rendererViewer_(entityManager_, renderManager_)
{}

void VkDrawSystem::Init()
{
	camera_.Init();
	camera_.reverseDirection = Vec3f::forward;
	camera_.position         = Vec3f::forward * 10.0f;
	CameraLocator::provide(&camera_);

	transformManager_.Init();
	renderManager_.Init();

	const Configuration& config = BasicEngine::GetInstance()->GetConfig();
	modelId_ = modelManager_.LoadModel(config.dataRootPath + "models/rock/rock.obj");

	testEntity_ = entityManager_.CreateEntity();
	transformManager_.AddComponent(testEntity_);
	transformManager_.SetGlobalPosition(testEntity_, Vec3f::forward * 2.0f);
	renderManager_.AddComponent(testEntity_);
	renderManager_.SetModel(testEntity_, modelId_);

	entityManager_.CreateEntity(1);
	transformManager_.AddComponent(1);
	transformManager_.SetGlobalPosition(1, Vec3f::zero);
	renderManager_.AddComponent(1);
	renderManager_.SetModel(1, modelId_);
	entityManager_.CreateEntity(2);
	transformManager_.AddComponent(2);
	transformManager_.SetGlobalPosition(2, Vec3f::right * 4.0f);
	renderManager_.AddComponent(2);
	renderManager_.SetModel(2, modelId_);
	entityManager_.CreateEntity(3);
	transformManager_.AddComponent(3);
	transformManager_.SetGlobalPosition(3, Vec3f::left * 4.0f);
	renderManager_.AddComponent(3);
	renderManager_.SetModel(3, modelId_);
	entityManager_.CreateEntity(4);
	transformManager_.AddComponent(4);
	transformManager_.SetGlobalPosition(4, Vec3f::up * 2.0f);
	renderManager_.AddComponent(4);
	renderManager_.SetModel(4, modelId_);

	BasicEngine::GetInstance()->RegisterSystem(textureManager_);
	BasicEngine::GetInstance()->RegisterSystem(modelManager_);
	BasicEngine::GetInstance()->RegisterOnDrawUi(*this);
}

void VkDrawSystem::Update(seconds dt)
{
	dt_ = dt;
	camera_.Update(dt);

	transformManager_.Update();
	renderManager_.Update(dt);

	RendererLocator::get().Render(this);
}

void VkDrawSystem::Render()
{
	renderManager_.Render();
}

void VkDrawSystem::Destroy() { camera_.Destroy(); }

void VkDrawSystem::OnEvent(const SDL_Event& event) { camera_.OnEvent(event); }

void VkDrawSystem::DrawImGui()
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

	Begin("Dockspace", reinterpret_cast<bool*>(true), windowFlags);
	{
		PopStyleVar();
		PopStyleVar(2);

		const ImGuiID dockspaceId = GetID("ShowroomDockSpace");
		DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), kDockspaceFlags);

		//Editor Menu
		BeginMenuBar();
		{
			const auto fpsText  = fmt::format("{:.0f} FPS", 1.0f / dt_.count());
			const float spacing = GetStyle().ItemSpacing.x + GetStyle().FramePadding.x;
			const float nextPos = GetWindowWidth() - CalcTextSize(fpsText.c_str()).x - spacing;
			SetCursorPosX(nextPos);
			Text("%s", fpsText.c_str());

			EndMenuBar();
		}

		//ImGuizmo::SetDrawlist();

		End();
	}
}
}    // namespace neko::vk
