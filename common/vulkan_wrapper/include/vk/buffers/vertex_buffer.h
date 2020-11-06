#pragma once
#include "vk/buffers/buffer.h"

namespace neko::vk
{
class VertexBuffer : public Buffer
{
public:
    void Init(const Vertex vertices[], std::size_t vertexNum);
};
}
