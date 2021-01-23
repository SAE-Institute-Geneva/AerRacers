#include "aer/draw_system.h"

#include "imgui.h"

#ifdef EASY_PROFILE_USE
	#include <easy/profiler.h>
#endif

namespace neko::aer
{
DrawSystem::DrawSystem(AerEngine& engine)
   : engine_(engine),
	 rContainer_(engine.GetResourceManagerContainer()),
	 cContainer_(engine.GetComponentManagerContainer())
{
	engine.RegisterSystem(camera_);
	engine.RegisterOnEvent(camera_);

#ifdef NEKO_GLES3
	gizmosRenderer_ = std::make_unique<GizmoRenderer>(&camera_);
#endif

	engine.RegisterSystem(*gizmosRenderer_);
}

void DrawSystem::Init()
{
	camera_.position         = Vec3f::forward * 2.0f;
	camera_.reverseDirection = Vec3f::forward;
	camera_.fovY             = degree_t(45.0f);
	camera_.nearPlane        = 0.1f;
	camera_.farPlane         = 100.0f;

	gizmosRenderer_->SetCamera(&camera_);

	const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
    testEntity_        = cContainer_.entityManager.CreateEntity();
    cContainer_.transform3dManager.AddComponent(testEntity_);
    cContainer_.renderManager.AddComponent(testEntity_);
    cContainer_.renderManager.SetModel(
        testEntity_, config.dataRootPath + "models/nanosuit2/nanosuit.obj");
}

void DrawSystem::Update(seconds)
{
	const auto modelId = cContainer_.renderManager.GetComponent(testEntity_).modelId;
	if (!rContainer_.modelManager.IsLoaded(modelId)) return;

	const auto& model = rContainer_.modelManager.GetModel(modelId);
	for (size_t i = 0; i < model->GetMeshCount(); ++i)
	{
		const auto& meshAabb = model->GetMesh(i).aabb;
		gizmosRenderer_->DrawCube(meshAabb.CalculateCenter(), meshAabb.CalculateExtends());
	}
}

void DrawSystem::Destroy() {}

void DrawSystem::DrawImGui() {}

void DrawSystem::OnEvent(const SDL_Event&) {}
}    // namespace neko::aer
