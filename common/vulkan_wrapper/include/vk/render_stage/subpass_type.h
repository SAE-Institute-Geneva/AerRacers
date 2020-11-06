#pragma once
#include <utility>

#include "vk/vulkan_include.h"

namespace neko::vk
{
struct SubpassType
{
    SubpassType() = default;
    explicit SubpassType(const std::uint32_t binding,
                         std::vector<uint32_t> attachmentBindings,
                         const bool multisampling = false)
            : binding(binding),
              attachmentBindings(std::move(attachmentBindings)),
              multisampling(multisampling)
    {}

    std::uint32_t binding = 0;
    std::vector<std::uint32_t> attachmentBindings;
    bool multisampling = false;
};
}