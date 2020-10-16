#pragma once
#include "graphics/shape.h"
#include "vk/shapes/shape.h"

namespace neko::vk
{
class RenderQuad final : public neko::RenderQuad, VkRenderShape
{
public:
    using neko::RenderQuad::RenderQuad;
    void Init() override;

    void Draw() const override;

    void Destroy() override;
	
    const static Vertex kVertices[];
    const static uint16_t kIndices[];
};

const inline Vertex RenderQuad::kVertices[] =
{
        {{-0.5, -0.5, 0}, Vec3f::back, {1, 0}, {1, 0, 0}, {0, 1, 0}}, //Bottom Left
        {{ 0.5, -0.5, 0}, Vec3f::back, {0, 0}, {1, 0, 0}, {0, 1, 0}}, //Bottom Right
        {{ 0.5,  0.5, 0}, Vec3f::back, {0, 1}, {1, 0, 0}, {0, 1, 0}}, //Top Right
        {{-0.5,  0.5, 0}, Vec3f::back, {1, 1}, {1, 0, 0}, {0, 1, 0}}, //Top Left
};

const inline uint16_t RenderQuad::kIndices[] = { 0, 1, 2, 2, 3, 0 };
}