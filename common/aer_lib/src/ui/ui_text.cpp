#include "aer/ui/ui_text.h"

namespace neko::aer
{
#ifdef NEKO_GLES3
void UiText::Draw(gl::FontManager& fontManager, const FontId& fontId) const
{
	fontManager.RenderText(fontId, text_, position_, TextAnchor(uiAnchor_), scale_, color_);
}
#else
#endif

void UiText::Destroy() {}
}    // namespace neko::aer
