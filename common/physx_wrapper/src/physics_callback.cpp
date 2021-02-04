#include <px/physics_callbacks.h>

#include "engine/log.h"


namespace neko::physics {

void PhysicsSimulationEventCallback::onConstraintBreak(
    physx::PxConstraintInfo* constraints,
    physx::PxU32 count) {

    //logDebug("onConstraintBreak");

}

void PhysicsSimulationEventCallback::onWake(
    physx::PxActor** actors,
    physx::PxU32 count)
{
    //logDebug("onWake");
}

void PhysicsSimulationEventCallback::onSleep(
    physx::PxActor** actors,
    physx::PxU32 count)
{
    //logDebug("onSleep");
}

void PhysicsSimulationEventCallback::onContact(
    const physx::PxContactPairHeader& pairHeader,
    const physx::PxContactPair* pairs,
    physx::PxU32 nbPairs)
{
    for (physx::PxU32 i = 0; i < nbPairs; i++) {
        const physx::PxContactPair& cp = pairs[i];

        if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
            //logDebug("onContactEnter");
            onCollisionEnterAction.Execute(pairHeader);
        }
        if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_PERSISTS) {
            //logDebug("onContactStay");
            onCollisionStayAction.Execute(pairHeader);
        }
        if (cp.events & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
            //logDebug("onContactExit");
            onCollisionExitAction.Execute(pairHeader);
        }
    }
}

void PhysicsSimulationEventCallback::onTrigger(
    physx::PxTriggerPair* pairs,
    physx::PxU32 count)
{
    if (pairs->status & physx::PxPairFlag::eNOTIFY_TOUCH_FOUND) {
        //logDebug("onTriggerEnter");
        onTriggerEnterAction.Execute(pairs);
    }
    if (pairs->status & physx::PxPairFlag::eNOTIFY_TOUCH_LOST) {
        //logDebug("onTriggerExit");
        onTriggerExitAction.Execute(pairs);
    }
}

void PhysicsSimulationEventCallback::onAdvance(
    const physx::PxRigidBody* const* bodyBuffer,
    const physx::PxTransform* poseBuffer,
    const physx::PxU32 count)
{
    //logDebug("onAdvance");
}
}
