#include "vk/images/texture_manager.h"

namespace neko::vk
{
TextureManager::TextureManager() { TextureManagerLocator::provide(this); }

ResourceHash TextureManager::AddTexture2d(std::string_view texturePath)
{
	const ResourceHash resourceId = HashString(texturePath);
	textures2d_.emplace(resourceId,
		std::make_unique<Image2d>(texturePath,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_FILTER_LINEAR,
			VK_SAMPLER_ADDRESS_MODE_REPEAT,
			true,
			false,
			false));
	textures2d_[resourceId]->Load();
	return resourceId;
}

std::optional_const_ref<Image2d> TextureManager::GetImage2d(std::string_view texturePath) const
{
	return GetImage2d(HashString(texturePath));
}

std::optional_const_ref<Image2d> TextureManager::GetImage2d(ResourceHash resourceId) const
{
	const auto it = textures2d_.find(resourceId);
	if (it != textures2d_.end()) { return *it->second; }

	return std::nullopt;
}

void TextureManager::Clear() { textures2d_.clear(); }
}    // namespace neko::vk