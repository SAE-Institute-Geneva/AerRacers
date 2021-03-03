#pragma once
#include "vk_draw_system.h"

namespace neko::vk
{
class VkEngine final : public sdl::SdlEngine
{
public:
	VkEngine() = delete;
	VkEngine(const FilesystemInterface& filesystem, Configuration* config = nullptr);

	void Init() override;
	void Destroy() override;

	void GenerateUiFrame() override;

	void SetWindowAndRenderer(Window* window, neko::Renderer* renderer) override;

private:
	VkDrawSystem drawSystem_;
};
}    // namespace neko::vk