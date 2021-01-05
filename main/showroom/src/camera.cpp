#include "showroom/camera.h"

namespace neko::sr
{
ShowRoomCamera3D& neko::sr::ShowRoomCamera3D::operator=(const ShowRoomCamera3D& other)
{
	position = other.position;
	reverseDirection = other.reverseDirection;

	farPlane = other.farPlane;
	nearPlane = other.nearPlane;
	fovY = other.fovY;

	mouseSpeed = other.mouseSpeed;
	moveSpeed = other.moveSpeed;
	mouseMotion_ = other.mouseMotion_;
	wheelSpeed = other.wheelSpeed;
	wheelMotion_ = other.wheelMotion_;

	return *this;
}

void ShowRoomCamera3D::Init()
{
	const auto& config = BasicEngine::GetInstance()->config;
	SetAspect(config.windowSize.x, config.windowSize.y);
	moveSpeed = 0.3f;
	mouseSpeed = 3.0f;
}

void ShowRoomCamera3D::Update(seconds dt)
{
	//Checking if keys are down
	const Uint8* keys = SDL_GetKeyboardState(nullptr);

	int mouseX, mouseY;
	const Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
	if (mouseState & SDL_BUTTON_MIDDLE)
	{
		if (keys[SDL_SCANCODE_LSHIFT]) //Move
		{
			position += GetRight() * mouseMotion_.x * moveSpeed;
			position += -GetUp() * mouseMotion_.y * moveSpeed;
			mouseMotion_ = Vec2f::zero;
		}
		else //Rotate
		{
			const Vec3f up = GetUp();
			const Vec3f right = GetRight();
			const Vec3f focusPoint = position - reverseDirection.Normalized() * 20.0f;
			Vec3f camFocusVector = position - focusPoint;
			camFocusVector = Quaternion::AngleAxis(degree_t(mouseMotion_.x * mouseSpeed), up) * camFocusVector;
			camFocusVector = Quaternion::AngleAxis(degree_t(mouseMotion_.y * mouseSpeed), right) * camFocusVector;
			position = focusPoint + camFocusVector;
			WorldLookAt(focusPoint);
			mouseMotion_ = Vec2f::zero;
		}

		const auto& windowSize = BasicEngine::GetInstance()->config.windowSize;
		const auto window = SDL_GL_GetCurrentWindow();
		const int margin = 2;
		if (mouseX < margin)
		{
			SDL_WarpMouseInWindow(window, static_cast<int>(windowSize.x) - margin, mouseY);
			mouseX = static_cast<int>(windowSize.x) - margin;
		}
		else if (mouseX > static_cast<int>(windowSize.x) - margin)
		{
			SDL_WarpMouseInWindow(window, margin, mouseY);
			mouseX = margin;
		}

		if (mouseY < margin)
		{
			SDL_WarpMouseInWindow(window, mouseX, static_cast<int>(windowSize.y) - margin);
			mouseY = static_cast<int>(windowSize.y) - margin;
		}
		else if (mouseY > static_cast<int>(windowSize.y) - margin)
		{
			SDL_WarpMouseInWindow(window, mouseX, margin);
			mouseY = margin;
		}
	}

	if (wheelMotion_ != 0.0f)
	{
		position += reverseDirection.Normalized() * wheelMotion_ * wheelSpeed;
		wheelMotion_ = 0.0f;
	}
}

void ShowRoomCamera3D::OnEvent(const SDL_Event& event)
{
	if (event.window.event == SDL_WINDOWEVENT_RESIZED)
	{
		SetAspect(event.window.data1, event.window.data2);
	}

	if (event.type == SDL_MOUSEMOTION)
		mouseMotion_ = Vec2f(-event.motion.xrel, -event.motion.yrel) * mouseSensitivity;

	if (event.type == SDL_MOUSEWHEEL)
		wheelMotion_ = -event.wheel.y;
}
}