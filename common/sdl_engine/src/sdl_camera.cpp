/*
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
 */

#include "sdl_engine/sdl_camera.h"

namespace neko::sdl
{
//-----------------------------------------------------------------------------
// MovableCamera
//-----------------------------------------------------------------------------
MovableCamera& MovableCamera::operator=(const MovableCamera& other)
{
    mouseSpeed = other.mouseSpeed;
    moveSpeed = other.moveSpeed;
    mouseMotion_ = other.mouseMotion_;

    return *this;
}

//-----------------------------------------------------------------------------
// MovableCamera2D
//-----------------------------------------------------------------------------
MovableCamera2D& MovableCamera2D::operator=(const MovableCamera2D& other)
{
    position = other.position;
    reverseDirection = other.reverseDirection;

    farPlane = other.farPlane;
    nearPlane = other.nearPlane;
    size = other.size;

    mouseSpeed = other.mouseSpeed;
    moveSpeed = other.moveSpeed;
    mouseMotion_ = other.mouseMotion_;

    return *this;
}

void MovableCamera2D::Update(const seconds dt)
{
    //Checking if keys are down
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
        position += GetRight() * moveSpeed * dt.count();
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
        position -= GetRight() * moveSpeed * dt.count();
    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
        size -= moveSpeed * dt.count();
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
        size += moveSpeed * dt.count();
    if (keys[SDL_SCANCODE_SPACE])
        position.y += moveSpeed * dt.count();
    if (keys[SDL_SCANCODE_LSHIFT])
        position.y -= moveSpeed * dt.count();

    const auto mouseState = SDL_GetMouseState(nullptr, nullptr);
    if (mouseState & SDL_BUTTON(3))
    {
        Rotate(EulerAngles(
                degree_t(mouseMotion_.y),
                degree_t(mouseMotion_.x),
                degree_t(0.0f)
        ));
        mouseMotion_ = Vec2f::zero;
    }
}

void MovableCamera2D::OnEvent(const SDL_Event& event)
{
    if(event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        SetAspect(event.window.data1, event.window.data2);
    }

    if (event.type == SDL_MOUSEMOTION)
        mouseMotion_ = Vec2f(-event.motion.xrel, -event.motion.yrel) / moveSpeed;
}

//-----------------------------------------------------------------------------
// MovableCamera3D
//-----------------------------------------------------------------------------
MovableCamera3D& MovableCamera3D::operator=(const MovableCamera3D& other)
{
    position = other.position;
    reverseDirection = other.reverseDirection;

    farPlane = other.farPlane;
    nearPlane = other.nearPlane;
    fovY = other.fovY;

    mouseSpeed = other.mouseSpeed;
    moveSpeed = other.moveSpeed;
    mouseMotion_ = other.mouseMotion_;

    return *this;
}

MovableCamera3D& MovableCamera3D::operator=(const Camera3D& other)
{
	position = other.position;
	reverseDirection = other.reverseDirection;

	farPlane = other.farPlane;
	nearPlane = other.nearPlane;
	fovY = other.fovY;

	return *this;
}

void MovableCamera3D::Update(const seconds dt)
{
    //Checking if keys are down
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    Vec3f move{};
    //Checking if keys are down
    if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
        move += GetRight() * dt.count();
    if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
        move -= GetRight() * dt.count();
    if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
        move -= reverseDirection * dt.count();
    if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
        move += reverseDirection * dt.count();
    if (keys[SDL_SCANCODE_SPACE])
        move.y += dt.count();
    if (keys[SDL_SCANCODE_LSHIFT])
        move.y -= dt.count();

    if (keys[SDL_SCANCODE_R])
        position += move * moveSpeed * 5.0f;
    else
        position += move * moveSpeed;

    const auto mouseState = SDL_GetMouseState(nullptr, nullptr);
    if (mouseState & SDL_BUTTON(3))
    {
        Rotate(EulerAngles(
                degree_t(mouseMotion_.y),
                degree_t(mouseMotion_.x),
                degree_t(0.0f)
        ));
        mouseMotion_ = Vec2f::zero;
    }
}

void MovableCamera3D::OnEvent(const SDL_Event& event)
{
    if (event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        SetAspect(event.window.data1, event.window.data2);
    }

    if (event.type == SDL_MOUSEMOTION)
        mouseMotion_ = Vec2f(-event.motion.xrel, -event.motion.yrel) * mouseSensitivity;
}

//-----------------------------------------------------------------------------
// FpsCamera
//-----------------------------------------------------------------------------
FpsCamera& FpsCamera::operator=(const FpsCamera& other)
{
    position = other.position;
    reverseDirection = other.reverseDirection;

    farPlane = other.farPlane;
    nearPlane = other.nearPlane;
    fovY = other.fovY;

    mouseSpeed = other.mouseSpeed;
    moveSpeed = other.moveSpeed;
    mouseMotion_ = other.mouseMotion_;

    freezeCam = other.freezeCam;

    return *this;
}

void FpsCamera::Init()
{
    SDL_SetRelativeMouseMode(SDL_TRUE);
}

void FpsCamera::Update(const seconds dt)
{
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    if (!freezeCam)
    {
        Vec3f move{};
        //Checking if keys are down
        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D])
            move += GetRight() * dt.count();
        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A])
            move -= GetRight() * dt.count();
        if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W])
            move -= reverseDirection * dt.count();
        if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S])
            move += reverseDirection * dt.count();
        if (keys[SDL_SCANCODE_SPACE])
            move.y += dt.count();
        if (keys[SDL_SCANCODE_LSHIFT])
            move.y -= dt.count();

        if (keys[SDL_SCANCODE_R])
            position += move * moveSpeed * 5.0f;
        else
            position += move * moveSpeed;

        Rotate(EulerAngles(
                degree_t(mouseMotion_.y),
                degree_t(mouseMotion_.x),
                degree_t(0.0f)
        ));
        mouseMotion_ = Vec2f::zero;
    }
}

