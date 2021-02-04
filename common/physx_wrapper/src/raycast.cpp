#include <px/raycast.h>
#include <engine/log.h>
#include "px/physics_engine.h"
#include "px/physx_utility.h"


namespace neko::physics {
Vec3f RaycastInfo::GetPoint() const
{
    return ConvertFromPxVec(pxRaycastBuffer.block.position);
}

Vec3f RaycastInfo::GetNormal() const
{
    return ConvertFromPxVec(pxRaycastBuffer.block.normal);
}

unsigned RaycastInfo::GetFaceId() const
{
    return pxRaycastBuffer.block.faceIndex;
}

float RaycastInfo::GetDistance() const
{
    return pxRaycastBuffer.block.distance;
}

Vec2f RaycastInfo::GetUv() const
{
    return Vec2f(pxRaycastBuffer.block.u, pxRaycastBuffer.block.v);
}
}
