#include <gizmos_renderer.h>

#ifdef NEKO_GLES3

#include <engine/engine.h>

namespace neko
{
Gles3GizmosRenderer::Gles3GizmosRenderer(Camera3D* camera)
	: camera_(camera)
{
	GizmosLocator::provide(this);
	isRunning_ = true;
}

void Gles3GizmosRenderer::Init()
{
	const auto& config = BasicEngine::GetInstance()->GetConfig();
	shaderCube_.LoadFromFile(
		config.dataRootPath + "shaders/opengl/gizmoCube.vert",
		config.dataRootPath + "shaders/opengl/gizmoCube.frag");
	shaderLine_.LoadFromFile(
		config.dataRootPath + "shaders/opengl/gizmoLine.vert",
		config.dataRootPath + "shaders/opengl/gizmoLine.frag");
	cube_.Init();
	line_.Init();
	gizmosQueue_.reserve(kGizmoReserveSize);
}

void Gles3GizmosRenderer::Update(seconds dt)
{
}

void Gles3GizmosRenderer::Render()
{
	if (isRunning_)
	{
		for (auto gizmo : gizmosQueue_)
		{
			if (shaderCube_.GetProgram() == 0) continue;

			std::lock_guard<std::mutex> lock(renderMutex_);
			switch (gizmo.shape)
			{
			case GizmoShape::CUBE:
			{
				shaderCube_.Bind();
				shaderCube_.SetMat4("view", camera_->GenerateViewMatrix());
				shaderCube_.SetMat4("projection", camera_->GenerateProjectionMatrix());
				shaderCube_.SetVec4("color", gizmo.color);
				Mat4f model = Mat4f::Identity;
				model = Transform3d::Scale(model, gizmo.cubeSize);
				model = Transform3d::Translate(model, gizmo.pos);
				shaderCube_.SetMat4("model", model);
				cube_.SetLineWidth(gizmo.lineThickness);
				cube_.Draw();
			}
			break;
			case GizmoShape::LINE:
			{
				shaderLine_.Bind();
				shaderLine_.SetMat4("view", camera_->GenerateViewMatrix());
				shaderLine_.SetMat4("projection", camera_->GenerateProjectionMatrix());
				shaderCube_.SetVec4("color", gizmo.color);
				Mat4f model = Transform3d::Translate(Mat4f::Identity, gizmo.pos);
				shaderLine_.SetMat4("model", model);
				shaderLine_.SetVec3("endPos", gizmo.lineEndPos - gizmo.pos);
				line_.SetLineWidth(gizmo.lineThickness);
				line_.Draw();
			}
			break;
			default:
				logDebug("Invalid Gizmo shape!");
			}
		}
		gizmosQueue_.clear();
		gizmosQueue_.reserve(kGizmoReserveSize);
	}
}

void Gles3GizmosRenderer::Destroy()
{
	cube_.Destroy();
	line_.Destroy();
	shaderCube_.Destroy();
}

void Gles3GizmosRenderer::Start()
{
	isRunning_ = true;
}

void Gles3GizmosRenderer::Stop()
{
	isRunning_ = false;
}

void Gles3GizmosRenderer::DrawCube(
	const Vec3f& pos,
	const Vec3f& size,
	const Color4& color,
	const float lineThickness)
{
	if (isRunning_)
	{
		std::lock_guard<std::mutex> lock(renderMutex_);
		Gizmos gizmo;
		gizmo.pos = pos;
		gizmo.cubeSize = size;
		gizmo.color = color;
		gizmo.shape = GizmoShape::CUBE;
		gizmo.lineThickness = lineThickness;
		gizmosQueue_.push_back(gizmo);
	}
}

void Gles3GizmosRenderer::DrawLine(
	const Vec3f& startPos,
	const Vec3f& endPos,
	const Color4& color,
	const float lineThickness)
{
	if (isRunning_)
	{
		std::lock_guard<std::mutex> lock(renderMutex_);
		Gizmos gizmo;
		gizmo.pos = startPos;
		gizmo.lineEndPos = endPos;
		gizmo.color = color;
		gizmo.shape = GizmoShape::LINE;
		gizmo.lineThickness = lineThickness;
		gizmosQueue_.push_back(gizmo);
	}
}

void Gles3GizmosRenderer::SetCamera(Camera3D* camera)
{
	camera_ = camera;
}
}
#endif


#ifdef NEKO_VULKAN

