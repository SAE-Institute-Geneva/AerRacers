#include "vk/images/texture_manager.h"

namespace neko::vk
{
TextureManager::TextureManager() { TextureManagerLocator::provide(this); }

void TextureManager::Init() { TextureManagerLocator::provide(this); }

void TextureManager::Update(seconds)
{
	while(!textureLoaders_.empty())
	{
		auto& textureLoader = textureLoaders_.front();
		if(textureLoader.HasErrors())
		{
			switch (textureLoader.GetErrors())
			{
				case TextureLoader::TextureLoaderError::ASSET_LOADING_ERROR:
					logDebug(fmt::format("[Error] Could not load texture {} from disk", textureLoader.GetPath()));
					break;
				case TextureLoader::TextureLoaderError::DECOMPRESS_ERROR:
					logDebug(fmt::format("[Error] Could not decompress texture {} from disk", textureLoader.GetPath()));
					break;
				case TextureLoader::TextureLoaderError::UPLOAD_TO_GPU_ERROR:
					logDebug(fmt::format("[Error] Could not upload texture {} from disk", textureLoader.GetPath()));
					break;
				default:
					break;
			}
			textureLoaders_.pop();
		}
		else if(textureLoader.IsDone())
		{
			textureMap_[textureLoader.GetTextureId()] = textureLoader.GetTexture();
			textureLoaders_.pop();
		}
		else
		{
			break;
		}
	}
}

void TextureManager::Destroy()
{
	for (auto& textureName : textureMap_) textureName.second->Destroy();
}

ResourceHash TextureManager::AddTexture2d(std::string_view texturePath)
{
	const ResourceHash resourceId = HashString(texturePath);
	textureMap_.emplace(resourceId,
		std::make_unique<Image2d>(texturePath,
			VK_FORMAT_R8G8B8A8_SRGB,
			VK_FILTER_LINEAR,
			VK_SAMPLER_ADDRESS_MODE_REPEAT,
			true,
			false,
			false));
	textureMap_[resourceId]->Load();
	return resourceId;
}

std::optional_const_ref<Image2d> TextureManager::GetImage2d(std::string_view texturePath) const
{
	return GetImage2d(HashString(texturePath));
}

std::optional_const_ref<Image2d> TextureManager::GetImage2d(ResourceHash resourceId) const
{
	const auto it = textureMap_.find(resourceId);
	if (it != textureMap_.end()) { return *it->second; }

	logDebug("Texture doesn't exist!");
	return std::nullopt;
}

void TextureManager::Clear() { textureMap_.clear(); }
}    // namespace neko::vk