#pragma once
#include "vk/buffers/buffer.h"

namespace neko::vk
{
class CommandBuffer;

class InstanceBuffer : public Buffer
{
public:
	explicit InstanceBuffer(VkDeviceSize size);

	void Update(const std::vector<char>& newInstanceData) const;
};
}
