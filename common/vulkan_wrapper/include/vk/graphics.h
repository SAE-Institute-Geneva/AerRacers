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

 Author : Simon
 Co-Author :
---------------------------------------------------------- */
#include "graphics/graphics.h"
#include "sdl_engine/sdl_camera.h"
#include "vk/buffers/uniform_buffer.h"
#include "vk/commands/command_pool.h"
#include "vk/commands/model_command_buffer.h"
#include "vk/core/instance.h"
#include "vk/core/logical_device.h"
#include "vk/core/physical_device.h"
#include "vk/core/surface.h"
#include "vk/descriptors/descriptor_pool.h"
#include "vk/framebuffers/framebuffers.h"
#include "vk/framebuffers/swapchain.h"
#include "vk/material/material_manager.h"
#include "vk/pipelines/graphics_pipeline.h"
#include "vk/pipelines/material_pipeline.h"
#include "vk/pipelines/material_pipeline_container.h"
#include "vk/renderers/renderer.h"
#include "vk/shaders/shader.h"
#include "vk/vulkan_window.h"

namespace neko::vk
{
const int kMaxFramesInFlight = 2;

struct IVkObjects
{
    explicit IVkObjects(sdl::VulkanWindow* window) : vkWindow(window) {}

    sdl::VulkanWindow* vkWindow = nullptr;

    Instance instance;
    Surface surface;
    PhysicalDevice gpu;
    LogicalDevice device;

    std::unique_ptr<Swapchain> swapchain{};

    std::unique_ptr<CommandPool> commandPools{};
    std::vector<std::unique_ptr<CommandBuffer>> commandBuffers{};

    DescriptorPool descriptorPool;
    ModelCommandBuffer modelCommandBuffer;
	std::unique_ptr<GraphicsPipeline> graphicsPipeline{};

    VkPipelineCache pipelineCache{};

    std::unique_ptr<Renderer> renderer{};

    [[nodiscard]] virtual RenderStage& GetRenderStage(std::uint32_t index) const = 0;
	[[nodiscard]] virtual MaterialPipeline& AddMaterialPipeline(
			const Pipeline::Stage& pipelineStage,
			const GraphicsPipelineCreateInfo& pipelineCreate) const = 0;
};

struct NullVkObjects : IVkObjects
{
    explicit NullVkObjects() : IVkObjects(nullptr) {}

    [[nodiscard]] RenderStage& GetRenderStage([[maybe_unused]] std::uint32_t index) const override
    {
        return renderer->GetRenderStage(INVALID_INDEX);
    }

	[[nodiscard]] MaterialPipeline& AddMaterialPipeline(
			const Pipeline::Stage &pipelineStage,
			const GraphicsPipelineCreateInfo &pipelineCreate) const override
	{
    	neko_assert(false, "Vulkan Engine is null!")
	}
};

using VkObjectsLocator = Locator<IVkObjects, NullVkObjects>;

class VkRenderer final : public neko::Renderer, IVkObjects
{
public:
    explicit VkRenderer(sdl::VulkanWindow* window);
    ~VkRenderer() override;

    void ClearScreen() override;

    void BeforeRenderLoop() override;
    void AfterRenderLoop() override;

    void SetWindow(sdl::VulkanWindow* window);
    void SetRenderer(std::unique_ptr<vk::Renderer>&& renderer);

    [[nodiscard]] RenderStage& GetRenderStage(std::uint32_t index) const override;
	[[nodiscard]] MaterialPipeline& AddMaterialPipeline(
    		const Pipeline::Stage& pipelineStage,
    		const GraphicsPipelineCreateInfo& pipelineCreate) const override;

private:
    bool StartRenderPass(RenderStage& renderStage);
    void EndRenderPass(const RenderStage& renderStage);

    void ResetRenderStages();

    void RecreateSwapChain();
    void RecreateCommandBuffers();
    void RecreatePass(RenderStage& renderStage);
    void RecreateAttachments();

    void CreatePipelineCache();

	void RenderAll() override;

    bool isFramebufferResized_ = false;

	std::unique_ptr<MaterialPipelineContainer> materialPipelineContainer_ =
			std::make_unique<MaterialPipelineContainer>();

	MaterialManager materialManager_;

    std::size_t currentFrame_ = 0;
    std::vector<VkFence> inFlightFences_{};
    std::vector<VkSemaphore> imageAvailableSemaphores_{};
    std::vector<VkSemaphore> renderFinishedSemaphores_{};

    std::map<XXH64_hash_t, const IDescriptor&> attachments_{};
};
}
