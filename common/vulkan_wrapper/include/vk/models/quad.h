#pragma once
#include "graphics/shape.h"
#include "vk/models/mesh.h"

namespace neko::vk
{
class RenderQuad final : public Mesh
{
public:
    explicit RenderQuad(Vec3f offset, Vec2f size);
    void Init() override;

private:
    Vec3f offset_;
    Vec2f size_;
};
}