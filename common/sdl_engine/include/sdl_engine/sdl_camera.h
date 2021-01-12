#pragma once

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

#include "sdl_engine.h"
#include "graphics/camera.h"

namespace neko::sdl
{
struct MovableCamera : SdlEventSystemInterface, SystemInterface
{
    float moveSpeed = 5.0f;
    float mouseSpeed = 1.0f;
    float mouseSensitivity = 0.5f;

    MovableCamera& operator=(const MovableCamera& other);

protected:
    Vec2f mouseMotion_;
};

struct MovableCamera2D final : neko::Camera2D, MovableCamera
{
    MovableCamera2D& operator=(const MovableCamera2D& other);

    void Init() override {}

    void Update(seconds dt) override;
    void OnEvent(const SDL_Event& event) override;

    void Destroy() override {}
};

struct MovableCamera3D : neko::Camera3D, public MovableCamera
{
	MovableCamera3D& operator=(const MovableCamera3D& other);

	void Init() override;
	void Update(seconds dt) override;
	void Destroy() override {}

	void OnEvent(const SDL_Event& event) override;
};

struct FpsCamera final : public MovableCamera3D
{
    bool freezeCam = false;

    FpsCamera& operator=(const FpsCamera& other);

    void Init() override;

    void Update(seconds dt) override;
    void OnEvent(const SDL_Event& event) override;

    void Destroy() override {}
};
}
