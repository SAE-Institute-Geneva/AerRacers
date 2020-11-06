#pragma once
#include "vk/vulkan_include.h"

namespace neko::vk
{
struct Attachment
{
public:
    enum class Type : std::uint8_t
    {
        IMAGE = 0,
        DEPTH,
        SWAPCHAIN,
        NONE
    };

    explicit Attachment(const std::uint32_t binding = 0,
            const char* name = "",
            const Type type = Type::NONE,
            const bool multisampling = false,
            const VkFormat format = VK_FORMAT_R8G8B8A8_UNORM)
            : binding(binding),
            name(name),
            type(type),
            multisampling(multisampling),
            format(format)
    {}

    std::uint32_t binding = 0;
    const char* name = "";
    Type type = Type::NONE;
    bool multisampling = false;
    VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;
};
}