#include "aer/aer_engine.h"

namespace neko::aer
{
AerEngine::AerEngine(const FilesystemInterface& filesystem, Configuration* config, ModeEnum mode)
   : SdlEngine(filesystem, *config),
	 mode_(mode),
	 drawSystem_(*this)
{
	RegisterSystem(drawSystem_);
	RegisterOnEvent(drawSystem_);
	RegisterOnDrawUi(drawSystem_);
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
