#pragma once
#include "engine/system.h"
#include "sdl_engine/sdl_engine.h"
#include "sdl_engine/sdl_camera.h"

namespace neko::vk
{
class VkDrawSystem final : public SystemInterface, public sdl::SdlEventSystemInterface, public RenderCommandInterface
{
public:
    explicit VkDrawSystem();

    void Init() override;
    void Update(seconds dt) override;
	void Render() override;
    void Destroy() override;
	
    void OnEvent(const SDL_Event& event) override;

private:
    sdl::MovableCamera3D camera_;
	RendererInterface& renderer_;
};
}
