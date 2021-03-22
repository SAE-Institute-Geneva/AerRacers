#include "vk/vk_resources.h"

namespace neko::vk
{
VkResources::VkResources(sdl::VulkanWindow* window) : vkWindow(window) { Inst = this; }

VkResources::~VkResources()
{
}
void VkResources::DestroyResources()
{
	vkDeviceWaitIdle(VkDevice(device));

	const auto& graphicsQueue = device.GetGraphicsQueue();
	vkQueueWaitIdle(graphicsQueue);
	vkDestroyPipelineCache(VkDevice(device), pipelineCache, nullptr);

	for (size_t i = 0; i < inFlightFences_.size(); i++)
	{
		vkDestroyFence(VkDevice(device), inFlightFences_[i], nullptr);
		vkDestroySemaphore(VkDevice(device), availableSemaphores_[i], nullptr);
		vkDestroySemaphore(VkDevice(device), finishedSemaphores_[i], nullptr);
	}

	for (auto& commandBuffer : commandBuffers_)
		commandBuffer.Destroy();

	for (const auto& commandPool : commandPools_)
		commandPool.second.Destroy();

	renderer_->Destroy();


	swapchain.Destroy();

	imgui_.Destroy();
	device.Destroy();
	surface.Destroy();
	instance.Destroy();
	commandBuffers_.clear();
	commandPools_.clear();
	inFlightFences_.clear();
	availableSemaphores_.clear();
	finishedSemaphores_.clear();
	attachments_.clear();
	//renderer_.release();
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