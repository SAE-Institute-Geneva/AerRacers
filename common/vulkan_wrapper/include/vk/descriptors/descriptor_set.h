#pragma once
#include "mathematics/matrix.h"

#include "vk/core/logical_device.h"
#include "vk/buffers/uniform_buffer.h"
#include "vk/descriptors/descriptor_pool.h"
#include "vk/framebuffers/swapchain.h"

namespace neko::vk
{
class DescriptorSets
{
public:
    explicit DescriptorSets() = default;

    void InitLayout();

    void Init(const Swapchain& swapchain,
              const std::vector<UniformBuffer>& uniformBuffers,
              const DescriptorPool& descriptorPool,
              const VkDeviceSize& uboSize);
    void Destroy() const;

    explicit operator const VkDescriptorSetLayout &() const { return descriptorSetLayout_; }
    [[nodiscard]] const std::vector<VkDescriptorSet>& GetDescriptorSets() const { return descriptorSets_; }
    [[nodiscard]] const VkDescriptorSetLayout& GetDescriptorSetLayout() const { return descriptorSetLayout_; }
    VkDescriptorSetLayout& GetDescriptorSetLayout() { return descriptorSetLayout_; }

    const VkDescriptorSet& operator[](const size_t index) const
    { return descriptorSets_[index]; }

    VkDescriptorSet& operator[](const size_t index)
    { return descriptorSets_[index]; }

private:
    std::vector<VkDescriptorSet> descriptorSets_{};
    VkDescriptorSetLayout descriptorSetLayout_{};
};
}