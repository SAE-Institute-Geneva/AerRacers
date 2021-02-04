#include <sdl_engine/sdl_input.h>

#include <string>

#include "sdl_engine/sdl_engine.h"

namespace neko::sdl {
InputManager::InputManager()
{
    InputLocator::provide(this);
}

void InputManager::Init()
{
    /* Print information about the joysticks */
    logDebug("There are " + std::to_string(SDL_NumJoysticks()) +
             " joysticks attached");
    for (int i = 0; i < SDL_NumJoysticks(); ++i) { PrintJoystick(i); }

    if (SDL_NumJoysticks() > 0)
    {
        int device = 0;
        joystick_ = SDL_JoystickOpen(device);

        if (joystick_ != nullptr)
        {
            SDL_assert(
                SDL_JoystickFromInstanceID(SDL_JoystickInstanceID(
                    joystick_)) == joystick_);
        }
    }

    for (auto& controllerInputs : controllerInputs_)
    {
        controllerInputs.controllerAxis[static_cast<size_t>(
                ControllerAxisType::LEFT_BUMPER)] = -1.0f;
        controllerInputs.controllerAxis[static_cast<size_t>(
                ControllerAxisType::RIGHT_BUMPER)] = -1.0f;
    }
}

void InputManager::OnPreUserInput()
{
    for (size_t i = 0; i < static_cast<size_t>(KeyCodeType::KEYBOARD_SIZE); i++)
    {
        if (keyPressedState_[i] == ButtonState::UP)
            keyPressedState_[i] = ButtonState::NONE;
        else if (keyPressedState_[i] == ButtonState::DOWN)
            keyPressedState_[i] = ButtonState::HELD;
    }

    for (size_t i = 0; i < static_cast<size_t>(MouseButtonType::LENGTH); i++)
    {
        if (buttonState_[i] == ButtonState::UP)
            buttonState_[i] = ButtonState::NONE;
        else if (buttonState_[i] == ButtonState::DOWN)
            buttonState_[i] = ButtonState::HELD;
    }

    for (auto& controllerInputs : controllerInputs_)
    {
        for (size_t i = 0; i < static_cast<size_t>(ControllerButtonType::LENGTH); i++)
        {
            if (controllerInputs.controllerButtonStates[i] == ButtonState::UP)
                controllerInputs.controllerButtonStates[i] = ButtonState::NONE;
            else if (controllerInputs.controllerButtonStates[i] == ButtonState::DOWN)
                controllerInputs.controllerButtonStates[i] = ButtonState::HELD;
        }
    }
    mouseScroll_ = Vec2f::zero;
}

unsigned InputManager::FindControllerIndexFromId(
    const ControllerId controllerId) const
{
    const auto controllerInputIt = std::find_if(
        controllerInputs_.begin(),
        controllerInputs_.end(),
        [controllerId](ControllerInputs controllerInputs)
        { return controllerInputs.controllerId == controllerId; });

    if (controllerInputIt >= controllerInputs_.end())
    {
        logDebug(&"Invalid controllerId : " [ controllerId]);
        return controllerInputs_.size();
    }

    const unsigned index =
            std::distance(controllerInputs_.begin(), controllerInputIt);
    return index;
}

unsigned InputManager::FindSwitchIndexFromId(
    const SwitchJoyId switchJoyId) const
{
    const auto switchInputIt = std::find_if(
        switchInputs_.begin(),
        switchInputs_.end(),
        [switchJoyId](
        SwitchInputs switchInputs)
        {
            return switchInputs.switchJoyId == switchJoyId;
        });
    if (switchInputIt >= switchInputs_.end())
    {
        logDebug(&"Invalid switchJoyId : " [switchJoyId]);
        return switchInputs_.size();
    }
    const unsigned index = std::distance(switchInputs_.begin(),
                                         switchInputIt);
    return index;
}

void InputManager::OnEvent(SDL_Event event)
{
    switch (event.type) {
#pragma region keyboard
        case SDL_TEXTINPUT:
        {
            // TODO(@Luca) Setup Text Input
            break;
        }
        case SDL_TEXTEDITING:
        {
            // TODO(@Luca) Setup Text Editing
            break;
        }

        case SDL_KEYDOWN:
        {
            const size_t index = event.key.keysym.scancode;
            if (keyPressedState_[index] != ButtonState::HELD)
            { keyPressedState_[index] = ButtonState::DOWN; }
            break;
        }
        case SDL_KEYUP:
        {
            const size_t index = event.key.keysym.scancode;
            keyPressedState_[index] = ButtonState::UP;
            break;
        }
#pragma endregion

#pragma region Mouse
        case SDL_MOUSEMOTION:
        {
            mousePos_.x = event.motion.x;
            mousePos_.y = event.motion.y;
            mouseRelativePos_.x = event.motion.xrel;
            mouseRelativePos_.y = event.motion.yrel;
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        {
            const size_t index = event.button.button - 1;
            buttonState_[index] =
                ButtonState::DOWN;

            break;
        }
        case SDL_MOUSEBUTTONUP:
        {
            const size_t index = event.button.button - 1;
            buttonState_[index] =
                ButtonState::UP;

            break;
        }
        case SDL_MOUSEWHEEL:
        {
            mouseScroll_.x = event.wheel.x;
            mouseScroll_.y = event.wheel.y;
            break;
        }
#pragma endregion

#pragma region Joystick
        case SDL_JOYDEVICEREMOVED:
        {
            logDebug("Joystick device  removed." + std::to_string(
                         static_cast<int>(event.jdevice.which)));
            if (joystick_ != nullptr)
            {
                logDebug("Our instance ID is" + std::to_string(
                             static_cast<int>(SDL_JoystickInstanceID(joystick_)
                             )));
                logDebug("There are " + std::to_string(SDL_NumJoysticks()) +
                         " joysticks attached");
                SDL_JoystickClose(joystick_);
            }
            else
            {
                logDebug("No more joystick");
            }

            const unsigned controllerIndex = FindControllerIndexFromId(
                event.jdevice.which);
            if (controllerIndex < controllerInputs_.size())
            {
                controllerInputs_.erase(
                    controllerInputs_.begin() + controllerIndex);
            }
            else
            {
                logDebug(&"Controller removed out of range : Joystick " [
                    event.jdevice.which]);
            }

            break;
        }
        case SDL_JOYDEVICEADDED:
        {
            const int device = event.jdevice.which;
            joystick_ = SDL_JoystickOpen(device);
            if (joystick_ != nullptr)
            {
                SDL_assert(SDL_JoystickFromInstanceID(SDL_JoystickInstanceID
                        (joystick_)) == joystick_);
            }
            logDebug("There are " + std::to_string(SDL_NumJoysticks()) +
                     " joysticks attached");
            //for (int i = 0; i < SDL_NumJoysticks(); ++i) { PrintJoystick(i); }
            const unsigned controllerId = SDL_JoystickInstanceID(joystick_);
            const unsigned controllerIndex = FindControllerIndexFromId(
                controllerId);
            if (controllerIndex >= controllerInputs_.size())
            {
                ControllerInputs controllerInputs;
                controllerInputs.controllerId = controllerId;
                controllerInputs_.push_back(controllerInputs);
            }
            else
            {
                logDebug(&"Controller already added : Joystick "[controllerId]);
            }
            break;
        }

        case SDL_JOYBALLMOTION:
        {
            logDebug("Joystick " + std::to_string(event.jball.which) +
                     " ball " + std::
                     to_string(event.jball.ball) + " delta: (" +
                     std::to_string(
                         event.jball.xrel) + "," +
                     std::to_string(event.jball.yrel) + ")");
            logDebug("JoyBall not support");

            break;
        }

        case SDL_JOYBUTTONDOWN:
        {
            const unsigned controllerIndex = FindControllerIndexFromId(
                event.jdevice.which);
            if (controllerIndex < controllerInputs_.size())
            {
                controllerInputs_[controllerIndex].controllerButtonStates[
                        event.jbutton.button] = ButtonState::DOWN;
            }
            else
            {
                logDebug(
                    &"JoyButton pressed down from unknown Joystick : Joystick "
                    [event.jdevice.which]);
            }
            break;
        }

        case SDL_JOYBUTTONUP:
        {
            const unsigned controllerIndex = FindControllerIndexFromId(
                event.jdevice.which);
            if (controllerIndex < controllerInputs_.size())
            {
                controllerInputs_[controllerIndex].controllerButtonStates[
                        event.jbutton.button] =
                    ButtonState::UP;
            }
            else
            {
                logDebug(
                    &"JoyButton released up from unknow Joystick : Joystick "
                    [event.jdevice.which]);
            }
            break;
        }

        case SDL_JOYAXISMOTION:
        {
            const int deadZone = 3200;
            float value = 0;
            if (event.jaxis.value < -deadZone || event.jaxis.value > deadZone)
            {
                value = static_cast<float>(event.jaxis.value) / kMaxJoyValue_;
            }
            const unsigned controllerIndex = FindControllerIndexFromId(
                event.jaxis.which);
            if (controllerIndex < controllerInputs_.size())
            {
                controllerInputs_[controllerIndex]
                    .controllerAxis[event.jaxis.axis] = value;
            }
            else {
                logDebug(&"JoyAxis from unknown Joystick : Joystick "
                    [event.jdevice.which]);
            }
            break;
        }
        case SDL_JOYHATMOTION:
        {
            const unsigned controllerIndex = FindControllerIndexFromId(
                event.jdevice.which);
            if (controllerIndex < controllerInputs_.size())
            {
                if (event.jhat.value == SDL_HAT_CENTERED)
                {
                    controllerInputs_[controllerIndex].controllerAxis[
                        static_cast<size_t>(
                            ControllerAxisType::
                            PAD_VERTICAL)] = 0;
                    controllerInputs_[controllerIndex].controllerAxis[
                        static_cast<size_t>(
                            ControllerAxisType::
                            PAD_HORIZONTAL)] = 0;
                }
                if (event.jhat.value & SDL_HAT_UP)
                {
                    controllerInputs_[controllerIndex].controllerAxis[
                        static_cast<size_t>(
                            ControllerAxisType::
                            PAD_VERTICAL)] = 1.0f;
                }
                if (event.jhat.value & SDL_HAT_RIGHT)
                {
                    controllerInputs_[controllerIndex].controllerAxis[
                        static_cast<size_t>(
                            ControllerAxisType::
                            PAD_HORIZONTAL)] = 1.0f;
                }
                if (event.jhat.value & SDL_HAT_DOWN)
                {
                    controllerInputs_[controllerIndex].controllerAxis[
                        static_cast<size_t>(
                            ControllerAxisType::
                            PAD_VERTICAL)] = -1.0f;
                }
                if (event.jhat.value & SDL_HAT_LEFT)
                {
                    controllerInputs_[controllerIndex].controllerAxis[
                        static_cast<size_t>(
                            ControllerAxisType::
                            PAD_HORIZONTAL)] = -1.0f;
                }
            }
            else {
                logDebug(
                    &"JoyHat from unknown Joystick : Joystick " [ event
                                                                .jdevice.which]);
            }
        }
        break;

            /* Fall through to signal quit */
        case SDL_FINGERDOWN:
        {
            logDebug("Finger down");
            break;
        }
        case SDL_FINGERMOTION:
        {
            logDebug("Finger motion");
            break;
        }
        case SDL_FINGERUP:
        {
            logDebug("Finger up");
            break;
        }
#pragma endregion

            //Use of Joystick instead of Controller
            /*
    #pragma region Controller
            case SDL_CONTROLLERAXISMOTION: {
                logDebug("Controller" + std::to_string(event.caxis.which) +
                         "axis" + std::to_string(
                             event.caxis.axis) + "value:" + std::to_string(
                             event.caxis.value));
                break;
            }
            case SDL_CONTROLLERBUTTONDOWN: {
                logDebug("Controller " + std::to_string(event.cbutton.which) +
                         " button " + std::to_string(
                             event.cbutton.button) + " up");
    
                break;
            }
            case SDL_CONTROLLERBUTTONUP: {
                logDebug("Controller " + std::to_string(event.cbutton.which) +
                         " button " + std::to_string(
                             event.cbutton.button) + " up");
                break;
            }
            case SDL_CONTROLLERDEVICEADDED: {
                const int device = event.cdevice.which;
                controller_ = SDL_GameControllerOpen(device);
                std::cout << "Controller device  added." + std::to_string(
                    static_cast<int>(event.cdevice.which)) << '\n';
                break;
            }
            case SDL_CONTROLLERDEVICEREMOVED: {
                logDebug("Controller device removed." + std::to_string(
                             static_cast<int>(event.cdevice.which)));
                SDL_GameControllerClose(controller_);
                break;
            }
            case SDL_CONTROLLERDEVICEREMAPPED: {
                logDebug("Controller device remapped." + std::to_string(
                             static_cast<int>(event.cdevice.which)));
                break;
            }
    #pragma endregion
    */
        default:
            break;
    }
}

ButtonState InputManager::GetKeyState(KeyCodeType key) const
{
    // TODO(@Luca) add an asert
    return keyPressedState_[static_cast<size_t>(key)];
}

ButtonState InputManager::GetSwitchButtonState(
    SwitchJoyId switchJoyId,
    SwitchButtonType switchButton) const
{
    const unsigned switchIndex =
            FindSwitchIndexFromId(switchJoyId);
    if (switchIndex >= switchInputs_.size())
    {
        logDebug(&"Unknown Joystick : " [switchJoyId]);
        return ButtonState::NONE;
    }
    return switchInputs_[switchIndex].switchButtonStates[
        static_cast<size_t>(switchButton)];
}

float InputManager::GetSwitchAxis(
    SwitchJoyId switchJoyId,
    SwitchAxisType axis) const
{
    const unsigned switchIndex = FindSwitchIndexFromId(
        switchJoyId);
    if (switchIndex >= switchInputs_.size())
    {
        logDebug(&"Unknown Joystick : " [switchJoyId]);
        return 0.0f;
    }
    return switchInputs_[switchIndex].switchAxis[
        static_cast<size_t>(axis)];
}

ButtonState InputManager::GetControllerButtonState(
    ControllerId controllerId,
    ControllerButtonType controllerButton) const
{
    const unsigned controllerIndex = FindControllerIndexFromId(
        controllerId);
    if (controllerIndex >= controllerInputs_.size())
    {
        logDebug(&"Unknown Controller : " [controllerId]);
        return ButtonState::NONE;
    }
    return controllerInputs_[controllerIndex].controllerButtonStates[
        static_cast<size_t>(controllerButton)];
}

float InputManager::GetControllerAxis(
    ControllerId controllerId,
    ControllerAxisType axis) const
{
    const unsigned controllerIndex = FindControllerIndexFromId(
        controllerId);
    if (controllerIndex >= controllerInputs_.size())
    {
        logDebug(&"Unknown Controller : " [controllerId]);
        return 0.0f;
    }
    return controllerInputs_[controllerIndex].controllerAxis[
        static_cast<size_t>(axis)];
}

std::vector<ControllerId> InputManager::GetControllerIdVector() const
{
    std::vector<ControllerId> controllerIdVector;
    for (const ControllerInputs& controllerInput : controllerInputs_)
    {
        controllerIdVector.push_back(controllerInput.controllerId);
    }
    return controllerIdVector;
}

std::vector<SwitchJoyId> InputManager::GetSwitchJoyIdVector() const
{
    std::vector<SwitchJoyId> switchJoyIdVector;
    for (const SwitchInputs& switchInput : switchInputs_)
    {
        switchJoyIdVector.push_back(switchInput.switchJoyId);
    }
    return switchJoyIdVector;
}

void InputManager::PrintJoystick(const int device)
{
    //Print info
    const char* name = SDL_JoystickNameForIndex(device);

    logDebug("Joystick " + std::to_string(device) + ": " + name +
        " Unknown Joystick");

    const auto joystick = SDL_JoystickOpen(device);
    if (joystick == nullptr)
    {
        logDebug("SDL_JoystickOpen " + std::to_string(device) +
            " failed: %s\n");
    }
    else
    {
        std::string type;

        char guid[64];
        SDL_enabled_assert(
            SDL_JoystickFromInstanceID(SDL_JoystickInstanceID(joystick)) ==
            joystick);
        SDL_JoystickGetGUIDString(
            SDL_JoystickGetGUID(joystick),
            guid,
            sizeof(guid));
        switch (SDL_JoystickGetType(joystick))
        {
            case SDL_JOYSTICK_TYPE_GAMECONTROLLER:
                type = "Game Controller";
                break;
            case SDL_JOYSTICK_TYPE_WHEEL:
                type = "Wheel";
                break;
            case SDL_JOYSTICK_TYPE_ARCADE_STICK:
                type = "Arcade Stick";
                break;
            case SDL_JOYSTICK_TYPE_FLIGHT_STICK:
                type = "Flight Stick";
                break;
            case SDL_JOYSTICK_TYPE_DANCE_PAD:
                type = "Dance Pad";
                break;
            case SDL_JOYSTICK_TYPE_GUITAR:
                type = "Guitar";
                break;
            case SDL_JOYSTICK_TYPE_DRUM_KIT:
                type = "Drum Kit";
                break;
            case SDL_JOYSTICK_TYPE_ARCADE_PAD:
                type = "Arcade Pad";
                break;
            case SDL_JOYSTICK_TYPE_THROTTLE:
                type = "Throttle";
                break;
            default:
                type = "Unknown";
                break;
        }
        //Display for more information
        /*logDebug("type: " + type + " \n" +
                 "axes: " + std::to_string(SDL_JoystickNumAxes(joystick)) +
                 '\n' +
                 "balls: " + std::to_string(SDL_JoystickNumBalls(joystick)) +
                 '\n' +
                 "hats: " + std::to_string(SDL_JoystickNumHats(joystick)) +
                 '\n' +
                 "buttons: " + std::to_string(SDL_JoystickNumButtons(joystick))
                 + '\n' +
                 "instance id: " + std::to_string(
                     SDL_JoystickInstanceID(joystick)) +
                 '\n' +
                 "power level: " + std::to_string(
                     static_cast<int>(SDL_JoystickCurrentPowerLevel(joystick)))
                 + '\n' +
                 "guid: " + *guid +
                 '\n' +
                 "VID/PID: " + std::to_string(
                     static_cast<int>(SDL_JoystickGetVendor(joystick))) +
                 ":" + std::to_string(
                     static_cast<int>(SDL_JoystickGetProduct(
                         joystick))));*/

        SDL_JoystickClose(joystick);
    }
}


std::string InputManager::PcInputsEnumToString(const KeyCodeType keyCode)
{
    switch (keyCode)
    {
        case KeyCodeType::A:
            return "A";
        case KeyCodeType::B:
            return "B";
        case KeyCodeType::C:
            return "C";
        case KeyCodeType::D:
            return "D";
        case KeyCodeType::E:
            return "E";
        case KeyCodeType::F:
            return "F";
        case KeyCodeType::G:
            return "G";
        case KeyCodeType::H:
            return "H";
        case KeyCodeType::I:
            return "I";
        case KeyCodeType::J:
            return "J";
        case KeyCodeType::K:
            return "K";
        case KeyCodeType::L:
            return "L";
        case KeyCodeType::M:
            return "M";
        case KeyCodeType::N:
            return "N";
        case KeyCodeType::O:
            return "O";
        case KeyCodeType::P:
            return "P";
        case KeyCodeType::Q:
            return "Q";
        case KeyCodeType::R:
            return "R";
        case KeyCodeType::S:
            return "S";
        case KeyCodeType::T:
            return "T";
        case KeyCodeType::U:
            return "U";
        case KeyCodeType::V:
            return "V";
        case KeyCodeType::W:
            return "W";
        case KeyCodeType::X:
            return "X";
        case KeyCodeType::Y:
            return "Y";
        case KeyCodeType::Z:
            return "Z";
        case KeyCodeType::ESCAPE:
            return "Escape";
        case KeyCodeType::SPACE:
            return "Space";
        case KeyCodeType::KEY_LEFT_SHIFT:
            return "Left_Shift";
        case KeyCodeType::KEY_RIGHT_SHIFT:
            return "Right_Shift";
        case KeyCodeType::KEY_LEFT_CTRL:
            return "Left_Ctrl";
        case KeyCodeType::KEY_RIGHT_CTRL:
            return "Right_Ctrl";
        case KeyCodeType::KEY_LEFT_ALT:
            return "Left_Alt";
        case KeyCodeType::LEFT:
            return "Left";
        case KeyCodeType::RIGHT:
            return "Rightt";
        case KeyCodeType::UP:
            return "Up";
        case KeyCodeType::DOWN:
            return "Down";
        default:
            return "Not register key";
    }
}

std::string InputManager::MouseInputsEnumToString(MouseButtonType mouseButton)
{
    switch (mouseButton)
    {
        case MouseButtonType::LEFT:
            return "LeftMouse";
        case MouseButtonType::RIGHT:
            return "RightMouse";
        case MouseButtonType::MIDDLE:
            return "MiddleMouse";
        default:
            return "";
    }
}

std::string InputManager::SwitchInputsEnumToString(const SwitchButtonType switchInputs)
{
    switch (switchInputs)
    {
        default:
            return "";
    }
}

std::string InputManager::SwitchInputsEnumToString(SwitchAxisType switchAxis)
{
    switch (switchAxis)
    {
        default:
            return "";
    }
}

std::string InputManager::ControllerInputsEnumToString(
    const ControllerButtonType controller)
{
    switch (controller)
    {
        case ControllerButtonType::PRESS_LEFT_STICK:
            return "Press_Left_Stick";
        case ControllerButtonType::LEFT_TRIGGER:
            return "Left_Trigger";
        case ControllerButtonType::PRESS_RIGHT_STICK:
            return "Press_Right_Click";
        case ControllerButtonType::RIGHT_TRIGGER:
            return "Right_Trigger";
        case ControllerButtonType::BUTTON_A:
            return "Button_A";
        case ControllerButtonType::BUTTON_B:
            return "Button_B";
        case ControllerButtonType::BUTTON_X:
            return "Button_X";
        case ControllerButtonType::BUTTON_Y:
            return "Button_Y";
        case ControllerButtonType::BUTTON_START:
            return "Button_Start";
        case ControllerButtonType::BUTTON_SELECT:
            return "Button_Select";
        default:
            return "";
    }
}

std::string InputManager::ControllerInputsEnumToString(
    const ControllerAxisType controller)
{
    switch (controller)
    {
        case ControllerAxisType::VERTICAL_LEFT_AXIS:
            return "Vertical_Left_Stick";
        case ControllerAxisType::HORIZONTAL_LEFT_AXIS:
            return "Horizontal_Left_Stick";
        case ControllerAxisType::LEFT_BUMPER:
            return "Left_Bumper";
        case ControllerAxisType::VERTICAL_RIGHT_AXIS:
            return "Vertical_Right_stick";
        case ControllerAxisType::HORIZONTAL_RIGHT_AXIS:
            return "Horizontal_Right_Stick";
        case ControllerAxisType::RIGHT_BUMPER:
            return "Right_Bumper";
        case ControllerAxisType::PAD_HORIZONTAL:
            return "Pad_Horizontal";
        case ControllerAxisType::PAD_VERTICAL:
            return "Pad_Vertical";
        default:
            return "";
    }
}
}
