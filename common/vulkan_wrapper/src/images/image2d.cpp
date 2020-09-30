#include "vk/images/image2d.h"
#include "vk/vk_utilities.h"

namespace neko::vk
{
Image2d::Image2d(
    const VkExtent2D& extent,
    VkFormat format,
    VkImageTiling tiling,
    VkImageUsageFlags usage,
    VkMemoryPropertyFlags properties,
    uint32_t mipLevels,
    uint32_t arrayLayers) :
        extent_(extent), tiling_(tiling), usage_(usage), format_(format), properties_(properties)
{
    const auto& gpu = PhysicalDeviceLocator ::get();
    const auto& device = LogicalDeviceLocator::get();

    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = extent.width;
    imageInfo.extent.height = extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = mipLevels;
    imageInfo.arrayLayers = arrayLayers;
    imageInfo.format = format;
    imageInfo.tiling = tiling;
    imageInfo.usage = usage;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    VkResult res = vkCreateImage(VkDevice(device), &imageInfo, nullptr, &image_);
    neko_assert(res == VK_SUCCESS, "Failed to create image!")

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(VkDevice(device), image_, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(VkPhysicalDevice(gpu), memRequirements.memoryTypeBits, properties);

    res = vkAllocateMemory(VkDevice(device), &allocInfo, nullptr, &memory_);
    neko_assert(res == VK_SUCCESS, "Failed to allocate image memory!")

    vkBindImageMemory(VkDevice(device), image_, memory_, 0);
}

Image2d::~Image2d()
{
    const auto& device = LogicalDeviceLocator::get();

    vkDestroySampler(VkDevice(device), sampler_, nullptr);
    vkDestroyImageView(VkDevice(device), view_, nullptr);
    vkDestroyImage(VkDevice(device), image_, nullptr);
    vkFreeMemory(VkDevice(device), memory_, nullptr);
}

void Image2d::CreateSampler(
    const VkFilter filter,
    const VkSamplerAddressMode addressMode,
    const bool anisotropic,
    const uint32_t mipLevels)
{
    const auto& device = LogicalDeviceLocator::get();

    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = filter;
    samplerInfo.minFilter = filter;
    samplerInfo.addressModeU = addressMode;
    samplerInfo.addressModeV = addressMode;
    samplerInfo.addressModeW = addressMode;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.anisotropyEnable = anisotropic;
    samplerInfo.maxAnisotropy = kMaxAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = mipLevels;

    const VkResult res = vkCreateSampler(VkDevice(device), &samplerInfo, nullptr, &sampler_);
    neko_assert(res == VK_SUCCESS, "Failed to create texture sampler!")
}
}