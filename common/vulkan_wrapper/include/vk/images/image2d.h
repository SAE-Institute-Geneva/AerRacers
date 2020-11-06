#pragma once
#include "vk/images/image.h"
#include "mathematics/vector.h"

namespace neko::vk
{
class Image2d final : public Image
{
public:
    explicit Image2d(
            std::string filename,
            VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
            VkFilter filter = VK_FILTER_LINEAR,
            VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_REPEAT,
            bool anisotropic = true,
            bool mipmap = true,
            bool load = true);

    explicit Image2d(
            Vec2u extent,
            VkFormat format = VK_FORMAT_R8G8B8A8_UNORM,
            VkImageLayout layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
            VkImageUsageFlags usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_STORAGE_BIT,
            VkFilter filter = VK_FILTER_LINEAR,
            VkSamplerAddressMode addressMode = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
            VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT,
            bool anisotropic = false,
            bool mipmap = false);

    void Load();

private:
    std::string filename_ = {};

    bool anisotropic_;
    bool mipmap_;

    std::uint32_t components_ = 0;
};
}