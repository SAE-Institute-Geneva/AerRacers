#include <physics_engine.h>
#include <engine\log.h>


#include "physx_utility.h"
#include "engine/engine.h"


namespace neko::physics {


PhysicsEngine::PhysicsEngine(
    EntityManager& entityManager,
    Transform3dManager& transform3d)
    : entityManager_(entityManager),
      transform3d_(transform3d),
      rigidDynamicManager_(
          entityManager,
          transform3d_),
      rigidStaticManager_(
          entityManager,
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
    transport_ = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST.c_str(), 5425, 10);
    bool connec = pvd_->connect(*transport_, physx::PxPvdInstrumentationFlag::eALL);
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
    sceneDesc.filterShader = ContactReportFilterShader;
    cpuDispatcher_ = physx::PxDefaultCpuDispatcherCreate(1);
    if (!cpuDispatcher_)
        std::cerr << "PxDefaultCpuDispatcherCreate failed!";
    sceneDesc.cpuDispatcher = cpuDispatcher_;
    sceneDesc.kineKineFilteringMode = physx::PxPairFilteringMode::eKEEP; // So kin-kin contacts with be reported
    sceneDesc.staticKineFilteringMode = physx::PxPairFilteringMode::eKEEP; // So static-kin constacts will be reported
    sceneDesc.simulationEventCallback = &eventCallback_;
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
    physicRunning_ = physicsStopped_;
    if (physicRunning_)
    {
        if (Advance(dt))
        {
            //scene_->simulate(stepSize_.count());
            scene_->collide(stepSize_.count());
            scene_->fetchCollision(true);
            scene_->advance(); // Can this be skipped
            scene_->fetchResults(true);
            rigidDynamicManager_.FixedUpdate(stepSize_);
            rigidStaticManager_.FixedUpdate(stepSize_);
            fixedUpdateAction_.Execute(stepSize_);
        }
    }
}

void PhysicsEngine::Destroy()
{
    pvd_->disconnect();
    physics_->release();
    cooking_->release();
    PxCloseExtensions();
    pvd_->release();
    transport_->release();
    foundation_->release();
}

physx::PxFilterFlags PhysicsEngine::ContactReportFilterShader(physx::PxFilterObjectAttributes attributes0,
    physx::PxFilterData filterData0,
    physx::PxFilterObjectAttributes attributes1,
    physx::PxFilterData filterData1,
    physx::PxPairFlags& pairFlags,
    const void* constantBlock,
    physx::PxU32 constantBlockSize)
{

    pairFlags = physx::PxPairFlag::eSOLVE_CONTACT | physx::PxPairFlag::eDETECT_DISCRETE_CONTACT
        | physx::PxPairFlag::eNOTIFY_TOUCH_FOUND
        | physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS
        | physx::PxPairFlag::eNOTIFY_TOUCH_LOST
        | physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
    return physx::PxFilterFlag::eDEFAULT; //eNOTIFY //:eCALLBACK; //physx::PxFilterFlag::eDEFAULT;
}

physx::PxPhysics* PhysicsEngine::GetPhysx()
{
    return physics_;
}

physx::PxScene* PhysicsEngine::GetScene()
{
    return scene_;
}

void PhysicsEngine::AddRigidStatic(Entity entity, const physics::RigidStaticData& rigidStaticData, BoxColliderData& shape)
{
    rigidStaticManager_.AddComponent(entity);
    Vec3f position = transform3d_.GetPosition(entity);
    EulerAngles euler = transform3d_.GetAngles(entity);
    Vec3f scale = transform3d_.GetScale(entity);
    shape.size = Vec3f(shape.size.x * scale.x, shape.size.y * scale.y, shape.size.z * scale.z);
    RigidStatic rigidStatic = rigidStaticManager_.GetComponent(entity);
    rigidStatic.Init(physics_, rigidStaticData, shape, position, euler);
    scene_->addActor(*rigidStatic.GetPxRigidStatic());
    rigidStaticManager_.SetComponent(entity, rigidStatic);
}

