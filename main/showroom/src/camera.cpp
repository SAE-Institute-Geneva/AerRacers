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
	const auto& config = BasicEngine::GetInstance()->GetConfig();
	SetAspect(config.windowSize.x, config.windowSize.y);
	moveSpeed = 100.0f;
	mouseSpeed = 1'500.0f;
}

void ShowRoomCamera3D::Update([[maybe_unused]] seconds dt)
{
	//Checking if keys are down
	SDL_PumpEvents();
	const Uint8* keys = SDL_GetKeyboardState(nullptr);

	int mouseX, mouseY;
	const Uint32 mouseState = SDL_GetMouseState(&mouseX, &mouseY);
	if (mouseState & SDL_BUTTON_MIDDLE)
	{
		if (keys[SDL_SCANCODE_LSHIFT]) //Move
		{
			position += GetRight() * mouseMotion_.x * moveSpeed * dt.count();
			position += -GetUp() * mouseMotion_.y * moveSpeed * dt.count();
			mouseMotion_ = Vec2f::zero;
		}
		else //Rotate
		{
			const Vec3f up = GetUp();
			const Vec3f right = GetRight();
			const Vec3f focusPoint = position - reverseDirection.Normalized() * 15.0f;
			Vec3f camFocusVector = position - focusPoint;
			camFocusVector = Quaternion::AngleAxis(degree_t(mouseMotion_.x * mouseSpeed * dt.count()), up) * camFocusVector;
			camFocusVector = Quaternion::AngleAxis(degree_t(mouseMotion_.y * mouseSpeed * dt.count()), right) * camFocusVector;
			position = focusPoint + camFocusVector;
			WorldLookAt(focusPoint);
			mouseMotion_ = Vec2f::zero;
		}

		const auto& windowSize = BasicEngine::GetInstance()->GetConfig().windowSize;
		const auto window = SDL_GL_GetCurrentWindow();
		const int margin = 2;
		if (mouseX < margin)
		{
			SDL_WarpMouseInWindow(window, static_cast<int>(windowSize.x) - margin, mouseY);
			mouseX = static_cast<int>(windowSize.x) - margin;
			mouseWarped_ = true;
		}
		else if (mouseX > static_cast<int>(windowSize.x) - margin)
		{
			SDL_WarpMouseInWindow(window, margin, mouseY);
			mouseX = margin;
			mouseWarped_ = true;
		}

		if (mouseY < margin)
		{
			SDL_WarpMouseInWindow(window, mouseX, static_cast<int>(windowSize.y) - margin);
			mouseY = static_cast<int>(windowSize.y) - margin;
			mouseWarped_ = true;
		}
		else if (mouseY > static_cast<int>(windowSize.y) - margin)
		{
			SDL_WarpMouseInWindow(window, mouseX, margin);
			mouseY = margin;
			mouseWarped_ = true;
		}
	}

	if (wheelMotion_ != 0.0f)
	{
		position += reverseDirection.Normalized() * wheelMotion_ * wheelSpeed * dt.count();
		wheelMotion_ = 0.0f;
	}
}

void ShowRoomCamera3D::OnEvent(const SDL_Event& event)
{
	if (event.window.event == SDL_WINDOWEVENT_RESIZED)
	{
		SetAspect(event.window.data1, event.window.data2);
	}

	if (event.type == SDL_MOUSEMOTION && !mouseWarped_)
		mouseMotion_ = Vec2f(-event.motion.xrel, -event.motion.yrel) * mouseSensitivity;
	else
		mouseWarped_ = false;

	if (event.type == SDL_MOUSEWHEEL)
		wheelMotion_ = -event.wheel.y;
}
}