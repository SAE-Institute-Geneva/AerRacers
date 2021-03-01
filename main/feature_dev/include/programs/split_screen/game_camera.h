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
/// Contains one camera for each player
struct GameCamera : SystemInterface, sdl::SdlEventSystemInterface
{
public:
	void Init() override;
	void Update(seconds dt) override;
	void Destroy() override {}

	void OnEvent(const SDL_Event &event) override;

	/// Creates the view matrix for the specified camera
	Mat4f GenerateViewMatrix(std::size_t playerNum)
	{ return cameras_[playerNum].GenerateViewMatrix(); }

	/// Creates the projection matrix for the specified camera
	Mat4f GenerateProjectionMatrix(std::size_t playerNum)
	{ return cameras_[playerNum].GenerateProjectionMatrix(); }

	/// Returns the specified camera
	Camera3D& GetCamera(std::size_t playerNum) { return cameras_[playerNum]; }

	/// Sets all player cameras from the given Camera3D
	void SetCameras(const Camera3D& newCam);

	/// Sets one player camera from the given Camera3D
	void SetCamera(const Camera3D& camera, std::size_t playerNum) { cameras_[playerNum] = camera; }

	/// Sets the position for all cameras
	void SetPositions(const Vec3f& position);

	/// Sets the position for one camera
	void SetPosition(const Vec3f& position, std::size_t playerNum);

	/// Sets the aspect ratio for all cameras
	void SetAspects(float aspect);

	/// Sets the aspect ratio for all cameras
	void SetAspects(float windowSizeX, float windowSize);

	/// Sets the aspect ratio for all cameras
	void SetAspects(Vec2u windowSize);

	/// Points all cameras to the specified position (world-space)
	void WorldAllLookAt(const Vec3f& position);

	/// Points a camera to the specified position (world-space)
	void WorldLookAt(const Vec3f& position, std::size_t playerNum);

private:
	Vec2f mouseMotion_;

	std::array<sdl::MovableCamera3D, 4> cameras_;
};
}
