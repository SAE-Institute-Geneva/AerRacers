#include <px/raycast.h>
#include <engine\log.h>
#include "px/physics_engine.h"
#include "px/physx_utility.h"


namespace neko::physics {
Vec3f PxRaycastInfo::GetPoint() const
{
    return ConvertFromPxVec(pxRaycastBuffer.block.position);
}

Vec3f PxRaycastInfo::GetNormal() const
{
    return ConvertFromPxVec(pxRaycastBuffer.block.normal);
}

unsigned PxRaycastInfo::GetFaceId() const
{
    return pxRaycastBuffer.block.faceIndex;
}

float PxRaycastInfo::GetDistance() const
{
    return pxRaycastBuffer.block.distance;
}

Vec2f PxRaycastInfo::GetUv() const
{
    return Vec2f(pxRaycastBuffer.block.u, pxRaycastBuffer.block.v);
}
}
