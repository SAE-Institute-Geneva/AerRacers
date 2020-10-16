#pragma once
#include "vk/buffers/buffer.h"

namespace neko::vk
{
struct Vertex
{
    Vec3f position = Vec3f::zero;
    Vec3f normal = Vec3f::zero;
    Vec2f texCoords = Vec2f::zero;
    Vec3f tangent = Vec3f::zero;
    Vec3f bitangent = Vec3f::zero;
};

class VertexBuffer : public Buffer
{
public:
    void Init(const Vertex vertices[], size_t vertexNum);
};
}
