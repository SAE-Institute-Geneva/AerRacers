#include "showroom/gizmos_renderer.h"

#include <engine/engine.h>

namespace neko::sr
{
GizmoRenderer::GizmoRenderer(Camera3D* camera) : camera_(camera)
{
	GizmosLocator::provide(this);
	isRunning_ = true;
}

void GizmoRenderer::Init()
{
	const auto& config = BasicEngine::GetInstance()->GetConfig();

	preRender_ = Job {[this, config]()
		{
			shaderCube_.LoadFromFile(config.dataRootPath + "shaders/opengl/gizmoCube.vert",
				config.dataRootPath + "shaders/opengl/gizmoCube.frag");
			shaderLine_.LoadFromFile(
				config.dataRootPath + "shaders/opengl/instancing/gizmoLine.vert",
				config.dataRootPath + "shaders/opengl/instancing/gizmoLine.frag");
			shaderSphere_.LoadFromFile(config.dataRootPath + "shaders/opengl/gizmoSphere.vert",
				config.dataRootPath + "shaders/opengl/gizmoSphere.frag");

			cube_.Init();
			sphere_.Init();

            line_.Init();
            glBindVertexArray(line_.VAO);
            glGenBuffers(1, &line_.VBO[1]);
            glBindBuffer(GL_ARRAY_BUFFER, line_.VBO[1]);
            glBufferData(GL_ARRAY_BUFFER,
                sizeof(GizmoLine) * lineGizmos_.size(),
                lineGizmos_.data(),
                GL_DYNAMIC_DRAW);
			glVertexAttribPointer(
				5, 3, GL_FLOAT, GL_FALSE, sizeof(GizmoLine), (void*) offsetof(GizmoLine, pos));
			glVertexAttribDivisor(5, 1);
			glEnableVertexAttribArray(5);

			glVertexAttribPointer(
				6, 3, GL_FLOAT, GL_FALSE, sizeof(GizmoLine), (void*) offsetof(GizmoLine, endPos));
			glVertexAttribDivisor(6, 1);
			glEnableVertexAttribArray(6);

			glVertexAttribPointer(
				7, 3, GL_FLOAT, GL_FALSE, sizeof(GizmoLine), (void*) offsetof(GizmoLine, color));
			glVertexAttribDivisor(7, 1);
			glEnableVertexAttribArray(7);
		    glBindVertexArray(0);

			shaderCube_.BindUbo(2 * sizeof(Mat4f) + sizeof(Vec3f));
			shaderLine_.BindUbo(2 * sizeof(Mat4f) + sizeof(Vec3f));
			shaderSphere_.BindUbo(2 * sizeof(Mat4f) + sizeof(Vec3f));
		}};

	gizmosQueue_.reserve(kGizmoReserveSize);
	lineGizmos_.reserve(kGizmoReserveSize);
	RendererLocator::get().AddPreRenderJob(&preRender_);
}

void GizmoRenderer::Update(seconds)
{
	//RendererLocator::get().Render(this);
}

void GizmoRenderer::Render()
{
	if (isRunning_)
	{
		const Mat4f camProj = camera_->GenerateProjectionMatrix();
		const Mat4f camView = camera_->GenerateViewMatrix();
		gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
		gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
		gl::Shader::SetUbo(sizeof(Vec3f), sizeof(Mat4f) * 2, &camera_->position);

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
					model       = Transform3d::Scale(model, gizmo.cubeSize);
					model       = Transform3d::Translate(model, gizmo.pos);
					shaderCube_.SetMat4("model", model);
					cube_.SetLineWidth(gizmo.lineThickness);
					cube_.Draw();
				}
				break;
				case GizmoShape::SPHERE:
				{
					if (shaderSphere_.GetProgram() == 0) continue;
					shaderSphere_.Bind();
					shaderSphere_.SetVec4("color", gizmo.color);
					Mat4f model = Mat4f::Identity;
					model       = Transform3d::Scale(model, Vec3f(gizmo.radius + 0.01f));
					model       = Transform3d::Translate(model, gizmo.pos);
					shaderSphere_.SetMat4("model", model);
					sphere_.SetLineWidth(gizmo.lineThickness);
					sphere_.Draw();
				}
				break;
				default: logDebug("Invalid Gizmo shape!");
			}
		}

		RenderLines();
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
	const Color4& color,
	const float lineThickness)
{
	if (isRunning_)
	{
		std::lock_guard<std::mutex> lock(renderMutex_);
		Gizmo gizmo;
		gizmo.pos           = pos;
		gizmo.cubeSize      = size;
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
		GizmoLine gizmoLine;
		gizmoLine.pos = startPos;
		gizmoLine.endPos = endPos;
		gizmoLine.color = Color3(color);
		lineGizmos_.push_back(gizmoLine);
	}
}

void GizmoRenderer::DrawSphere(
    const Vec3f& pos,
    const float& radius,
    const Color4& color,
    const float lineThickness)
{
	if (isRunning_)
	{
		std::lock_guard<std::mutex> lock(renderMutex_);
		Gizmo gizmo;
		gizmo.pos           = pos;
		gizmo.radius        = radius;
		gizmo.color         = color;
		gizmo.shape         = GizmoShape::SPHERE;
		gizmo.lineThickness = lineThickness;
		gizmosQueue_.push_back(gizmo);
	}
}

void GizmoRenderer::Clear()
{
	gizmosQueue_.clear();
	lineGizmos_.clear();
}

void GizmoRenderer::RenderLines()
{
	shaderLine_.Bind();
	glBindVertexArray(line_.VAO);
	glBindBuffer(GL_ARRAY_BUFFER, line_.VBO[1]);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GizmoLine) * lineGizmos_.size(), lineGizmos_.data());
	glVertexAttribPointer(
		5, 3, GL_FLOAT, GL_FALSE, sizeof(GizmoLine), (void*) offsetof(GizmoLine, pos));
	glVertexAttribDivisor(5, 1);
	glEnableVertexAttribArray(5);

	glVertexAttribPointer(
		6, 3, GL_FLOAT, GL_FALSE, sizeof(GizmoLine), (void*) offsetof(GizmoLine, endPos));
	glVertexAttribDivisor(6, 1);
	glEnableVertexAttribArray(6);

	glVertexAttribPointer(
		7, 3, GL_FLOAT, GL_FALSE, sizeof(GizmoLine), (void*) offsetof(GizmoLine, color));
	glVertexAttribDivisor(7, 1);
	glEnableVertexAttribArray(7);

	glDrawArraysInstanced(GL_LINES, 0, 2, lineGizmos_.size());
	glBindVertexArray(0);
}

void GizmoRenderer::RenderCubes() {}

void GizmoRenderer::RenderSpheres() {}
}    // namespace neko::sr
