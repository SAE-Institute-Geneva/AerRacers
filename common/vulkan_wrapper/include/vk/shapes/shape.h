#pragma once
#include "vk/buffers/index_buffer.h"
#include "vk/buffers/vertex_buffer.h"

namespace neko::vk
{
struct VkRenderShape
{
    VertexBuffer vertexBuffer;
    IndexBuffer indexBuffer;
};
}
