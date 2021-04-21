#include "aer/ui/ui_text.h"

namespace neko::aer
{
#ifdef NEKO_GLES3
void UiText::Draw(gl::FontManager& fontManager, const FontId& fontId, std::uint8_t playerNmb) const
{
	const float scale = screenId_ != 0u ? scale_ * 0.5f : scale_;
	const Vec2i size = fontManager.CalculateTextSize(fontId, text_, scale);
	Vec2i position   = GetAnchorPosition(position_);
	position         = GetAnchorFromScreenId(position, playerNmb);

	fontManager.RenderText(fontId, text_, position, scale, color_);
}
#else
#endif

void UiText::Destroy() {}
}    // namespace neko::aer
