#include <physics_engine.h>
#include <engine\log.h>

#include "engine/engine.h"


namespace neko::physics::px {


PhysicsEngine::PhysicsEngine(
    EntityManager& entityManager,
    Transform3dManager& transform3d)
    : entityManager_(entityManager),
      transform3d_(transform3d),
      boxPhysicsShapeManager_(entityManager),
      spherePhysicsShapeManager_(entityManager),
      rigidDynamicManager_(
          entityManager,
          boxPhysicsShapeManager_,
          spherePhysicsShapeManager_,
          transform3d_),
      rigidStaticManager_(
          entityManager,
          boxPhysicsShapeManager_,
          spherePhysicsShapeManager_,
          transform3d_)
{}

void PhysicsEngine::Start()
{
    static physx::PxDefaultErrorCallback gDefaultErrorCallback;
    static physx::PxDefaultAllocator gDefaultAllocatorCallback;

    foundation_ = PxCreateFoundation(
        PX_PHYSICS_VERSION,
        gDefaultAllocatorCallback,
        gDefaultErrorCallback);
    if (!foundation_)
        logDebug("PxCreateFoundation failed!");
    
    bool recordMemoryAllocations = true;
    std::string PVD_HOST = "localhost";
    pvd_ = physx::PxCreatePvd(*foundation_);
    //const auto& config = neko::BasicEngine::GetInstance()->config;
    //const char* filename = "C:\\PvdCapture.pxd2";  // filename where the stream will be written to
    //transport = physx::PxDefaultPvdFileTransportCreate(filename);
    transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST.c_str(), 5425, 10);
    bool connec = pvd_->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
    if (connec)
    {
        std::cout << "Connected to PhysX Visual Debugger!\n";
    }

    physics_ = PxCreatePhysics(
        PX_PHYSICS_VERSION,
        *foundation_,
        physx::PxTolerancesScale(),
        recordMemoryAllocations, pvd_);
    if (!physics_)
        logDebug("PxCreatePhysics failed!");

    cooking_ = PxCreateCooking(PX_PHYSICS_VERSION, *foundation_, physx::PxCookingParams(physx::PxTolerancesScale()));
    if (!cooking_)
        logDebug("PxCreateCooking failed!");

    if (!PxInitExtensions(*physics_, pvd_))
        logDebug("PxInitExtensions failed!");
    CreateScene();
    scene_->getScenePvdClient()->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
    scene_->getScenePvdClient()->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
    scene_->getScenePvdClient()->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true);
}

void PhysicsEngine::CreateScene()
{
    physx::PxSceneDesc sceneDesc = physx::PxTolerancesScale();
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    physx::PxSimulationFilterShader gDefaultFilterShader = physx::PxDefaultSimulationFilterShader;
    sceneDesc.filterShader = gDefaultFilterShader;
    cpuDispatcher_ = physx::PxDefaultCpuDispatcherCreate(1);
    if (!cpuDispatcher_)
        std::cerr << "PxDefaultCpuDispatcherCreate failed!";
    sceneDesc.cpuDispatcher = cpuDispatcher_;
    scene_ = physics_->createScene(sceneDesc);
    if (!scene_)
        std::cerr << "createScene failed!";
}

bool PhysicsEngine::Advance(physx::PxReal dt)
{
    accumulator_ += dt;
    if (accumulator_ < stepSize_.count())
        return false;

    accumulator_ -= stepSize_.count();

    return true;
}

void PhysicsEngine::Update(float dt)
{
    if (Advance(dt))
    {
        scene_->simulate(stepSize_.count());
        scene_->fetchResults(true);
        boxPhysicsShapeManager_.FixedUpdate(stepSize_);
        spherePhysicsShapeManager_.FixedUpdate(stepSize_);
        rigidDynamicManager_.FixedUpdate(stepSize_);
        rigidStaticManager_.FixedUpdate(stepSize_);
    }
}

void PhysicsEngine::Destroy()
{
    pvd_->disconnect();
    physics_->release();
    cooking_->release();
    PxCloseExtensions();
    pvd_->release();
    transport->release();
    foundation_->release();
}

physx::PxPhysics* PhysicsEngine::GetPhysx()
{
    return physics_;
}

physx::PxScene* PhysicsEngine::GetScene()
{
    return scene_;
}

