#include <physics_engine.h>
#include <engine\log.h>


namespace neko::physics {

void PhysicsEngine::Start()
{
    static physx::PxDefaultErrorCallback gDefaultErrorCallback;
    static physx::PxDefaultAllocator gDefaultAllocatorCallback;

    mFoundation_ = PxCreateFoundation(
        PX_PHYSICS_VERSION,
        gDefaultAllocatorCallback,
        gDefaultErrorCallback);
    if (!mFoundation_)
        logDebug("PxCreateFoundation failed!");
    
    bool recordMemoryAllocations = true;
    std::string PVD_HOST = "0";
    mPvd_ = physx::PxCreatePvd(*mFoundation_);
    physx::PxPvdTransport* transport = physx::PxDefaultPvdSocketTransportCreate(PVD_HOST.c_str(), 5425, 10);
    mPvd_->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);


    mPhysics_ = PxCreatePhysics(
        PX_PHYSICS_VERSION,
        *mFoundation_,
        physx::PxTolerancesScale(),
        recordMemoryAllocations);
    if (!mPhysics_)
        logDebug("PxCreatePhysics failed!");

    mCooking_ = PxCreateCooking(PX_PHYSICS_VERSION, *mFoundation_, physx::PxCookingParams(physx::PxTolerancesScale()));
    if (!mCooking_)
        logDebug("PxCreateCooking failed!");

    if (!PxInitExtensions(*mPhysics_, mPvd_))
        logDebug("PxInitExtensions failed!");

}

void PhysicsEngine::Update(float dt)
{
    scene_->simulate(dt);
    scene_->fetchResults(true);

}

void PhysicsEngine::Destroy()
{
    mPhysics_->release();
    mFoundation_->release();
}
}
