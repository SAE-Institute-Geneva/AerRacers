#include "aer/draw_system.h"

#ifdef EASY_PROFILE_USE
	#include <easy/profiler.h>
#endif

namespace neko::aer
{
DrawSystem::DrawSystem(AerEngine& engine)
   : engine_(engine),
	 rContainer_(engine.GetResourceManagerContainer()),
	 cContainer_(engine.GetComponentManagerContainer())
{
	engine.RegisterSystem(camera_);

#ifdef NEKO_GLES3
	gizmosRenderer_ = std::make_unique<GizmoRenderer>(&camera_.GetCamera(0));
	engine.RegisterSystem(*gizmosRenderer_);
#endif
}

void DrawSystem::Init()
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("DrawSystem::Init");
#endif
	Camera3D camera;
	camera.position         = Vec3f::forward * 2.0f;
	camera.reverseDirection = Vec3f::forward;
	camera.fovY             = degree_t(45.0f);
	camera.nearPlane        = 0.1f;
	camera.farPlane         = 100.0f;
	camera_.SetCameras(camera);

#ifdef NEKO_GLES3
	gizmosRenderer_->SetCamera(&camera_.GetCamera(0));
#endif
}

void DrawSystem::Update(seconds)
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("DrawSystem::Update");
#endif

	RendererLocator::get().Render(this);
}

void DrawSystem::Render()
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("DrawSystem::Render");
#endif

#ifdef NEKO_GLES3
	const auto& size = BasicEngine::GetInstance()->GetConfig().windowSize;
	switch (playerNum_)
	{
		case 1:
		{
			camera_.SetAspects(size.x, size.y);

			const Mat4f camProj = camera_.GenerateProjectionMatrix(0);
			const Mat4f camView = camera_.GenerateViewMatrix(0);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(0, 0, size.x, size.y);
			RenderScene(0);
			break;
		}
		case 2:
		{
			camera_.SetAspects(size.x / 2.0f, size.y);

			// Left
			Mat4f camProj = camera_.GenerateProjectionMatrix(0);
			Mat4f camView = camera_.GenerateViewMatrix(0);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(0, 0, size.x / 2.0f, size.y);
			RenderScene(0);

			// Right
			camProj = camera_.GenerateProjectionMatrix(1);
			camView = camera_.GenerateViewMatrix(1);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(size.x / 2.0f, 0, size.x / 2.0f, size.y);
			RenderScene(1);
			break;
		}
		case 3:
		{
			camera_.SetAspects(size.x / 2.0f, size.y / 2.0f);

			// Top Left
			Mat4f camProj = camera_.GenerateProjectionMatrix(0);
			Mat4f camView = camera_.GenerateViewMatrix(0);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(0, size.y / 2.0f, size.x / 2.0f, size.y / 2.0f);
			RenderScene(0);

			// Top Right
			camProj = camera_.GenerateProjectionMatrix(1);
			camView = camera_.GenerateViewMatrix(1);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(size.x / 2.0f, size.y / 2.0f, size.x / 2.0f, size.y / 2.0f);
			RenderScene(1);

			// Bottom Left
			camProj = camera_.GenerateProjectionMatrix(2);
			camView = camera_.GenerateViewMatrix(2);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(0, 0, size.x / 2.0f, size.y / 2.0f);
			RenderScene(2);
			break;
		}
		case 4:
		{
			camera_.SetAspects(size.x / 2.0f, size.y / 2.0f);

			// Top Left
			Mat4f camProj = camera_.GenerateProjectionMatrix(0);
			Mat4f camView = camera_.GenerateViewMatrix(0);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(0, size.y / 2.0f, size.x / 2.0f, size.y / 2.0f);
			RenderScene(0);

			// Top Right
			camProj = camera_.GenerateProjectionMatrix(1);
			camView = camera_.GenerateViewMatrix(1);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(size.x / 2.0f, size.y / 2.0f, size.x / 2.0f, size.y / 2.0f);
			RenderScene(1);

			// Bottom Left
			camProj = camera_.GenerateProjectionMatrix(2);
			camView = camera_.GenerateViewMatrix(2);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(0, 0, size.x / 2.0f, size.y / 2.0f);
			RenderScene(2);

			// Bottom Right
			camProj = camera_.GenerateProjectionMatrix(3);
			camView = camera_.GenerateViewMatrix(3);
			gl::Shader::SetUbo(sizeof(Mat4f), 0, &camProj);
			gl::Shader::SetUbo(sizeof(Mat4f), sizeof(Mat4f), &camView);
			glViewport(size.x / 2.0f, 0, size.x / 2.0f, size.y / 2.0f);
			RenderScene(3);
			break;
		}
		case 0:
		default: LogError("Invalid Player number!!"); break;
	}

	gizmosRenderer_->Clear();
#elif NEKO_VULKAN
	CameraLocator::provide(&camera_.GetCamera(0));
	RenderScene(0);
#endif
}

void DrawSystem::RenderScene(const std::size_t playerNum)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("DrawSystem::RenderScene");
#endif

	auto& cManagerContainer = engine_.GetComponentManagerContainer();
	cManagerContainer.renderManager.Render();

#ifdef NEKO_GLES3
	gizmosRenderer_->SetCamera(&camera_.GetCamera(playerNum));
	gizmosRenderer_->Render();
#endif
}

void DrawSystem::Destroy() {}

void DrawSystem::DrawImGui() {}

void DrawSystem::OnEvent(const SDL_Event& event)
{
	camera_.OnEvent(event);
}
}    // namespace neko::aer
