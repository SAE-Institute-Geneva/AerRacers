#include "aer/aer_engine.h"

namespace neko::aer
{
AerEngine::AerEngine(const FilesystemInterface& filesystem, Configuration* config, ModeEnum mode)
   : SdlEngine(filesystem, *config),
	 mode_(mode),
	 drawSystem_(*this),
	 cContainer_(rContainer_),
	 toolManager_(*this)
{
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

		boundInputManager_ = std::make_unique<InputBindingManager>();
		tagManager_        = std::make_unique<TagManager>(cContainer_.sceneManager);

		RegisterSystem(rContainer_);
		RegisterSystem(cContainer_);
	}
}

void AerEngine::Init()
{
	SdlEngine::Init();

	if (mode_ == ModeEnum::GAME) {}
}

void AerEngine::Destroy()
{
	drawSystem_.Destroy();
	SdlEngine::Destroy();
}

void AerEngine::ManageEvent() { SdlEngine::ManageEvent(); }

void AerEngine::GenerateUiFrame()
{
	window_->GenerateUiFrame();
	drawImGuiAction_.Execute();
}
}    // namespace neko::aer
