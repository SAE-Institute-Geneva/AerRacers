#include "vk/images/texture_manager.h"

namespace neko::vk
{
TextureManager::TextureManager()
{
	TextureManagerLocator::provide(this);
}

ResourceHash TextureManager::AddTexture2d(const std::string& texturePath)
{
	const ResourceHash resourceId = HashString(texturePath);
	textures2d_.emplace(
			resourceId,
			std::make_unique<Image2d>(
				texturePath,
				VK_FORMAT_R8G8B8A8_UNORM,
				VK_FILTER_LINEAR,
				VK_SAMPLER_ADDRESS_MODE_REPEAT,
				true,
				true,
				false
			));
	textures2d_[resourceId]->Load();
	return resourceId;
}

const Image2d& TextureManager::GetImage2d(const std::string& texturePath) const
{
	return GetImage2d(HashString(texturePath));
}

const Image2d& TextureManager::GetImage2d(ResourceHash resourceID) const
{
	const auto it = textures2d_.find(resourceID);
	if(it != textures2d_.end())
	{
		return *it->second;
	}
	return Image2d("");
}

void TextureManager::Clear()
{
	textures2d_.clear();
}
}