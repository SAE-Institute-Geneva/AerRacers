#include <sdl_engine/sdl_input.h>

#include <imgui.h>

#include "sdl_engine/sdl_engine.h"

namespace neko::sdl
{
InputManager::InputManager(SdlEngine& engine)
	: keyboard_(nullptr),
	  mouse_(0),
	  engine_(engine)
{
	InputLocator::provide(this);
}

void InputManager::BindAction()
{
	bindingPcInput_[static_cast<unsigned>(InputAction::FORWARD)] = static_cast<
		unsigned>(KeyCode::W);
	bindingPcInput_[static_cast<unsigned>(InputAction::BACKWARD)] = static_cast<
		unsigned>(KeyCode::S);
	bindingPcInput_[static_cast<unsigned>(InputAction::RIGHT)] = static_cast<
		unsigned>(KeyCode::D);
	bindingPcInput_[static_cast<unsigned>(InputAction::LEFT)] = static_cast<
		unsigned>(KeyCode::A);
	bindingPcInput_[static_cast<unsigned>(InputAction::MAIN_SHOOT)] =
		static_cast<unsigned>(KeyCode::E);
	bindingPcInput_[static_cast<unsigned>(InputAction::CAMERA)] =
		static_cast<unsigned>(KeyCode::F);
	bindingPcInput_[static_cast<unsigned>(InputAction::MENU)] =
		static_cast<unsigned>(KeyCode::ESCAPE);
}

void InputManager::Init()
{
	controllerAxis_[static_cast<unsigned>(ControllerAxis::LEFT_BUMPER)] = -1.0f;
	controllerAxis_[static_cast<unsigned>(ControllerAxis::RIGHT_BUMPER)] = -1.0f;
	BindAction();
}

void InputManager::OnPreUserInput()
{
	for (size_t i = 0; i < static_cast<int>(KeyCode::KEYBOARD_SIZE); i++)
	{
		if (keyPressedState_[i] == ButtonState::UP)
		{
			keyPressedState_[i] = ButtonState::NONE;
		}
		else if (keyPressedState_[i] == ButtonState::DOWN)
		{
			keyPressedState_[i] = ButtonState::HELD;
		}
	}

	for (size_t i = 0; i < static_cast<int>(MouseButtonCode::LENGTH); i++)
	{
		if (buttonState_[i] == ButtonState::UP)
			buttonState_[i] = ButtonState::NONE;
		else if (buttonState_[i] == ButtonState::DOWN)
			buttonState_[i] = ButtonState::HELD;
	}

	for (size_t i = 0; i < static_cast<int>(ControllerInputs::LENGTH); i++)
	{
		if (controllerButtonState_[i] == ButtonState::UP)
		{
			controllerButtonState_[i] = ButtonState::NONE;
		}
		else if (controllerButtonState_[i] == ButtonState::DOWN)
		{
			controllerButtonState_[i] = ButtonState::HELD;
		}
	}
	mouseScroll_ = Vec2f::zero;
}

void InputManager::ProcessInputs(SDL_Event event)
{
	switch (event.type)
	{
#pragma region Keyboard
		case SDL_TEXTINPUT:
			{
				break;
			}

		case SDL_KEYDOWN:
			{
				this->keyboard_ = SDL_GetKeyboardState(nullptr);

				const int index = event.key.keysym.scancode;
				keyPressedState_[index] = ButtonState::DOWN;
				break;
			}
		case SDL_KEYUP:
			{
				this->keyboard_ = SDL_GetKeyboardState(nullptr);

				const int index = event.key.keysym.scancode;
				keyPressedState_[index] = ButtonState::UP;
				break;
			}
#pragma endregion

#pragma region Mouse
		case SDL_MOUSEMOTION:
			{
				mousePos_.x = -event.motion.x;
				mousePos_.y = -event.motion.y;
				break;
			}
		case SDL_MOUSEBUTTONDOWN:
			{
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
		case SDL_MOUSEBUTTONUP:
			{
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
		case SDL_MOUSEWHEEL:
			{
				mouseScroll_.x = event.wheel.x;
				mouseScroll_.y = event.wheel.y;

				break;
			}
#pragma endregion

#pragma region Controller
		case SDL_CONTROLLERAXISMOTION:
			{
				std::cout << "Controller" + std::to_string(event.caxis.which) +
					"axis" + std::to_string(
						event.caxis.axis) + "value:" + std::to_string(
						event.caxis.value) << '\n';
				break;
			}
		case SDL_CONTROLLERBUTTONDOWN:
			{
				//std::cout << "Controller " + std::to_string(event.cbutton.which) +
				//	" button " + std::to_string(
				//		event.cbutton.button) + " up" << '\n';
				controllerButtonState_[event.cbutton.button] = ButtonState::DOWN;

				break;
			}
		case SDL_CONTROLLERBUTTONUP:
			{
				//std::cout << "Controller " + std::to_string(event.cbutton.which) +
				//	" button " + std::to_string(
				//		event.cbutton.button) + " up" << '\n';
				controllerButtonState_[event.cbutton.button] = ButtonState::UP;
				break;
			}
		case SDL_CONTROLLERDEVICEADDED:
			{
				const int device = event.cdevice.which;
				controller_ = SDL_GameControllerOpen(device);
				//std::cout << "Controller device  added." + std::to_string(
				//	static_cast<int>(event.cdevice.which)) << '\n';
				break;
			}
		case SDL_CONTROLLERDEVICEREMOVED:
			{
				std::cout << "Controller device removed." + std::to_string(
					static_cast<int>(event.cdevice.which)) << '\n';
				SDL_GameControllerClose(controller_);
				break;
			}
		case SDL_CONTROLLERDEVICEREMAPPED:
			{
				std::cout << "Controller device remapped." + std::to_string(
					static_cast<int>(event.cdevice.which)) << '\n';
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
	if (!keyboard_ || keyboard_ == nullptr)
		return false;

	const auto sdlKey = static_cast<int>(key);

	return keyboard_[sdlKey] != 0;
}

bool InputManager::IsControllerDown(ControllerInputs key) const
{
	return controllerButtonState_[static_cast<unsigned>(key)] == ButtonState::DOWN;
}

bool InputManager::IsControllerUp(ControllerInputs key) const
{
	return controllerButtonState_[static_cast<unsigned>(key)] == ButtonState::UP;
}

bool InputManager::IsControllerHeld(ControllerInputs key) const
{
	return controllerButtonState_[static_cast<unsigned>(key)] == ButtonState::HELD;
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

float InputManager::GetJoystickAxis(ControllerAxis axis) const
{
	return controllerAxis_[static_cast<unsigned>(axis)];
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
	switch (button)
	{
		case MouseButtonCode::LEFT:
			if (mouse_ & SDL_BUTTON(1))
			{
				return true;
			}
			break;
		case MouseButtonCode::RIGHT:
			if (mouse_ & SDL_BUTTON(3))
			{
				return true;
			}
			break;
		case MouseButtonCode::MIDDLE:
			if (mouse_ & SDL_BUTTON(2))
			{
				return true;
			}
			break;
		default:
			break;
	}

	return false;
}

bool InputManager::IsActionDown(InputAction button) const
{
	const int actionIndex = static_cast<int>(button);
	if (bindingPcInput_[actionIndex] < static_cast<int>(KeyCode::KEYBOARD_SIZE))
	{
		return IsKeyDown(static_cast<KeyCode>(bindingPcInput_[actionIndex])) ||
			IsControllerDown(static_cast<ControllerInputs>(
					bindingControllerInput_[actionIndex])
			);
	}
	return IsMouseButtonDown(
			static_cast<MouseButtonCode>(
				bindingPcInput_[actionIndex] - static_cast<int>(KeyCode::KEYBOARD_SIZE))) ||
		IsControllerDown(
			static_cast<ControllerInputs>(bindingControllerInput_[actionIndex]));
}

bool InputManager::IsActionUp(InputAction button) const
{
	const int actionIndex = static_cast<int>(button);
	if (bindingPcInput_[actionIndex] < static_cast<int>(KeyCode::KEYBOARD_SIZE))
	{
		return IsKeyUp(static_cast<KeyCode>(bindingPcInput_[actionIndex])) |
			IsControllerUp(
				static_cast<ControllerInputs>(bindingControllerInput_[actionIndex]));
	}

	return IsMouseButtonUp(
			static_cast<MouseButtonCode>(
				bindingPcInput_[actionIndex] - static_cast<int>(KeyCode::KEYBOARD_SIZE))) |
		IsControllerUp(static_cast<ControllerInputs>(
				bindingControllerInput_[actionIndex]
		));
}

bool InputManager::IsActionHeld(InputAction button) const
{
	const int actionIndex = static_cast<int>(button);
	if (bindingPcInput_[actionIndex] < static_cast<int>(KeyCode::KEYBOARD_SIZE))
	{
		return IsKeyHeld(static_cast<KeyCode>(bindingPcInput_[actionIndex])) ||
			IsControllerHeld(
				static_cast<ControllerInputs>(bindingControllerInput_[actionIndex])
			);
	}

	return IsMouseButtonHeld(
			static_cast<MouseButtonCode>(bindingPcInput_[actionIndex] -
				static_cast<int>(KeyCode::KEYBOARD_SIZE))) ||
		IsControllerHeld(
			static_cast<ControllerInputs>(bindingControllerInput_[actionIndex])
		);
}

std::string InputManager::ActionEnumToString(const InputAction action)
{
	switch (action)
	{
	case InputAction::FORWARD: return "Forward";
	case InputAction::BACKWARD: return "Backward";
	case InputAction::LEFT: return "Left";
	case InputAction::RIGHT: return "Right";
	case InputAction::MAIN_SHOOT: return "Main_Shoot";
	case InputAction::CAMERA: return "Camera";
	case InputAction::MENU: return "Menu";
	default: return "";
	}
}

std::string InputManager::PcInputsEnumToString(const KeyCode keyCode)
{
	switch (keyCode)
	{
		case KeyCode::A: return "A";
		case KeyCode::B: return "B";
		case KeyCode::C: return "C";
		case KeyCode::D: return "D";
		case KeyCode::E: return "E";
		case KeyCode::F: return "F";
		case KeyCode::G: return "G";
		case KeyCode::H: return "H";
		case KeyCode::I: return "I";
		case KeyCode::J: return "J";
		case KeyCode::K: return "K";
		case KeyCode::L: return "L";
		case KeyCode::M: return "M";
		case KeyCode::N: return "N";
		case KeyCode::O: return "O";
		case KeyCode::P: return "P";
		case KeyCode::Q: return "Q";
		case KeyCode::R: return "R";
		case KeyCode::S: return "S";
		case KeyCode::T: return "T";
		case KeyCode::U: return "U";
		case KeyCode::V: return "V";
		case KeyCode::W: return "W";
		case KeyCode::X: return "X";
		case KeyCode::Y: return "Y";
		case KeyCode::Z: return "Z";
		case KeyCode::ESCAPE: return "Escape";
		case KeyCode::SPACE: return "Space";
		case KeyCode::KEY_LEFT_SHIFT: return "Left_Shift";
		case KeyCode::KEY_RIGHT_SHIFT: return "Right_Shift";
		case KeyCode::KEY_LEFT_CTRL: return "Left_Ctrl";
		case KeyCode::KEY_RIGHT_CTRL: return "Right_Ctrl";
		case KeyCode::KEY_LEFT_ALT: return "Left_Alt";
		default: return "";
	}
}

std::string InputManager::PcInputsEnumToString(MouseButtonCode mouseButton)
{
	switch (mouseButton)
	{
		case MouseButtonCode::LEFT: return "LeftMouse";
		case MouseButtonCode::RIGHT: return "RightMouse";
		case MouseButtonCode::MIDDLE: return "MiddleMouse";
		default:
			return "";
	}
}

std::string InputManager::ControllerInputsEnumToString(
	const ControllerInputs controller)
{
	switch (controller)
	{
		case ControllerInputs::PRESS_LEFT_STICK: return "Press_Left_Stick";
		case ControllerInputs::LEFT_TRIGGER: return "Left_Trigger";
		case ControllerInputs::PRESS_RIGHT_STICK: return "Press_Right_Click";
		case ControllerInputs::RIGHT_TRIGGER: return "Right_Trigger";
		case ControllerInputs::BUTTON_A: return "Button_A";
		case ControllerInputs::BUTTON_B: return "Button_B";
		case ControllerInputs::BUTTON_X: return "Button_X";
		case ControllerInputs::BUTTON_Y: return "Button_Y";
		case ControllerInputs::BUTTON_START: return "Button_Start";
		case ControllerInputs::BUTTON_SELECT: return "Button_Select";
		default: return "";
	}
}

std::string InputManager::ControllerAxisEnumToString(
	const ControllerAxis controller)
{
	switch (controller)
	{
		case ControllerAxis::VERTICAL_LEFT_AXIS: return "Vertical_Left_Stick";
		case ControllerAxis::HORIZONTAL_LEFT_AXIS: return "Horizontal_Left_Stick";
		case ControllerAxis::LEFT_BUMPER: return "Left_Bumper";
		case ControllerAxis::VERTICAL_RIGHT_AXIS: return "Vertical_Right_stick";
		case ControllerAxis::HORIZONTAL_RIGHT_AXIS: return "Horizontal_Right_Stick";
		case ControllerAxis::RIGHT_BUMPER: return "Right_Bumper";
		case ControllerAxis::PAD_HORIZONTAL: return "Pad_Horizontal";
		case ControllerAxis::PAD_VERTICAL: return "Pad_Vertical";
		default: return "";
	}
}

void InputManager::SimulateKeyDown(const KeyCode key)
{
	SDL_Event sdlEvent = {};
	sdlEvent.type = SDL_KEYDOWN;
	sdlEvent.key.keysym.scancode = static_cast<SDL_Scancode>(key);
	if (SDL_PushEvent(&sdlEvent) < 1) { logDebug("SDL_Init failed: " + std::string (SDL_GetError())); }
}

void InputManager::SimulateKeyUp(const KeyCode key)
{
	SDL_Event sdlEvent = {};
	sdlEvent.type = SDL_KEYUP;
	sdlEvent.key.keysym.scancode = static_cast<SDL_Scancode>(key);
	if (SDL_PushEvent(&sdlEvent) < 1) { logDebug("SDL_Init failed: " + std::string(SDL_GetError())); }
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
	default:;
	}
	if (SDL_PushEvent(&sdlEvent) < 1) { logDebug("SDL_Init failed: " + std::string(SDL_GetError())); }
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
	default:;
	}
	if (SDL_PushEvent(&sdlEvent) < 1) { logDebug("SDL_Init failed: " + std::string(SDL_GetError())); }
}

void InputManager::SimulateControllerDown(const ControllerInputs key)
{
	SDL_Event sdlEvent = {};
	sdlEvent.type = SDL_CONTROLLERBUTTONDOWN;
	sdlEvent.cbutton.button = static_cast<Uint8>(key);
	if (SDL_PushEvent(&sdlEvent) < 1) { logDebug("SDL_Init failed: " + std::string(SDL_GetError())); }
}

void InputManager::SimulateControllerUp(const ControllerInputs key)
{
	SDL_Event sdlEvent = {};
	sdlEvent.type = SDL_CONTROLLERBUTTONUP;
	sdlEvent.cbutton.button = static_cast<Uint8>(key);
	if (SDL_PushEvent(&sdlEvent) < 1) { logDebug("SDL_Init failed: " + std::string(SDL_GetError())); }
}

void InputManager::SimulateActionDown(InputAction action)
{
	const int actionIndex = static_cast<int>(action);
	if (bindingPcInput_[actionIndex] < static_cast<int>(KeyCode::KEYBOARD_SIZE)) {
		SimulateKeyDown(static_cast<KeyCode>(bindingPcInput_[actionIndex]));
	}
	SimulateMouseDown(
		static_cast<MouseButtonCode>(
			bindingPcInput_[actionIndex] - static_cast<int>(KeyCode::KEYBOARD_SIZE)
			));
}

void InputManager::SimulateActionUp(InputAction action)
{
	const int actionIndex = static_cast<int>(action);
	if (bindingPcInput_[actionIndex] < static_cast<int>(KeyCode::KEYBOARD_SIZE)) {
		SimulateKeyUp(static_cast<KeyCode>(bindingPcInput_[actionIndex]));
	}
	SimulateMouseUp(
		static_cast<MouseButtonCode>(
			bindingPcInput_[actionIndex] - static_cast<int>(KeyCode::KEYBOARD_SIZE)
			));
}
}