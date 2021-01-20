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
}
