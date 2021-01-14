#pragma once
#ifdef NEKO_GLES3

#include "gl/shape.h"
#include <gl/shader.h>
#endif
#include <graphics/camera.h>
#include <graphics/color.h>
#include <graphics/graphics.h>
#include <utils/service_locator.h>


namespace neko
{
const static size_t kGizmoReserveSize = 128;

enum class GizmoShape : std::uint8_t
{
	CUBE = 0u,
	LINE,
	SPHERE,
};

struct Gizmos
{
	Gizmos() {}
	
	Vec3f pos = Vec3f::zero;
	Color4 color = Color4(Color::red, 1.0f);
	GizmoShape shape = GizmoShape::CUBE;
	float lineThickness = 1.0f;
	float radius = 1.0f;

	union
	{
		Vec3f cubeSize = Vec3f::zero;
		Vec3f lineEndPos;
	};
};

//-----------------------------------------------------------------------------
// GizmosManagerInterface
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class IGizmosRenderer
{
public:
	
	/**
	 * \brief Generate a wire cube.
	 */
	virtual void DrawCube(
		const Vec3f& pos,
		const Vec3f& size = Vec3f::one,
		const Color4& color = Color4(Color::red, 1.0f),
		float lineThickness = 1.0f) = 0;

	/**
	 * \brief Generate a line.
	 */
	virtual void DrawLine(
		const Vec3f& startPos,
		const Vec3f& endPos,
		const Color4& color = Color4(Color::red, 1.0f),
		float lineThickness = 1.0f) = 0;

	/**
	 * \brief Generate a sphere.
	 */
	virtual void DrawSphere(
		const Vec3f& pos,
		const float& radius = 1.0f,
		const Color4& color = Color4(Color::red, 1.0f),
		float lineThickness = 1.0f) = 0;

	virtual void SetCamera(Camera3D* camera) = 0;
	virtual Camera3D* GetCamera() const = 0;
	virtual Vec3f GetCameraPos() const = 0;
};

//-----------------------------------------------------------------------------
// NullGizmoManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class NullGizmosRenderer final : public IGizmosRenderer
{
public:
	void DrawCube(
		[[maybe_unused]] const Vec3f& pos,
		[[maybe_unused]] const Vec3f& size = Vec3f::one,
		[[maybe_unused]] const Color4& color = Color4(Color::red, 1.0f),
		[[maybe_unused]] float lineThickness = 1.0f) override
	{
	}

	void DrawLine(
		[[maybe_unused]] const Vec3f& startPos,
		[[maybe_unused]] const Vec3f& endPos,
		[[maybe_unused]] const Color4& color = Color4(Color::red, 1.0f),
		[[maybe_unused]] float lineThickness = 1.0f) override
	{
	}

	void DrawSphere(
		[[maybe_unused]] const Vec3f& pos,
		[[maybe_unused]] const float& radius = 1.0f,
		[[maybe_unused]] const Color4& color = Color4(Color::red, 1.0f),
		[[maybe_unused]] float lineThickness = 1.0f) override
	{
	}
	void SetCamera([[maybe_unused]] Camera3D* camera) override {}
	Camera3D* GetCamera() const override { return nullptr; }

	Vec3f GetCameraPos() const override { return {}; }
};


#ifdef NEKO_GLES3

//-----------------------------------------------------------------------------
// Gles3GizmosRenderer
//-----------------------------------------------------------------------------
/// \brief Draw gizmos
class Gles3GizmosRenderer final : public RenderCommandInterface,
                             public SystemInterface,
                             public IGizmosRenderer
{
public:
	explicit Gles3GizmosRenderer(Camera3D* camera);

	void Init() override;

	void Update(seconds dt) override;

	void Render() override;

	void Destroy() override;

	void Start();
	void Stop();
	

	void DrawCube(
		const Vec3f& pos,
		const Vec3f& size = Vec3f::one,
		const Color4& color = Color4(Color::red, 1.0f),
		float lineThickness = 1.0f) override;

	void DrawLine(
		const Vec3f& startPos,
		const Vec3f& endPos,
		const Color4& color = Color4(Color::red, 1.0f),
		float lineThickness = 1.0f) override;

	void DrawSphere(
		const Vec3f& pos,
		const float& radius = 1.0f,
		const Color4& color = Color4(Color::red, 1.0f),
		float lineThickness = 1.0f) override;

	void SetCamera(Camera3D* camera) override;
	Camera3D* GetCamera() const override { return camera_; }
	Vec3f GetCameraPos() const override { return camera_->position; }

    
private:
	std::mutex renderMutex_;

	Camera3D* camera_;
	gl::RenderWireFrameCuboid cube_{Vec3f::zero, Vec3f::one};
	gl::RenderWireFrameSphere sphere_{ Vec3f::zero, 1.0f };
	gl::RenderLine3d line_{Vec3f::zero, Vec3f::one};

	gl::Shader shaderCube_;
	gl::Shader shaderSphere_;
	gl::Shader shaderLine_;

	std::vector<Gizmos> gizmosQueue_;
	bool isRunning_ = true;
};
#endif

#ifdef NEKO_VULKAN

//-----------------------------------------------------------------------------
// NekoGizmosRenderer
//-----------------------------------------------------------------------------
/// \brief Draw gizmos
class NekoGizmosRenderer final : public RenderCommandInterface,
	public SystemInterface,
	public IGizmosRenderer
{
public:
	explicit NekoGizmosRenderer(Camera3D* camera);

	void Init() override;

	void Update(seconds dt) override;

	void Render() override;

	void Destroy() override;

	void Start();
	void Stop();


	void DrawCube(
		const Vec3f& pos,
		const Vec3f& size = Vec3f::one,
		const Color4& color = Color4(Color::red, 1.0f),
		float lineThickness = 1.0f) override;

	void DrawLine(
		const Vec3f& startPos,
		const Vec3f& endPos,
		const Color4& color = Color4(Color::red, 1.0f),
		float lineThickness = 1.0f) override;

	void SetCamera(Camera3D* camera) override;
	Camera3D* GetCamera() const override { return camera_; }
	Vec3f GetCameraPos() const override { return camera_->position; }

private:
	std::mutex renderMutex_;

	Camera3D* camera_;

	std::vector<Gizmos> gizmosQueue_;
	bool isRunning_ = true;
};
#endif
using GizmosLocator = Locator<IGizmosRenderer, NullGizmosRenderer>;
}
