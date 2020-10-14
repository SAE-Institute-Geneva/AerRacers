#pragma once
#include "vk/vulkan_include.h"
#include "vk/buffers/buffer.h"
#include "vk/core/logical_device.h"
#include "vk/commands/command_pool.h"
#include "vk/pipelines/graphics_pipeline.h"

namespace neko::vk
{
class VertexBuffer : public Buffer
{
public:
    void Init(const Vertex vertices[], size_t vertexNum);
};
}
