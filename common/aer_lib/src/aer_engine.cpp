#include "aer_engine.h"

namespace neko::aer
{
AerEngine::AerEngine(Configuration* config, ToolsMask toolsMask)
    : SdlEngine(config),
	drawSystem_(*this),
	toolsSystem_(*this)
{
    RegisterSystem(drawSystem_);
    RegisterOnEvent(drawSystem_);
    RegisterOnDrawUi(drawSystem_);

    if (toolsMask)
    {
        RegisterSystem(toolsSystem_);
        RegisterOnEvent(toolsSystem_);
        RegisterOnDrawUi(toolsSystem_);
    }
}

void AerEngine::Init()
{
	SdlEngine::Init();
}

void AerEngine::Destroy()
{
	drawSystem_.Destroy();
	toolsSystem_.Destroy();
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