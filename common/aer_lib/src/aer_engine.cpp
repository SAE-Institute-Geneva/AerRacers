#include "aer_engine.h"
#include "editor/tool/logger.h"

namespace neko::aer {
AerEngine::AerEngine(Configuration* config, ModeEnum mode)
    : SdlEngine(config),
      drawSystem_(*this),
      toolManager_(*this)
{
    mode_ = mode;
    RegisterSystem(drawSystem_);
    RegisterOnEvent(drawSystem_);
    RegisterOnDrawUi(drawSystem_);
    if (mode == ModeEnum::EDITOR)
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
        logManager_ = std::make_unique<LogManager>();
    }

}

void AerEngine::Destroy()
{
    drawSystem_.Destroy();
    toolManager_.Destroy();
    SdlEngine::Destroy();
}

void AerEngine::ManageEvent() { SdlEngine::ManageEvent(); }

void AerEngine::GenerateUiFrame() { SdlEngine::GenerateUiFrame(); }
Tool* AerEngine::GetTool(TypeTool tool) {
    return toolManager_.GetTool(tool);
}
}



