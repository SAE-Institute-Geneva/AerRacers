#include "aer/draw_system.h"

#include "imgui.h"

#ifdef EASY_PROFILE_USE
	#include <easy/profiler.h>
#endif

namespace neko::aer
{
DrawSystem::DrawSystem(AerEngine& engine)
   : engine_(engine),
	 sceneManager_(entityManager_, engine.GetFilesystem(), transform3dManager_),
	 toolManager_(engine),
	 transform3dManager_(entityManager_),
	 renderManager_(entityManager_, modelManager_, transform3dManager_)
{
	engine.RegisterSystem(camera_);
	engine.RegisterOnEvent(camera_);

	logManager_ = std::make_unique<LogManager>();
	if (engine.GetMode() != ModeEnum::TEST)
	{
		boundInputManager_ = std::make_unique<InputBindingManager>();
		tagManager_        = std::make_unique<TagManager>(sceneManager_);
	}

	if (engine.GetMode() == ModeEnum::EDITOR)
	{
		engine.RegisterSystem(toolManager_);
		engine.RegisterOnEvent(toolManager_);
		engine.RegisterOnDrawUi(toolManager_);
	}

	engine.RegisterSystem(textureManager_);
	engine.RegisterSystem(modelManager_);
	engine.RegisterSystem(renderManager_);

#ifdef NEKO_GLES3
	gizmosRenderer_ = std::make_unique<GizmosRenderer>(&camera_);
#elif NEKO_VULKAN
	gizmosRenderer_ = std::make_unique<NekoGizmosRenderer>(&camera_);
#endif

	engine.RegisterSystem(*gizmosRenderer_);
}

void DrawSystem::Init()
{
	//gizmosRenderer_->Init();
	camera_.position         = Vec3f::zero;
	camera_.reverseDirection = Vec3f::back;
	camera_.fovY             = degree_t(45.0f);
	camera_.nearPlane        = 0.1f;
	camera_.farPlane         = 100.0f;

	gizmosRenderer_->SetCamera(&camera_);

	const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
	testEntity_ = entityManager_.CreateEntity();
	transform3dManager_.AddComponent(testEntity_);
	renderManager_.AddComponent(testEntity_);
	renderManager_.SetModel(testEntity_, config.dataRootPath + "models/nanosuit2/nanosuit.obj");
}

void DrawSystem::Update(seconds)
{
	gizmosRenderer_->DrawCube(Vec3f::zero);
}

void DrawSystem::Destroy()
{
	logManager_->Destroy();
}

void DrawSystem::DrawImGui() {}

void DrawSystem::OnEvent(const SDL_Event&) {}
}    // namespace neko::aer
