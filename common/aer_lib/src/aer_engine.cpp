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
	 cContainer_(*this, rContainer_, physicsEngine_),
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

		tagManager_ = std::make_unique<TagManager>(cContainer_.sceneManager);

		physicsEngine_.InitPhysics();
#ifdef NEKO_FMOD
		RegisterSystem(fmodEngine_);
#endif
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
#ifdef NEKO_GLES3
	SdlEngine::Init();
#elif NEKO_VULKAN
	jobSystem_.Init();
	initAction_.Execute();
	inputManager_.Init();
#endif

	if (mode_ == ModeEnum::GAME) {
		if (false)
		{
			cContainer_.sceneManager.LoadScene(
				GetConfig().dataRootPath +
				"scenes/PlayGroundLuca2021-03-01withoutShip.aerscene");
			cContainer_.playerManager.CreatePlayer(Vec3f(0, 10.0f, 30));
			cContainer_.playerManager.CreatePlayer(Vec3f(20.0f, 10.0f, 30));
			cContainer_.playerManager.CreatePlayer(Vec3f(40.0f, 10.0f, 30));
			cContainer_.playerManager.CreatePlayer(Vec3f(60.0f, 10.0f, 30.0f));
		} else if (false) {
			cContainer_.sceneManager.LoadScene(
				GetConfig().dataRootPath +
					"scenes/test_leveldesign_cube.aerscene");
			cContainer_.playerManager.CreatePlayer(Vec3f(222.0f, 84.0f, 56.0f), EulerAngles(degree_t(0.0f), degree_t(180.0f), degree_t(0.0f)));
			cContainer_.playerManager.CreatePlayer(Vec3f(202.0f, 84.0f, 56.0f), EulerAngles(degree_t(0.0f), degree_t(180.0f), degree_t(0.0f)));
			cContainer_.playerManager.CreatePlayer(Vec3f(182.0f, 84.0f, 56.0f), EulerAngles(degree_t(0.0f), degree_t(180.0f), degree_t(0.0f)));
			cContainer_.playerManager.CreatePlayer(Vec3f(162.0f, 84.0f, 56.0f), EulerAngles(degree_t(0.0f), degree_t(180.0f), degree_t(0.0f)));
		}
		else if (true) {
			cContainer_.sceneManager.LoadScene(
				GetConfig().dataRootPath + "scenes/LevelDesign05-04.aerscene");
			cContainer_.playerManager.CreatePlayer(Vec3f(-1108.0f, 185.0f, -788.0f));
			cContainer_.playerManager.CreatePlayer(Vec3f(-1128.0f, 185.0f, -788.0f));
			cContainer_.playerManager.CreatePlayer(Vec3f(-1148.0f, 185.0f, -788.0f));
			cContainer_.playerManager.CreatePlayer(Vec3f(-1168.0f, 185.0f, -788.0f));
		}
		else {
			cContainer_.gameManager.StartGameManager(4);
			cContainer_.waypointManager.StartDetection();
		}
	}
}

void AerEngine::Destroy()
{
	drawSystem_.Destroy();
	SdlEngine::Destroy();
}

void AerEngine::ManageEvent() { SdlEngine::ManageEvent(); }

void AerEngine::GenerateUiFrame()
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("AerEngine::GenerateUiFrame");
#endif

#ifdef NEKO_GLES3
		drawImGuiAction_.Execute();
#elif NEKO_VULKAN
		if (ImGui::GetCurrentContext()) drawImGuiAction_.Execute();
#endif
}
}    // namespace neko::aer
