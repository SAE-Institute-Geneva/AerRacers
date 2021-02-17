#pragma once
/* ----------------------------------------------------
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

 Author : Simon Canas
 Co-Author :
 Date : 16.02.2021
---------------------------------------------------------- */
#include "sdl_engine/sdl_camera.h"

#include "graphics/camera.h"

namespace neko::dev
{
struct GameCamera : SystemInterface, sdl::SdlEventSystemInterface
{
public:
	void Init() override;
	void Update(seconds dt) override;
	void Destroy() override {}

	void OnEvent(const SDL_Event& event) override {}

	Mat4f GenerateViewMatrix(std::size_t playerNum)
	{ return cameras_[playerNum].GenerateViewMatrix(); }
	Mat4f GenerateProjectionMatrix(std::size_t playerNum)
	{ return cameras_[playerNum].GenerateProjectionMatrix(); }

	void SetPosition(const Vec3f& position);
	void SetPosition(const Vec3f& position, std::size_t playerNum);

	void SetAspect(float aspect);
	void SetAspect(float windowSizeX, float windowSize);
	void SetAspect(Vec2u windowSize);

	void WorldLookAt(const Vec3f& position);
	void WorldLookAt(const Vec3f& position, std::size_t playerNum);

private:
	std::array<sdl::MovableCamera3D, 4> cameras_;
};
}
