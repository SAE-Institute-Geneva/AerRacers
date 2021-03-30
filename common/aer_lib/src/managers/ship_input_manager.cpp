#include "aer/managers/ship_input_manager.h"
#include "aer/editor/tool/logger.h"
#include "aer/log.h"
#include "aer/managers/player_manager.h"

namespace neko::aer {
ShipInputManager::ShipInputManager(PlayerManager& playerManager) : playerManager_(playerManager)
{
    shipInputs_.resize(INIT_PLAYER_NMB, ShipInput());
}

float ShipInputManager::GetRudder(PlayerId playerId)
{
    return shipInputs_[playerId].GetRudder();
}

float ShipInputManager::GetThruster(PlayerId playerId)
{
    return shipInputs_[playerId].GetThruster();
}

float ShipInputManager::GetIntensity(PlayerId playerId)
{
    return shipInputs_[playerId].GetIntensity();
}

float ShipInputManager::GetJoystickAxis(PlayerId playerId, Joystick joystick, Axis axis) {
    return shipInputs_[playerId].GetJoystickAxis(joystick, axis);
}

bool ShipInputManager::IsBreaking(PlayerId playerId)
{
    return shipInputs_[playerId].IsBreaking();
}

void ShipInputManager::Init() {}

void ShipInputManager::Update(seconds dt)
{
    for (PlayerId playerId = 0; playerId < playerManager_.GetPlayerCount(); ++playerId) {
        auto& shipInput = shipInputs_[playerId];
        shipInput.Update(playerManager_.GetPlayerComponent(playerId).linkedJoystick, dt);
    }
}

void ShipInputManager::EnumToString(Joystick joystick, Direction direction)
{
    std::string name = "";

    switch (direction) {
    case Direction::Center: name = "center";
        break;
    case Direction::Forward: name = "forward";
        break;
    case Direction::ForwardLeft: name = "forwardLeft";
        break;
    case Direction::ForwardRight: name = "forwwardRight";
        break;
    case Direction::Backward: name = "backward";
        break;
    case Direction::BackwardLeft: name = "backwardLeft";
        break;
    case Direction::BackwardRight: name = "backwardRight";
        break;
    case Direction::Left: name = "left";
        break;
    case Direction::Right: name = "right";
        break;
    }

    //switch (joystick) {
    //case Joystick::Left: leftStickName_ = name;

    //    break;
    //case Joystick::Right: rightStickName_ = name;
    //    break;
    //}
}

void ShipInputManager::Destroy() { }

void ShipInputManager::ShipInput::Update(sdl::ControllerId controllerId, seconds dt)
{
    auto& inputLocator = sdl::InputLocator::get();
    std::string currentGestureName = "";
    if (inputLocator.GetControllerIdVector().empty()) return;
    rightJoystick_ = Vec2f(
        inputLocator.GetControllerAxis(controllerId,
            sdl::ControllerAxisType::HORIZONTAL_RIGHT_AXIS),
        -inputLocator.GetControllerAxis(controllerId,
            sdl::ControllerAxisType::VERTICAL_RIGHT_AXIS));

    leftJoystick_ = Vec2f(
        inputLocator.GetControllerAxis(controllerId,
            sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS),
        -inputLocator.GetControllerAxis(controllerId,
            sdl::ControllerAxisType::VERTICAL_LEFT_AXIS));

    rightJoystickDirection_ = GetJoystickDirection(Joystick::Right);
    leftJoystickDirection_ = GetJoystickDirection(Joystick::Left);
    currentGesture_ = GetCurrentGesture();

    float intensity = GetIntensity();
    float turnIntensity = (GetJoystickAngle(Joystick::Left) / 180) + (
        GetJoystickAngle(Joystick::Right) / 180);

    isBreaking_ = false;

    switch (currentGesture_) {
    case Gesture::Default: {
        thruster_ = 0;
        rudder_ = 0;
        currentGestureName = "Default";
    }
                         break;

    case Gesture::Forward: {
        thruster_ = intensity;
        rudder_ = 0;
        currentGestureName = "Forward";
    }
                         break;

    case Gesture::TurnLeftForward: {
        thruster_ = intensity;
        rudder_ = intensity * (turnIntensity);
        currentGestureName = "turnleftforward";
    }
                                 break;

    case Gesture::TurnLeftBackward: {
        thruster_ = -intensity * 0.25f;
        rudder_ = intensity * (turnIntensity * 0.5f);
        currentGestureName = "turnlefbackward";
    }
                                  break;

    case Gesture::TurnRightForward: {
        thruster_ = intensity;
        rudder_ = -intensity * (turnIntensity);
        currentGestureName = "turnrightforward";
    }
                                  break;

    case Gesture::TurnRightBackward: {
        thruster_ = -intensity * .25f;
        rudder_ = -intensity * (turnIntensity * 0.5f);
        currentGestureName = "turnrightbackward";
    }
                                   break;

    case Gesture::RotateLeft: {
        thruster_ = 0;
        rudder_ = intensity;
        currentGestureName = "rotateleft";
    }
                            break;

    case Gesture::RotateRight: {
        thruster_ = 0;
        rudder_ = -intensity;
        currentGestureName = "rotateright";
    }
                             break;

    case Gesture::Backward: {
        thruster_ = -intensity;
        rudder_ = 0;
        isBreaking_ = true;
        currentGestureName = "backward";
    }
                          break;
    }

    //LogDebug("Right Joystick : " + std::to_string(rightJoystick_.x) + "," + std::to_string(rightJoystick_.y));
    //LogDebug("Left Joystick : " + std::to_string(leftJoystick_.x) + "," + std::to_string(leftJoystick_.y));
    //LogDebug("Gesture : " + currentGestureName);
    //LogDebug("Right Joystick : " + rightStickName_);
    //LogDebug("Left Joystick : " + leftStickName_);
}



//bool ShipInputManager::IsJoystickActive(Joystick joystick)
//{
//    switch (joystick) {
//        case Joystick::Right: return (rightJoystick_.y > joystickDeadzone_.y || rightJoystick_.y < -
//                                      joystickDeadzone_.y);
//        case Joystick::Left: return (leftJoystick_.y > joystickDeadzone_.y || leftJoystick_.y < -
//                                     joystickDeadzone_.y);
//        default: return false;
//    }
//}

bool ShipInputManager::ShipInput::IsJoystickAxisInDeadzone(Joystick joystick, Axis axis)
{
    switch (axis) {
        case Axis::Horizontal: return (GetJoystick(joystick).x < joystickDeadzone_.x &&
                                       GetJoystick(joystick).x > -joystickDeadzone_.x);
        case Axis::Vertical: return (GetJoystick(joystick).y < joystickDeadzone_.y &&
                                     GetJoystick(joystick).y > -joystickDeadzone_.y);
        default: return true;
    }
}

bool ShipInputManager::ShipInput::GetBoostButton() { return false; }

float ShipInputManager::ShipInput::GetJoystickAxis(Joystick joystick, Axis axis)
{
    switch (axis) {
        case Axis::Vertical: return GetJoystick(joystick).y;
        case Axis::Horizontal: return GetJoystick(joystick).x;
        default: return 0;
    }
}

float ShipInputManager::ShipInput::GetJoystickAngle(Joystick joystick)
{
    return abs(degree_t(Vec2f::AngleBetween(Vec2f::up, GetJoystick(joystick))).value());
}

float ShipInputManager::ShipInput::GetJoystickMagnitude(Joystick joystick)
{
    return GetJoystick(joystick).Magnitude();
}

float ShipInputManager::ShipInput::GetIntensity()
{
    float rightIntensity = Abs(GetJoystickMagnitude(Joystick::Right));
    float leftIntensity  = Abs(GetJoystickMagnitude(Joystick::Left));

    return (rightIntensity + leftIntensity) * .5f;
}

float ShipInputManager::ShipInput::GetRudder()
{
    return rudder_;
}

float ShipInputManager::ShipInput::GetThruster()
{
    return thruster_;
}

bool ShipInputManager::ShipInput::IsBreaking()
{
    return isBreaking_;
}

Vec2f ShipInputManager::ShipInput::GetJoystick(Joystick joystick)
{
    switch (joystick) {
        case Joystick::Right: return rightJoystick_;
        case Joystick::Left: return leftJoystick_;
        default: return Vec2f::zero;
    }
}

ShipInputManager::Direction ShipInputManager::ShipInput::GetJoystickDirection(Joystick joystick)
{
    //Forward
    if (!IsJoystickAxisInDeadzone(joystick, Axis::Vertical) && GetJoystick(joystick).y >
        joystickDeadzone_.y && IsJoystickAxisInDeadzone(joystick, Axis::Horizontal)) {
        //EnumToString(joystick, Direction::Forward);
        return Direction::Forward;
    }
        //Forward Left
    else if (!IsJoystickAxisInDeadzone(joystick, Axis::Vertical) && GetJoystick(joystick).y >
             joystickDeadzone_.y && GetJoystick(joystick).x < joystickDeadzone_.x) {
        //EnumToString(joystick, Direction::ForwardLeft);
        return Direction::ForwardLeft;
    }
        //Forward Right
    else if (!IsJoystickAxisInDeadzone(joystick, Axis::Vertical) && GetJoystick(joystick).y >
             joystickDeadzone_.y && GetJoystick(joystick).x > joystickDeadzone_.x) {
        //EnumToString(joystick, Direction::ForwardRight);
        return Direction::ForwardRight;
    }
        //Backward
    else if (!IsJoystickAxisInDeadzone(joystick, Axis::Vertical) && GetJoystick(joystick).y <
             joystickDeadzone_.y && IsJoystickAxisInDeadzone(joystick, Axis::Horizontal)) {
        //EnumToString(joystick, Direction::Backward);
        return Direction::Backward;
    }
        //Forward Left
    else if (!IsJoystickAxisInDeadzone(joystick, Axis::Vertical) && GetJoystick(joystick).y <
             joystickDeadzone_.y && GetJoystick(joystick).x < joystickDeadzone_.x) {
        //EnumToString(joystick, Direction::BackwardLeft);
        return Direction::BackwardLeft;
    }
        //Forward Right
    else if (!IsJoystickAxisInDeadzone(joystick, Axis::Vertical) && GetJoystick(joystick).y <
             joystickDeadzone_.y && GetJoystick(joystick).x > joystickDeadzone_.x) {
        //EnumToString(joystick, Direction::BackwardRight);
        return Direction::BackwardRight;
    }
        //Left
    else if (!IsJoystickAxisInDeadzone(joystick, Axis::Horizontal) && GetJoystick(joystick).x <
             joystickDeadzone_.x && IsJoystickAxisInDeadzone(joystick, Axis::Vertical)) {
        //EnumToString(joystick, Direction::Left);
        return Direction::Left;
    }
        //Right
    else if (!IsJoystickAxisInDeadzone(joystick, Axis::Horizontal) && GetJoystick(joystick).x >
             joystickDeadzone_.x && IsJoystickAxisInDeadzone(joystick, Axis::Vertical)) {
        //EnumToString(joystick, Direction::Right);
        return Direction::Right;
    } else {
        //EnumToString(joystick, Direction::Center);
        return Direction::Center;
    }
}

ShipInputManager::Gesture ShipInputManager::ShipInput::GetCurrentGesture()
{
    Direction leftJoystickDir  = GetJoystickDirection(Joystick::Left);
    Direction rightJoystickDir = GetJoystickDirection(Joystick::Right);

    //Default
    if (leftJoystickDir == Direction::Center && rightJoystickDir == Direction::Center) {
        return Gesture::Default;
    }
    //Forward
    if (leftJoystickDir == Direction::Forward && rightJoystickDir == Direction::Forward) {
        return Gesture::Forward;
    }
    //Backward
    if (leftJoystickDir == Direction::Backward && rightJoystickDir == Direction::Backward) {
        return Gesture::Backward;
    }
    //Turn Left Forward
    if (((leftJoystickDir == Direction::Left || leftJoystickDir == Direction::ForwardLeft) && (
             rightJoystickDir == Direction::Forward || rightJoystickDir == Direction::ForwardLeft ||
             rightJoystickDir == Direction::Left))
        ||
        (leftJoystickDir == Direction::Forward) && (
            rightJoystickDir == Direction::Left || rightJoystickDir == Direction::ForwardLeft)
        ||
        (leftJoystickDir == Direction::Center) && (
            rightJoystickDir == Direction::ForwardLeft || rightJoystickDir == Direction::Forward ||
            rightJoystickDir == Direction::ForwardRight)) { return Gesture::TurnLeftForward; }
    //Turn Left Backward
    if (((leftJoystickDir == Direction::Left || leftJoystickDir == Direction::BackwardLeft) && (
             rightJoystickDir == Direction::Backward || rightJoystickDir == Direction::BackwardLeft)
    )) { return Gesture::TurnLeftBackward; }
    //Turn Right Forward
    if (((leftJoystickDir == Direction::Forward || leftJoystickDir == Direction::ForwardRight ||
          leftJoystickDir == Direction::Right) && (
             rightJoystickDir == Direction::Right || rightJoystickDir == Direction::ForwardRight))
        ||
        ((leftJoystickDir == Direction::Right || leftJoystickDir == Direction::ForwardRight) && (
             rightJoystickDir == Direction::Forward))
        ||
        (leftJoystickDir == Direction::ForwardLeft || leftJoystickDir == Direction::ForwardRight ||
         leftJoystickDir == Direction::Forward) && (rightJoystickDir == Direction::Center)) {
        return Gesture::TurnRightForward;
    }
    //Turn Right Backward
    if (((leftJoystickDir == Direction::Right || leftJoystickDir == Direction::BackwardRight) && (
             rightJoystickDir == Direction::Backward || rightJoystickDir == Direction::BackwardRight
         ))) { return Gesture::TurnRightBackward; }
    //Rotate Left
    if ((leftJoystickDir == Direction::Backward || leftJoystickDir == Direction::BackwardLeft ||
         leftJoystickDir == Direction::BackwardRight) &&
        (rightJoystickDir == Direction::Forward || rightJoystickDir == Direction::ForwardLeft ||
         rightJoystickDir == Direction::ForwardRight)) { return Gesture::RotateLeft; }
    //Rotate Right
    if ((leftJoystickDir == Direction::Forward || leftJoystickDir == Direction::ForwardLeft ||
         leftJoystickDir == Direction::ForwardRight) &&
        (rightJoystickDir == Direction::Backward || rightJoystickDir == Direction::BackwardLeft ||
         rightJoystickDir == Direction::BackwardRight)) { return Gesture::RotateRight; }
    //Default
    return Gesture::Default;
}
} // namespace neko::aer
