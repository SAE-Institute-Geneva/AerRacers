#pragma once
#include "engine/transform.h"
#include "sdl_engine/sdl_camera.h"

#include "aer/managers/render_manager.h"
#include "vk/material/material_manager.h"
#include "vk/models/model_manager.h"

namespace neko::vk
{
constexpr ImGuiDockNodeFlags kDockspaceFlags = ImGuiDockNodeFlags_NoDockingInCentralNode |
                                               ImGuiDockNodeFlags_AutoHideTabBar |
                                               ImGuiDockNodeFlags_PassthruCentralNode;
class VkDrawSystem final : public SystemInterface,
						   public DrawImGuiInterface,
						   public sdl::SdlEventSystemInterface,
						   public RenderCommandInterface
{
public:
	VkDrawSystem();

	void Init() override;
	void Update(seconds dt) override;
	void Render() override;
	void Destroy() override;

	void OnEvent(const SDL_Event& event) override;
	void DrawImGui() override;

private:
	seconds dt_ {};

	sdl::MovableCamera3D camera_;

	TextureManager textureManager_;
	MaterialManager materialManager_;
	ModelManager modelManager_;

	EntityManager entityManager_;
	Transform3dManager transformManager_;
	aer::RenderManager renderManager_;
	aer::RendererViewer rendererViewer_;

	Entity testEntity_ {};
	ModelId modelId_ = sole::uuid();
};
}
 