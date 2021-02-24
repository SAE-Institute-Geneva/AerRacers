#include "aer/game/game_camera.h"

namespace neko::aer
{
//-----------------------------------------------------------------------------
// GameCamera
//-----------------------------------------------------------------------------
void GameCamera::Init()
{
	for (auto& camera : cameras_)
	{
		camera.Init();
		camera.mouseSpeed = 100.0f;
	}
}

void GameCamera::Update(const seconds dt)
{
	const auto& inputManager = sdl::InputLocator::get();
	const auto& controllers = inputManager.GetControllerIdVector();
	for (std::size_t i = 0; i < controllers.size() / 2; ++i)
	{
		// Query left joystick movement
		const float xMove =
			inputManager.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS);
		const float zMove =
			inputManager.GetControllerAxis(i, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS);
		const Vec3f right = cameras_[i].GetRight();

		Vec3f move {};
		move += right * xMove * dt.count();
		move += cameras_[i].reverseDirection * zMove * dt.count();

		// Vertical Joystick Movement
		if (inputManager.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_B) ==
			sdl::ButtonState::HELD)
			move += Vec3f::up * dt.count();
		else if (inputManager.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_A) ==
				 sdl::ButtonState::HELD)
			move += Vec3f::down * dt.count();

		// Apply movement and check for turbo mode
		const sdl::ButtonState buttonState =
			inputManager.GetControllerButtonState(i, sdl::ControllerButtonType::BUTTON_X);
		if (buttonState == sdl::ButtonState::DOWN)
			cameras_[i].position += move * cameras_[i].moveSpeed * 5.0f;
		else
			cameras_[i].position += move * cameras_[i].moveSpeed;

		// Joystick Rotation
		const float xCamera =
			inputManager.GetControllerAxis(i, sdl::ControllerAxisType::VERTICAL_RIGHT_AXIS);
		const float yCamera =
			inputManager.GetControllerAxis(i, sdl::ControllerAxisType::HORIZONTAL_RIGHT_AXIS);
		cameras_[i].Rotate(EulerAngles(degree_t(xCamera * cameras_[i].mouseSpeed * dt.count()),
			degree_t(yCamera * cameras_[i].mouseSpeed * dt.count()),
			degree_t(0.0f)));
	}

	// Query Keyboard movement
	Vec3f move {};
	const Vec3f right = cameras_[0].GetRight();
	if (inputManager.GetKeyState(sdl::KeyCodeType::D) == sdl::ButtonState::HELD)
		move += right * dt.count();
	if (inputManager.GetKeyState(sdl::KeyCodeType::A) == sdl::ButtonState::HELD)
		move -= right * dt.count();
	if (inputManager.GetKeyState(sdl::KeyCodeType::W) == sdl::ButtonState::HELD)
		move -= cameras_[0].reverseDirection * dt.count();
	if (inputManager.GetKeyState(sdl::KeyCodeType::S) == sdl::ButtonState::HELD)
		move += cameras_[0].reverseDirection * dt.count();
	if (inputManager.GetKeyState(sdl::KeyCodeType::SPACE) == sdl::ButtonState::HELD)
		move.y += dt.count();
	if (inputManager.GetKeyState(sdl::KeyCodeType::KEY_LEFT_SHIFT) == sdl::ButtonState::HELD)
		move.y -= dt.count();

	// Apply movement and check for turbo mode
	if (inputManager.GetKeyState(sdl::KeyCodeType::R) == sdl::ButtonState::HELD)
		cameras_[0].position += move * cameras_[0].moveSpeed * 5.0f;
	else
		cameras_[0].position += move * cameras_[0].moveSpeed;

	const auto mouseState = SDL_GetMouseState(nullptr, nullptr);
	if (mouseState & SDL_BUTTON(3))
	{
		cameras_[0].Rotate(EulerAngles(
			degree_t(mouseMotion_.y),
			degree_t(mouseMotion_.x),
			degree_t(0.0f)
		));
		mouseMotion_ = Vec2f::zero;
	}
}

void GameCamera::OnEvent(const SDL_Event& event)
{
	if (event.type == SDL_MOUSEMOTION)
		mouseMotion_ = Vec2f(-event.motion.xrel, -event.motion.yrel) * cameras_[0].mouseSensitivity;
}

void GameCamera::SetCameras(const Camera3D& newCam)
{
	for (auto& camera : cameras_)
		camera = newCam;
}

void GameCamera::SetPositions(const Vec3f& position)
{
	for (auto& camera : cameras_)
		camera.position = position;
}

void GameCamera::SetPosition(const Vec3f& position, std::size_t playerNum)
{
	cameras_[playerNum].position = position;
}

void GameCamera::SetAspects(float aspect)
{
	for (auto& camera : cameras_)
		camera.SetAspect(aspect);
}

void GameCamera::SetAspects(float windowSizeX, float windowSize)
{
	for (auto& camera : cameras_)
		camera.SetAspect(windowSizeX, windowSize);
}

void GameCamera::SetAspects(Vec2u windowSize)
{
	for (auto& camera : cameras_)
		camera.SetAspect(windowSize.x, windowSize.y);
}

void GameCamera::WorldAllLookAt(const Vec3f& position)
{
	for (auto& camera : cameras_)
		camera.WorldLookAt(position);
}

void GameCamera::WorldLookAt(const Vec3f& position, std::size_t playerNum)
{
	cameras_[playerNum].WorldLookAt(position);
}
}
