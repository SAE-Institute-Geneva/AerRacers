#include "vk/images/image2d.h"

#include <utility>

#include "engine/engine.h"
#include "utils/file_utility.h"

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
		 VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		 VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
			 VK_IMAGE_USAGE_SAMPLED_BIT,
		 format,
		 1,
		 1,
		 {0, 0, 1}),
	 filePath_(std::move(filename)),
	 anisotropic_(anisotropic),
	 mipmap_(mipmap)
{
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
	Load();
}

void Image2d::Load()
{
	if (GetFilenameExtension(filePath_) == ".ktx") LoadKtx();
	else neko_assert(false, fmt::format("{} isn't a valid KTX image!", filePath_))
}

void Image2d::LoadKtx()
{
	VkResources* vkObj = VkResources::Inst;
	ktxVulkanDeviceInfo vdi;
	ktxVulkanTexture texture;
	ktxTexture* kTexture;
	ktxVulkanDeviceInfo_Construct(&vdi,
		vkObj->gpu,
		vkObj->device,
		vkObj->device.GetGraphicsQueue(),
		vkObj->GetCurrentCmdPool(),
		nullptr);
	ktxTexture_CreateFromNamedFile((filePath_).c_str(), KTX_TEXTURE_CREATE_NO_FLAGS, &kTexture);
	const auto err = ktxTexture_VkUploadEx(kTexture,
		&vdi,
		&texture,
		VK_IMAGE_TILING_OPTIMAL,
		usage_,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	if (err != KTX_SUCCESS)
		logDebug(fmt::format("KTX Error: {} for file '{}'", ktxErrorString(err), filePath_));
	ktxTexture_Destroy(kTexture);
	ktxVulkanDeviceInfo_Destruct(&vdi);

	SetFromKtxVkTexture(texture);

	if (extent_.width == 0 || extent_.height == 0) return;

	sampler_ = CreateImageSampler(filter_, addressMode_, anisotropic_, mipLevels_);

	view_ = CreateImageView(
		image_, VK_IMAGE_VIEW_TYPE_2D, format_, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels_, 0, 1, 0);

	if (mipmap_)
	{
		TransitionImageLayout(image_,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_IMAGE_ASPECT_COLOR_BIT,
			mipLevels_,
			0,
			1,
			0);

		CreateMipmaps(image_, extent_, format_, layout_, mipLevels_, 0, arrayLayers_);
	}
	else
	{
		TransitionImageLayout(image_,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			layout_,
			VK_IMAGE_ASPECT_COLOR_BIT,
			mipLevels_,
			0,
			arrayLayers_,
			0);
	}
}

void Image2d::SetFromKtxVkTexture(const ktxVulkanTexture& texture)
{
	image_       = texture.image;
	format_      = texture.imageFormat;
	layout_      = texture.imageLayout;
	memory_      = texture.deviceMemory;
	extent_      = {texture.width, texture.height, texture.height};
	mipLevels_   = texture.levelCount;
	arrayLayers_ = texture.layerCount;
}
}    // namespace neko::vk