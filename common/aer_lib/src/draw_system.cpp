#include "aer/aer_engine.h"

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
	if (engine.GetMode() != ModeEnum::TEST)
	{
		engine.RegisterSystem(camera_);

#ifdef NEKO_GLES3
		gizmosRenderer_ = std::make_unique<GizmoRenderer>(&camera_.GetCamera(0));
		engine.RegisterSystem(*gizmosRenderer_);
#endif
		uiManager_ = std::make_unique<UiManager>(engine_);
		engine.RegisterSystem(*uiManager_);
		engine.RegisterOnEvent(*uiManager_);
	}
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
	camera.farPlane         = 10000.0f;
	camera_.SetCameras(camera);
	sdl::MultiCameraLocator::provide(&camera_);

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
	gl::Shader& shader = cContainer_.renderManager.GetShader();
	const Vec2u size = BasicEngine::GetInstance()->GetConfig().windowSize;
	switch (playerNum_)
	{
		case 1:
		{
			camera_.SetAspects(static_cast<float>(size.x), static_cast<float>(size.y));

			camera_.Bind(0, shader);
			glViewport(0, 0, size.x, size.y);
			RenderScene(0);
			break;
		}
		case 2:
		{
			camera_.SetAspects(static_cast<float>(size.x) / 2.0f, static_cast<float>(size.y));

			// Left
			camera_.Bind(0, shader);
			glViewport(0, 0, size.x / 2, size.y);
			RenderScene(0);

			// Right
			camera_.Bind(1, shader);
			glViewport(size.x / 2, 0, size.x / 2, size.y);
			RenderScene(1);
			break;
		}
		case 3:
		{
			camera_.SetAspects(
				static_cast<float>(size.x) / 2.0f, static_cast<float>(size.y) / 2.0f);

			// Top Left
			camera_.Bind(0, shader);
			glViewport(0, size.y / 2, size.x / 2, size.y / 2);
			RenderScene(0);

			// Top Right
			camera_.Bind(1, shader);
			glViewport(size.x / 2, size.y / 2, size.x / 2, size.y / 2);
			RenderScene(1);

			// Bottom Left
			camera_.Bind(2, shader);
			glViewport(0, 0, size.x / 2, size.y / 2);
			RenderScene(2);
			break;
		}
		case 4:
		{
			camera_.SetAspects(
				static_cast<float>(size.x) / 2.0f, static_cast<float>(size.y) / 2.0f);

			// Top Left
			camera_.Bind(0, shader);
			glViewport(0, size.y / 2, size.x / 2, size.y / 2);
			RenderScene(0);

			// Top Right
			camera_.Bind(1, shader);
			glViewport(size.x / 2, size.y / 2, size.x / 2, size.y / 2);
			RenderScene(1);

			// Bottom Left
			camera_.Bind(2, shader);
			glViewport(0, 0, size.x / 2, size.y / 2);
			RenderScene(2);

			// Bottom Right
			camera_.Bind(3, shader);
			glViewport(size.x / 2, 0, size.x / 2, size.y / 2);
			RenderScene(3);
			break;
		}
		case 0:
		default: LogError("Invalid Player number!!"); break;
	}
	uiManager_->Render(playerNum_);
	gizmosRenderer_->Clear();
#elif NEKO_VULKAN
	vk::VkResources::Inst->SetViewportCount(playerNum_);
	const Vec2f size = Vec2f(BasicEngine::GetInstance()->GetConfig().windowSize);
	switch (playerNum_)
	{
		case 1: camera_.SetAspects(size.x, size.y); break;
		case 2: camera_.SetAspects(size.x / 2.0f, size.y); break;

		case 3:
		case 4: camera_.SetAspects(size.x / 2.0f, size.y / 2.0f); break;

		case 0:
		default: LogError("Invalid Player number!!"); break;
	}

	engine_.GetComponentManagerContainer().renderManager.Render();
#endif
}

#ifdef NEKO_GLES3
void DrawSystem::RenderScene(const std::size_t playerNum)
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("DrawSystem::RenderScene");
#endif

	auto& cManagerContainer = engine_.GetComponentManagerContainer();
	cManagerContainer.renderManager.Render();

	gizmosRenderer_->SetCamera(&camera_.GetCamera(playerNum));
	gizmosRenderer_->Render();
}
#endif

void DrawSystem::Destroy() {}

void DrawSystem::DrawImGui() {}

void DrawSystem::OnEvent(const SDL_Event& event) { camera_.OnEvent(event); }
}    // namespace neko::aer