void PhysicsEngine::AddRigidStatic(Entity entity, RigidStatic& rigidStatic)
{
    rigidStaticManager_.AddComponent(entity);
    Vec3f position = transform3d_.GetPosition(entity);
    EulerAngles euler = transform3d_.GetAngles(entity);
    if (entityManager_.HasComponent(entity, EntityMask(ComponentType::BOX_COLLIDER))) {
        BoxPhysicsShape boxPhysicsShape = boxPhysicsShapeManager_.GetComponent(entity);
        rigidStatic.Init(physics_, boxPhysicsShape, position, euler);
    } else if (entityManager_.HasComponent(entity, EntityMask(ComponentType::SPHERE_COLLIDER))) {
        SpherePhysicsShape spherePhysicsShape = spherePhysicsShapeManager_.GetComponent(entity);
        rigidStatic.Init(physics_, spherePhysicsShape, position, euler);
    } else {
        neko_assert(false, "No shape link to the rigidBody")
    }
    scene_->addActor(*rigidStatic.GetPxRigidStatic());
    rigidStaticManager_.SetComponent(entity, rigidStatic);
}

const RigidStatic& PhysicsEngine::GetRigidStatic(Entity entity) const {
    return     rigidStaticManager_.GetComponent(entity);
}

void PhysicsEngine::SetRigidStatic(Entity entity, const RigidStatic& body)
{
    rigidStaticManager_.SetComponent(entity, body);
}

void PhysicsEngine::AddRigidDynamic(Entity entity, RigidDynamic& rigidDynamic)
{
    rigidDynamicManager_.AddComponent(entity);
    Vec3f position = transform3d_.GetPosition(entity);
    EulerAngles euler = transform3d_.GetAngles(entity);
    if (entityManager_.HasComponent(entity, EntityMask(ComponentType::BOX_COLLIDER))) {
        BoxPhysicsShape boxPhysicsShape = boxPhysicsShapeManager_.GetComponent(entity);
        rigidDynamic.Init(physics_, boxPhysicsShape, position, euler);
    }
    else if (entityManager_.HasComponent(entity, EntityMask(ComponentType::SPHERE_COLLIDER))) {
        SpherePhysicsShape spherePhysicsShape = spherePhysicsShapeManager_.GetComponent(entity);
        rigidDynamic.Init(physics_, spherePhysicsShape, position, euler);
    }
    else {
        neko_assert(false, "No shape link to the rigidBody")
    }
    scene_->addActor(*rigidDynamic.GetPxRigidDynamic());
    rigidDynamicManager_.SetComponent(entity, rigidDynamic);
    
}

const RigidDynamic& PhysicsEngine::GetRigidDynamic(Entity entity) const {
    return     rigidDynamicManager_.GetComponent(entity);
}

void PhysicsEngine::SetRigidDynamic(Entity entity, const RigidDynamic& body)
{
    rigidDynamicManager_.SetComponent(entity, body);
}

void PhysicsEngine::AddBoxPhysicsShape(Entity entity, BoxPhysicsShape& boxPhysicsShape)
{
    boxPhysicsShapeManager_.AddComponent(entity);
    boxPhysicsShape.Init(physics_);
    boxPhysicsShapeManager_.SetComponent(entity, boxPhysicsShape);
}

const BoxPhysicsShape& PhysicsEngine::GetBoxPhysicsShape(Entity entity) const {
    return boxPhysicsShapeManager_.GetComponent(entity);
}

void PhysicsEngine::SetBoxPhysicsShape(
    Entity entity,
    const BoxPhysicsShape& body)
{
    boxPhysicsShapeManager_.SetComponent(entity, body);
}

void PhysicsEngine::AddSpherePhysicsShape(Entity entity, SpherePhysicsShape& spherePhysicsShape)
{
    spherePhysicsShapeManager_.AddComponent(entity);
    spherePhysicsShape.Init(physics_);
    spherePhysicsShapeManager_.SetComponent(entity, spherePhysicsShape);
}

const SpherePhysicsShape& PhysicsEngine::GetSpherePhysicsShape(
    Entity entity) const {
    return spherePhysicsShapeManager_.GetComponent(entity);
}

void PhysicsEngine::SetSpherePhysicsShape(
    Entity entity,
    const SpherePhysicsShape& body)
{
    spherePhysicsShapeManager_.SetComponent(entity, body);
    
}
}
