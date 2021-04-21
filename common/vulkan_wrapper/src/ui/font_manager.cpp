#include "vk/ui/font_manager.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

namespace neko::vk
{
FontManager::FontManager(const FilesystemInterface& filesystem) : filesystem_(filesystem) {}

void FontManager::Init() {}

void FontManager::Destroy()
{
	for (auto& font : fonts_) DestroyFont(font.first);

	fonts_.clear();
}

void FontManager::DestroyFont(FontId font)
{
	auto it = fonts_.find(font);
	if (it == fonts_.end()) return;
	for (auto& character : it->second.characters) character.texture.Destroy();
	fonts_.erase(font);
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

	// load first 128 characters of ASCII set
	std::array<Character, 128> characters;
	for (unsigned char c = 0; c < 128; c++)
	{
		// Load character glyph
		if (FT_Load_Char(face, c, FT_LOAD_DEFAULT))
		{
			logDebug("[Error] Freetype failed to load Glyph");
			continue;
		}

		if (!face->glyph->bitmap.buffer) continue;

		neko::Image image;
		image.nbChannels = 1;
		image.width      = static_cast<int>(face->glyph->bitmap.width);
		image.height     = static_cast<int>(face->glyph->bitmap.rows);
		image.data       = face->glyph->bitmap.buffer;

		Image2d texture("",
			VK_FORMAT_R8_SRGB,
			VK_FILTER_LINEAR,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			false,
			false,
			false);
		texture.CreateFromStb(image);

		// Now store character for later use
		Character character = {texture,
			Vec2i(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			Vec2i(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x};
		characters[c]       = character;
	}

	// destroy FreeType once we're finished
	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	fonts_[fontId] = {characters};
	return fontId;
}

void FontManager::RenderText(
	FontId fontId, std::string text, Vec2i position, float scale, const Color4& color)
{

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
}