#include "vk/commands/light_command_buffer.h"

namespace neko::vk
{
LightCommandBuffer::LightCommandBuffer() { lights_.reserve(kMaxLights); }

void LightCommandBuffer::Draw(const PointLight& light)
{
	if (lights_.size() + 1 < kMaxLights) lights_.emplace_back(light);
}

void LightCommandBuffer::Draw(std::vector<PointLight>& lights)
{
	if (lights_.empty()) lights_ = std::move(lights);
	else if (lights_.size() + 1 < kMaxLights)
	{
		std::size_t sizeDiff = kMaxLights - lights_.size();
		lights_.insert(lights_.end(), lights.begin(), lights.end() - sizeDiff);
	}
}

void LightCommandBuffer::Clear() { lights_.clear(); }
}    // namespace neko::vk