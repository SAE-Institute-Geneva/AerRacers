#include "aer/draw_system.h"

#include "imgui.h"

#ifdef EASY_PROFILE_USE
	#include <easy/profiler.h>
#endif

namespace neko::aer
{
DrawSystem::DrawSystem(AerEngine& engine)
   : engine_(engine),
	 rContainer_(engine.GetResourceManagerContainer()),
	 cContainer_(engine.GetComponentManagerContainer())
{
	engine.RegisterSystem(camera_);
	engine.RegisterOnEvent(camera_);

#ifdef NEKO_GLES3
	gizmosRenderer_ = std::make_unique<GizmoRenderer>(&camera_);
#endif

	engine.RegisterSystem(*gizmosRenderer_);
}

void DrawSystem::Init()
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("DrawSystem::Init");
#endif
	camera_.position         = Vec3f::forward * 2.0f;
	camera_.reverseDirection = Vec3f::forward;
	camera_.fovY             = degree_t(45.0f);
	camera_.nearPlane        = 0.1f;
	camera_.farPlane         = 100.0f;

	gizmosRenderer_->SetCamera(&camera_);

    //For Test
	if (engine_.GetMode() == ModeEnum::GAME) {
        const Configuration config = BasicEngine::GetInstance()->GetConfig();
        engine_.GetComponentManagerContainer().sceneManager.LoadScene(
            config.dataRootPath +
            "scenes/SceneForNeko02-18withship.aerscene");
        Camera3D* camera = GizmosLocator::get().GetCamera();
        camera->farPlane = 100'000.0f;
        camera->position = Vec3f(0.0f, 5.0f, -15.0f);
        shipEntity_ = 15;
        for (Entity entity = 0;
            entity < engine_
            .GetComponentManagerContainer().entityManager.
            GetEntitiesSize(); ++entity) {
            if (TagLocator::get().IsEntityTag(entity, "Camera")) {
                cameraEntity_ = entity;
                break;
            }
        }
        engine_.GetComponentManagerContainer().shipControllerManager.
            AddComponent(15);
        engine_.GetComponentManagerContainer().entityManager.SetEntityParent(
            cameraEntity_,
            shipEntity_);
        engine_.GetComponentManagerContainer().transform3dManager.
            SetRelativePosition(cameraEntity_, Vec3f(0.0f, 5.0f, -15.0f));
        engine_.GetComponentManagerContainer().transform3dManager.
            SetRelativeRotation(
                cameraEntity_,
                EulerAngles(
                    degree_t(0.0f),
                    degree_t(-90.0f),
                    degree_t(0.0f)));
	}
}

void DrawSystem::Update(seconds)
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("DrawSystem::Update");
#endif

    //For Test
	if (engine_.GetMode() == ModeEnum::GAME) {
		Camera3D* camera = GizmosLocator::get().GetCamera();
			camera->WorldLookAt(
				engine_.GetComponentManagerContainer().transform3dManager.
				GetGlobalPosition(shipEntity_));
			camera->position = engine_
				.GetComponentManagerContainer().
				transform3dManager.GetGlobalPosition(
					cameraEntity_);
	}
}

void DrawSystem::Destroy() {}

void DrawSystem::DrawImGui() {
}

void DrawSystem::OnEvent(const SDL_Event&) {}
}    // namespace neko::aer
