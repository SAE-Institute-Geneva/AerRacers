#pragma once
#include "sdl_engine/sdl_camera.h"

namespace neko::sr
{
struct ShowRoomCamera3D final : Camera3D, sdl::MovableCamera
{
	float wheelSpeed = 800.0f;

	ShowRoomCamera3D& operator=(const ShowRoomCamera3D& other);

	void Init() override;

	void Update(seconds dt) override;
	void OnEvent(const SDL_Event& event) override;

	void Destroy() override {}

private:
	float wheelMotion_ = 0.0f;
	bool mouseWarped_ = false;
};
}
