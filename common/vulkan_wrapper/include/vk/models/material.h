#pragma once
#include "vk/vulkan_include.h"

namespace neko::vk
{
class Material
{
public:
    /*enum class RenderMode : uint8_t
    {
        OPAQUE,
        TRANSPARENT,
        LENGTH
    };*/

    explicit Material() = default;
    virtual ~Material() = default;
};
}