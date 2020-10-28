#pragma once
#include "vk/images/image.h"
#include "mathematics/vector.h"

namespace neko::vk
{
class ImageDepth : public Image
{
public:
    explicit ImageDepth(const Vec2u& extent, VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT);

private:
    inline static const std::vector<VkFormat> kAvailableFormats =
    {
            VK_FORMAT_D32_SFLOAT_S8_UINT,
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM_S8_UINT,
            VK_FORMAT_D16_UNORM
    };
};
}