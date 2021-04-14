#include "aer/ui/ui_image.h"

#include "mathematics/transform.h"

namespace neko::aer
{
UiImage::UiImage(std::string_view texturePath,
	Vec2i position,
	Vec2u size,
	UiAnchor anchor,
	std::uint8_t screenId,
	const Color4& color)
	: UiElement(position, anchor, color, screenId), size_(size), texturePath_(texturePath)
{}

#ifdef NEKO_GLES3
void UiImage::Init(gl::TextureManager& textureManager)
{
	textureId_ = textureManager.LoadTexture(texturePath_, Texture::DEFAULT);
	textureName_ = textureManager.GetTextureName(textureId_);
}

void UiImage::Draw(
	gl::TextureManager& textureManager, const uint8_t playerNmb, const gl::Shader& uiImageShader)
{
	if (textureName_ == INVALID_TEXTURE_NAME)
	{
		textureName_ = textureManager.GetTextureName(textureId_);
		return;
	}

	const float multiplier = (playerNmb > 1u && screenId_ != 0u ? 0.5f : 1.0f);
	Vec2f size             = Vec2f(size_) * multiplier;

	const Vec2f anchoredPosition = Vec2f(GetPosition(playerNmb, size));
	size *= slidingCrop_;

	Mat4f model = Transform3d::Scale(Mat4f::Identity, Vec3f(size, 1.0f));
	model       = Transform3d::Translate(model, Vec3f(anchoredPosition));
	uiImageShader.SetMat4("model", model);
	uiImageShader.SetTexture("tex", textureName_, 0);
	uiImageShader.SetVec4("color", color_);
	uiImageShader.SetVec2("slidingCrop", slidingCrop_);
}
#else
void UiImage::Init() {}

void UiImage::Draw(const Vec2u&) {}
#endif

void UiImage::Destroy()
{
#ifdef NEKO_GLES3
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

Vec2i UiImage::GetPosition(const std::uint8_t playerNmb, Vec2f size) const
{
	Vec2i anchoredPosition = GetAnchorPosition(position_);
	anchoredPosition       = GetAnchorFromScreenId(anchoredPosition, playerNmb);
	Vec2f slidingOffset    = (Vec2f::one - slidingCrop_) * Vec2f(size);

	return anchoredPosition - Vec2i(slidingOffset * 0.5f);
}

Vec2i UiImage::FixAnchorPosition(Vec2i anchorPos, std::uint8_t playerNmb) const
{
	Vec2f sizeOffset = Vec2f::zero;
	switch (uiAnchor_)
	{
		case UiAnchor::TOP_LEFT:     sizeOffset = Vec2f::up; break;
		case UiAnchor::TOP:          sizeOffset = Vec2f(0.5f, 1.0f); break;
		case UiAnchor::TOP_RIGHT:    sizeOffset = Vec2f::one; break;

		case UiAnchor::CENTER_LEFT:  sizeOffset = Vec2f(0.0f, 0.5f); break;
		case UiAnchor::CENTER:       sizeOffset = Vec2f(0.5f, 0.5f); break;
		case UiAnchor::CENTER_RIGHT: sizeOffset = Vec2f(1.0f, 0.5f); break;

		case UiAnchor::BOTTOM_LEFT:  sizeOffset = Vec2f::zero; break;
		case UiAnchor::BOTTOM:       sizeOffset = Vec2f(0.5f, 0.0f); break;
		case UiAnchor::BOTTOM_RIGHT: sizeOffset = Vec2f::right; break;
	}

	sizeOffset = sizeOffset * Vec2f(size_) * (screenId_ != 0u ? Vec2f(0.5f) : Vec2f::one);
	anchorPos = GetAnchorFromScreenId(anchorPos, playerNmb);
	return anchorPos - Vec2i(sizeOffset);
}
}    // namespace neko::aer
