#pragma once
#include "vk/vulkan_include.h"
#include "vk/buffers/index_buffer.h"
#include "vk/buffers/vertex_buffer.h"
#include "vk/commands/command_pool.h"
#include "vk/framebuffers/framebuffers.h"
#include "vk/pipelines/graphics_pipeline.h"
#include "vk/shader.h"

namespace neko::vk
{
class CommandBuffers
{
public:
	explicit CommandBuffers() = default;

	void Init(const VertexBuffer& vertexBuffer,
            const IndexBuffer& indexBuffer,
            const Shader shaders[]);
	void Destroy();

	const VkCommandBuffer& operator[](const size_t index) const
    { return commandBuffers_[index]; }

    VkCommandBuffer& operator[](const size_t index)
    { return commandBuffers_[index]; }
	
private:
	std::vector<VkCommandBuffer> commandBuffers_;
};
}
