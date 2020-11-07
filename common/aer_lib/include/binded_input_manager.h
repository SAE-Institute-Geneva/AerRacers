#pragma once
/* ----------------------------------------------------
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author : Floreau Luca
 Co-Author :
 Date : 23.10.2020
---------------------------------------------------------- */

#include <cstdint>

#include "sdl_engine/sdl_input.h"

namespace neko::aer {

using PlayerId = std::uint8_t;
/**
* \brief Enum of the action button
*/
enum class ActionButtonType : std::uint8_t {
    FORWARD = 0,
    BACKWARD = 1,
    LEFT = 2,
    RIGHT = 3,
    MAIN_SHOOT = 4,
    CAMERA = 5,
    MENU = 6,
    LENGTH = 7
};

/**
* \brief Enum of the action axis
*/
enum class ActionAxisType : std::uint8_t {
    HORIZONTAL = 0,
    VERTICAL = 1,
    CAMERA_HORIZONTAL = 2,
    CAMERA_VERTICAL = 3,
    LENGTH = 4
};

/**
 * \brief Struct of inputs (button state and axis) and id of a controller
 */
struct ActionInputs {
    std::array<sdl::ButtonState, static_cast<size_t>(ActionButtonType::LENGTH)> controllerButtonStates;
    std::array<float, static_cast<size_t>(ActionAxisType::LENGTH)> controllerAxis;
    PlayerId playerId;
};

/**
 * \brief Manage inputs binding
 */
class IBindedInputManager {
public:
    virtual ~IBindedInputManager() = default;

    virtual void SetPlayerAction()

    /**
     * \brief  Get action button state
     * \param playerId Id of the player to get
     * \return 
     */
    virtual sdl::ButtonState GetActionButtonState(
        unsigned playerId,
        ActionButtonType actionButton) const;

    /**
     * \brief  Get action axis value
     * \param playerId Id of the player to get
     * \return
     */
    virtual float GetActionAxis(
        unsigned playerId,
        ActionAxisType actionAxis) const;

    /**
    * \brief Translate Action enum to string
    */
    virtual std::string ActionEnumToString(ActionButtonType actionInputs) = 0;

    /**
    * \brief Translate Action enum to string
    */
    virtual std::string ActionEnumToString(ActionAxisType actionAxis) = 0;
};

class 
}
