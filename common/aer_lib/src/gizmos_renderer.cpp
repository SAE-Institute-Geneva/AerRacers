#include "aer/gizmos_renderer.h"

#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

#ifdef NEKO_GLES3
#include "engine/engine.h"
#include "engine/resource_locations.h"
#include "mathematics/transform.h"

namespace neko
{
GizmoRenderer::GizmoRenderer(Camera3D* camera) : camera_(camera)
{
	GizmosLocator::provide(this);
	isRunning_ = true;
}

void GizmoRenderer::Init()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("GizmoRenderer::Init");
#endif

	preRender_ = Job {[this]()
		{
			shaderCube_.LoadFromFile(GetGlShadersFolderPath() + "gizmoCube.vert",
				GetGlShadersFolderPath() + "gizmoCube.frag");
			shaderLine_.LoadFromFile(GetGlShadersFolderPath() + "gizmoLine.vert",
				GetGlShadersFolderPath() + "gizmoLine.frag");
			shaderSphere_.LoadFromFile(GetGlShadersFolderPath() + "gizmoSphere.vert",
				GetGlShadersFolderPath() + "gizmoSphere.frag");

			shaderCube_.BindUbo(2 * sizeof(Mat4f), gl::kUboMatricesBinding);
			shaderLine_.BindUbo(2 * sizeof(Mat4f), gl::kUboMatricesBinding);
			shaderSphere_.BindUbo(2 * sizeof(Mat4f), gl::kUboMatricesBinding);

			cube_.Init();
			sphere_.Init();
			line_.Init();
		}};

	gizmosQueue_.reserve(kGizmoReserveSize);
	RendererLocator::get().AddPreRenderJob(&preRender_);
}

void GizmoRenderer::Update(seconds)
{
    #ifdef EASY_PROFILE_USE
    EASY_BLOCK("GizmoRenderer::Update");
    #endif
}

void GizmoRenderer::Render()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("GizmoRenderer::Render");
#endif
	if (isRunning_)
	{
		for (auto gizmo : gizmosQueue_)
		{
			std::lock_guard<std::mutex> lock(renderMutex_);
			switch (gizmo.shape)
			{
			case GizmoShape::CUBE:
			{
				if (shaderCube_.GetProgram() == 0) continue;
				shaderCube_.Bind();
				shaderCube_.SetVec4("color", gizmo.color);
				Mat4f model = Mat4f::Identity;
				model = Transform3d::Scale(model, gizmo.cubeSize);
				model = Transform3d::Rotate(model, gizmo.rot);
				model = Transform3d::Translate(model, gizmo.pos);
				shaderCube_.SetMat4("model", model);
				cube_.SetLineWidth(gizmo.lineThickness);
				cube_.Draw();
			}
			break;
			case GizmoShape::LINE:
			{
				if (shaderLine_.GetProgram() == 0) continue;
				shaderLine_.Bind();
				shaderLine_.SetVec4("color", gizmo.color);
				Mat4f model = Transform3d::Translate(Mat4f::Identity, gizmo.pos);
				shaderLine_.SetMat4("model", model);
				shaderLine_.SetVec3("endPos", gizmo.lineEndPos - gizmo.pos);
				line_.SetLineWidth(gizmo.lineThickness);
				line_.Draw();
			}
			break;
			case GizmoShape::SPHERE:
			{
				if (shaderSphere_.GetProgram() == 0) continue;
				shaderSphere_.Bind();
				shaderSphere_.SetVec4("color", gizmo.color);
				Mat4f model = Mat4f::Identity;
				model = Transform3d::Scale(model, Vec3f(gizmo.radius+0.01f));
				model = Transform3d::Rotate(model, gizmo.rot);
				model = Transform3d::Translate(model, gizmo.pos);
				shaderSphere_.SetMat4("model", model);
				sphere_.SetLineWidth(gizmo.lineThickness);
				sphere_.Draw();
			}
			break;
			default:
				logDebug("Invalid Gizmo shape!");
			}
		}
	}
}

void GizmoRenderer::Destroy()
{
	cube_.Destroy();
	sphere_.Destroy();
	line_.Destroy();
	shaderCube_.Destroy();
}

void GizmoRenderer::Start() { isRunning_ = true; }

void GizmoRenderer::Stop() { isRunning_ = false; }

void GizmoRenderer::DrawCube(
	const Vec3f& pos,
	const Vec3f& size,
	const EulerAngles& rot,
	const Color4& color,
	const float lineThickness)
{
	if (isRunning_)
	{
		std::lock_guard<std::mutex> lock(renderMutex_);
		Gizmo gizmo;
		gizmo.pos           = pos;
		gizmo.cubeSize      = size;
		gizmo.rot           = rot;
		gizmo.color         = color;
		gizmo.shape         = GizmoShape::CUBE;
		gizmo.lineThickness = lineThickness;
		gizmosQueue_.push_back(gizmo);
	}
}

void GizmoRenderer::DrawLine(
	const Vec3f& startPos,
	const Vec3f& endPos,
	const Color4& color,
	const float lineThickness)
{
	if (isRunning_)
	{
		std::lock_guard<std::mutex> lock(renderMutex_);
		Gizmo gizmo;
		gizmo.pos           = startPos;
		gizmo.lineEndPos    = endPos;
		gizmo.color         = color;
		gizmo.shape         = GizmoShape::LINE;
		gizmo.lineThickness = lineThickness;
		gizmosQueue_.push_back(gizmo);
	}
}

void GizmoRenderer::DrawSphere(
    const Vec3f& pos,
    const float& radius,
	const EulerAngles& rot,
    const Color4& color,
    float lineThickness)
{
	if (isRunning_)
	{
		std::lock_guard<std::mutex> lock(renderMutex_);
		Gizmo gizmo;
		gizmo.pos           = pos;
		gizmo.radius        = radius;
        gizmo.color         = color;
        gizmo.rot           = rot;
		gizmo.shape         = GizmoShape::SPHERE;
		gizmo.lineThickness = lineThickness;
		gizmosQueue_.push_back(gizmo);
	}
}

void GizmoRenderer::SetCamera(Camera3D* camera) { camera_ = camera; }
}    // namespace neko
#endif
