#include "draw_system.h"

#include <imgui.h>

#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

namespace neko::aer
{
DrawSystem::DrawSystem(AerEngine& engine)
	: engine_(engine)
{
}

void DrawSystem::Init()
{
}

void DrawSystem::Update(seconds dt)
{
}

void DrawSystem::Destroy()
{
}

void DrawSystem::DrawImGui()
{
}

void DrawSystem::OnEvent(const SDL_Event& event)
{
}
}
