#pragma once
/* ----------------------------------------------------
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author: Canas Simon
 Date:
---------------------------------------------------------- */
#include "vk/commands/command_pool.h"
#include "vk/commands/model_command_buffer.h"
#include "vk/core/instance.h"
#include "vk/core/logical_device.h"
#include "vk/core/physical_device.h"
#include "vk/pipelines/material_pipeline_container.h"
#include "vk/renderers/renderer.h"
#include "vk/vulkan_window.h"
#include "vk_imgui.h"

namespace neko::vk
{
class VkResources
{
public:
	explicit VkResources(sdl::VulkanWindow* window);
	~VkResources();

	/// Add a new material pipeline to the render queue
	[[nodiscard]] MaterialPipeline& AddMaterialPipeline(const PipelineStage& pipelineStage,
		const GraphicsPipelineCreateInfo& pipelineCreate) const;

	[[nodiscard]] RenderStage& GetRenderStage() const;
	[[nodiscard]] const RenderPass& GetRenderPass() const;
	[[nodiscard]] CommandBuffer& GetCurrentCmdBuffer();
	[[nodiscard]] const CommandPool& GetCurrentCmdPool();

	[[nodiscard]] bool IsImGuiReady() const { return imgui_.get(); }

	static VkResources* Inst;

	sdl::VulkanWindow* vkWindow = nullptr;

	Instance instance;
	Surface surface;
	PhysicalDevice gpu;
	LogicalDevice device;

	std::unique_ptr<Swapchain> swapchain {};

	ModelCommandBuffer modelCommandBuffer;

	VkPipelineCache pipelineCache {};

protected:
	bool isFramebufferResized_ = false;

	std::unique_ptr<IRenderer> renderer_ {};
	std::vector<std::unique_ptr<CommandBuffer>> commandBuffers_ {};
	std::map<std::thread::id, std::unique_ptr<CommandPool>> commandPools_ {};

	std::uint32_t currentFrame_ = 0;
	std::vector<VkFence> inFlightFences_ {};
	std::vector<VkSemaphore> availableSemaphores_ {};
	std::vector<VkSemaphore> finishedSemaphores_ {};

	std::map<StringHash, const IDescriptor&> attachments_ {};

	std::unique_ptr<MaterialPipelineContainer> materialPipelineContainer_ =
		std::make_unique<MaterialPipelineContainer>();

	std::unique_ptr<VkImGui> imgui_;
};

inline VkResources* VkResources::Inst = nullptr;
}    // namespace neko::vk
