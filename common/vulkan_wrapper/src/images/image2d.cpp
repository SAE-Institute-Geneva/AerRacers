#include "vk/images/image2d.h"

#include <utility>

namespace neko::vk
{
Image2d::Image2d(
        std::string filename,
        VkFormat format,
        VkFilter filter,
        VkSamplerAddressMode addressMode,
        bool anisotropic,
        bool mipmap,
        bool load)
        : Image(filter, addressMode, VK_SAMPLE_COUNT_1_BIT,
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        format, 1, 1, { 0, 0, 1 }),
        filename_(std::move(filename)),
        anisotropic_(anisotropic),
        mipmap_(mipmap)
{
    if (load) Load();
}

Image2d::Image2d(
        const Vec2u extent,
        const VkFormat format,
        const VkImageLayout layout,
        const VkImageUsageFlags usage,
        const VkFilter filter,
        const VkSamplerAddressMode addressMode,
        const VkSampleCountFlagBits samples,
        const bool anisotropic,
        const bool mipmap)
        : Image(filter, addressMode, samples, layout,
              usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
              format, 1, 1, { static_cast<std::uint32_t>(extent.x), static_cast<std::uint32_t>(extent.y), 1 }),
        anisotropic_(anisotropic),
        mipmap_(mipmap),
        components_(4)
{
    Load();
}

void Image2d::Load()
{
    /*if (!filename_.empty() && !loadBitmap)
    {
        loadBitmap = std::make_unique<Bitmap>(filename_, FileType::TEXTURE);
        extent_ = { loadBitmap->GetSize().x, loadBitmap->GetSize().y, 1 };
        components_ = loadBitmap->GetBytesPerPixel();
    }*/

    if (extent_.width == 0 || extent_.height == 0)
        return;

    mipLevels_ = mipmap_ ? GetMipLevels(extent_) : 1;

    image_ = CreateImage(memory_, extent_, format_, sample_,
            VK_IMAGE_TILING_OPTIMAL, usage_,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            mipLevels_, 1, VK_IMAGE_TYPE_2D);

    sampler_ = CreateImageSampler(filter_, addressMode_, anisotropic_, mipLevels_);

    view_ = CreateImageView(image_, VK_IMAGE_VIEW_TYPE_2D, format_,
            VK_IMAGE_ASPECT_COLOR_BIT, mipLevels_, 0, 1, 0);

    if (/*!loadBitmap ||*/ mipmap_)
    {
        TransitionImageLayout(
                image_,
                VK_IMAGE_LAYOUT_UNDEFINED,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                VK_IMAGE_ASPECT_COLOR_BIT,
                mipLevels_,
                0,
                1,
                0);
    }

    /*if (loadBitmap)
    {
        const Buffer stagingBuffer(
                loadBitmap->GetLength(),
                VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        char* data;
        stagingBuffer.MapMemory(&data);
        std::memcpy(data, loadBitmap->GetData().get(), stagingBuffer.GetSize());
        stagingBuffer.UnmapMemory();

        CopyBufferToImage(
                stagingBuffer.GetBuffer(),
                image_,
                extent_,
                1,
                0);
    }*/

    if (mipmap_)
    {
        CreateMipmaps(
                image_,
                extent_,
                format_,
                layout_,
                mipLevels_,
                0,
                arrayLayers_);
    }
    /*else if (loadBitmap)
    {
        TransitionImageLayout(
                image_,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                layout_,
                VK_IMAGE_ASPECT_COLOR_BIT,
                mipLevels_,
                0,
                arrayLayers_,
                0);
    }*/
    else
    {
        TransitionImageLayout(
                image_,
                VK_IMAGE_LAYOUT_UNDEFINED,
                layout_,
                VK_IMAGE_ASPECT_COLOR_BIT,
                mipLevels_,
                0,
                arrayLayers_,
                0);
    }
}
}