void PhysicsEngine::AddRigidStatic(Entity entity, const physics::RigidStaticData& rigidStaticData, const SphereColliderData& shape)
{
    rigidStaticManager_.AddComponent(entity);
    Vec3f position = transform3d_.GetPosition(entity);
    EulerAngles euler = transform3d_.GetAngles(entity);
    RigidStatic rigidStatic = rigidStaticManager_.GetComponent(entity);
    rigidStatic.Init(physics_, rigidStaticData, shape, position, euler);
    scene_->addActor(*rigidStatic.GetPxRigidStatic());
    rigidStaticManager_.SetComponent(entity, rigidStatic);
}

const RigidStatic& PhysicsEngine::GetRigidStatic(Entity entity) const {
    return rigidStaticManager_.GetComponent(entity);
}

void PhysicsEngine::SetRigidStatic(Entity entity, const RigidStatic& body)
{
    rigidStaticManager_.SetComponent(entity, body);
}

void PhysicsEngine::AddRigidDynamic(Entity entity, const RigidDynamicData& rigidDynamicData, BoxColliderData& shape)
{
    rigidDynamicManager_.AddComponent(entity);
    Vec3f position = transform3d_.GetPosition(entity);
    EulerAngles euler = transform3d_.GetAngles(entity);
    Vec3f scale = transform3d_.GetScale(entity);
    shape.size = Vec3f(shape.size.x * scale.x, shape.size.y * scale.y, shape.size.z * scale.z);
    RigidDynamic rigidDynamic = rigidDynamicManager_.GetComponent(entity);
    rigidDynamic.Init(physics_, rigidDynamicData, shape, position, euler);
    scene_->addActor(*rigidDynamic.GetPxRigidDynamic());
    rigidDynamicManager_.SetComponent(entity, rigidDynamic);
}


void PhysicsEngine::AddRigidDynamic(Entity entity, const RigidDynamicData& rigidDynamicData, const SphereColliderData& shape)
{
    rigidDynamicManager_.AddComponent(entity);
    Vec3f position = transform3d_.GetPosition(entity);
    EulerAngles euler = transform3d_.GetAngles(entity);
    RigidDynamic rigidDynamic = rigidDynamicManager_.GetComponent(entity);
    rigidDynamic.Init(physics_, rigidDynamicData, shape, position, euler);
    scene_->addActor(*rigidDynamic.GetPxRigidDynamic());
    rigidDynamicManager_.SetComponent(entity, rigidDynamic);
}

const RigidDynamic& PhysicsEngine::GetRigidDynamic(Entity entity) const {
    return rigidDynamicManager_.GetComponent(entity);
}

void PhysicsEngine::SetRigidDynamic(Entity entity, const RigidDynamic& body)
{
    rigidDynamicManager_.SetComponent(entity, body);
}

const RigidDynamicData& PhysicsEngine::GetRigidDynamicData(Entity entity) const
{
    return GetRigidDynamic(entity).GetRigidDynamicData();
}

void PhysicsEngine::SetRigidDynamicData(
    Entity entity,
    const RigidDynamicData& rigidDynamicData) const
{
    if (!physicRunning_) {
        GetRigidDynamic(entity).SetRigidDynamicData(rigidDynamicData);
    }
}

const ColliderType& PhysicsEngine::GetColliderType(
    Entity entity) const
{
    return GetRigidDynamic(entity).GetType();
}

const BoxColliderData& PhysicsEngine::GetBoxColliderData(Entity entity) const
{
    if (entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_DYNAMIC))) {
        return GetRigidDynamic(entity).GetBoxColliderData();
    }
    else if (entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_STATIC))) {
        return GetRigidStatic(entity).GetBoxColliderData();
    }
    return {};
}

