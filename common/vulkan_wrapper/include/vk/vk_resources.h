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
#include "vk/commands/light_command_buffer.h"
#include "vk/commands/model_command_buffer.h"
#include "vk/core/instance.h"
#include "vk/core/logical_device.h"
#include "vk/core/physical_device.h"
#include "vk/pipelines/material_pipeline_container.h"
#include "vk/renderers/renderer.h"
#include "vk/vk_imgui.h"
#include "vk/vulkan_window.h"

namespace neko::vk
{
class VkResources
{
public:
	explicit VkResources(sdl::VulkanWindow* window);
	~VkResources();

	void DestroyResources();

	/// Add a new material pipeline to the render queue
	[[nodiscard]] MaterialPipeline& AddMaterialPipeline(
		const PipelineStage& pipelineStage, const GraphicsPipelineCreateInfo& pipelineCreate);

	[[nodiscard]] RenderStage& GetRenderStage() const;
	[[nodiscard]] const RenderPass& GetRenderPass() const;
	[[nodiscard]] CommandBuffer& GetCurrentCmdBuffer();
	[[nodiscard]] const CommandPool& GetCurrentCmdPool();
	[[nodiscard]] const CommandPool& GetCmdPool(std::thread::id threadId);

	[[nodiscard]] std::uint8_t GetViewportCount() const { return viewportCount_; }
	void SetViewportCount(std::uint8_t count) { viewportCount_ = count; }

	static VkResources* Inst;

	std::unique_ptr<sdl::VulkanWindow> vkWindow = nullptr;

	Instance instance;
	Surface surface;
	PhysicalDevice gpu;
	LogicalDevice device;

	Swapchain swapchain {};

	std::array<ModelCommandBuffer, 4> modelCommandBuffers;
	LightCommandBuffer lightCommandBuffer;

	VkPipelineCache pipelineCache {};

protected:
	bool isFramebufferResized_ = false;

	std::uint8_t viewportCount_ = 1;
	std::vector<CommandBuffer> commandBuffers_ {};
	std::map<std::thread::id, CommandPool> commandPools_ {};

	std::uint32_t currentFrame_ = 0;
	std::vector<VkFence> inFlightFences_ {};
	std::vector<VkSemaphore> availableSemaphores_ {};
	std::vector<VkSemaphore> finishedSemaphores_ {};

	std::map<StringHash, const IDescriptor&> attachments_ {};

	MaterialPipelineContainer materialPipelineContainer_ {};

	VkImGui imgui_;
	std::unique_ptr<IRenderer> renderer_{};
};

inline VkResources* VkResources::Inst = nullptr;
}    // namespace neko::vk
