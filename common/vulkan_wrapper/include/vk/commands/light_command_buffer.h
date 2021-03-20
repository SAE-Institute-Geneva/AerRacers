#pragma once
#include "graphics/lights.h"

namespace neko::vk
{
class LightCommandBuffer
{
public:
	LightCommandBuffer();

	void Draw(const PointLight& light);
	void Draw(std::vector<PointLight>& lights);

	[[nodiscard]] const std::vector<PointLight>& GetLights() const { return lights_; }
	[[nodiscard]] std::size_t GetLightNum() const { return lights_.size(); }

	void Clear();

private:
	std::vector<PointLight> lights_;
};
}    // namespace neko::vk
