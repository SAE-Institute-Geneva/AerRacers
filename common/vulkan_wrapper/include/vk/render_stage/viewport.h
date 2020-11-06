#pragma once
#include "mathematics/vector.h"

namespace neko::vk
{
struct Viewport
{
    Vec2f scale = Vec2f::one;
    Vec2f offset = Vec2f::zero;
    Vec2u size = Vec2u::zero;
};
}