#pragma once
#include "px/physics_engine.h"
#include "px/rigidbody.h"
#include "engine/component.h"
#include "engine/transform.h"
#include "sdl_engine/sdl_input.h"
namespace neko::aer
{

class ShipInputManager: public SystemInterface
{
public:	
	enum class Joystick {
		Right,
		Left
	};

	enum class Axis {
		Horizontal,
		Vertical
	};

	enum class Gesture {
		Default,
		Forward,
		Backward,
		TurnLeftForward,
		TurnLeftBackward,
		TurnRightForward,
		TurnRightBackward,
		RotateLeft,
		RotateRight,
	};

	enum class Direction {
		Center,
		Forward,
		ForwardLeft,
		ForwardRight,
		Backward,
		BackwardLeft,
		BackwardRight,
		Left,
		Right,
	};
	
    explicit ShipInputManager();

	void Init() override;
	void Update(seconds dt) override;
	void Destroy() override;
	
	bool IsJoystickActive(Joystick joystick);
	bool isJoystickAxisInDeadzone(Joystick joystick, Axis axis);
	bool GetBoostButton();
	
	float GetJoystickAxis(Joystick joystick, Axis axis);
	float GetJoystickAngle(Joystick joystick);
	float GetJoystickMagnitude(Joystick joystick);
	float GetIntensity();
	
	Vec2f GetJoystick(Joystick joystick);
	Direction GetJoystickDirection(Joystick joystick);
	Gesture GetCurrentGesture();

	Vec2f leftJoystick_;
	Vec2f rightJoystick_;
	Vec2f joystickDeadzone_;
	Direction rightJoystickDirection_;
	Direction leftJoystickDirection_;
	Gesture currentGesture_;

	float rudder_;
	float thruster_;
	float isBreaking_;
protected:
};
}
