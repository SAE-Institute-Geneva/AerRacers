#include "vk/models/quad.h"

namespace neko::vk
{
RenderQuad::RenderQuad(const Vec3f& offset, const Vec2f& size) : offset_(offset), size_(size) {}

void RenderQuad::Init()
{
    const std::vector<Vertex> vertices =
    {
        {Vec3f(Vec2f(-0.5f, -0.5f) * size_) + offset_, Vec3f::back, {1, 1}}, //Bottom Left
        {Vec3f(Vec2f( 0.5f, -0.5f) * size_) + offset_, Vec3f::back, {0, 1}}, //Bottom Right
        {Vec3f(Vec2f( 0.5f,  0.5f) * size_) + offset_, Vec3f::back, {0, 0}}, //Top Right
        {Vec3f(Vec2f(-0.5f,  0.5f) * size_) + offset_, Vec3f::back, {1, 0}}, //Top Left
    };

	const std::vector<std::uint32_t> indices = {0, 1, 2, 2, 3, 0};

	InitData(vertices, indices);
}
}