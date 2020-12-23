#include "draw_system.h"

#include "imgui.h"

#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

namespace neko::aer {
DrawSystem::DrawSystem(AerEngine& engine) : engine_(engine) {
    gizmosRenderer_ = std::make_unique<GizmosRenderer>(&camera_);
}

void DrawSystem::Init()
{
    //gizmosRenderer_->Init();
    camera_.position = Vec3f::zero;
    camera_.reverseDirection = neko::Vec3f::forward;
    camera_.fovY = neko::degree_t(45.0f);
    camera_.nearPlane = 0.1f;
    camera_.farPlane = 100.0f;
    gizmosRenderer_->SetCamera(&camera_);
}

void DrawSystem::Update(seconds dt) {

    const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
    camera_.SetAspect(static_cast<float>(config.windowSize.x) / config.windowSize.y);

    gizmosRenderer_->Update(dt);
    gizmosRenderer_->DrawCube(Vec3f::zero);
    neko::RendererLocator::get().Render(gizmosRenderer_.get());
}

void DrawSystem::Destroy() {
    gizmosRenderer_->Destroy();
}

void DrawSystem::DrawImGui() {}

void DrawSystem::OnEvent(const SDL_Event& event) {}
}
