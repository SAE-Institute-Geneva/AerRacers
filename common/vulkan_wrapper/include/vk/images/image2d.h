#pragma once
#include "vk/vulkan_include.h"

namespace neko::vk
{
class Image2d
{
public:
    Image2d(const VkExtent2D& extent,
        VkFormat format,
        VkImageTiling tiling,
        VkImageUsageFlags usage,
        VkMemoryPropertyFlags properties,
        uint32_t mipLevels,
        uint32_t arrayLayers);
    ~Image2d();

    void CreateSampler(
        VkFilter filter,
        VkSamplerAddressMode addressMode,
        bool anisotropic,
        uint32_t mipLevels);
private:
    VkExtent2D extent_;

    VkImage image_ = nullptr;
    VkImageView view_ = nullptr;
    VkImageTiling tiling_;
    VkImageUsageFlags usage_;

    VkFormat format_;

    VkSampler sampler_ = nullptr;
    VkDeviceMemory memory_ = nullptr;
    VkMemoryPropertyFlags properties_;

    inline static const float kMaxAnisotropy = 16.0f;
};
}