#pragma once
#include "graphics/shape.h"
#include "vk/buffers/index_buffer.h"
#include "vk/buffers/vertex_buffer.h"

namespace neko::vk
{
class RenderQuad : public neko::RenderQuad
{
public:
    using neko::RenderQuad::RenderQuad;
    void Init() override;

    void Draw() const override;

    void Destroy() override;

    VertexBuffer vertexBuffer_;
    IndexBuffer indexBuffer_;

    const static Vertex Vertices[];
    const static uint16_t Indices[];
};

const inline Vertex RenderQuad::Vertices[] =
{
        {{-0.5, -0.5, 0}, Vec3f::back, {1, 0}, {1, 0, 0}, {0, 1, 0}}, //Bottom Left
        {{ 0.5, -0.5, 0}, Vec3f::back, {0, 0}, {1, 0, 0}, {0, 1, 0}}, //Bottom Right
        {{ 0.5,  0.5, 0}, Vec3f::back, {0, 1}, {1, 0, 0}, {0, 1, 0}}, //Top Right
        {{-0.5,  0.5, 0}, Vec3f::back, {1, 1}, {1, 0, 0}, {0, 1, 0}}, //Top Left
};

const inline uint16_t RenderQuad::Indices[] = { 0, 1, 2, 2, 3, 0 };
}