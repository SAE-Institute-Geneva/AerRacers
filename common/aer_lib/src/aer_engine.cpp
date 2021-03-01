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

		//boundInputManager_ = std::make_unique<InputBindingManager>();
		tagManager_        = std::make_unique<TagManager>(cContainer_.sceneManager);

        physicsEngine_.InitPhysics();
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
	SdlEngine::Init();

	if (mode_ == ModeEnum::GAME) {}
}

void AerEngine::Destroy()
{
    //boundInputManager_->Destroy();
    //boundInputManager_.release();
	drawSystem_.Destroy();
	SdlEngine::Destroy();
}

void AerEngine::ManageEvent() { SdlEngine::ManageEvent(); }

void AerEngine::GenerateUiFrame()
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("AerEngine::GenerateUiFrame");
#endif
	window_->GenerateUiFrame();
	drawImGuiAction_.Execute();
}
}    // namespace neko::aer
