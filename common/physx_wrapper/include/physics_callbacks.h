#pragma once
/* ----------------------------------------------------
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author : Floreau Luca
 Co-Author :
 Date : 28.12.2020
---------------------------------------------------------- */

#include "PxPhysicsAPI.h"
#include "utilities/action_utility.h"
#include "utilities/time_utility.h"

namespace neko::physics {

class PhysicsSimulationEventCallback : public physx::PxSimulationEventCallback
{
public:
    virtual void onConstraintBreak(
        physx::PxConstraintInfo* constraints,
        physx::PxU32 count) override;

    virtual void onWake(physx::PxActor** actors, physx::PxU32 count) override;

    virtual void onSleep(physx::PxActor** actors, physx::PxU32 count) override;

    virtual void onContact(
        const physx::PxContactPairHeader& pairHeader,
        const physx::PxContactPair* pairs,
        physx::PxU32 nbPairs) override;

    virtual void onTrigger(
        physx::PxTriggerPair* pairs,
        physx::PxU32 count) override;

    virtual void onAdvance(
        const physx::PxRigidBody* const* bodyBuffer,
        const physx::PxTransform* poseBuffer,
        const physx::PxU32 count) override;

    Action<const physx::PxContactPairHeader&> onCollisionEnterAction;
    Action<const physx::PxContactPairHeader&> onCollisionStayAction;
    Action<const physx::PxContactPairHeader&> onCollisionExitAction;
    Action<physx::PxTriggerPair*> onTriggerEnterAction;
    Action<physx::PxTriggerPair*> onTriggerExitAction;
};


class FixedUpdateInterface
{
public:
    virtual void FixedUpdate(seconds dt) = 0;
};

class OnCollisionInterface
{
public:
    virtual void OnCollisionEnter(const physx::PxContactPairHeader& pairHeader) {}
    virtual void OnCollisionStay(const physx::PxContactPairHeader& pairHeader) {}
    virtual void OnCollisionExit(const physx::PxContactPairHeader& pairHeader) {}
};


class OnTriggerInterface
{
public:
    virtual void OnTriggerEnter(physx::PxTriggerPair* pairs) {}
    virtual void OnTriggerExit(physx::PxTriggerPair* pairs) {}
};

}
