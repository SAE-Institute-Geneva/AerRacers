#include "aer/ui/ui_element.h"

#include "engine/engine.h"

#ifdef NEKO_VULKAN
#include "vk/material/material_manager.h"
#endif

namespace neko::aer
{
UiElement::UiElement(Vec2i pos, UiAnchor uiAnchor, const Color4& color, std::uint8_t screenId)
   : position_(pos), uiAnchor_(uiAnchor), color_(color), screenId_(screenId)
{
#ifdef NEKO_VULKAN
	auto& materialManager = vk::MaterialManagerLocator::get();
	std::string matName   = "Ui Material " + sole::uuid0().str();
	materialId_           = materialManager.AddNewMaterial(matName, vk::MaterialType::UI);

	vk::UiMaterial& material = materialManager.GetUiMaterial(materialId_);
	material.SetColor(color);
	material.SetSlidingCrop(slidingCrop_);
#endif
}

void UiElement::Destroy() {}

void UiElement::SetEnable(bool enable)
{
	enable ? (flags_ |= UiFlag::ENABLED) : (flags_ &= ~UiFlag::ENABLED);
}

void UiElement::AddFlag(UiFlag::Enum flag) { flags_ |= flag; }

void UiElement::RemoveFlag(UiFlag::Enum flag) { flags_ &= ~flag; }

Vec2i UiElement::GetAnchorPosition(const Vec2i position) const
{
	const Vec2f winSize = Vec2f(BasicEngine::GetInstance()->GetConfig().windowSize);

	Vec2f anchorPos;
	switch (uiAnchor_)
	{
		case UiAnchor::TOP_LEFT:     anchorPos = Vec2f::up; break;
		case UiAnchor::TOP:          anchorPos = Vec2f(0.5f, 1.0f); break;
		case UiAnchor::TOP_RIGHT:    anchorPos = Vec2f::one; break;

		case UiAnchor::CENTER_LEFT:  anchorPos = Vec2f(0.0f, 0.5f); break;
		case UiAnchor::CENTER:       anchorPos = Vec2f(0.5f, 0.5f); break;
		case UiAnchor::CENTER_RIGHT: anchorPos = Vec2f(1.0f, 0.5f); break;

		case UiAnchor::BOTTOM_LEFT:  anchorPos = Vec2f::zero; break;
		case UiAnchor::BOTTOM:       anchorPos = Vec2f(0.5f, 0.0f); break;
		case UiAnchor::BOTTOM_RIGHT: anchorPos = Vec2f::right; break;
	}

	return Vec2i(anchorPos * winSize + Vec2f(position) * 0.5f);
}

Vec2i UiElement::GetAnchorFromScreenId(Vec2i anchorPos, std::uint8_t playerNmb) const
{
	const Vec2f halfWinSize = Vec2f(BasicEngine::GetInstance()->GetConfig().windowSize) * 0.5f;
	if (screenId_ < playerNmb || screenId_ != 0)
	{
		switch (screenId_)
		{
			case 1:
			{
				switch (playerNmb)
				{
					case 2: anchorPos.x = anchorPos.x * 0.5f; break;
					case 3:
					case 4:
						anchorPos.x = anchorPos.x * 0.5f;
						anchorPos.y = anchorPos.y * 0.5f + halfWinSize.y;
						break;
					default: break;
				}
				break;
			}
			case 2:
			{
				switch (playerNmb)
				{
					case 2: anchorPos.x = anchorPos.x * 0.5f + halfWinSize.x; break;
					case 3:
					case 4:
						anchorPos.x = anchorPos.x * 0.5f + halfWinSize.x;
						anchorPos.y = anchorPos.y * 0.5f + halfWinSize.y;
						break;
					default: break;
				}
				break;
			}
			case 3:
				anchorPos.x = anchorPos.x * 0.5f;
				anchorPos.y = anchorPos.y * 0.5f;
				break;
			case 4:
				anchorPos.x = anchorPos.x * 0.5f + halfWinSize.x;
				anchorPos.y = anchorPos.y * 0.5f;
				break;
			default: break;
		}
	}

	return anchorPos;
}
}
