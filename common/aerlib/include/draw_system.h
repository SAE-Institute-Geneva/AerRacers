#pragma once
#include <sdl_engine/sdl_camera.h>

namespace neko::aer
{
class AerEngine;
class DrawSystem final : public SystemInterface,
	                         public sdl::SdlEventSystemInterface,
	                         public DrawImGuiInterface
{
public:
	explicit DrawSystem(AerEngine& engine);
	void Init() override;
	void DrawImGui() override;

	void Update(seconds dt) override;
	void Destroy() override;
	void OnEvent(const SDL_Event& event) override;

protected:
	Camera3D* camera_;
	AerEngine& engine_;
};
}
