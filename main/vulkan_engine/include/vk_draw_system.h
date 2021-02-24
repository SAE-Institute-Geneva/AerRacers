#pragma once
#include "sdl_engine/sdl_camera.h"

#include "vk/material/material_manager.h"
#include "vk/models/model_manager.h"

namespace neko::vk
{
class VkDrawSystem final : public SystemInterface,
						   public DrawImGuiInterface,
						   public sdl::SdlEventSystemInterface,
						   public RenderCommandInterface
{
public:
	VkDrawSystem() = default;

	void Init() override;
	void Update(seconds dt) override;
	void Render() override;
	void Destroy() override;

	void OnEvent(const SDL_Event& event) override;
	void DrawImGui() override;

private:
	sdl::MovableCamera3D camera_;

	TextureManager textureManager_;
	MaterialManager materialManager_;
	ModelManager modelManager_;

	ModelId modelId_ = sole::uuid();
};
}
 