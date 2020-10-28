#pragma once
#include "mathematics/matrix.h"

#include "vk/buffers/buffer.h"
#include "vk/core/logical_device.h"
#include "vk/descriptors/descriptor_set.h"
#include "vk/graphics.h"

namespace neko::vk
{
struct UniformBufferObject
{
    Mat4f model = Mat4f::Identity;
    Mat4f view = Mat4f::Identity;
    Mat4f proj = Mat4f::Identity;
};

struct UniformBuffer : public IDescriptor, public Buffer
{
    explicit UniformBuffer(VkDeviceSize size,
            const std::vector<char>& uniformData = {});

    void Update(const std::vector<char>& newUniformData) const;

    static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(
            uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stage);

    [[nodiscard]] WriteDescriptorSet GetWriteDescriptor(uint32_t binding,
            VkDescriptorType descriptorType) const override;
};
}