void PhysicsEngine::SetBoxColliderData(
    Entity entity,
    const BoxColliderData& boxColliderData) const
{
    if (!physicRunning_) {
        if (entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_DYNAMIC))) {
            GetRigidDynamic(entity).SetBoxColliderData(boxColliderData);
        }
        else if (entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_STATIC))) {
            GetRigidStatic(entity).SetBoxColliderData(boxColliderData);
        }
    }
}

const SphereColliderData& PhysicsEngine::GetSphereColliderData(
    Entity entity) const
{
    if (entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_DYNAMIC))) {
        return GetRigidDynamic(entity).GetSphereColliderData();
    }
    else if (entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_STATIC))) {
        return GetRigidStatic(entity).GetSphereColliderData();
    }
    return {};
}

void PhysicsEngine::SetSphereColliderData(
    Entity entity,
    const SphereColliderData& sphereColliderData) const
{
    if (!physicRunning_) {
        if (entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_DYNAMIC))) {
            GetRigidDynamic(entity).SetSphereColliderData(sphereColliderData);
        }
        else if (entityManager_.HasComponent(entity, EntityMask(ComponentType::RIGID_STATIC))) {
            GetRigidStatic(entity).SetSphereColliderData(sphereColliderData);
        }
    }
}

void PhysicsEngine::AddForceAtPosition(
    Entity entity,
    const Vec3f& force,
    const Vec3f& position) const
{
    GetRigidDynamic(entity).AddForceAtPosition(force, position);
}

void PhysicsEngine::AddForce(Entity entity, const Vec3f& force) const
{
    GetRigidDynamic(entity).AddForce(force);
}

const PxRaycastInfo PhysicsEngine::Raycast(
    Vec3f origin,
    Vec3f direction,
    float maxDistance) const
{
    PxRaycastInfo raycastHit;
    physx::PxQueryFilterData fd;
    fd.flags |= physx::PxQueryFlag::eANY_HIT; // note the OR with the default value
    raycastHit.touch = scene_->raycast(ConvertToPxVec(origin), ConvertToPxVec(direction), maxDistance, raycastHit.pxRaycastBuffer,
                                  physx::PxHitFlags(physx::PxHitFlag::eDEFAULT), fd);
    return raycastHit;
}


void PhysicsEngine::RegisterCollisionListener(OnCollisionInterface& collisionInterface)
{
    eventCallback_.onCollisionEnterAction.RegisterCallback(
        [&collisionInterface](const physx::PxContactPairHeader& pairHeader) { collisionInterface.OnCollisionEnter(pairHeader); });
    eventCallback_.onCollisionStayAction.RegisterCallback(
        [&collisionInterface](const physx::PxContactPairHeader& pairHeader) { collisionInterface.OnCollisionStay(pairHeader); });
    eventCallback_.onCollisionExitAction.RegisterCallback(
        [&collisionInterface](const physx::PxContactPairHeader& pairHeader) { collisionInterface.OnCollisionExit(pairHeader); });
}

void PhysicsEngine::RegisterTriggerListener(OnTriggerInterface& triggerInterface)
{
    eventCallback_.onTriggerEnterAction.RegisterCallback(
        [&triggerInterface](physx::PxTriggerPair* pairs) { triggerInterface.OnTriggerEnter(pairs); });
    eventCallback_.onTriggerExitAction.RegisterCallback(
        [&triggerInterface](physx::PxTriggerPair* pairs) { triggerInterface.OnTriggerExit(pairs); });
}

void PhysicsEngine::RegisterFixedUpdateListener(
    FixedUpdateInterface& fixedUpdateInterface)
{
    fixedUpdateAction_.RegisterCallback(
        [&fixedUpdateInterface](seconds dt) { fixedUpdateInterface.FixedUpdate(dt); });
}

void PhysicsEngine::StartPhysic()
{
    physicsStopped_ = true;
}

void PhysicsEngine::StopPhysic()
{
    physicsStopped_ = false;
}

bool PhysicsEngine::IsPhysicRunning() const
{
    return physicRunning_;
}

}
