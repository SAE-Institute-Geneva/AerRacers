#include <sdl_engine/sdl_input.h>

#include <string>

#include "sdl_engine/sdl_engine.h"

namespace neko::sdl {
InputManager::InputManager(SdlEngine& engine)
    : keyboard_(nullptr),
      mouse_(0),
      engine_(engine)
{
    InputLocator::provide(this);
}

void InputManager::BindAction()
{
    //TODO(@Luca) Function to bind action
    for (unsigned i = 0; i < 4; i++) {
        bindingPcInput_[i][static_cast<unsigned>(ActionInput::FORWARD)] =
            static_cast<unsigned>(KeyCode::W);
        bindingPcInput_[i][static_cast<unsigned>(ActionInput::BACKWARD)] =
            static_cast<unsigned>(KeyCode::S);
        bindingPcInput_[i][static_cast<unsigned>(ActionInput::RIGHT)] =
            static_cast<unsigned>(KeyCode::D);
        bindingPcInput_[i][static_cast<unsigned>(ActionInput::LEFT)] =
            static_cast<unsigned>(KeyCode::A);
        bindingPcInput_[i][static_cast<unsigned>(ActionInput::MAIN_SHOOT)] =
            static_cast<unsigned>(KeyCode::E);
        bindingPcInput_[i][static_cast<unsigned>(ActionInput::CAMERA)] =
            static_cast<unsigned>(KeyCode::F);
        bindingPcInput_[i][static_cast<unsigned>(ActionInput::MENU)] =
            static_cast<unsigned>(KeyCode::ESCAPE);

        bindingControllerInput_[i][static_cast<unsigned>(ActionInput::FORWARD)]
            = PairedControllerInput(ControllerInputs::BUTTON_A, i);
        bindingControllerInput_[i][static_cast<unsigned>(ActionInput::BACKWARD)]
            = PairedControllerInput(ControllerInputs::BUTTON_A, i);
        bindingControllerInput_[i][static_cast<unsigned>(ActionInput::RIGHT)] =
            PairedControllerInput(ControllerInputs::BUTTON_A, i);
        bindingControllerInput_[i][static_cast<unsigned>(ActionInput::LEFT)] =
            PairedControllerInput(ControllerInputs::BUTTON_A, i);
        bindingControllerInput_[i][static_cast<unsigned>(ActionInput::MAIN_SHOOT
            )] =
            PairedControllerInput(ControllerInputs::BUTTON_A, i);
        bindingControllerInput_[i][static_cast<unsigned>(ActionInput::CAMERA)] =
            PairedControllerInput(ControllerInputs::BUTTON_A, i);
        bindingControllerInput_[i][static_cast<unsigned>(ActionInput::MENU)] =
            PairedControllerInput(ControllerInputs::BUTTON_A, i);
    }
}

void InputManager::Init()
{
    //BindFromJson();
    /* Print information about the joysticks */
    logDebug("There are " + std::to_string(SDL_NumJoysticks()) +
             " joysticks attached");
    for (int i = 0; i < SDL_NumJoysticks(); ++i) { PrintJoystick(i); }

    if (SDL_NumJoysticks() > 0) {
        int device = 0;
        joystick_ = SDL_JoystickOpen(device);

        if (joystick_ != NULL) {
            SDL_assert(
                SDL_JoystickFromInstanceID(SDL_JoystickInstanceID(
                    joystick_)) == joystick_);
        }
    }
    for (int i = 0; i < kMaxController_; i++) {
        controllerAxis_[i][static_cast<unsigned>(ControllerAxis::LEFT_BUMPER)]
            = -1.0f;
        controllerAxis_[i][static_cast<unsigned>(ControllerAxis::RIGHT_BUMPER)]
            = -1.0f;
    }
    BindAction();
}

void InputManager::OnPreUserInput()
{
    for (size_t i = 0; i < static_cast<int>(KeyCode::KEYBOARD_SIZE); i++) {
        if (keyPressedState_[i] == ButtonState::UP) {
            keyPressedState_[i] = ButtonState::NONE;
        }
        else if (keyPressedState_[i] == ButtonState::DOWN) {
            keyPressedState_[i] = ButtonState::HELD;
        }
    }

    for (size_t i = 0; i < static_cast<int>(MouseButtonCode::LENGTH); i++) {
        if (buttonState_[i] == ButtonState::UP)
            buttonState_[i] = ButtonState::NONE;
        else if (buttonState_[i] == ButtonState::DOWN)
            buttonState_[i] = ButtonState::HELD;
    }
    for (auto& controllerButtonState : controllerButtonState_) {

        for (size_t i = 0; i < static_cast<int>(ControllerInputs::LENGTH); i++
        ) {
            if (controllerButtonState[i] == ButtonState::UP) {
                controllerButtonState[i] = ButtonState::NONE;
            }
            else if (controllerButtonState[i] == ButtonState::DOWN) {
                controllerButtonState[i] = ButtonState::HELD;
            }
        }
    }
    mouseScroll_ = Vec2f::zero;
}

void InputManager::ProcessInputs(SDL_Event event)
{
    switch (event.type) {
#pragma region keyboard
        case SDL_TEXTINPUT: {
            //TODD(@Luca) Setup Text Input
            break;
        }
        case SDL_TEXTEDITING: {
            //TODD(@Luca) Setup Text Editing
            break;
        }

        case SDL_KEYDOWN: {
            this->keyboard_ = SDL_GetKeyboardState(nullptr);

            const int index = event.key.keysym.scancode;
            keyPressedState_[index] = ButtonState::DOWN;
            break;
        }
        case SDL_KEYUP: {
            this->keyboard_ = SDL_GetKeyboardState(nullptr);

            const int index = event.key.keysym.scancode;
            keyPressedState_[index] = ButtonState::UP;
            break;
        }
#pragma endregion

#pragma region Mouse
        case SDL_MOUSEMOTION: {
            mousePos_.x = -event.motion.x;
            mousePos_.y = -event.motion.y;
            mouseRelativePos_.x = -event.motion.xrel;
            mouseRelativePos_.y = -event.motion.yrel;
            break;
        }
        case SDL_MOUSEBUTTONDOWN: {
            this->mouse_ = SDL_GetMouseState(nullptr, nullptr);

            if (event.button.button == SDL_BUTTON_LEFT)
                buttonState_[static_cast<int>(MouseButtonCode::LEFT)] =
                    ButtonState::DOWN;

            else if (event.button.button == SDL_BUTTON_RIGHT)
                buttonState_[static_cast<int>(MouseButtonCode::RIGHT)] =
                    ButtonState::DOWN;

            else if (event.button.button == SDL_BUTTON_MIDDLE)
                buttonState_[static_cast<int>(MouseButtonCode::MIDDLE)] =
                    ButtonState::DOWN;

            break;
        }
        case SDL_MOUSEBUTTONUP: {
            this->mouse_ = SDL_GetMouseState(nullptr, nullptr);

            if (event.button.button == SDL_BUTTON_LEFT)
                buttonState_[static_cast<int>(MouseButtonCode::LEFT)] =
                    ButtonState::UP;

            else if (event.button.button == SDL_BUTTON_RIGHT)
                buttonState_[static_cast<int>(MouseButtonCode::RIGHT)] =
                    ButtonState::UP;

            else if (event.button.button == SDL_BUTTON_MIDDLE)
                buttonState_[static_cast<int>(MouseButtonCode::MIDDLE)] =
                    ButtonState::UP;

            break;
        }
        case SDL_MOUSEWHEEL: {
            mouseScroll_.x = event.wheel.x;
            mouseScroll_.y = event.wheel.y;

            break;
        }
#pragma endregion

#pragma region Joystick
        case SDL_JOYDEVICEREMOVED: {
            logDebug("Joystick device  removed." + std::to_string(
                         static_cast<int>(event.jdevice.which)));
            if (joystick_ != nullptr) {
                logDebug("Our instance ID is" + std::to_string(
                             static_cast<int>(SDL_JoystickInstanceID(joystick_)
                             )));
                logDebug("There are " + std::to_string(SDL_NumJoysticks()) +
                         " joysticks attached");
                SDL_JoystickClose(joystick_);
            }
            else {
                logDebug("No more joystick");
            }
            if (event.jdevice.which < 8) {
                controllerButtonState_[event.jdevice.which] =
                    std::array<
                        ButtonState, static_cast<int>(
                            ControllerInputs::LENGTH)>();
                controllerAxis_[event.jdevice.which] =
                    std::array<
                        float, static_cast<int>(ControllerAxis::LENGTH)>();
            }
            else {
                logDebug(
                    "Controller removed out of range : Joystick " + event
                                                                    .jdevice.
                                                                    which);
            }

            break;
        }
        case SDL_JOYDEVICEADDED: {
            const int device = event.jdevice.which;
            joystick_ = SDL_JoystickOpen(device);
            if (joystick_ != nullptr) {
                SDL_assert(
                    SDL_JoystickFromInstanceID(SDL_JoystickInstanceID
                        (joystick_)) == joystick_);
            }
            logDebug("There are " + std::to_string(SDL_NumJoysticks()) +
                     " joysticks attached");
            for (int i = 0; i < SDL_NumJoysticks(); ++i) { PrintJoystick(i); }

            if (event.jdevice.which < 8) {
                controllerButtonState_[event.jdevice.which] =
                    std::array<
                        ButtonState, static_cast<int>(
                            ControllerInputs::LENGTH)>();
                controllerAxis_[event.jdevice.which] =
                    std::array<float,
                               static_cast<int>(ControllerAxis::LENGTH)>();
            }
            else {
                logDebug(
                    "Controller added out of range : Joystick " + event
                                                                  .jdevice.
                                                                  which);
                
            }
            break;
        }
        case SDL_JOYBALLMOTION: {
            logDebug("Joystick " + std::to_string(event.jball.which) +
                     " ball " + std::
                     to_string(event.jball.ball) + " delta: (" +
                     std::to_string(
                         event.jball.xrel) + "," +
                     std::to_string(event.jball.yrel) + ")");
            logDebug("JoyBall not support");

            break;
        }

        case SDL_JOYBUTTONDOWN: {
            if (event.jdevice.which < 8) {
                controllerButtonState_[event.jbutton.which][event.jbutton.button
                ] = ButtonState::DOWN;
            }
            else {
                logDebug(
                    "JoyButton pressed down out of range : Joystick " + event
                                                                        .jdevice
                                                                        .which);
                
            }
            break;
        }

        case SDL_JOYBUTTONUP: {
            if (event.jdevice.which < 8) {
                controllerButtonState_[event.jbutton.which][event.jbutton.button] =
                    ButtonState::UP;
            }
            else {
                logDebug(
                    "JoyButton released up out of range : Joystick " + event
                                                                       .jdevice.
                                                                       which);
                
            }
            break;
        }

        case SDL_JOYAXISMOTION: {
            const int deadZone = 3200;
            float value = 0;
            if (event.jaxis.value < -deadZone || event.jaxis.value > deadZone) {
                value = static_cast<float>(event.jaxis.value) / kMaxJoyValue_;
            }
            if (event.jdevice.which < 8) {
                controllerAxis_[event.jaxis.which][event.jaxis.axis] = value;
            }
            else {
                logDebug(
                    "JoyAxis out of range : Joystick " + event.jdevice.which);
            }
            break;
        }
        case SDL_JOYHATMOTION: {
            if (event.jdevice.which < 8) {
                if (event.jhat.value == SDL_HAT_CENTERED) {
                    controllerAxis_[event.jbutton.which][static_cast<unsigned>(
                        ControllerAxis::
                        PAD_VERTICAL)] = 0;
                    controllerAxis_[event.jbutton.which][static_cast<unsigned>(
                        ControllerAxis::
                        PAD_HORIZONTAL)] = 0;
                }
                if (event.jhat.value & SDL_HAT_UP) {
                    controllerAxis_[event.jbutton.which][static_cast<unsigned>(
                        ControllerAxis::
                        PAD_VERTICAL)] = 1.0f;
                }
                if (event.jhat.value & SDL_HAT_RIGHT) {
                    controllerAxis_[event.jbutton.which][static_cast<unsigned>(
                        ControllerAxis::
                        PAD_HORIZONTAL)] = 1.0f;
                }
                if (event.jhat.value & SDL_HAT_DOWN) {
                    controllerAxis_[event.jbutton.which][static_cast<unsigned>(
                        ControllerAxis::
                        PAD_VERTICAL)] = -1.0f;
                }
                if (event.jhat.value & SDL_HAT_LEFT) {
                    controllerAxis_[event.jbutton.which][static_cast<unsigned>(
                        ControllerAxis::
                        PAD_HORIZONTAL)] = -1.0f;
                }
            }
        }
        break;

            /* Fall through to signal quit */
        case SDL_FINGERDOWN: {
            logDebug("Finger down");
            break;
        }
        case SDL_FINGERMOTION: {
            logDebug("Finger motion");
            break;
        }
        case SDL_FINGERUP: {
            logDebug("Finger up");
            break;
        }
#pragma endregion

            //Use of Joystick instead of Controller
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
        default:
            break;
    }
}

bool InputManager::IsKeyDown(KeyCode key) const
{
    return keyPressedState_[static_cast<int>(key)] == ButtonState::DOWN;
}

bool InputManager::IsKeyUp(KeyCode key) const
{
    return keyPressedState_[static_cast<int>(key)] == ButtonState::UP;
}

bool InputManager::IsKeyHeld(KeyCode key) const
{
     return keyPressedState_[static_cast<int>(key)] == ButtonState::HELD;
}

bool InputManager::IsSwitchButtonDown(
    unsigned switchJoyId,
    SwitchInputs key) const
{
    return switchButtonState_[switchJoyId][static_cast<unsigned>(key)] ==
           ButtonState::DOWN;
}

bool InputManager::IsSwitchButtonUp(
    unsigned switchJoyId,
    SwitchInputs key) const
{
    return switchButtonState_[switchJoyId][static_cast<unsigned>(key)] ==
           ButtonState::UP;
}

bool InputManager::IsSwitchButtonHeld(
    unsigned switchJoyId,
    SwitchInputs key) const
{
    return switchButtonState_[switchJoyId][static_cast<unsigned>(key)] ==
           ButtonState::HELD;
}

bool InputManager::IsControllerDown(
    unsigned controllerId,
    ControllerInputs key) const
{
    return controllerButtonState_[controllerId][static_cast<unsigned>(key)] ==
           ButtonState::DOWN;
}

bool InputManager::IsControllerUp(
    unsigned controllerId,
    ControllerInputs key) const
{
    return controllerButtonState_[controllerId][static_cast<unsigned>(key)] ==
           ButtonState::UP;
}

bool InputManager::IsControllerHeld(
    unsigned controllerId,
    ControllerInputs key) const
{
    return controllerButtonState_[controllerId][static_cast<unsigned>(key)] ==
           ButtonState::HELD;
}

Vec2f InputManager::GetMousePosition() const
{
    return mousePos_ * -1.0f;
}

Vec2f InputManager::GetRelativeMousePosition() const
{
    return Vec2f(mousePos_.x / engine_.config.windowSize.x,
                 mousePos_.y / engine_.config.windowSize.y) * -1.0f;
}

Vec2f InputManager::GetMouseScroll() const
{
    return mouseScroll_;
}

float InputManager::GetControllerAxis(
    unsigned controllerId,
    ControllerAxis axis) const
{
    return controllerAxis_[controllerId][static_cast<unsigned>(axis)];
}

float InputManager::GetSwitchAxis(unsigned switchJoyId, SwitchAxis axis) const
{
    return switchAxis_[switchJoyId][static_cast<unsigned>(axis)];
}

bool InputManager::IsMouseButtonDown(MouseButtonCode button) const
{
    return buttonState_[static_cast<int>(button)] == ButtonState::DOWN;
}

bool InputManager::IsMouseButtonUp(MouseButtonCode button) const
{
    return buttonState_[static_cast<int>(button)] == ButtonState::UP;
}

bool InputManager::IsMouseButtonHeld(const MouseButtonCode button) const
{
    return buttonState_[static_cast<int>(button)] == ButtonState::HELD;
}

bool InputManager::IsActionDown(unsigned playerId, ActionInput button) const
{
    const int actionIndex = static_cast<int>(button);
    return IsKeyDown(
               static_cast<KeyCode>(bindingPcInput_[playerId][actionIndex])) ||
           IsControllerDown(
               bindingControllerInput_[playerId][actionIndex].controllerId,
               static_cast<ControllerInputs>(
                   bindingControllerInput_[playerId][actionIndex].
                   controllerInput)
           );
}

bool InputManager::IsActionUp(unsigned playerId, ActionInput button) const
{
    const int actionIndex = static_cast<int>(button);
    return IsKeyUp(static_cast<KeyCode>(bindingPcInput_[playerId][actionIndex]))
           |
           IsControllerUp(
               bindingControllerInput_[playerId][actionIndex].controllerId,
               static_cast<ControllerInputs>(bindingControllerInput_[playerId][
                   actionIndex].controllerInput));
}

bool InputManager::IsActionHeld(unsigned playerId, ActionInput button) const
{
    const int actionIndex = static_cast<int>(button);
    return IsKeyHeld(
               static_cast<KeyCode>(bindingPcInput_[playerId][actionIndex])) ||
           IsControllerHeld(
               bindingControllerInput_[playerId][actionIndex].controllerId,
               static_cast<ControllerInputs>(bindingControllerInput_[playerId][
                   actionIndex].controllerInput)
           );
}

void InputManager::PrintJoystick(const int device) const
{
    //Print info
    const char* name = SDL_JoystickNameForIndex(device);

    std::cout << "Joystick " + std::to_string(device) + ": " + name +
        " Unknown Joystick" << '\n';

    const auto joystick = SDL_JoystickOpen(device);
    if (joystick == nullptr) {
        std::cout << "SDL_JoystickOpen " + std::to_string(device) +
            " failed: %s\n" << '\n';
    }
    else {
        const char* type;

        char guid[64];
        SDL_enabled_assert(
            SDL_JoystickFromInstanceID(SDL_JoystickInstanceID(joystick)) ==
            joystick);
        SDL_JoystickGetGUIDString(
            SDL_JoystickGetGUID(joystick),
            guid,
            sizeof(guid));
        switch (SDL_JoystickGetType(joystick)) {
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
        std::cout << "type: " + *type << '\n';
        std::cout << "axes: " + std::to_string(SDL_JoystickNumAxes(joystick)) <<
            '\n';
        std::cout << "balls: " + std::to_string(SDL_JoystickNumBalls(joystick))
            << '\n';
        std::cout << "hats: " + std::to_string(SDL_JoystickNumHats(joystick)) <<
            '\n';
        std::cout << "buttons: " + std::to_string(
            SDL_JoystickNumButtons(joystick)) << '\n';
        std::cout << "instance id: " + std::to_string(
            SDL_JoystickInstanceID(joystick)) << '\n';
        std::cout << "power level: " + std::to_string(
            static_cast<int>(SDL_JoystickCurrentPowerLevel(joystick))) << '\n';
        std::cout << "guid: " + *guid << '\n';
        std::cout << "VID/PID: " +
            std::to_string(static_cast<int>(SDL_JoystickGetVendor(joystick))) +
            ":" + std::to_string(
                static_cast<int>(SDL_JoystickGetProduct(joystick))) << '\n';

        SDL_JoystickClose(joystick);
    }
}

std::string InputManager::ActionEnumToString(const ActionInput actionInputs)
{
    switch (actionInputs) {
        case ActionInput::FORWARD:
            return "Forward";
        case ActionInput::BACKWARD:
            return "Backward";
        case ActionInput::LEFT:
            return "Left";
        case ActionInput::RIGHT:
            return "Right";
        case ActionInput::MAIN_SHOOT:
            return "Main_Shoot";
        case ActionInput::CAMERA:
            return "Camera";
        case ActionInput::MENU:
            return "Menu";
        default:
            return "";
    }
}

std::string InputManager::ActionEnumToString(ActionAxis actionAxis)
{
    switch (actionAxis) {
        case ActionAxis::HORIZONTAL:
            return "Horizontal";
        case ActionAxis::VERTICAL:
            return "Vertical";
        case ActionAxis::CAMERA_HORIZONTAL:
            return "CameraHorizontal";
        case ActionAxis::CAMERA_VERTICAL:
            return "CameraVertical";
        default:
            return "";
    }
}

std::string InputManager::PcInputsEnumToString(const KeyCode keyCode)
{
    switch (keyCode) {
        case KeyCode::A:
            return "A";
        case KeyCode::B:
            return "B";
        case KeyCode::C:
            return "C";
        case KeyCode::D:
            return "D";
        case KeyCode::E:
            return "E";
        case KeyCode::F:
            return "F";
        case KeyCode::G:
            return "G";
        case KeyCode::H:
            return "H";
        case KeyCode::I:
            return "I";
        case KeyCode::J:
            return "J";
        case KeyCode::K:
            return "K";
        case KeyCode::L:
            return "L";
        case KeyCode::M:
            return "M";
        case KeyCode::N:
            return "N";
        case KeyCode::O:
            return "O";
        case KeyCode::P:
            return "P";
        case KeyCode::Q:
            return "Q";
        case KeyCode::R:
            return "R";
        case KeyCode::S:
            return "S";
        case KeyCode::T:
            return "T";
        case KeyCode::U:
            return "U";
        case KeyCode::V:
            return "V";
        case KeyCode::W:
            return "W";
        case KeyCode::X:
            return "X";
        case KeyCode::Y:
            return "Y";
        case KeyCode::Z:
            return "Z";
        case KeyCode::ESCAPE:
            return "Escape";
        case KeyCode::SPACE:
            return "Space";
        case KeyCode::KEY_LEFT_SHIFT:
            return "Left_Shift";
        case KeyCode::KEY_RIGHT_SHIFT:
            return "Right_Shift";
        case KeyCode::KEY_LEFT_CTRL:
            return "Left_Ctrl";
        case KeyCode::KEY_RIGHT_CTRL:
            return "Right_Ctrl";
        case KeyCode::KEY_LEFT_ALT:
            return "Left_Alt";
        default:
            return "";
    }
}

std::string InputManager::PcInputsEnumToString(MouseButtonCode mouseButton)
{
    switch (mouseButton) {
        case MouseButtonCode::LEFT:
            return "LeftMouse";
        case MouseButtonCode::RIGHT:
            return "RightMouse";
        case MouseButtonCode::MIDDLE:
            return "MiddleMouse";
        default:
            return "";
    }
}

std::string InputManager::SwitchInputsEnumToString(
    const SwitchInputs switchInputs)
{
    switch (switchInputs) {
        default:
            return "";
    }
}

std::string InputManager::SwitchInputsEnumToString(SwitchAxis switchAxis)
{
    switch (switchAxis) {
        default:
            return "";
    }
}

std::string InputManager::ControllerInputsEnumToString(
    const ControllerInputs controller)
{
    switch (controller) {
        case ControllerInputs::PRESS_LEFT_STICK:
            return "Press_Left_Stick";
        case ControllerInputs::LEFT_TRIGGER:
            return "Left_Trigger";
        case ControllerInputs::PRESS_RIGHT_STICK:
            return "Press_Right_Click";
        case ControllerInputs::RIGHT_TRIGGER:
            return "Right_Trigger";
        case ControllerInputs::BUTTON_A:
            return "Button_A";
        case ControllerInputs::BUTTON_B:
            return "Button_B";
        case ControllerInputs::BUTTON_X:
            return "Button_X";
        case ControllerInputs::BUTTON_Y:
            return "Button_Y";
        case ControllerInputs::BUTTON_START:
            return "Button_Start";
        case ControllerInputs::BUTTON_SELECT:
            return "Button_Select";
        default:
            return "";
    }
}

std::string InputManager::ControllerAxisEnumToString(
    const ControllerAxis controller)
{
    switch (controller) {
        case ControllerAxis::VERTICAL_LEFT_AXIS:
            return "Vertical_Left_Stick";
        case ControllerAxis::HORIZONTAL_LEFT_AXIS:
            return "Horizontal_Left_Stick";
        case ControllerAxis::LEFT_BUMPER:
            return "Left_Bumper";
        case ControllerAxis::VERTICAL_RIGHT_AXIS:
            return "Vertical_Right_stick";
        case ControllerAxis::HORIZONTAL_RIGHT_AXIS:
            return "Horizontal_Right_Stick";
        case ControllerAxis::RIGHT_BUMPER:
            return "Right_Bumper";
        case ControllerAxis::PAD_HORIZONTAL:
            return "Pad_Horizontal";
        case ControllerAxis::PAD_VERTICAL:
            return "Pad_Vertical";
        default:
            return "";
    }
}

void InputManager::SimulateKeyDown(const KeyCode key)
{
    SDL_Event sdlEvent = {};
    sdlEvent.type = SDL_KEYDOWN;
    sdlEvent.key.keysym.scancode = static_cast<SDL_Scancode>(key);
    if (SDL_PushEvent(&sdlEvent) < 1) {
        logDebug("SDL_Init failed: " + std::string(SDL_GetError()));
    }
}

void InputManager::SimulateKeyUp(const KeyCode key)
{
    SDL_Event sdlEvent = {};
    sdlEvent.type = SDL_KEYUP;
    sdlEvent.key.keysym.scancode = static_cast<SDL_Scancode>(key);
    if (SDL_PushEvent(&sdlEvent) < 1) {
        logDebug("SDL_Init failed: " + std::string(SDL_GetError()));
    }
}

void InputManager::SimulateMouseDown(const MouseButtonCode button)
{
    SDL_Event sdlEvent = {};
    sdlEvent.type = SDL_MOUSEBUTTONDOWN;
    switch (button) {
        case MouseButtonCode::LEFT:
            sdlEvent.button.button = SDL_BUTTON_LEFT;
            break;
        case MouseButtonCode::RIGHT:
            sdlEvent.button.button = SDL_BUTTON_RIGHT;
            break;
        case MouseButtonCode::MIDDLE:
            sdlEvent.button.button = SDL_BUTTON_MIDDLE;
            break;
        default: ;
    }
    if (SDL_PushEvent(&sdlEvent) < 1) {
        logDebug("SDL_Init failed: " + std::string(SDL_GetError()));
    }
}

void InputManager::SimulateMouseUp(const MouseButtonCode button)
{
    SDL_Event sdlEvent = {};
    sdlEvent.type = SDL_MOUSEBUTTONUP;
    switch (button) {
        case MouseButtonCode::LEFT:
            sdlEvent.button.button = SDL_BUTTON_LEFT;
            break;
        case MouseButtonCode::RIGHT:
            sdlEvent.button.button = SDL_BUTTON_RIGHT;
            break;
        case MouseButtonCode::MIDDLE:
            sdlEvent.button.button = SDL_BUTTON_MIDDLE;
            break;
        default: ;
    }
    if (SDL_PushEvent(&sdlEvent) < 1) {
        logDebug("SDL_Init failed: " + std::string(SDL_GetError()));
    }
}

void InputManager::SimulateControllerDown(
    unsigned controllerId,
    const ControllerInputs key)
{
    SDL_Event sdlEvent = {};
    sdlEvent.type = SDL_JOYBUTTONDOWN;
    sdlEvent.jbutton.button = static_cast<Uint8>(key);
    sdlEvent.jbutton.which = static_cast<SDL_JoystickID>(controllerId);
    if (SDL_PushEvent(&sdlEvent) < 1) {
        logDebug("SDL_Init failed: " + std::string(SDL_GetError()));
    }
}

void InputManager::SimulateControllerUp(
    unsigned controllerId,
    const ControllerInputs key)
{
    SDL_Event sdlEvent = {};
    sdlEvent.type = SDL_JOYBUTTONUP;
    sdlEvent.jbutton.button = static_cast<Uint8>(key);
    sdlEvent.jbutton.which = static_cast<SDL_JoystickID>(controllerId);
    if (SDL_PushEvent(&sdlEvent) < 1) {
        logDebug("SDL_Init failed: " + std::string(SDL_GetError()));
    }
}

void InputManager::SimulateActionDown(unsigned playerId, ActionInput action)
{
    const int actionIndex = static_cast<int>(action);
    SimulateKeyDown(
        static_cast<KeyCode>(bindingPcInput_[playerId][actionIndex]));
    SimulateControllerDown(
        bindingControllerInput_[playerId][actionIndex].controllerId,
        static_cast<ControllerInputs>(bindingControllerInput_[playerId][
            actionIndex].controllerInput));
}

void InputManager::SimulateActionUp(unsigned playerId, ActionInput action)
{
    const int actionIndex = static_cast<int>(action);
    SimulateKeyUp(static_cast<KeyCode>(bindingPcInput_[playerId][actionIndex]));
    SimulateControllerUp(
        bindingControllerInput_[playerId][actionIndex].controllerId,
        static_cast<ControllerInputs>(bindingControllerInput_[playerId][
            actionIndex].controllerInput));
}
}
