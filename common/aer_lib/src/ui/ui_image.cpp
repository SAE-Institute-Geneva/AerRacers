#include "aer/ui/ui_image.h"

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

	glCheckError();
	// configure VAO/VBO for texture quads
	// -----------------------------------
	glGenVertexArrays(1, &quad_.VAO);
	glGenBuffers(1, &quad_.VBO[0]);
	glBindVertexArray(quad_.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, quad_.VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4f) * 6, nullptr, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vec4f), nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glCheckError();
}

void UiImage::Draw(
	gl::TextureManager& textureManager, uint8_t playerNmb, const gl::Shader& uiImageShader)
{
	const float multiplier = (playerNmb > 1u && screenId_ != 0u ? 0.5f : 1.0f);
	Vec2f size             = Vec2f(size_) * multiplier;

	const Vec2f anchoredPosition = Vec2f(GetPosition(playerNmb, size));
	size *= slidingCrop_;
	SetValues(size * 0.5f, anchoredPosition);

	if (textureName_ == INVALID_TEXTURE_NAME)
	{
		textureName_ = textureManager.GetTextureName(textureId_);
		return;
	}

	uiImageShader.SetTexture("tex", textureName_, 0);
	uiImageShader.SetVec4("color", color_);
	uiImageShader.SetVec2("slidingCrop", slidingCrop_);
	Draw();
}
#else
void UiImage::Init() {}

void UiImage::Draw(const Vec2u&) {}
#endif

void UiImage::Destroy()
{
#ifdef NEKO_GLES3
	glDeleteVertexArrays(1, &quad_.VAO);

	gl::DestroyTexture(textureName_);
#else
#endif
}

#ifdef NEKO_GLES3
void UiImage::Draw() const
{
	glBindVertexArray(quad_.VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void UiImage::SetValues(const Vec2f size, const Vec2f offset)
{
	const Vec4f vertices[6] = {
		{offset.x - size.x, offset.y + size.y, 0.0f, 0.0f},    // Top Left
		{offset.x - size.x, offset.y - size.y, 0.0f, 1.0f},    // Bottom Left
		{offset.x + size.x, offset.y - size.y, 1.0f, 1.0f},    // Bottom Right

		{offset.x - size.x, offset.y + size.y, 0.0f, 0.0f},    // Top Left
		{offset.x + size.x, offset.y - size.y, 1.0f, 1.0f},    // Bottom Right
		{offset.x + size.x, offset.y + size.y, 1.0f, 0.0f}};   // Top Right

	glBindBuffer(GL_ARRAY_BUFFER, quad_.VBO[0]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
#endif

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
