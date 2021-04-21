#include "vk/vk_resources.h"

namespace neko::vk
{
Image2d::Image2d(std::string_view filename,
	VkFormat format,
	VkFilter filter,
	VkSamplerAddressMode addressMode,
	bool anisotropic,
	bool mipmap,
	bool load)
   : Image(filter,
		 addressMode,
		 VK_SAMPLE_COUNT_1_BIT,
		 kLayout,
		 VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
			 VK_IMAGE_USAGE_SAMPLED_BIT,
		 format,
		 1,
		 1,
		 {0, 0, 1}),
	 filePath_(filename),
	 anisotropic_(anisotropic),
	 mipmap_(mipmap)
{
	VkFormatProperties properties;
	vkGetPhysicalDeviceFormatProperties(VkResources::Inst->gpu, format_, &properties);
	neko_assert(properties.linearTilingFeatures != 0 && properties.optimalTilingFeatures != 0,
		fmt::format("Unsupported format: {}", format_));

	if (load) Load();
}

Image2d::Image2d(const Vec2u extent,
	const VkFormat format,
	const VkImageLayout layout,
	const VkImageUsageFlags usage,
	const VkFilter filter,
	const VkSamplerAddressMode addressMode,
	const VkSampleCountFlagBits samples,
	const bool anisotropic,
	const bool mipmap)
   : Image(filter,
		 addressMode,
		 samples,
		 layout,
		 usage | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
			 VK_IMAGE_USAGE_SAMPLED_BIT,
		 format,
		 1,
		 1,
		 {static_cast<std::uint32_t>(extent.x), static_cast<std::uint32_t>(extent.y), 1}),
	 anisotropic_(anisotropic),
	 mipmap_(mipmap),
	 components_(4)
{
	VkFormatProperties properties;
	vkGetPhysicalDeviceFormatProperties(VkResources::Inst->gpu, format_, &properties);
	neko_assert(properties.linearTilingFeatures != 0 && properties.optimalTilingFeatures != 0,
		fmt::format("Unsupported format: {}", format_));

	Load();
}

Image2d& Image2d::operator=(const Image2d& other) noexcept
{
	extent_      = other.extent_;
	format_      = other.format_;
	sample_      = other.sample_;
	usage_       = other.usage_;
	mipLevels_   = other.mipLevels_;
	arrayLayers_ = other.arrayLayers_;
	filter_      = other.filter_;
	addressMode_ = other.addressMode_;
	layout_      = other.layout_;
	image_       = other.image_;
	memory_      = other.memory_;
	sampler_     = other.sampler_;
	view_        = other.view_;
	filePath_    = other.filePath_;
	anisotropic_ = other.anisotropic_;
	mipmap_      = other.mipmap_;
	components_  = other.components_;
	return *this;
}

void Image2d::Load()
{
	neko::Image image;
	image.data = stbi_load(
		filePath_.c_str(), &image.width, &image.height, &image.nbChannels, STBI_rgb_alpha);

	CreateFromStb(image);
}

void Image2d::CreateFromStb(const neko::Image& image)
{
	extent_    = {image.width, image.height, 1};
	mipLevels_ = mipmap_ ? GetMipLevels(extent_) : 1u;
	image_     = CreateImage(extent_,
        format_,
        sample_,
        kTiling,
        usage_,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        mipLevels_,
        1,
        GetType());

	sampler_ = CreateImageSampler(filter_, addressMode_, anisotropic_, mipLevels_);
	view_    = CreateImageView(image_, GetViewType(), format_, kAspect, mipLevels_, 0, 1, 0);

	// Copy pixel data to image
	VkDeviceSize imageSize = image.width * image.height * image.nbChannels;
	const Buffer stagingBuffer(imageSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	char* data;
	stagingBuffer.MapMemory(&data);
	std::memcpy(data, image.data, stagingBuffer.GetSize());
	stagingBuffer.UnmapMemory();

	CopyBufferToImage(stagingBuffer.GetBuffer(), image_, extent_, 1, 0);

	// Change the image layout as needed
	if (mipmap_)
	{
		TransitionImageLayout(image_,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			kAspect,
			mipLevels_,
			0,
			1,
			0);

		CreateMipmaps(image_, extent_, format_, layout_, mipLevels_, 0, arrayLayers_);
	}
	else
	{
		TransitionImageLayout(image_, kLayout, layout_, kAspect, mipLevels_, 0, arrayLayers_, 0);
	}
}
}    // namespace neko::vk