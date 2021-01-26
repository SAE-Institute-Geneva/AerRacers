#include <px/physics_engine.h>
#include <engine\log.h>


#include "px/physx_utility.h"
#include "engine/engine.h"


namespace neko::physics {


PhysicsEngine::PhysicsEngine(
    EntityManager& entityManager,
    Transform3dManager& transform3d)
    : entityManager_(entityManager),
      transform3d_(transform3d)
{}

void PhysicsEngine::Init() {
    Start();
    StartPhysic(); }

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

void PhysicsEngine::Update(seconds dt)
{
    physicRunning_ = physicsStopped_;
    if (physicRunning_)
    {
        if (Advance(dt.count()))
        {
            //scene_->simulate(stepSize_.count());
            scene_->collide(stepSize_.count());
            scene_->fetchCollision(true);
            scene_->advance();    // Can this be skipped
            scene_->fetchResults(true);
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

const PxRaycastInfo PhysicsEngine::Raycast(
    const Vec3f& origin,
    const Vec3f& direction,
    float maxDistance) const
{
    PxRaycastInfo raycastHit;
    physx::PxQueryFilterData fd;
    fd.flags |= physx::PxQueryFlag::eANY_HIT; // note the OR with the default value
    raycastHit.touch = scene_->raycast(ConvertToPxVec(origin), ConvertToPxVec(direction.Normalized()), maxDistance, raycastHit.pxRaycastBuffer,
                                  physx::PxHitFlags(physx::PxHitFlag::eDEFAULT), fd);
    return raycastHit;
}


void PhysicsEngine::RegisterCollisionListener(OnCollisionInterface& collisionInterface)
{
    eventCallback_.onCollisionEnterAction.RegisterCallback(
        [&collisionInterface](const physx::PxContactPair& pairHeader) { collisionInterface.OnCollisionEnter(pairHeader); });
    eventCallback_.onCollisionStayAction.RegisterCallback(
        [&collisionInterface](const physx::PxContactPair& pairHeader) { collisionInterface.OnCollisionStay(pairHeader); });
    eventCallback_.onCollisionExitAction.RegisterCallback(
        [&collisionInterface](const physx::PxContactPair& pairHeader) { collisionInterface.OnCollisionExit(pairHeader); });
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
