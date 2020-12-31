#include "aer_engine.h"

namespace neko::aer {
AerEngine::AerEngine(const FilesystemInterface& filesystem, Configuration* config, ModeEnum mode)
    : SdlEngine(filesystem, *config),
      drawSystem_(*this),
      toolManager_(*this)
{
    logManager_ = std::make_unique<LogManager>();
    if (mode_ != ModeEnum::TEST)
    {
        bindedInputManager_ = std::make_unique<InputBindingManager>();
    }
    mode_ = mode;
    RegisterSystem(drawSystem_);
    RegisterOnEvent(drawSystem_);
    RegisterOnDrawUi(drawSystem_);
	if (mode_ == ModeEnum::EDITOR)
    {
        RegisterSystem(toolManager_);
        RegisterOnEvent(toolManager_);
        RegisterOnDrawUi(toolManager_);
    }
}

void AerEngine::Init()
{
    SdlEngine::Init();
	
    if (mode_ == ModeEnum::GAME)
    {
   
    }
}

void AerEngine::Destroy()
{
    drawSystem_.Destroy();
    toolManager_.Destroy();
    SdlEngine::Destroy();
}

void AerEngine::ManageEvent()
{
	SdlEngine::ManageEvent();
}

void AerEngine::GenerateUiFrame()
{
	SdlEngine::GenerateUiFrame();
}

}