void FpsCamera::OnEvent(const SDL_Event& event)
{
    const Uint8* keys = SDL_GetKeyboardState(nullptr);
    if (keys[SDL_SCANCODE_ESCAPE] && event.type == SDL_KEYDOWN)
    {
        freezeCam = !freezeCam;
        SDL_SetRelativeMouseMode(static_cast<SDL_bool>(!freezeCam));
    }

    if(event.window.event == SDL_WINDOWEVENT_RESIZED)
    {
        SetAspect(event.window.data1, event.window.data2);
    }

    if (!freezeCam)
    {
        if (event.type == SDL_MOUSEMOTION)
            mouseMotion_ = Vec2f(-event.motion.xrel, -event.motion.yrel) * mouseSensitivity;

        SDL_WarpMouseGlobal(event.window.data1 / 2, event.window.data2 / 2);
    }
}

//-----------------------------------------------------------------------------
// MultiCamera
//-----------------------------------------------------------------------------
void MultiCamera::Init()
{
	for (auto& camera : cameras_)
	{
		camera.Init();
		mouseSpeed = 100.0f;
	}
}

void MultiCamera::Update(const seconds dt)
{
	const auto& inputManager = sdl::InputLocator::get();
	const auto& controllers  = inputManager.GetControllerIdVector();
	for (std::size_t i = 0; i < controllers.size() / 2; ++i)
	{
		if (inputManager.GetControllerIdVector().size() <= i) continue;
		sdl::ControllerId controllerId = inputManager.GetControllerIdVector()[i];
		// Query left joystick movement
		const float xMove = inputManager.GetControllerAxis(
			controllerId, sdl::ControllerAxisType::HORIZONTAL_LEFT_AXIS);
		const float zMove = inputManager.GetControllerAxis(
			controllerId, sdl::ControllerAxisType::VERTICAL_LEFT_AXIS);
		const Vec3f right = cameras_[i].GetRight();

		Vec3f move {};
		move += right * xMove * dt.count();
		move += cameras_[i].reverseDirection * zMove * dt.count();

		// Vertical Joystick Movement
		if (inputManager.GetControllerButtonState(
				controllerId, sdl::ControllerButtonType::BUTTON_B) == sdl::ButtonState::HELD)
			move += Vec3f::up * dt.count();
		else if (inputManager.GetControllerButtonState(
					 controllerId, sdl::ControllerButtonType::BUTTON_A) == sdl::ButtonState::HELD)
			move += Vec3f::down * dt.count();

		// Apply movement and check for turbo mode
		const sdl::ButtonState buttonState = inputManager.GetControllerButtonState(
			controllerId, sdl::ControllerButtonType::BUTTON_X);
		if (buttonState == sdl::ButtonState::DOWN) cameras_[i].position += move * moveSpeed * 5.0f;
		else cameras_[i].position += move * moveSpeed;

		// Joystick Rotation
		const float xCamera = inputManager.GetControllerAxis(
			controllerId, sdl::ControllerAxisType::VERTICAL_RIGHT_AXIS);
		const float yCamera = inputManager.GetControllerAxis(
			controllerId, sdl::ControllerAxisType::HORIZONTAL_RIGHT_AXIS);
		cameras_[i].Rotate(EulerAngles(degree_t(xCamera * mouseSpeed * dt.count()),
			degree_t(yCamera * mouseSpeed * dt.count()),
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
		cameras_[0].position += move * moveSpeed * 5.0f;
	else
		cameras_[0].position += move * moveSpeed;

	const auto mouseState = SDL_GetMouseState(nullptr, nullptr);
	if (mouseState & SDL_BUTTON(3))
	{
		cameras_[0].Rotate(EulerAngles(
			degree_t(mouseMotion_.y),
			degree_t(mouseMotion_.x),
			degree_t(0.0f)));
		mouseMotion_ = Vec2f::zero;
	}
}

void MultiCamera::OnEvent(const SDL_Event& event)
{
	if (event.type == SDL_MOUSEMOTION)
		mouseMotion_ = Vec2f(-event.motion.xrel, -event.motion.yrel) * mouseSensitivity;
}

Mat4f MultiCamera::GenerateViewMatrix(std::size_t index) const
{
	return cameras_[index].GenerateViewMatrix();
}

Mat4f MultiCamera::GenerateProjectionMatrix(std::size_t index) const
{
	return cameras_[index].GenerateProjectionMatrix();
}

#ifdef NEKO_GLES3
void GameCamera::Bind(std::size_t index, gl::Shader& shader)
{
	Mat4f camProj = GenerateProjectionMatrix(index);
	Mat4f camView = GenerateViewMatrix(index);
	shader.SetUbo(sizeof(Mat4f), 0, &camProj, gl::kUboMatricesBinding);
	shader.SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView, gl::kUboMatricesBinding);
	CameraLocator::provide(&cameras_[index]);
}
#endif

void MultiCamera::SetCameras(const Camera3D& newCam)
{
	for (auto& camera : cameras_) camera = newCam;
}

const Vec3f& MultiCamera::GetPosition(std::size_t index) const { return cameras_[index].position; }

void MultiCamera::SetPositions(const Vec3f& position)
{
	for (auto& camera : cameras_) camera.position = position;
}

void MultiCamera::SetPosition(const Vec3f& position, std::size_t index)
{
	cameras_[index].position = position;
}

void MultiCamera::SetAspects(float aspect)
{
	for (auto& camera : cameras_) camera.SetAspect(aspect);
}

void MultiCamera::SetAspects(const float windowSizeX, const float windowSize)
{
	for (auto& camera : cameras_) camera.SetAspect(windowSizeX, windowSize);
}

void MultiCamera::SetAspects(Vec2u windowSize)
{
	for (auto& camera : cameras_) camera.SetAspect(windowSize.x, windowSize.y);
}

void MultiCamera::WorldAllLookAt(const Vec3f& position)
{
	for (auto& camera : cameras_) camera.WorldLookAt(position);
}

void MultiCamera::WorldLookAt(const Vec3f& position, std::size_t index)
{
	cameras_[index].WorldLookAt(position);
}
}
