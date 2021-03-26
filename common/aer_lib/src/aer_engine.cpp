#include "aer/aer_engine.h"

#ifdef EASY_PROFILE_USE
    #include <easy/profiler.h>
#endif

namespace neko::aer
{
AerEngine::AerEngine(const FilesystemInterface& filesystem, Configuration* config, ModeEnum mode)
   : SdlEngine(filesystem, *config),
	 mode_(mode),
	 drawSystem_(*this),
	 physicsEngine_(cContainer_.entityManager, cContainer_.transform3dManager),
	 cContainer_(rContainer_, physicsEngine_),
	 toolManager_(*this)
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("AerEngine::Constructor");
#endif
	logManager_ = std::make_unique<LogManager>();

	if (mode_ == ModeEnum::EDITOR)
	{
		RegisterSystem(toolManager_);
		RegisterOnEvent(toolManager_);
		RegisterOnDrawUi(toolManager_);
	}

	if (mode_ != ModeEnum::TEST)
	{
		RegisterSystem(drawSystem_);
		RegisterOnEvent(drawSystem_);
		RegisterOnDrawUi(drawSystem_);

		tagManager_ = std::make_unique<TagManager>(cContainer_.sceneManager);

		physicsEngine_.InitPhysics();
		RegisterSystem(fmodEngine_);
		RegisterSystem(rContainer_);
		RegisterSystem(cContainer_);
		RegisterSystem(physicsEngine_);
	}
}

void AerEngine::Init()
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("AerEngine::Init");
#endif

#ifdef NEKO_GLES3
	SdlEngine::Init();
#elif NEKO_VULKAN
	jobSystem_.Init();
	initAction_.Execute();
	inputManager_.Init();
#endif
}

void AerEngine::Destroy()
{
	drawSystem_.Destroy();
	SdlEngine::Destroy();
}

void AerEngine::ManageEvent() { SdlEngine::ManageEvent(); }

void AerEngine::GenerateUiFrame()
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("AerEngine::GenerateUiFrame");
#endif

	if (mode_ == ModeEnum::EDITOR)
	{
#ifdef NEKO_GLES3
		window_->GenerateUiFrame();
		drawImGuiAction_.Execute();
#elif NEKO_VULKAN
		if (ImGui::GetCurrentContext()) drawImGuiAction_.Execute();
#endif
	}
}
}    // namespace neko::aer
