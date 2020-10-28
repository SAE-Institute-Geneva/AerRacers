#include "vk/models/quad.h"

namespace neko::vk
{
RenderQuad::RenderQuad(Vec3f offset, Vec2f size) : offset_(offset), size_(size){}

void RenderQuad::Init()
{
    const std::vector<Vertex> vertices =
    {
            {{-size_.x + offset_.x, -size_.y + offset_.y, offset_.z}, Vec3f::back, {1, 0}}, //Bottom Left
            {{ size_.x + offset_.x, -size_.y + offset_.y, offset_.z}, Vec3f::back, {0, 0}}, //Bottom Right
            {{ size_.x + offset_.x,  size_.y + offset_.y, offset_.z}, Vec3f::back, {0, 1}}, //Top Right
            {{-size_.x + offset_.x,  size_.y + offset_.y, offset_.z}, Vec3f::back, {1, 1}}, //Top Left
    };

    const std::vector<uint32_t> indices = { 0, 1, 2, 2, 3, 0 };

    InitData(vertices, indices);
}
}