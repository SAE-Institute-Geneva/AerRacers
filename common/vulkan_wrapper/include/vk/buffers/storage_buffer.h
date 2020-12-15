#pragma once
#include "vk/buffers/buffer.h"
#include "vk/descriptors/descriptor_interface.h"

namespace neko::vk
{
class StorageBuffer final : public IDescriptor, public Buffer
{
public:
    explicit StorageBuffer(
            const VkDeviceSize& size,
            const std::vector<char>& data = {});

    void Update(const std::vector<char>& newStorageData) const;
	void Destroy() const override;

    static VkDescriptorSetLayoutBinding GetDescriptorSetLayout(
            std::uint32_t binding,
            VkDescriptorType descriptorType,
            VkShaderStageFlags stage);

    [[nodiscard]] WriteDescriptorSet GetWriteDescriptor(
            std::uint32_t binding,
            VkDescriptorType descriptorType) const override;
};
}