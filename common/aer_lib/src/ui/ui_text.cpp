#include "aer/ui/ui_text.h"

namespace neko::aer
{
#ifdef NEKO_GLES3
void UiText::Draw(gl::FontManager& fontManager, const FontId& fontId, std::uint8_t playerNmb) const
{
	float scale = (screenId_ == 0 || playerNmb <= 1) ? scale_: scale_ * 0.5f;
	scale *= 0.3f; //Quick fix rescale
	const Vec2i size = fontManager.CalculateTextSize(fontId, text_, scale);
	Vec2i position   = GetAnchorPosition(position_);
	position         = GetAnchorFromScreenId(position, playerNmb);

	fontManager.RenderText(fontId, text_, position, TextAnchor(uiAnchor_), scale, color_);
}
#else
#endif

void UiText::Destroy() {}
}    // namespace neko::aer
