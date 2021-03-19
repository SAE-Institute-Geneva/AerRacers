#include "aer/ui/ui_image.h"

#include "aer/ui/ui_element.h"

namespace neko::aer
{
UiImage::UiImage(const std::string_view& texturePath,
    const Vec2f& position,
    const Vec2u& size,
    UiAnchor anchor,
	uint8_t screenId,
	const Color4& color)
    : UiElement(position, anchor, screenId),
	  size_(size),
      texturePath_(std::move(texturePath)),
	  color_(color)
{
}

void UiImage::Init(gl::TextureManager& textureManager)
{
	textureId_ = textureManager.LoadTexture(texturePath_, Texture::DEFAULT);
	textureName_ = textureManager.GetTextureName(textureId_);
	quad_ = gl::RenderQuad(Vec3f::zero, Vec2f::one);
	quad_.Init();
}

void UiImage::Draw(gl::TextureManager& textureManager, const Vec2u& screenSize, const gl::Shader& uiImageShader)
{
	const Vec2f normalSpaceSize = Vec2f(size_ * 2.0f) / Vec2f(screenSize);
	const Vec3f anchoredPosition = Vec3f(CalculateUiElementPosition(Vec2f(position_), Vec2f(screenSize), uiAnchor_));
	quad_.SetValues(normalSpaceSize, anchoredPosition);
	glActiveTexture(GL_TEXTURE0);
	if (textureName_ == INVALID_TEXTURE_NAME) {
	    textureName_ = textureManager.GetTextureName(textureId_);
		return;
	}
	glBindTexture(GL_TEXTURE_2D, textureName_);
	uiImageShader.SetVec4("imageColor", color_);
	quad_.Draw();
}

void UiImage::Destroy()
{
	quad_.Destroy();
	gl::DestroyTexture(textureName_);
}

void UiImage::ChangeTexture(gl::TextureManager& textureManager, const std::string& texturePath)
{
    texturePath_ = texturePath;
    textureId_   = textureManager.LoadTexture(texturePath_, Texture::DEFAULT);
    textureName_ = textureManager.GetTextureName(textureId_);
}
}
