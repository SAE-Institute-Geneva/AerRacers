#include "aer_engine.h"

namespace neko::aer {
AerEngine::AerEngine(Configuration* config, ToolsMask toolsMask)
    : SdlEngine(config),
      drawSystem_(*this)
{
    toolsMask_ = toolsMask;
    RegisterSystem(drawSystem_);
    RegisterOnEvent(drawSystem_);
    RegisterOnDrawUi(drawSystem_);
    if (toolsMask) { }
}

void AerEngine::Init()
{
    SdlEngine::Init();
    if ((toolsMask_ & ToolsMask(ToolsFlags::LOGGER)) ==
        ToolsMask(ToolsFlags::LOGGER)) {
        logManager_ = new LogManager;
    }
}

void AerEngine::Destroy()
{
    drawSystem_.Destroy();
    if ((toolsMask_ & ToolsMask(ToolsFlags::LOGGER)) ==
        ToolsMask(ToolsFlags::LOGGER)) {
        logManager_->WriteToFile();
        LogDebug("Destroy Aer Engine");
        logManager_->Wait();
        logManager_->Destroy();
    }

    SdlEngine::Destroy();
}

void AerEngine::ManageEvent() { SdlEngine::ManageEvent(); }

void AerEngine::GenerateUiFrame() { SdlEngine::GenerateUiFrame(); }
}
