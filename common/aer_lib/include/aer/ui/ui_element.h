#pragma once
/* ----------------------------------------------------
 MIT License
 Copyright (c) 2020 SAE Institute Switzerland AG
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 Author : Canas Simon
 Co-Author : Floreau Luca
 Date : 13.03.2021
---------------------------------------------------------- */
#include <utility>

#include "gl/texture.h"
#include "gl/shape.h"

namespace neko::aer
{
struct UiElement
{
	enum UiFlags : uint8_t
	{
		DIRTY = 1u << 0u,
	};
	
	explicit UiElement(const Vec3f& pos = Vec3f::zero,
	                   const Vec2u& newSize = Vec2u::one)
		: position(pos), size(newSize) {}
	
	explicit UiElement(std::string texPath, 
	                   const Vec3f& pos = Vec3f::zero,
	                   const Vec2u& newSize = Vec2u::one)
		: position(pos), size(newSize), texturePath(std::move(texPath)) {}

	void Init(const Vec2u& screenSize);
	
	void Draw(gl::TextureManager& textureManager, const Vec2u& screenSize);
	void Update(const Vec2u& screenSize);

	void Destroy();
	
	Vec3f position = Vec3f::zero; //In percent
	Vec2u size = Vec2u(100u); //In pixel

	std::string texturePath = "";
	TextureId textureId = INVALID_TEXTURE_ID;
	TextureName textureName = INVALID_TEXTURE_NAME;

	uint8_t flags = 0;

	gl::RenderQuad quad{Vec3f::zero, Vec2f::one};

};
}