#pragma once
#include "mathematics/matrix.h"

#include "vk/buffers/buffer.h"
#include "vk/core/logical_device.h"

namespace neko::vk
{
struct UniformBufferObject
{
    Mat4f model = Mat4f::Identity;
    Mat4f view = Mat4f::Identity;
    Mat4f proj = Mat4f::Identity;
};

struct UniformBuffer : public Buffer
{
    void Init(const PhysicalDevice& gpu, const VkDeviceSize& uboSize);

    template<class UboType>
    void Update(const UboType& bufferData) const
    {
        static_assert(std::is_class<UboType>::value, "Type is not a class / struct");

        const auto& device = LogicalDeviceLocator::get();

        void* data = nullptr;
        vkMapMemory(VkDevice(device), memory_, 0, sizeof(bufferData), 0, &data);
        memcpy(data, &bufferData, sizeof(bufferData));
        vkUnmapMemory(VkDevice(device), memory_);
    }
};
}