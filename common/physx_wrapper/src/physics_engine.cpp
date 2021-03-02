#include <px/physics_engine.h>
#include <engine/log.h>


#include "px/physx_utility.h"
#include "engine/engine.h"


namespace neko::physics {


PhysicsEngine::PhysicsEngine(
    EntityManager& entityManager,
    Transform3dManager& transform3d)
    : entityManager_(entityManager),
      transform3d_(transform3d)
{
}

void PhysicsEngine::Init() {
    //Automatically starts simulation
    StartPhysic();
}

void PhysicsEngine::InitPhysics()
{
    static physx::PxDefaultErrorCallback gDefaultErrorCallback;
    static physx::PxDefaultAllocator gDefaultAllocatorCallback;

    foundation_ = PxCreateFoundation(
        PX_PHYSICS_VERSION,
        gDefaultAllocatorCallback,
        gDefaultErrorCallback);
    if (!foundation_)
    {
        assert("PxCreateFoundation failed!");
        return;
    }

    //Use to link with PhysX Visual Debugger
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
    {
        assert("PxCreatePhysics failed!");
        return;
    }

    cooking_ = PxCreateCooking(PX_PHYSICS_VERSION, *foundation_, physx::PxCookingParams(physx::PxTolerancesScale()));
    if (!cooking_)
        assert("PxCreateCooking failed!");

    if (!PxInitExtensions(*physics_, pvd_))
        assert("PxInitExtensions failed!");
    CreateScene();

}

void PhysicsEngine::CreateScene()
{
    physx::PxSceneDesc sceneDesc = physx::PxTolerancesScale();
    sceneDesc.gravity = physx::PxVec3(0.0f, -9.81f, 0.0f);
    physx::PxSimulationFilterShader gDefaultFilterShader = physx::PxDefaultSimulationFilterShader;
    sceneDesc.filterShader = ContactReportFilterShader;
    //sceneDesc.filterShaderData = &mapFilter_;
    //sceneDesc.filterShaderDataSize = sizeof(mapFilter_);
    cpuDispatcher_ = physx::PxDefaultCpuDispatcherCreate(1);
    if (!cpuDispatcher_)
        std::cerr << "PxDefaultCpuDispatcherCreate failed!";
    sceneDesc.cpuDispatcher = cpuDispatcher_;
    sceneDesc.kineKineFilteringMode = physx::PxPairFilteringMode::eKEEP; // So kin-kin contacts with be reported
    sceneDesc.staticKineFilteringMode = physx::PxPairFilteringMode::eKEEP; // So static-kin constacts will be reported
    sceneDesc.simulationEventCallback = &eventCallback_;
    sceneDesc.flags |= physx::PxSceneFlag::eENABLE_CCD; //Use when Continous Detection
    scene_ = physics_->createScene(sceneDesc);
    if (transport_->isConnected()) {
        if (!scene_) std::cerr << "createScene failed!";
        scene_->getScenePvdClient()->setScenePvdFlag(
            physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS,
            true);
        scene_->getScenePvdClient()->setScenePvdFlag(
            physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS,
            true);
        scene_->getScenePvdClient()->setScenePvdFlag(
            physx::PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES,
            true);
    }
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
        if (Advance(dt.count())) {
            FixedUpdate(stepSize_);
        }
    }
}

void PhysicsEngine::FixedUpdate(seconds dt)
{
    //scene_->simulate(stepSize_.count());
    scene_->collide(dt.count());
    scene_->fetchCollision(true);
    scene_->advance();    // Can this be skipped
    scene_->fetchResults(true);
    fixedUpdateAction_.Execute(dt);
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
    //const std::unordered_map<physx::PxU32, physx::PxU32>* mapFilter =  static_cast<const std::unordered_map<physx::PxU32, physx::PxU32>*>(constantBlock);
    // let triggers through
    if (physx::PxFilterObjectIsTrigger(attributes0) || physx::PxFilterObjectIsTrigger(attributes1))
    {
        pairFlags = physx::PxPairFlag::eTRIGGER_DEFAULT;
        return physx::PxFilterFlag::eDEFAULT;
    }
    // generate contacts for all that were not filtered above
    // trigger the contact callback for pairs (A,B) where
    // the filtermask of A contains the ID of B and vice versa.
    //if (!(filterData0.word0 & mapFilter->at(filterData1.word0)) &&
    //    !(filterData1.word0 & mapFilter->at(filterData0.word0)))
    {
        pairFlags = physx::PxPairFlag::eCONTACT_DEFAULT;
        pairFlags |= physx::PxPairFlag::eDETECT_CCD_CONTACT;
        pairFlags |= physx::PxPairFlag::eNOTIFY_TOUCH_FOUND |
            physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS |
            physx::PxPairFlag::eNOTIFY_TOUCH_LOST |
            physx::PxPairFlag::eNOTIFY_CONTACT_POINTS;
    }

    return physx::PxFilterFlag::eDEFAULT;
}

physx::PxPhysics* PhysicsEngine::GetPhysx() const
{
    return physics_;
}

physx::PxScene* PhysicsEngine::GetScene() const
{
    return scene_;
}

physx::PxCooking* PhysicsEngine::GetCooking() const
{
    return cooking_;
}

const RaycastInfo PhysicsEngine::Raycast(
    const Vec3f& origin,
    const Vec3f& direction,
    float maxDistance,
    FilterGroup::Enum filterGroup) const
{
    RaycastInfo raycastHit;
    physx::PxQueryFilterData fd;
    fd.data.word0 = filterGroup;
    raycastHit.touch = scene_->raycast(ConvertToPxVec(origin), ConvertToPxVec(direction.Normalized()), maxDistance, raycastHit.pxRaycastBuffer,
                                  physx::PxHitFlags(physx::PxHitFlag::eDEFAULT), fd);
    return raycastHit;
}


void PhysicsEngine::RegisterCollisionListener(OnCollisionInterface& collisionInterface)
{
    eventCallback_.onCollisionEnterAction.RegisterCallback(
        [&collisionInterface](const physx::PxContactPairHeader& pairHeader) {
            collisionInterface.OnCollisionEnter(pairHeader);
        });
    eventCallback_.onCollisionStayAction.RegisterCallback(
        [&collisionInterface](const physx::PxContactPairHeader& pairHeader) {
            collisionInterface.OnCollisionStay(pairHeader);
        });
    eventCallback_.onCollisionExitAction.RegisterCallback(
        [&collisionInterface](const physx::PxContactPairHeader& pairHeader) {
            collisionInterface.OnCollisionExit(pairHeader);
        });
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
