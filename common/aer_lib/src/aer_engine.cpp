#include "aer_engine.h"
#include "editor/tool/logger.h"

namespace neko::aer {
AerEngine::AerEngine(Configuration* config, ToolsMask toolsMask)
    : SdlEngine(config),
      drawSystem_(*this),
      toolManager_(*this),
      editor_(*this, toolManager_)
{   
    toolsMask_ = toolsMask;
    RegisterSystem(drawSystem_);
    RegisterOnEvent(drawSystem_);
    RegisterOnDrawUi(drawSystem_);
    RegisterSystem(toolManager_);
    RegisterSystem(editor_);
    RegisterOnEvent(editor_);
    RegisterOnDrawUi(editor_);

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
    toolManager_.Destroy();
    SdlEngine::Destroy();
}

void AerEngine::ManageEvent() { SdlEngine::ManageEvent(); }

void AerEngine::GenerateUiFrame() { SdlEngine::GenerateUiFrame(); }
Tool& AerEngine::GetTool(TypeTool tool) {
    return toolManager_.GetTool(tool);
}
}



