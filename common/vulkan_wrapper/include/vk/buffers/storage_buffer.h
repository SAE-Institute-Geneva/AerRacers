#pragma once
#include <vector>

#include "vk/buffers/buffer.h"
#include "vk/descriptors/descriptor_set.h"

namespace neko::vk
{
class IDescriptor;

class StorageBuffer : public IDescriptor, public Buffer
{
public:
    explicit StorageBuffer(
            const VkDeviceSize& size,
            const std::vector<char>& data = {});

    void Update(const std::vector<char>& newStorageData) const;

    static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(
            uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stage);

    [[nodiscard]] WriteDescriptorSet GetWriteDescriptor(
            uint32_t binding,
            VkDescriptorType descriptorType) const override;
};
}