#pragma once
#include <utility>

#include "vk/vulkan_include.h"

namespace neko::vk
{
struct SubpassType
{
    SubpassType() = default;
    explicit SubpassType(uint32_t binding,
                         std::vector<uint32_t> attachmentBindings,
                         bool multisampling = false)
            : binding(binding),
              attachmentBindings(std::move(attachmentBindings)),
              multisampling(multisampling)
    {}

    uint32_t binding;
    std::vector<uint32_t> attachmentBindings;
    bool multisampling = false;
};
}