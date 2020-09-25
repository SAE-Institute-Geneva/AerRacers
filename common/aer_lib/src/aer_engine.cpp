#include <aer_engine.h>

namespace neko::aer
{
	AerEngine::AerEngine(Configuration* config)
		: SdlEngine(config),
		drawSystem_(*this)
	{
		RegisterSystem(drawSystem_);
		RegisterOnEvent(drawSystem_);
		RegisterOnDrawUi(drawSystem_);
#ifdef EASY_PROFILE_USE
		EASY_PROFILER_ENABLE;
#endif
	}

	void AerEngine::Init()
	{
		SdlEngine::Init();
	}

	void AerEngine::Destroy()
	{
		drawSystem_.Destroy();
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