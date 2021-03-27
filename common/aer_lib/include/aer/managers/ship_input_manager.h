#pragma once
#include "player_manager.h"
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

	struct ShipInput {
	public:
		void Update(sdl::ControllerId controllerId, seconds dt);
		float GetRudder();
		float GetIntensity();
		float GetThruster();
		bool IsBreaking();
		float GetJoystickAxis(Joystick joystick, Axis axis);

	private:
		bool IsJoystickActive(Joystick joystick);
		bool IsJoystickAxisInDeadzone(Joystick joystick, Axis axis);
		bool GetBoostButton();

		float GetJoystickAngle(Joystick joystick);
		float GetJoystickMagnitude(Joystick joystick);

		Vec2f GetJoystick(Joystick joystick);
		Direction GetJoystickDirection(Joystick joystick);
		Gesture GetCurrentGesture();
		Vec2f leftJoystick_;
		Vec2f rightJoystick_;
		Vec2f joystickDeadzone_ = Vec2f(0.3f, 0.3f);
		Direction rightJoystickDirection_;
		Direction leftJoystickDirection_;
		Gesture currentGesture_;

		float rudder_;
		float thruster_;
		float isBreaking_;
	};

    explicit ShipInputManager(PlayerManager& playerManager);

	float GetRudder(PlayerId playerId);
	float GetThruster(PlayerId playerId);
	float GetIntensity(PlayerId playerId);
	float GetJoystickAxis(PlayerId playerId, Joystick joystick, Axis axis);

	bool IsBreaking(PlayerId playerId);

	void Init() override;
	void Update(seconds dt) override;
	void Destroy() override;

private:
	std::vector<ShipInput> shipInputs_;

	PlayerManager& playerManager_;

	//DEBUG
	//std::string rightStickName_ = "";
	//std::string leftStickName_ = "";
	void EnumToString(Joystick joystick, Direction direction);
};
}
