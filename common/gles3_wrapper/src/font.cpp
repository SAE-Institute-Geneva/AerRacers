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
FontManager::FontManager(const FilesystemInterface& filesystem) : filesystem_(filesystem) {}

void FontManager::Init()
{
	InitQuad();
	textShader_.LoadFromFile(
		GetGlShadersFolderPath() + "ui_text.vert", GetGlShadersFolderPath() + "ui_text.frag");
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
	const float scale,
	const Color4& color)
{
	textShader_.Bind();
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Render Text");
#endif
	auto& font = fonts_[fontId];

	// Activate corresponding render state
	textShader_.SetVec4("color", color);
	textShader_.SetVec2("slidingCrop", Vec2f::one);

	glBindVertexArray(quad_.VAO);
	const Vec2f textSize = Vec2f(CalculateTextSize(fontId, text, scale));
	float x              = position.x - textSize.x * 0.5f;
	float y              = position.y - textSize.y * 0.5f;

	// Iterate through all characters
	for (const auto* c = text.c_str(); *c != 0; c++)
	{
		const Character& character = font.characters[*c];

		const float xPos = x + character.bearing.x * scale * 0.5f;
		const float yPos = y - static_cast<float>(character.size.y - character.bearing.y) * scale;
		const float w = character.size.x * scale;
		const float h = character.size.y * scale;

		// Bind shader values and draw
		Mat4f model = Transform3d::Scale(Mat4f::Identity, Vec3f(w, h, 1.0f));
		model       = Transform3d::Translate(model, Vec3f(xPos + w * 0.5f, yPos + h * 0.5f, 0.0f));
		textShader_.SetMat4("model", model);
		textShader_.SetTexture("tex", character.textureName, 0);
		DrawQuad();

		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += static_cast<float>(character.advance >> 6) * scale;
		// Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
	}

	glBindTexture(GL_TEXTURE_2D, 0);
}

void FontManager::Destroy()
{
    for(auto& font : fonts_)
        for(auto& character : font.second.characters)
            glDeleteTextures(1, &character.textureName);

    fonts_.clear();
    glDeleteVertexArrays(1, &quad_.VAO);
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

void FontManager::SetWindowSize(const Vec2f& windowSize)
{
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

void FontManager::InitQuad()
{
	// FT's glyphs are upside down so we need to invert the y tex coord
	Vec4f vertices[4] = {
		{-0.5f,  0.5f, 0.0f, 0.0f},    // Top Left
		{-0.5f, -0.5f, 0.0f, 1.0f},    // Bottom Left
		{ 0.5f, -0.5f, 1.0f, 1.0f},    // Bottom Right
		{ 0.5f,  0.5f, 1.0f, 0.0f},    // Top Right
	};

	unsigned indices[6] = {0, 1, 3, 1, 2, 3};

	glCheckError();
	// configure VAO/VBO for texture quads
	// -----------------------------------
	glGenBuffers(1, &quad_.VBO[0]);
	glGenBuffers(1, &quad_.EBO);
	glGenVertexArrays(1, &quad_.VAO);
	glBindVertexArray(quad_.VAO);

	glBindBuffer(GL_ARRAY_BUFFER, quad_.VBO[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vec4f), nullptr);
	glEnableVertexAttribArray(0);
	glCheckError();

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_.EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glCheckError();
}

void FontManager::DrawQuad() const
{
	glBindVertexArray(quad_.VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
}    // namespace neko::gl
