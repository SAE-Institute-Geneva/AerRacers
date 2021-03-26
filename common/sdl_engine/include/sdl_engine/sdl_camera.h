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
#include "engine/assert.h"
#include "graphics/camera.h"

#include "engine/assert.h"
#include "sdl_engine/sdl_engine.h"

#ifdef NEKO_GLES3
#include "gl/shader.h"
#endif

namespace neko::sdl
{
struct MovableCamera : SdlEventSystemInterface, SystemInterface
{
    float moveSpeed = 5.0f;
    float mouseSpeed = 0.5f;
    float mouseSensitivity = 0.25f;

    MovableCamera& operator=(const MovableCamera& other);

protected:
    Vec2f mouseMotion_;
};

struct MovableCamera2D final : neko::Camera2D, MovableCamera
{
    MovableCamera2D& operator=(const MovableCamera2D& other);

	void Init() override { Camera::Init(); }
    void Update(seconds dt) override;
    void OnEvent(const SDL_Event& event) override;

    void Destroy() override {}
};

struct MovableCamera3D : neko::Camera3D, public MovableCamera
{
	MovableCamera3D& operator=(const MovableCamera3D& other);
	MovableCamera3D& operator=(const Camera3D& other);

	void Init() override { Camera::Init(); }
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

//-----------------------------------------------------------------------------
// MultiCamera
//-----------------------------------------------------------------------------
struct IMultiCamera : SystemInterface, sdl::SdlEventSystemInterface
{
public:
	/// Creates the view matrix for the specified camera
	[[nodiscard]] virtual Mat4f GenerateViewMatrix(std::size_t) const = 0;

	/// Creates the projection matrix for the specified camera
	[[nodiscard]] virtual Mat4f GenerateProjectionMatrix(std::size_t) const = 0;

#ifdef NEKO_GLES3
	virtual void Bind(std::size_t, gl::Shader&) = 0;
#endif

	/// Returns the specified camera
	virtual Camera3D& GetCamera(std::size_t) = 0;

	/// Sets all cameras from the given Camera3D
	virtual void SetCameras(const Camera3D&) = 0;

	/// Sets one camera from the given Camera3D
	virtual void SetCamera(const Camera3D&, std::size_t) = 0;

	/// Returns the position of the specified camera
	[[nodiscard]] virtual const Vec3f& GetPosition(std::size_t) const = 0;

	/// Sets the position for all cameras
	virtual void SetPositions(const Vec3f&) = 0;

	/// Sets the position for one camera
	virtual void SetPosition(const Vec3f&, std::size_t) = 0;

	/// Sets the aspect ratio for all cameras
	virtual void SetAspects(float) = 0;

	/// Sets the aspect ratio for all cameras
	virtual void SetAspects(float, float) = 0;

	/// Sets the aspect ratio for all cameras
	virtual void SetAspects(Vec2u) = 0;

	/// Points all cameras to the specified position (world-space)
	virtual void WorldAllLookAt(const Vec3f&) = 0;

	/// Points a camera to the specified position (world-space)
	virtual void WorldLookAt(const Vec3f&, std::size_t) = 0;
};

struct NullMultiCamera : IMultiCamera
{
public:
	void Init() override {}
	void Update(seconds) override {}
	void Destroy() override {}

	void OnEvent(const SDL_Event&) override {}

	[[nodiscard]] Mat4f GenerateViewMatrix(std::size_t) const override { return {}; }
	[[nodiscard]] Mat4f GenerateProjectionMatrix(std::size_t) const override { return {}; }

#ifdef NEKO_GLES3
	void Bind(std::size_t, gl::Shader&) override {}
#endif

	Camera3D& GetCamera(std::size_t) override { neko_assert(false, "MultiCamera is null!"); }

	void SetCameras(const Camera3D&) override {}
	void SetCamera(const Camera3D&, std::size_t) override {}

	[[nodiscard]] const Vec3f& GetPosition(std::size_t) const override { return Vec3f::zero; }
	void SetPositions(const Vec3f&) override {}
	void SetPosition(const Vec3f&, std::size_t) override {}

	void SetAspects(float) override {}
	void SetAspects(float, float) override {}
	void SetAspects(Vec2u) override {}

	void WorldAllLookAt(const Vec3f&) override {}
	void WorldLookAt(const Vec3f&, std::size_t) override {}
};

/// Contains 4 camera for multiple viewports
struct MultiCamera : IMultiCamera
{
public:
	float moveSpeed = 5.0f;
	float mouseSpeed = 0.5f;
	float mouseSensitivity = 0.25f;

	void Init() override;
	void Update(seconds dt) override;
	void Destroy() override {}

	void OnEvent(const SDL_Event &event) override;

	[[nodiscard]] Mat4f GenerateViewMatrix(std::size_t index) const override;
	[[nodiscard]] Mat4f GenerateProjectionMatrix(std::size_t index) const override;

#ifdef NEKO_GLES3
	void Bind(std::size_t index, gl::Shader& shader) override;
#endif

	Camera3D& GetCamera(std::size_t index) override { return cameras_[index]; }

	void SetCameras(const Camera3D& newCam) override;
	void SetCamera(const Camera3D& camera, std::size_t index) override { cameras_[index] = camera; }

	[[nodiscard]] const Vec3f& GetPosition(std::size_t index) const override;
	void SetPositions(const Vec3f& position) override;
	void SetPosition(const Vec3f& position, std::size_t index) override;

	void SetAspects(float aspect) override;
	void SetAspects(float windowSizeX, float windowSize) override;
	void SetAspects(Vec2u windowSize) override;

	void WorldAllLookAt(const Vec3f& position) override;
	void WorldLookAt(const Vec3f& position, std::size_t index) override;

private:
	Vec2f mouseMotion_;

	std::array<Camera3D, 4> cameras_ {};
};

using MultiCameraLocator = Locator<IMultiCamera, NullMultiCamera>;
}
