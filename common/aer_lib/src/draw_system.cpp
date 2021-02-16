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
	const auto& config = neko::BasicEngine::GetInstance()->GetConfig();
	Entity groundEntity = engine_.GetComponentManagerContainer().entityManager.CreateEntity();
	engine_.GetComponentManagerContainer().renderManager.AddComponent(groundEntity);
	engine_.GetComponentManagerContainer().renderManager.SetModel(groundEntity, config.dataRootPath + "models/cube/cube.obj");
	engine_.GetComponentManagerContainer().transform3dManager.SetRelativeScale(groundEntity, Vec3f(10000.0f, 0.5f, 10000.0f));
	physics::RigidStaticData rigidStaticData;
	rigidStaticData.colliderType = physics::ColliderType::BOX;
	rigidStaticData.filterGroup = physics::FilterGroup::GROUND;
	engine_.GetComponentManagerContainer().rigidStaticManager.AddRigidStatic(
		groundEntity,
		rigidStaticData);
	
	Entity shipEntity = engine_.GetComponentManagerContainer().entityManager.CreateEntity();
	engine_.GetComponentManagerContainer().entityManager.SetEntityName(shipEntity, "ship");
	engine_.GetComponentManagerContainer().transform3dManager.AddComponent(shipEntity);
	engine_.GetComponentManagerContainer().transform3dManager.SetGlobalPosition(shipEntity, Vec3f(0, 2, 0));
	engine_.GetComponentManagerContainer().rigidDynamicManager.AddComponent(shipEntity);
	physics::RigidDynamicData rigidDynamic;
	rigidDynamic.useGravity = false;
	rigidDynamic.colliderType = physics::ColliderType::BOX;
	engine_.GetComponentManagerContainer().rigidDynamicManager.AddRigidDynamic(shipEntity, rigidDynamic);
	engine_.GetComponentManagerContainer().shipControllerManager.AddComponent(shipEntity);
	engine_.GetComponentManagerContainer().renderManager.AddComponent(shipEntity);
	engine_.GetComponentManagerContainer().renderManager.SetModel(shipEntity, config.dataRootPath + "models/cube/cube.obj");
	
	cameraEntity_ = engine_.GetComponentManagerContainer().entityManager.CreateEntity();
	engine_.GetComponentManagerContainer().entityManager.SetEntityName(cameraEntity_, "cameraEntity");
	engine_.GetComponentManagerContainer().transform3dManager.AddComponent(cameraEntity_);
	engine_.GetComponentManagerContainer().cameraControllerManager.AddComponent(cameraEntity_);

}

void DrawSystem::Update(seconds)
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("DrawSystem::Update");
#endif
	camera_.position = engine_.GetComponentManagerContainer().transform3dManager.GetGlobalPosition(cameraEntity_);
}

void DrawSystem::Destroy() {}

void DrawSystem::DrawImGui() {}

void DrawSystem::OnEvent(const SDL_Event&) {}
}    // namespace neko::aer
