#pragma once
#include <vector>

#include "mathematics/matrix.h"

#include "vk/buffers/buffer.h"
#include "vk/descriptors/descriptor_interface.h"

namespace neko::vk
{
struct UniformBufferObject
{
    Mat4f model = Mat4f::Identity;
    Mat4f view = Mat4f::Identity;
    Mat4f proj = Mat4f::Identity;
};

struct UniformBuffer final : IDescriptor, Buffer
{
    explicit UniformBuffer(VkDeviceSize size,
            const std::vector<char>& uniformData = {});

    void Update(const std::vector<char>& newUniformData) const;
	void Destroy() const override;

    static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(
            std::uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stage);

    [[nodiscard]] WriteDescriptorSet GetWriteDescriptor(std::uint32_t binding,
            VkDescriptorType descriptorType) const override;
};
}