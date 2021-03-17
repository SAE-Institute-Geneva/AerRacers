#include "aer/ui/ui_element.h"

namespace neko::aer
{
UiElement::UiElement(const Vec3f& pos,UiAnchor uiAnchor, uint8_t screenId)
    : position_(pos),
    uiAnchor_(uiAnchor),
    screenId_(screenId)
{
}

void UiElement::Destroy()
{
}

void UiElement::SetEnable(bool enable)
{
    if (enable) {
        flags_ |= UiFlag::ENABLE;
    } else {
        flags_ &= ~UiFlag::ENABLE;
    }
}

void UiElement::AddFlag(UiFlag::Enum flag)
{
    flags_ |= flag;
}

void UiElement::RemoveFlag(UiFlag::Enum flag)
{
    flags_ &= ~flag;
}

Vec2f UiElement::CalculateUiElementPosition(Vec2f position, Vec2f windowSize, UiAnchor anchor)
{
    switch (anchor)
    {
    case UiAnchor::TOP_LEFT:
        return position + Vec2f{ -1, 1 };
    case UiAnchor::TOP:
        return position + Vec2f::up;
    case UiAnchor::TOP_RIGHT:
        return position + Vec2f::one;
    case UiAnchor::CENTER_LEFT:
        return position + Vec2f::left;
    case UiAnchor::CENTER:
        return position + Vec2f::zero;
    case UiAnchor::CENTER_RIGHT:
        return position + Vec2f::right;
    case UiAnchor::BOTTOM_LEFT:
        return position + Vec2f::one * -1.0f;
    case UiAnchor::BOTTOM:
        return position + Vec2f::down;
    case UiAnchor::BOTTOM_RIGHT:
        return position + Vec2f{1, -1};
    }
    return position;
}
}
