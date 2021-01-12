#pragma once
#include <map>
#include <memory>

#include "mathematics/hash.h"
#include "utilities/service_locator.h"
#include "vk/images/image2d.h"

namespace neko::vk
{
using ResourceHash = XXH64_hash_t;

class ITextureManager
{
public:
	virtual ~ITextureManager() = default;

	virtual ResourceHash AddTexture2d(const std::string& texturePath) = 0;
	[[nodiscard]] virtual const Image2d& GetImage2d(ResourceHash resourceID) const = 0;
	[[nodiscard]] virtual const Image2d& GetImage2d(const std::string& texturePath) const = 0;

	virtual void Clear() = 0;
};

class NullTextureManager : public ITextureManager
{
public:
	ResourceHash AddTexture2d([[maybe_unused]] const std::string &texturePath) override { return 0; }
	[[nodiscard]] const Image2d& GetImage2d([[maybe_unused]] ResourceHash resourceID) const override
	{ neko_assert(false, "Texture Manager is Null!") }

	[[nodiscard]] const Image2d& GetImage2d([[maybe_unused]] const std::string& texturePath) const override
	{ neko_assert(false, "Texture Manager is Null!") }

	void Clear() override {}
};

class TextureManager : public ITextureManager
{
public:
	TextureManager();

	ResourceHash AddTexture2d(const std::string &texturePath) override;
	[[nodiscard]] const Image2d& GetImage2d(ResourceHash resourceID) const override;
	[[nodiscard]] const Image2d& GetImage2d(const std::string &texturePath) const override;

	void Clear() override;

private:
	std::map<ResourceHash, std::unique_ptr<Image2d>> textures2d_;
};

using TextureManagerLocator = Locator<ITextureManager, NullTextureManager>;
}
