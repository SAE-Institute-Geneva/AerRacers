#pragma once
#include "vk/descriptors/write_desctiptor_set.h"

namespace neko::vk
{
class WriteDescriptorSet;
class IDescriptor
{
public:
    IDescriptor() = default;
    virtual ~IDescriptor() = default;
	virtual void Destroy() const = 0;

    [[nodiscard]] virtual WriteDescriptorSet GetWriteDescriptor(
            std::uint32_t binding, VkDescriptorType descriptorType) const = 0;
};
}