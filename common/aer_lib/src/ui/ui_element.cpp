#include "aer/ui/ui_element.h"

namespace neko::aer
{
void UiElement::Init(const Vec2u& screenSize)
{
	const Vec2f normalSpaceSize = Vec2f(size) / Vec2f(screenSize);
	quad = gl::RenderQuad(position, normalSpaceSize);
	quad.Init();
}

void UiElement::Draw(gl::TextureManager& textureManager, const Vec2u& screenSize)
{
	glActiveTexture(GL_TEXTURE0);
	if (textureName == INVALID_TEXTURE_NAME)
	{
		textureName = textureManager.GetTextureName(textureId);
		return;
	}
	glBindTexture(GL_TEXTURE_2D, textureName);
	quad.Draw();
}

void UiElement::Update(const Vec2u& screenSize)
{
	const Vec2f normalSpaceSize = Vec2f(size) / Vec2f(screenSize);
	quad.SetValues(normalSpaceSize, position);
}

void UiElement::Destroy()
{
	quad.Destroy();
	gl::DestroyTexture(textureName);
}
}
