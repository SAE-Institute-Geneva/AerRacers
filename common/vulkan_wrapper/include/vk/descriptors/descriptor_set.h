#pragma once
#include "vk/core/logical_device.h"

namespace neko::vk
{
class DescriptorSet
{
public:
    DescriptorSet();
    ~DescriptorSet();

    explicit operator const VkDescriptorSet &() const { return descriptorSet_; }
    [[nodiscard]] const VkDescriptorSet& GetDescriptorSet() const { return descriptorSet_; }

private:
    VkDescriptorSet descriptorSet_;
};
}