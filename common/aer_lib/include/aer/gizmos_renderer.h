#pragma once
#ifdef NEKO_GLES3
	#include "gl/shader.h"
	#include "gl/shape.h"
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

struct Gizmo
{
	Gizmo() {}

	Vec3f pos = Vec3f::zero;
	EulerAngles rot = EulerAngles(degree_t(0));
	Color4 color = Color::red;
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
// IGizmoRenderer
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class IGizmoRenderer
{
public:
	
	/**
	 * \brief Generate a wire cube
	 */
	virtual void DrawCube(
		const Vec3f& pos,
		const Vec3f& size = Vec3f::one,
		const EulerAngles& rot = EulerAngles(degree_t(0)),
		const Color4& color = Color::red,
		float lineThickness = 1.0f) = 0;

	/**
	 * \brief Generate a line
	 */
	virtual void DrawLine(
		const Vec3f& startPos,
		const Vec3f& endPos,
		const Color4& color = Color::red,
		float lineThickness = 1.0f) = 0;

	/**
	 * \brief Generate a wire sphere
	 */
	virtual void DrawSphere(
		const Vec3f& pos,
		const float& radius = 1.0f,
		const EulerAngles& rot = EulerAngles(degree_t(0)),
		const Color4& color = Color::red,
		float lineThickness = 1.0f) = 0;

	/**
	 * \brief Set the camera
	 */
	virtual void SetCamera(Camera3D* camera) = 0;
	virtual Camera3D* GetCamera() const = 0;
	virtual Vec3f GetCameraPos() const = 0;
};

//-----------------------------------------------------------------------------
// NullGizmoManager
//-----------------------------------------------------------------------------
/// \brief Used for the service locator
class NullGizmoRenderer final : public IGizmoRenderer
{
public:
	void DrawCube(
		[[maybe_unused]] const Vec3f& pos,
		[[maybe_unused]] const Vec3f& size = Vec3f::one,
		[[maybe_unused]] const EulerAngles& rot = EulerAngles(degree_t(0)),
		[[maybe_unused]] const Color4& color = Color::red,
		[[maybe_unused]] float lineThickness = 1.0f) override
	{
	}

	void DrawLine(
		[[maybe_unused]] const Vec3f& startPos,
		[[maybe_unused]] const Vec3f& endPos,
		[[maybe_unused]] const Color4& color = Color::red,
		[[maybe_unused]] float lineThickness = 1.0f) override
	{
	}

	void DrawSphere(
		[[maybe_unused]] const Vec3f& pos,
		[[maybe_unused]] const float& radius = 1.0f,
		[[maybe_unused]] const EulerAngles& rot = EulerAngles(degree_t(0)),
		[[maybe_unused]] const Color4& color = Color::red,
		[[maybe_unused]] float lineThickness = 1.0f) override
	{
	}
	void SetCamera([[maybe_unused]] Camera3D* camera) override {}

	Camera3D* GetCamera() const override { return nullptr; }
	Vec3f GetCameraPos() const override { return {}; }
};

#ifdef NEKO_GLES3
//-----------------------------------------------------------------------------
// GizmoRenderer
//-----------------------------------------------------------------------------
/// \brief Draw gizmos
class GizmoRenderer final : public RenderCommandInterface,
							 public SystemInterface,
							 public IGizmoRenderer
{
public:
	explicit GizmoRenderer(Camera3D* camera);

	void Init() override;
	void Update(seconds dt) override;
	void Render() override;
	void Destroy() override;

	void Start();
	void Stop();

	void DrawCube(
		const Vec3f& pos,
		const Vec3f& size = Vec3f::one,
		const EulerAngles& rot = EulerAngles(degree_t(0)),
		const Color4& color = Color::red,
		float lineThickness = 1.0f) override;

	void DrawLine(const Vec3f& startPos,
		const Vec3f& endPos,
		const Color4& color = Color::red,
		float lineThickness = 1.0f) override;

	void DrawSphere(const Vec3f& pos,
		const float& radius = 1.0f,
		const EulerAngles& rot = EulerAngles(degree_t(0)),
		const Color4& color = Color::red,
		float lineThickness = 1.0f) override;

	void Clear() { gizmosQueue_.clear(); }

	Camera3D* GetCamera() const override { return camera_; }
	Vec3f GetCameraPos() const override { return camera_->position; }

	void SetCamera(Camera3D* camera) override;

private:
	std::mutex renderMutex_;

	Camera3D* camera_;
	gl::RenderWireFrameCuboid cube_ {Vec3f::zero, Vec3f::one};
	gl::RenderWireFrameSphere sphere_ {Vec3f::zero, 1.0f};
	gl::RenderLine3d line_ {Vec3f::zero, Vec3f::one};

	gl::Shader shaderCube_;
	gl::Shader shaderSphere_;
	gl::Shader shaderLine_;

	std::vector<Gizmo> gizmosQueue_;
	std::atomic<bool> isRunning_ = true;

	Job preRender_;
};
#endif

using GizmosLocator = Locator<IGizmoRenderer, NullGizmoRenderer>;
}
