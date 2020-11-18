//
// Created by efarhan on 11/16/19.
//
#ifdef WIN32
#define _USE_MATH_DEFINES
#endif
#include <cmath>
#include <random>
#include <gtest/gtest.h>
#include <mathematics/func_table.h>
#include <mathematics/aabb.h>

#include <mathematics/quaternion.h>
#include <mathematics/matrix.h>
#include "mathematics/vector.h"
#include "PxPhysicsAPI.h"
#include <log.h>

const float maxNmb = 100.0f;

TEST(PhysX, TestPhysX)
{
    static physx::PxDefaultErrorCallback gDefaultErrorCallback;
    static physx::PxDefaultAllocator gDefaultAllocatorCallback;

    physx::PxFoundation* mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gDefaultAllocatorCallback,
        gDefaultErrorCallback);
    if (!mFoundation)
        neko::LogDebug("PxCreateFoundation failed!");
    mFoundation->release();
}