#include <engine/engine.h>

namespace neko
{
	NekoGizmosRenderer::NekoGizmosRenderer(Camera3D* camera)
		: camera_(camera)
	{
		GizmosLocator::provide(this);
		isRunning_ = true;
	}

	void NekoGizmosRenderer::Init()
	{
		//const auto& config = BasicEngine::GetInstance()->GetConfig();
		//shaderCube_.LoadFromFile(
		//	config.dataRootPath + "shaders/opengl/gizmoCube.vert",
		//	config.dataRootPath + "shaders/opengl/gizmoCube.frag");
		//shaderLine_.LoadFromFile(
		//	config.dataRootPath + "shaders/opengl/gizmoLine.vert",
		//	config.dataRootPath + "shaders/opengl/gizmoLine.frag");
		//cube_.Init();
		//line_.Init();
		//gizmosQueue_.reserve(kGizmoReserveSize);
	}

	void NekoGizmosRenderer::Update(seconds dt)
	{
	}

	void NekoGizmosRenderer::Render()
	{
	/*	if (isRunning_)
		{
			for (auto gizmo : gizmosQueue_)
			{
				if (shaderCube_.GetProgram() == 0) continue;

				std::lock_guard<std::mutex> lock(renderMutex_);
				switch (gizmo.shape)
				{
				case GizmoShape::CUBE:
				{
					shaderCube_.Bind();
					shaderCube_.SetMat4("view", camera_->GenerateViewMatrix());
					shaderCube_.SetMat4("projection", camera_->GenerateProjectionMatrix());
					shaderCube_.SetVec4("color", gizmo.color);
					Mat4f model = Mat4f::Identity;
					model = Transform3d::Scale(model, gizmo.cubeSize);
					model = Transform3d::Translate(model, gizmo.pos);
					shaderCube_.SetMat4("model", model);
					cube_.SetLineWidth(gizmo.lineThickness);
					cube_.Draw();
				}
				break;
				case GizmoShape::LINE:
				{
					shaderLine_.Bind();
					shaderLine_.SetMat4("view", camera_->GenerateViewMatrix());
					shaderLine_.SetMat4("projection", camera_->GenerateProjectionMatrix());
					shaderCube_.SetVec4("color", gizmo.color);
					Mat4f model = Transform3d::Translate(Mat4f::Identity, gizmo.pos);
					shaderLine_.SetMat4("model", model);
					shaderLine_.SetVec3("endPos", gizmo.lineEndPos - gizmo.pos);
					line_.SetLineWidth(gizmo.lineThickness);
					line_.Draw();
				}
				break;
				default:
					logDebug("Invalid Gizmo shape!");
				}
			}
			gizmosQueue_.clear();
			gizmosQueue_.reserve(kGizmoReserveSize);
		}*/
	}

	void NekoGizmosRenderer::Destroy()
	{
		//cube_.Destroy();
		//line_.Destroy();
		//shaderCube_.Destroy();
	}

	void NekoGizmosRenderer::Start()
	{
		isRunning_ = true;
	}

	void NekoGizmosRenderer::Stop()
	{
		isRunning_ = false;
	}

	void NekoGizmosRenderer::DrawCube(
		const Vec3f& pos,
		const Vec3f& size,
		const Color4& color,
		const float lineThickness)
	{
		if (isRunning_)
		{
			std::lock_guard<std::mutex> lock(renderMutex_);
			Gizmos gizmo;
			gizmo.pos = pos;
			gizmo.cubeSize = size;
			gizmo.color = color;
			gizmo.shape = GizmoShape::CUBE;
			gizmo.lineThickness = lineThickness;
			gizmosQueue_.push_back(gizmo);
		}
	}

	void NekoGizmosRenderer::DrawLine(
		const Vec3f& startPos,
		const Vec3f& endPos,
		const Color4& color,
		const float lineThickness)
	{
		if (isRunning_)
		{
			std::lock_guard<std::mutex> lock(renderMutex_);
			Gizmos gizmo;
			gizmo.pos = startPos;
			gizmo.lineEndPos = endPos;
			gizmo.color = color;
			gizmo.shape = GizmoShape::LINE;
			gizmo.lineThickness = lineThickness;
			gizmosQueue_.push_back(gizmo);
		}
	}

	void NekoGizmosRenderer::SetCamera(Camera3D* camera)
	{
		camera_ = camera;
	}
}
#endif
