#pragma once
#include "graphics/font.h"

namespace neko::vk
{
struct Character
{
	Image2d texture;     // Glyph texture
	Vec2i size;          // Size of glyph
	Vec2i bearing;       // Offset from baseline to left/top of glyph
	long advance = 0;    // Horizontal offset to advance to next glyph
};

struct Font
{
	std::array<Character, 128> characters;
};

class FontManager : public neko::FontManager
{
public:
	explicit FontManager(const FilesystemInterface& filesystem);

	void Init() override;
	void Render() override {}
	void Destroy() override;
	void DestroyFont(FontId font) override;

	FontId LoadFont(std::string_view fontPath, int pixelHeight) override;
	void RenderText(
		FontId fontId, std::string text, Vec2i position, float scale, const Color4& color) override;

	void SetWindowSize(const Vec2f& windowSize) override;

	[[nodiscard]] Vec2i CalculateTextSize(
		FontId fontId, std::string_view text, float scale) override;

private:
	const FilesystemInterface& filesystem_;

	std::map<FontId, Font> fonts_;

	Mat4f projection_;
};
}    // namespace neko::vk
