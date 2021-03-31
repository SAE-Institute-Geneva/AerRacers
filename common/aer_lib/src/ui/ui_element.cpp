#include "aer/ui/ui_element.h"

#include "engine/engine.h"

namespace neko::aer
{
UiElement::UiElement(Vec2i pos, UiAnchor uiAnchor, std::uint8_t screenId)
   : position_(pos), uiAnchor_(uiAnchor), screenId_(screenId)
{}

void UiElement::Destroy() {}

void UiElement::SetEnable(bool enable)
{
	enable ? (flags_ |= UiFlag::ENABLED) : (flags_ &= ~UiFlag::ENABLED);
}

void UiElement::AddFlag(UiFlag::Enum flag) { flags_ |= flag; }

void UiElement::RemoveFlag(UiFlag::Enum flag) { flags_ &= ~flag; }

Vec2i UiElement::GetAnchorPosition(const Vec2i position) const
{
	const Vec2u winSize = BasicEngine::GetInstance()->GetConfig().windowSize;

	Vec2f anchorPos;
	switch (uiAnchor_)
	{
		case UiAnchor::TOP_LEFT:     anchorPos = Vec2f(0.0f, winSize.y); break;
		case UiAnchor::TOP:          anchorPos = Vec2f(winSize.x * 0.5f, winSize.y); break;
		case UiAnchor::TOP_RIGHT:    anchorPos = Vec2f(winSize); break;

		case UiAnchor::CENTER_LEFT:  anchorPos = Vec2f(0.0f, winSize.y * 0.5f); break;
		case UiAnchor::CENTER:       anchorPos = Vec2f(winSize) * 0.5f; break;
		case UiAnchor::CENTER_RIGHT: anchorPos = Vec2f(winSize.x * 0.5f, winSize.y * 0.5f); break;

		case UiAnchor::BOTTOM_LEFT:  anchorPos = Vec2f(winSize); break;
		case UiAnchor::BOTTOM:       anchorPos = Vec2f(winSize.x * 0.5f, 0.0f); break;
		case UiAnchor::BOTTOM_RIGHT: anchorPos = Vec2f(winSize.x, 0.0f); break;
	}

	return position + Vec2i(anchorPos);
}

Vec2i UiElement::GetAnchorFromScreenId(Vec2i anchorPos, std::uint8_t playerNmb) const
{
	if (screenId_ < playerNmb || screenId_ != 0)
	{
		switch (screenId_)
		{
			case 1:
			{
				switch (playerNmb)
				{
					case 2: anchorPos.x = (anchorPos.x - 1.0f) * 0.5f; break;
					case 3:
					case 4:
						anchorPos.x = (anchorPos.x - 1.0f) * 0.5f;
						anchorPos.y = (anchorPos.y + 1.0f) * 0.5f;
						break;
					default: break;
				}
				break;
			}
			case 2:
			{
				switch (playerNmb)
				{
					case 2: anchorPos.x = (anchorPos.x + 1.0f) * 0.5f; break;
					case 3:
					case 4:
						anchorPos.x = (anchorPos.x + 1.0f) * 0.5f;
						anchorPos.y = (anchorPos.y + 1.0f) * 0.5f;
						break;
					default: break;
				}
				break;
			}
			case 3:
				anchorPos.x = (anchorPos.x - 1.0f) * 0.5f;
				anchorPos.y = (anchorPos.y - 1.0f) * 0.5f;
				break;
			case 4:
				anchorPos.x = (anchorPos.x + 1.0f) * 0.5f;
				anchorPos.y = (anchorPos.y - 1.0f) * 0.5f;
				break;
			default: break;
		}
	}

	return anchorPos;
}
}
