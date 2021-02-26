#include "vk_draw_system.h"

#include "mathematics/transform.h"

namespace neko::vk
{
void VkDrawSystem::Init()
{
	camera_.Init();
	camera_.reverseDirection = Vec3f::back;
	camera_.position         = Vec3f::back * 10.0f;
	CameraLocator::provide(&camera_);

	modelId_ = modelManager_.LoadModel("models/rock/rock.obj");
	BasicEngine::GetInstance()->RegisterSystem(textureManager_);
	BasicEngine::GetInstance()->RegisterSystem(modelManager_);
	BasicEngine::GetInstance()->RegisterOnDrawUi(*this);
}

void VkDrawSystem::Update(seconds dt)
{
	dt_ = dt;
	camera_.Update(dt);

	RendererLocator::get().Render(this);
}

void VkDrawSystem::Render()
{
	const Model* model = modelManager_.GetModel(modelId_);
	if (modelManager_.IsLoaded(modelId_))
	{
		const auto& meshCount       = model->GetMeshCount();
		std::vector<Mat4f> matrices = {
			Transform3d::Translate(Mat4f::Identity, Vec3f::zero),
			Transform3d::Translate(Mat4f::Identity, Vec3f::right * 4.0f),
			Transform3d::Translate(Mat4f::Identity, Vec3f::left * 4.0f),
			Transform3d::Translate(Mat4f::Identity, Vec3f::up * 2.0f),
		};

		for (std::size_t i = 0; i < meshCount; ++i)
		{
			const auto& mesh     = model->GetMesh(i);
			const auto& material = materialManager_.GetMaterial(mesh.GetMaterialId());
			VkResources::Inst->modelCommandBuffer.AddModelInstanceIndex(material, mesh, matrices);
		}
	}
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

	Begin("Showroom", reinterpret_cast<bool*>(true), windowFlags);
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
