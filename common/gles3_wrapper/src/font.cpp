/*
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
 */
#include "engine/resource_locations.h"
#include "mathematics/transform.h"

#include "gl/font.h"

#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

namespace neko::gl
{

FontManager::FontManager(const FilesystemInterface& filesystem) :
        filesystem_(filesystem)
{

}

void FontManager::Init()
{
	textShader_.LoadFromFile(
		GetGlShadersFolderPath() + "ui_text.vert", GetGlShadersFolderPath() + "ui_text.frag");

	glCheckError();
	// configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &textureQuad_.VAO);
    glGenBuffers(1, &textureQuad_.VBO[0]);
    glBindVertexArray(textureQuad_.VAO);
    glBindBuffer(GL_ARRAY_BUFFER, textureQuad_.VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vec4f) * 6, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vec4f), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glCheckError();
}

FontId FontManager::LoadFont(std::string_view fontPath, int pixelHeight)
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("Load Font");
#endif
	const std::string metaPath = std::string(fontPath) + ".meta";
	auto metaJson              = LoadJson(metaPath);
	FontId fontId              = INVALID_FONT_ID;
	if (CheckJsonExists(metaJson, "uuid"))
	{
		fontId = sole::rebuild(metaJson["uuid"].get<std::string>());
	}
	else
	{
		logDebug("[Error] Could not find font id in json file");
		return fontId;
	}

	if (fontId == INVALID_FONT_ID)
	{
		logDebug("[Error] Invalid font id on texture load");
		return fontId;
	}

	auto it = fonts_.find(fontId);
	if (it != fonts_.end())
	{
		//Font is already loaded
		return it->first;
	}

	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		logDebug("[Error] Freetype could not init FreeType Library");
		return INVALID_FONT_ID;
	}

	FT_Face face;
	BufferFile fontFile = filesystem_.LoadFile(fontPath);
	if (FT_New_Memory_Face(ft, fontFile.dataBuffer, fontFile.dataLength, 0, &face))
	{
		logDebug("[Error] Freetype: Failed to load font");
		return INVALID_FONT_ID;
	}

	// set size to load glyphs as
	FT_Set_Pixel_Sizes(face, 0, pixelHeight);

	// disable byte-alignment restriction
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// load first 128 characters of ASCII set
	std::array<Character, 128> characters;
	for (unsigned char c = 0; c < 128; c++)
	{
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			logDebug("[Error] Freetype failed to load Glyph");
			continue;
		}

		// generate texture
		unsigned int texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer);

		// set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		// now store character for later use
		Character character = {texture,
			Vec2i(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			Vec2i(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x};
		characters[c]       = character;
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	// destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
    fonts_[fontId] = {characters};
    return fontId;
}

void FontManager::RenderText(const FontId fontId,
	const std::string text,
	const Vec2i position,
	const TextAnchor anchor,
	const float scale,
	const Color4& color)
{
	textShader_.Bind();
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Render Text");
#endif
	auto& font = fonts_[fontId];

	// activate corresponding render state
	textShader_.SetVec4("color", color);
	textShader_.SetVec2("slidingCrop", Vec2f::one);
	glBindVertexArray(textureQuad_.VAO);

	const Vec2i textSize = CalculateTextSize(fontId, text, scale);
	float x              = position.x - textSize.x * 0.5f;
	float y              = position.y - textSize.y * 0.5f;

	// iterate through all characters
	for (const auto* c = text.c_str(); *c != 0; c++)
	{
		const Character& character = font.characters[*c];

		const float xPos = x + character.bearing.x * scale * 0.5f;
		const float yPos = y - static_cast<float>(character.size.y - character.bearing.y) * scale;
		const float w = character.size.x * scale;
		const float h = character.size.y * scale;

		// update VBO for each character
		const Vec4f vertices[6] = {{xPos, yPos + h, 0.0f, 0.0f},
			{xPos, yPos, 0.0f, 1.0f},
			{xPos + w, yPos, 1.0f, 1.0f},

			{xPos, yPos + h, 0.0f, 0.0f},
			{xPos + w, yPos, 1.0f, 1.0f},
			{xPos + w, yPos + h, 1.0f, 0.0f}};

		// render glyph texture over quad
		textShader_.SetTexture("tex", character.textureName, 0);

		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, textureQuad_.VBO[0]);

		// be sure to use glBufferSubData and not glBufferData
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += static_cast<float>(character.advance >> 6) * scale;
		// bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void FontManager::Destroy()
{
    for(auto& font : fonts_)
        for(auto& character : font.second.characters)
            glDeleteTextures(1, &character.textureName);

    fonts_.clear();
    glDeleteVertexArrays(1, &textureQuad_.VAO);
}

void FontManager::Render()
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("Render Font Manager");
#endif
}

void FontManager::DestroyFont(FontId font)
{
	auto it = fonts_.find(font);
	if (it == fonts_.end()) return;
	for (auto& character : it->second.characters) glDeleteTextures(1, &character.textureName);
	fonts_.erase(font);
}

Vec2f FontManager::CalculateTextPosition(Vec2f position, TextAnchor anchor)
{
	Vec2f anchorPos;
    switch (anchor)
    {
		case TextAnchor::TOP_LEFT:     anchorPos = Vec2f::up; break;
		case TextAnchor::TOP:          anchorPos = Vec2f(0.5f, 1.0f); break;
		case TextAnchor::TOP_RIGHT:    anchorPos = Vec2f::one; break;

		case TextAnchor::CENTER_LEFT:  anchorPos = Vec2f(0.0f, 0.5f); break;
		case TextAnchor::CENTER:       anchorPos = Vec2f(0.5f, 0.5f); break;
		case TextAnchor::CENTER_RIGHT: anchorPos = Vec2f(1.0f, 0.5f); break;

		case TextAnchor::BOTTOM_LEFT:  anchorPos = Vec2f::zero; break;
		case TextAnchor::BOTTOM:       anchorPos = Vec2f(0.5f, 0.0f); break;
		case TextAnchor::BOTTOM_RIGHT: anchorPos = Vec2f::right; break;
	}

	return anchorPos * windowSize_ + position * 0.5f;
}

void FontManager::SetWindowSize(const Vec2f& windowSize)
{
	windowSize_ = windowSize;
	projection_ = Transform3d::Orthographic(0.0f, windowSize.x, 0.0f, windowSize.y);
}

Vec2i FontManager::CalculateTextSize(FontId fontId, std::string_view text, float scale)
{
	Vec2f size;
	const Font& font = fonts_[fontId];
	for (const auto* c = text.data(); *c != 0; c++)
	{
		const Character& ch = font.characters[*c];
		size.x += static_cast<float>(ch.advance >> 6) * scale;
		size.y = std::max(size.y * scale, ch.size.y * scale);
	}

	return Vec2i(size);
}
}    // namespace neko::gl
