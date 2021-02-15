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
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("DrawSystem::Init");
#endif
	camera_.position         = Vec3f::forward * 2.0f;
	camera_.reverseDirection = Vec3f::forward;
	camera_.fovY             = degree_t(45.0f);
	camera_.nearPlane        = 0.1f;
	camera_.farPlane         = 100.0f;

	gizmosRenderer_->SetCamera(&camera_);

	cameraEntity_ = engine_.GetComponentManagerContainer().entityManager.CreateEntity();
	engine_.GetComponentManagerContainer().entityManager.SetEntityName(cameraEntity_, "cameraEntity");
	engine_.GetComponentManagerContainer().transform3dManager.AddComponent(cameraEntity_);
	engine_.GetComponentManagerContainer().cameraControllerManager.AddComponent(cameraEntity_);

}

void DrawSystem::Update(seconds)
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("DrawSystem::Update");
#endif
	camera_.position = engine_.GetComponentManagerContainer().transform3dManager.GetGlobalPosition(cameraEntity_);
}

void DrawSystem::Destroy() {}

void DrawSystem::DrawImGui() {}

void DrawSystem::OnEvent(const SDL_Event&) {}
}    // namespace neko::aer
