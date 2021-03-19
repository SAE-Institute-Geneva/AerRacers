#include "vk/vk_resources.h"

namespace neko::vk
{
VkResources::VkResources(sdl::VulkanWindow* window) : vkWindow(window) { Inst = this; }

VkResources::~VkResources()
{
	vkDeviceWaitIdle(VkDevice(device));
	imgui_.Destroy();

	const auto& graphicsQueue = device.GetGraphicsQueue();
	vkQueueWaitIdle(graphicsQueue);
	vkDestroyPipelineCache(VkDevice(device), pipelineCache, nullptr);

	for (size_t i = 0; i < inFlightFences_.size(); i++)
	{
		vkDestroyFence(VkDevice(device), inFlightFences_[i], nullptr);
		vkDestroySemaphore(VkDevice(device), availableSemaphores_[i], nullptr);
		vkDestroySemaphore(VkDevice(device), finishedSemaphores_[i], nullptr);
	}

	renderer_->Destroy();

	for (auto& commandBuffer : commandBuffers_)
		commandBuffer.Destroy();

	for (const auto& commandPool : commandPools_)
		commandPool.second.Destroy();

	swapchain.Destroy();

	device.Destroy();
	surface.Destroy();
	instance.Destroy();
}

MaterialPipeline& VkResources::AddMaterialPipeline(
	const PipelineStage& pipelineStage, const GraphicsPipelineCreateInfo& pipelineCreate)
{
	return materialPipelineContainer_.AddMaterial(pipelineStage, pipelineCreate);
}

RenderStage& VkResources::GetRenderStage() const { return renderer_->GetRenderStage(); }

const RenderPass& VkResources::GetRenderPass() const { return renderer_->GetRenderPass(); }

CommandBuffer& VkResources::GetCurrentCmdBuffer()
{
	return commandBuffers_[swapchain.GetCurrentImageIndex()];
}

const CommandPool& VkResources::GetCurrentCmdPool()
{
	const std::thread::id threadId = std::this_thread::get_id();

	auto it = commandPools_.find(threadId);
	if (it != commandPools_.end()) return it->second;

	commandPools_.emplace(threadId, CommandPool());

	it = commandPools_.find(threadId);
	return it->second;
}
}    // namespace neko::vk