#include "aer/ui/ui_image.h"

namespace neko::aer
{
UiImage::UiImage(const std::string_view& texturePath,
    const Vec2f& position,
    const Vec2u& size,
    UiAnchor anchor,
	std::uint8_t screenId,
	const Color4& color)
    : UiElement(position, anchor, screenId),
	  size_(size),
      texturePath_(texturePath),
	  color_(color)
{
}

#ifdef NEKO_GLES3
void UiImage::Init(gl::TextureManager& textureManager)
{
	textureId_ = textureManager.LoadTexture(texturePath_, Texture::TextureFlags(Texture::DEFAULT | Texture::FLIP_Y));
	textureName_ = textureManager.GetTextureName(textureId_);
	quad_ = gl::RenderQuad(Vec3f::zero, Vec2f::one);
	quad_.Init();
}

void UiImage::Draw(gl::TextureManager& textureManager, const Vec2u& screenSize, const gl::Shader& uiImageShader)
{
	Vec2f normalSpaceSize = Vec2f(size_ * 2.0f) / Vec2f(screenSize);
	const Vec3f anchoredPosition = Vec3f(CalculateUiElementPosition(position_, uiAnchor_)) + Vec3f(normalSpaceSize.x * (1.0f - slidingCrop_.x), normalSpaceSize.y * (1.0f - slidingCrop_.y), 1.0f) * -0.5f;
	normalSpaceSize = Vec2f(normalSpaceSize.x * slidingCrop_.x, normalSpaceSize.y * slidingCrop_.y);
	quad_.SetValues(normalSpaceSize, anchoredPosition);
	glActiveTexture(GL_TEXTURE0);
	if (textureName_ == INVALID_TEXTURE_NAME) {
	    textureName_ = textureManager.GetTextureName(textureId_);
		return;
	}
	glBindTexture(GL_TEXTURE_2D, textureName_);
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
}
