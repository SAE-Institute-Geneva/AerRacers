#include "aer/inputs_binding_manager.h"

#include "aer/log.h"

namespace neko::aer
{
InputBindingManager::InputBindingManager()
{
	inputLocator_ = &sdl::InputLocator::get();
	BindedInputLocator::provide(this);
}

BindingInputs InputBindingManager::GetPlayerActions(PlayerId playerId)
{
	unsigned playerIndex = FindActionIndexFromId(playerId);
	if (playerIndex >= actionBindingInputs_.size())
	{
		LogDebug("Unknown Player : " + std::to_string(playerId));
		return BindingInputs();
	}

	return actionBindingInputs_[playerIndex];
}

void InputBindingManager::SetPlayerActions(BindingInputs actionInputs)
{
	unsigned playerIndex = FindActionIndexFromId(actionInputs.playerId);
	if (playerIndex >= actionBindingInputs_.size())
	{
		actionBindingInputs_.push_back(actionInputs);
	}

	actionBindingInputs_[playerIndex] = actionInputs;
}

sdl::ButtonState InputBindingManager::GetActionButtonState(
	unsigned playerId,
	ActionButtonType actionButton) const
{
	unsigned playerIndex = FindActionIndexFromId(playerId);
	if (playerIndex >= actionBindingInputs_.size())
	{
		LogDebug("Unknow Player : " + std::to_string(playerId));
		return sdl::ButtonState::NONE;
	}

	const auto& switchJoyIdVector = inputLocator_->GetSwitchJoyIdVector();
	if (std::find(switchJoyIdVector.begin(),
			switchJoyIdVector.end(),
			actionBindingInputs_[playerIndex].bindedSwitchJoyId) != switchJoyIdVector.end())
	{
		if (inputLocator_->GetSwitchButtonState(actionBindingInputs_[playerIndex].bindedSwitchJoyId,
				actionBindingInputs_[playerIndex]
					.switchBindingButtons[static_cast<size_t>(actionButton)]) !=
			sdl::ButtonState::NONE)
		{
			return inputLocator_->GetSwitchButtonState(
				actionBindingInputs_[playerIndex].bindedSwitchJoyId,
				actionBindingInputs_[playerIndex]
					.switchBindingButtons[static_cast<size_t>(actionButton)]);
		}
	}

	const auto& controllerIdVector = inputLocator_->GetControllerIdVector();
	if (std::find(controllerIdVector.begin(),
			controllerIdVector.end(),
			actionBindingInputs_[playerIndex].bindedControllerId) != controllerIdVector.end())
	{
		if (inputLocator_->GetControllerButtonState(
				actionBindingInputs_[playerIndex].bindedControllerId,
				actionBindingInputs_[playerIndex]
					.controllerBindingButtons[static_cast<size_t>(actionButton)]) !=
			sdl::ButtonState::NONE)
		{
			return inputLocator_->GetControllerButtonState(
				actionBindingInputs_[playerIndex].bindedControllerId,
				actionBindingInputs_[playerIndex]
					.controllerBindingButtons[static_cast<size_t>(actionButton)]);
		}
	}

	if (inputLocator_->GetKeyState(actionBindingInputs_[playerIndex]
									   .pcBindingButtons[static_cast<size_t>(actionButton)]) !=
		sdl::ButtonState::NONE)
	{
		return inputLocator_->GetKeyState(
			actionBindingInputs_[playerIndex].pcBindingButtons[static_cast<size_t>(actionButton)]);
	}
	else
	{
		return sdl::ButtonState::NONE;
	}
}

float InputBindingManager::GetActionAxis(
    unsigned playerId,
    ActionAxisType actionAxis) const
{
	unsigned playerIndex = FindActionIndexFromId(playerId);
	if (playerIndex >= actionBindingInputs_.size())
	{
		LogDebug("Unknown Player : " + std::to_string(playerId));
		return 0.0f;
	}

	const auto& switchJoyIdVector = inputLocator_->GetSwitchJoyIdVector();
	if (std::find(switchJoyIdVector.begin(),
			switchJoyIdVector.end(),
			actionBindingInputs_[playerIndex].bindedSwitchJoyId) != switchJoyIdVector.end())
	{
		if (inputLocator_->GetSwitchAxis(actionBindingInputs_[playerIndex].bindedSwitchJoyId,
				actionBindingInputs_[playerIndex]
					.switchBindingAxis[static_cast<size_t>(actionAxis)]) != 0.0f)
		{
			return inputLocator_->GetSwitchAxis(actionBindingInputs_[playerIndex].bindedSwitchJoyId,
				actionBindingInputs_[playerIndex]
					.switchBindingAxis[static_cast<size_t>(actionAxis)]);
		}
	}

	const auto& controllerIdVector = inputLocator_->GetControllerIdVector();
	if (std::find(controllerIdVector.begin(),
			controllerIdVector.end(),
			actionBindingInputs_[playerIndex].bindedControllerId) != controllerIdVector.end())
	{
		if (inputLocator_->GetControllerAxis(actionBindingInputs_[playerIndex].bindedControllerId,
				actionBindingInputs_[playerIndex]
					.controllerBindingAxis[static_cast<size_t>(actionAxis)]) != 0.0f)
		{
			return inputLocator_->GetControllerAxis(
				actionBindingInputs_[playerIndex].bindedControllerId,
				actionBindingInputs_[playerIndex]
					.controllerBindingAxis[static_cast<size_t>(actionAxis)]);
		}
	}

	return 0.0f;
}

unsigned InputBindingManager::FindActionIndexFromId(const PlayerId playerId) const
{
	const auto actionInputIt = std::find_if(actionBindingInputs_.begin(),
		actionBindingInputs_.end(),
		[playerId](BindingInputs actionInputs) { return actionInputs.playerId == playerId; });
	if (actionInputIt >= actionBindingInputs_.end())
	{
		LogDebug("Unknown Player : " + std::to_string(playerId));
		return actionBindingInputs_.size();
	}
	const unsigned index = std::distance(actionBindingInputs_.begin(), actionInputIt);
	return index;
}

std::string InputBindingManager::ActionEnumToString(const ActionButtonType actionInputs)
{
	switch (actionInputs)
	{
		case ActionButtonType::FORWARD: return "Forward";
		case ActionButtonType::BACKWARD: return "Backward";
		case ActionButtonType::LEFT: return "Left";
		case ActionButtonType::RIGHT: return "Right";
		case ActionButtonType::MAIN_SHOOT: return "Main_Shoot";
		case ActionButtonType::CAMERA: return "Camera";
		case ActionButtonType::MENU: return "Menu";
		default: return "";
	}
}

std::string InputBindingManager::ActionEnumToString(ActionAxisType actionAxis)
{
	switch (actionAxis)
	{
		case ActionAxisType::HORIZONTAL: return "Horizontal";
		case ActionAxisType::VERTICAL: return "Vertical";
		case ActionAxisType::CAMERA_HORIZONTAL: return "CameraHorizontal";
		case ActionAxisType::CAMERA_VERTICAL: return "CameraVertical";
		default: return "";
	}
}

void InputBindingManager::Destroy()
{
    actionBindingInputs_.clear();
    //// Get current flag
    //int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);

    //// Turn on leak-checking bit.
    //tmpFlag |= _CRTDBG_LEAK_CHECK_DF;

    //// Turn off CRT block checking bit.
    //tmpFlag &= ~_CRTDBG_CHECK_CRT_DF;

    //// Set flag to the new value.
    //_CrtSetDbgFlag(tmpFlag);
}
}    // namespace neko::aer
