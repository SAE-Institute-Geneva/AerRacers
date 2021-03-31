#include "aer/ui/ui_image.h"

namespace neko::aer
{
UiImage::UiImage(std::string_view texturePath,
	Vec2i position,
	Vec2u size,
	UiAnchor anchor,
	std::uint8_t screenId,
	const Color4& color)
   : UiElement(position, anchor, screenId), size_(size), texturePath_(texturePath), color_(color)
{}

#ifdef NEKO_GLES3
void UiImage::Init(gl::TextureManager& textureManager)
{
	textureId_ = textureManager.LoadTexture(texturePath_, Texture::DEFAULT);
	textureName_ = textureManager.GetTextureName(textureId_);
	quad_ = gl::RenderQuad(Vec3f::zero, Vec2f::one);
	quad_.Init();
}

void UiImage::Draw(gl::TextureManager& textureManager,
	Vec2u screenSize,
	uint8_t playerNmb,
	const gl::Shader& uiImageShader)
{
	const float multiplier = (playerNmb > 1u && screenId_ != 0u ? 1.0f : 2.0f);
	Vec2f normalSpaceSize = Vec2f(size_) * multiplier / Vec2f(screenSize);

	const Vec3f anchoredPosition = Vec3f(GetPosition(playerNmb));
	normalSpaceSize = Vec2f(normalSpaceSize.x * slidingCrop_.x, normalSpaceSize.y * slidingCrop_.y);
	quad_.SetValues(normalSpaceSize, anchoredPosition);

	if (textureName_ == INVALID_TEXTURE_NAME)
	{
		textureName_ = textureManager.GetTextureName(textureId_);
		return;
	}

	uiImageShader.SetTexture("tex", textureName_, 0);
	uiImageShader.SetVec4("imageColor", color_);
	uiImageShader.SetVec2("slidingCrop", slidingCrop_);
	quad_.Draw();
}
#else
void UiImage::Init() {}

void UiImage::Draw(const Vec2u&) {}
#endif

void UiImage::Destroy()
{
#ifdef NEKO_GLES3
	quad_.Destroy();
	gl::DestroyTexture(textureName_);
#else
#endif
}

#ifdef NEKO_GLES3
void UiImage::ChangeTexture(gl::TextureManager& textureManager, const std::string& texturePath)
{
    texturePath_ = texturePath;
    textureId_   = textureManager.LoadTexture(texturePath_, Texture::DEFAULT);
    textureName_ = textureManager.GetTextureName(textureId_);
}
#else
void UiImage::ChangeTexture(const std::string& texturePath)
{
	texturePath_ = texturePath;
	textureId_   = vk::TextureManagerLocator::get().AddTexture(texturePath_, Texture::DEFAULT);
}
#endif

Vec2i UiImage::GetPosition(const std::uint8_t playerNmb) const
{
	Vec2i anchoredPosition = GetAnchorPosition(position_);
	anchoredPosition       = FixAnchorPosition(anchoredPosition, playerNmb);
	Vec2i slidingOffset(size_.x * (1.0f - slidingCrop_.x), size_.y * (1.0f - slidingCrop_.y));

	return anchoredPosition - slidingOffset * 0.5f;
}

Vec2i UiImage::FixAnchorPosition(Vec2i anchorPos, std::uint8_t playerNmb) const
{
	Vec2i sizeOffset = Vec2i::zero;
	if (uiAnchor_ != UiAnchor::CENTER)
	{
		if (anchorPos.x != 0.0f) sizeOffset.x = size_.x * 0.5f * Signf(anchorPos.x);
		if (anchorPos.y != 0.0f) sizeOffset.y = size_.y * 0.5f * Signf(anchorPos.y);
	}

	anchorPos = GetAnchorFromScreenId(anchorPos, playerNmb);
	return anchorPos - sizeOffset;
}
}    // namespace neko::aer
