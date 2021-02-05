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
	camera_.position         = Vec3f::back * 5.0f + Vec3f::up * 2.0f;
	camera_.reverseDirection = Vec3f::back;
	camera_.fovY             = degree_t(45.0f);
	camera_.nearPlane        = 0.1f;
	camera_.farPlane         = 100.0f;

	gizmosRenderer_->SetCamera(&camera_);

}

void DrawSystem::Update(seconds)
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("DrawSystem::Update");
#endif
}

void DrawSystem::Destroy() {}

void DrawSystem::DrawImGui() {}

void DrawSystem::OnEvent(const SDL_Event&) {}
}    // namespace neko::aer
