#pragma once
#include <array>

#include "vk/descriptors/descriptor_interface.h"

namespace neko::vk
{
class Image : public IDescriptor
{
public:
    Image(VkFilter filter,
          VkSamplerAddressMode addressMode,
          VkSampleCountFlagBits samples,
          VkImageLayout layout,
          VkImageUsageFlags usage,
          VkFormat format,
          std::uint32_t mipLevels,
          std::uint32_t arrayLayers,
          const VkExtent3D &extent);
    void Destroy() const override;

    [[nodiscard]] WriteDescriptorSet GetWriteDescriptor(
            std::uint32_t binding, VkDescriptorType descriptorType) const override;

    [[nodiscard]] const VkFormat& GetFormat() const { return format_; }
    [[nodiscard]] const VkImage& GetImage() const { return image_; }
    [[nodiscard]] const VkDeviceMemory& GetMemory() const { return memory_; }
    VkSampler& GetSampler() { return sampler_; }
    [[nodiscard]] const VkImageView& GetView() const { return view_; }

    static std::uint32_t GetMipLevels(const VkExtent3D& extent);

    static bool HasDepth(VkFormat format);
    static bool HasStencil(VkFormat format);

    static VkImage CreateImage(
            VkDeviceMemory& memory,
            VkExtent3D extent,
            VkFormat format,
            VkSampleCountFlagBits samples,
            VkImageTiling tiling,
            VkImageUsageFlags usage,
            VkMemoryPropertyFlags properties,
            std::uint32_t mipLevels,
            std::uint32_t arrayLayers,
            VkImageType type);

    static VkSampler CreateImageSampler(
            VkFilter filter,
            VkSamplerAddressMode addressMode,
            bool anisotropic,
            std::uint32_t mipLevels);

    static VkImageView CreateImageView(
            const VkImage& image,
            VkImageViewType type,
            VkFormat format,
            VkImageAspectFlags imageAspect,
            std::uint32_t mipLevels,
            std::uint32_t baseMipLevel,
            std::uint32_t layerCount,
            std::uint32_t baseArrayLayer);

    static void CreateMipmaps(
            const VkImage& image,
            VkExtent3D extent,
            VkFormat format,
            VkImageLayout dstImageLayout,
            std::uint32_t mipLevels,
            std::uint32_t baseArrayLayer,
            std::uint32_t layerCount);

    static void TransitionImageLayout(
            const VkImage& image,
            VkImageLayout srcImageLayout,
            VkImageLayout dstImageLayout,
            VkImageAspectFlags imageAspect,
            std::uint32_t mipLevels,
            std::uint32_t baseMipLevel,
            std::uint32_t layerCount,
            std::uint32_t baseArrayLayer);

    static VkFormat FindSupportedFormat(
            const std::vector<VkFormat>& candidates,
            VkImageTiling tiling,
            VkFormatFeatureFlags features);

protected:
    VkExtent3D extent_;
    VkFormat format_;
    VkSampleCountFlagBits sample_;
    VkImageUsageFlags usage_;
    std::uint32_t mipLevels_;
    std::uint32_t arrayLayers_;

    VkFilter filter_;
    VkSamplerAddressMode addressMode_;

    VkImageLayout layout_;

    VkImage image_{};
    VkDeviceMemory memory_{};
    VkSampler sampler_{};
    VkImageView view_{};

    inline static const float kAnisotropy = 16.0f;

    inline static const std::array<VkFormat, 6> kDepthFormat =
    {
            VK_FORMAT_D16_UNORM,
            VK_FORMAT_X8_D24_UNORM_PACK32,
            VK_FORMAT_D32_SFLOAT,
            VK_FORMAT_D16_UNORM_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D32_SFLOAT_S8_UINT
    };

    inline static const std::array<VkFormat, 4> kStencilFormats =
    {
            VK_FORMAT_S8_UINT,
            VK_FORMAT_D16_UNORM_S8_UINT,
            VK_FORMAT_D24_UNORM_S8_UINT,
            VK_FORMAT_D32_SFLOAT_S8_UINT
    };
};
}
