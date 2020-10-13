#include "aer_engine.h"

namespace neko::aer {
AerEngine::AerEngine(Configuration* config, ToolsMask toolsMask)
    : SdlEngine(config),
      drawSystem_(*this),
      editor_(*this)
{   
    InfoLog("Lauche DrawSystem");
    RegisterSystem(drawSystem_);
    RegisterOnEvent(drawSystem_);
    RegisterOnDrawUi(drawSystem_);
    InfoLog("Launch Editor");
    RegisterSystem(editor_);
    RegisterOnEvent(editor_);
    RegisterOnDrawUi(editor_);
    if (toolsMask) { }
}

void AerEngine::Init() { SdlEngine::Init(); }

void AerEngine::Destroy()
{
    drawSystem_.Destroy();

    editor_.Destroy();
    SdlEngine::Destroy();
}

void AerEngine::ManageEvent() { SdlEngine::ManageEvent(); }

void AerEngine::GenerateUiFrame() { SdlEngine::GenerateUiFrame(); }
}
