#include "aer/ui/ui_text.h"

namespace neko::aer
{
void UiText::Draw(gl::FontManager& fontManager, const FontId& fontId) const
{
    fontManager.RenderText(fontId, text_, Vec2f(position_), TextAnchor(uiAnchor_), scale_, color_);
}

void UiText::Destroy()
{

}
}
