#include "vk/images/texture_manager.h"

namespace neko::vk
{
TextureManager::TextureManager() { TextureManagerLocator::provide(this); }

void TextureManager::Init() {}

void TextureManager::Update(seconds)
{
	while (!textureLoaders_.empty())
	{
		auto& textureLoader = textureLoaders_.front();
		if (textureLoader.HasErrors())
		{
			switch (textureLoader.error_)
			{
				case TextureLoader::TextureLoaderError::ASSET_LOADING_ERROR:
					logDebug(fmt::format(
						"[Error] Could not load texture {} from disk", textureLoader.path_));
					break;
				case TextureLoader::TextureLoaderError::DECOMPRESS_ERROR:
					logDebug(fmt::format(
						"[Error] Could not decompress texture {} from disk", textureLoader.path_));
					break;
				case TextureLoader::TextureLoaderError::UPLOAD_TO_GPU_ERROR:
					logDebug(fmt::format(
						"[Error] Could not upload texture {} from disk", textureLoader.path_));
					break;
				default: break;
			}
			textureLoaders_.pop();
		}
		else if (textureLoader.IsDone())
		{
			textureMap_[textureLoader.textureId_] = textureLoader.texture_;
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
	for (auto& textureName : textureMap_) textureName.second.Destroy();
}

ResourceHash TextureManager::AddTexture(std::string_view path)
{
	return AddTexture(path, Texture::DEFAULT);
}

ResourceHash TextureManager::AddTexture(std::string_view path, Texture::TextureFlags flags)
{
	const ResourceHash textureId = HashString(path);
	const auto it                = textureMap_.find(textureId);
	if (it != textureMap_.end()) return it->first;

	//const std::string metaPath = fmt::format("{}.meta", path.substr(0, path.size() - 4));
	//const json metaJson = LoadJson(metaPath);
	if (textureId == kInvalidTextureId)
	{
		logDebug("[Error] Invalid texture id on texture load");
		return textureId;
	}

	const auto& config = BasicEngine::GetInstance()->GetConfig();
	textureLoaders_.push({path, textureId, flags});
	textureLoaders_.back().Start();
	return textureId;
}

const Image2d* TextureManager::GetTexture(std::string_view texturePath) const
{
	return GetTexture(HashString(texturePath));
}

const Image2d* TextureManager::GetTexture(ResourceHash resourceId) const
{
	const auto it = textureMap_.find(resourceId);
	if (it != textureMap_.end()) { return &it->second; }
;
	return nullptr;
}

void TextureManager::Clear() { textureMap_.clear(); }
}    // namespace neko::vk