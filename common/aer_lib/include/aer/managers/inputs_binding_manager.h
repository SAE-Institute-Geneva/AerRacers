//#pragma once
///* ----------------------------------------------------
// MIT License
//
// Copyright (c) 2020 SAE Institute Switzerland AG
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// Author : Floreau Luca
// Co-Author :
// Date : 07.11.2020
//---------------------------------------------------------- */
//
//#include <cstdint>
//
//#include "sdl_engine/sdl_input.h"
//
//namespace neko::aer
//{
//using PlayerId = unsigned;
///**
//* \brief Enum of the action button
//*/
//enum class ActionButtonType : std::uint8_t
//{
//	FORWARD    = 0,
//	BACKWARD   = 1,
//	LEFT       = 2,
//	RIGHT      = 3,
//	MAIN_SHOOT = 4,
//	CAMERA     = 5,
//	MENU       = 6,
//	LENGTH     = 7
//};
//
///**
//* \brief Enum of the action axis
//*/
//enum class ActionAxisType : std::uint8_t
//{
//	HORIZONTAL        = 0,
//	VERTICAL          = 1,
//	CAMERA_HORIZONTAL = 2,
//	CAMERA_VERTICAL   = 3,
//	LENGTH            = 4
//};
//
///**
// * \brief Struct of inputs (button state and axis) and id of a player
// */
//struct BindingInputs
//{
//	std::array<sdl::KeyCodeType, static_cast<size_t>(ActionButtonType::LENGTH)> pcBindingButtons;
//	std::array<sdl::ControllerButtonType, static_cast<size_t>(ActionButtonType::LENGTH)>
//		controllerBindingButtons;
//	std::array<sdl::SwitchButtonType, static_cast<size_t>(ActionButtonType::LENGTH)>
//		switchBindingButtons;
//	std::array<sdl::ControllerAxisType, static_cast<size_t>(ActionAxisType::LENGTH)>
//		controllerBindingAxis;
//	std::array<sdl::SwitchAxisType, static_cast<size_t>(ActionAxisType::LENGTH)>
//		switchBindingAxis;
//	PlayerId playerId                    = 0;
//	sdl::JoystickId bindedControllerId = 0;
//	sdl::SwitchJoyId bindedSwitchJoyId   = 0;
//};
//
///**
// * \brief Manage inputs binding
// */
//class IInputBindingManager
//{
//public:
//	virtual ~IInputBindingManager() = default;
//
//	/**
//     * \brief Get Player action inputs binding
//     */
//	virtual BindingInputs GetPlayerActions(PlayerId playerId) = 0;
//
//	/**
//     * \brief  Set Player action inputs binding
//     * \param bindingInputs BindingInputs to save
//     */
//	virtual void SetPlayerActions(BindingInputs bindingInputs) = 0;
//
//	/**
//     * \brief  Get action button state
//     * \param playerId Id of the player to get
//     * \return 
//     */
//	[[nodiscard]] virtual sdl::ButtonState GetActionButtonState(
//		unsigned playerId, ActionButtonType actionButton) const = 0;
//
//	/**
//     * \brief  Get action axis value
//     * \param playerId Id of the player to get
//     * \return
//     */
//	[[nodiscard]] virtual float GetActionAxis(
//		unsigned playerId, ActionAxisType actionAxis) const = 0;
//
//	/**
//    * \brief Translate Action enum to string
//    */
//	virtual std::string ActionEnumToString(ActionButtonType actionInputs) = 0;
//
//	/**
//    * \brief Translate Action enum to string
//    */
//	virtual std::string ActionEnumToString(ActionAxisType actionAxis) = 0;
//};
//
//class InputBindingManager final : public IInputBindingManager
//{
//public:
//	InputBindingManager();
//
//	BindingInputs GetPlayerActions(PlayerId playerId) override;
//	void SetPlayerActions(BindingInputs actionInputs) override;
//
//	[[nodiscard]] sdl::ButtonState GetActionButtonState(
//		unsigned playerId, ActionButtonType actionButton) const override;
//
//	[[nodiscard]] float GetActionAxis(unsigned playerId, ActionAxisType actionAxis) const override;
//
//	std::string ActionEnumToString(ActionButtonType actionInputs) override;
//	std::string ActionEnumToString(ActionAxisType actionAxis) override;
//
//	void Destroy();
//
//private:
//	[[nodiscard]] unsigned FindActionIndexFromId(PlayerId playerId) const;
//
//	std::vector<BindingInputs> actionBindingInputs_;
//	sdl::IInputManager* inputLocator_;
//};
//
//class NullInputBindingManager final : public IInputBindingManager
//{
//public:
//	~NullInputBindingManager() override = default;
//
//	BindingInputs GetPlayerActions(PlayerId) override { return BindingInputs(); }
//	void SetPlayerActions(BindingInputs) override {}
//
//	[[nodiscard]] sdl::ButtonState GetActionButtonState(unsigned, ActionButtonType) const override
//	{
//		return sdl::ButtonState::NONE;
//	}
//
//	[[nodiscard]] float GetActionAxis(unsigned, ActionAxisType) const override { return 0.0f; }
//
//	std::string ActionEnumToString(ActionButtonType) override { return ""; }
//	std::string ActionEnumToString(ActionAxisType) override { return ""; }
//};
//
//using BindedInputLocator = Locator<IInputBindingManager, NullInputBindingManager>;
//}
