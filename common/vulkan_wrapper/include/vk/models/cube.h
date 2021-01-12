#pragma once
#include "graphics/shape.h"
#include "vk/models/mesh.h"

namespace neko::vk
{
class RenderCuboid final : public Mesh
{
public:
    explicit RenderCuboid(Vec3f offset, Vec3f size);
    void Init() override;

private:
    Vec3f offset_;
    Vec3f size_;
};
}