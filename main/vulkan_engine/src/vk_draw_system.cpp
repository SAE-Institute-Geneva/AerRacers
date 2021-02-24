#include "vk_draw_system.h"

namespace neko::vk
{
void VkDrawSystem::Init()
{
	camera_.Init();
	camera_.reverseDirection = Vec3f::back;
	camera_.position         = Vec3f::back * 10.0f;
	CameraLocator::provide(&camera_);

	BasicEngine::GetInstance()->RegisterSystem(modelManager_);
	BasicEngine::GetInstance()->RegisterOnDrawUi(*this);
}

void VkDrawSystem::Update(seconds dt)
{
	if (modelId_ == sole::uuid()) modelId_ = modelManager_.LoadModel("models/rock/rock.obj");

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
	ImGui::ShowDemoWindow();
}
}    // namespace neko::vk
