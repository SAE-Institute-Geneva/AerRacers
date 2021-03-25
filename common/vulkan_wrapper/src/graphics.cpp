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
 Date :
---------------------------------------------------------- */
#include "vk/graphics.h"

#include "vk/subrenderers/subrenderer_opaque.h"

#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

namespace neko::vk
{
VkRenderer::VkRenderer(sdl::VulkanWindow* window) : Renderer(), VkResources(window)
{
	instance.Init();
	surface.Init(*window->GetWindow());
	gpu.Init();
	surface.SetFormat();
	device.Init();

	commandPools_.emplace(std::this_thread::get_id(), CommandPool());

	CreatePipelineCache();

	swapchain.Init(swapchain);
}

void VkRenderer::ClearScreen()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Clear Screen");
#endif
}

void VkRenderer::BeforeRenderLoop()
{
	const std::uint32_t windowFlags = SDL_GetWindowFlags(vkWindow->GetWindow());
	if (renderer_ == nullptr || windowFlags & SDL_WINDOW_MINIMIZED) return;

	if (!renderer_->HasStarted())
	{
		ResetRenderStages();
		renderer_->Start();
		imgui_.Init();
	}

	const VkResult res = swapchain.AcquireNextImage(
		availableSemaphores_[currentFrame_], inFlightFences_[currentFrame_]);
	if (res == VK_ERROR_OUT_OF_DATE_KHR)
	{
		RecreateSwapChain();
		return;
	}

	if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR) return;

	RenderStage& renderStage = renderer_->GetRenderStage();
	renderStage.Update();

	renderer_->GetRendererContainer().Get<SubrendererOpaque>().GetUniformScene(0).Push(
		kProjHash, Mat4f::Identity);

	if (!StartRenderPass(renderStage)) return;

	Renderer::BeforeRenderLoop();
}

void VkRenderer::AfterRenderLoop()
{
	Renderer::AfterRenderLoop();

	PipelineStage stage;
	RenderStage& renderStage = renderer_->GetRenderStage();
	CommandBuffer& commandBuffer = GetCurrentCmdBuffer();
	for (const auto& subpass : renderStage.GetSubpasses())
	{
		stage.subPassId = subpass.binding;

		// Renders subpass subrender pipelines.
		renderer_->GetRendererContainer().RenderStage(stage, commandBuffer);

		if (subpass.binding != renderStage.GetSubpasses().back().binding)
			vkCmdNextSubpass(commandBuffer, VK_SUBPASS_CONTENTS_INLINE);
	}

	VkImGui::Render(commandBuffer);
	EndRenderPass(renderStage);

	lightCommandBuffer.Clear();
	stage.renderPassId++;
}

bool VkRenderer::StartRenderPass(RenderStage& renderStage)
{
	if (renderStage.IsOutOfDate())
	{
		RecreatePass(renderStage);
		return false;
	}

	CommandBuffer& currentCmdBuffer = GetCurrentCmdBuffer();
	if (!currentCmdBuffer.IsRunning())
	{
		vkWaitForFences(device,
			1,
			&inFlightFences_[currentFrame_],
			VK_TRUE,
			std::numeric_limits<uint64_t>::max());
		currentCmdBuffer.Begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
	}

	VkRect2D renderArea;
	renderArea.offset = {0, 0};
	renderArea.extent = {static_cast<uint32_t>(renderStage.GetSize().x),
		static_cast<uint32_t>(renderStage.GetSize().y)};

	VkRect2D scissor;
	scissor.offset = {0, 0};
	scissor.extent = renderArea.extent;
	vkCmdSetScissor(currentCmdBuffer, 0, 1, &scissor);

	auto clearValues                     = renderStage.GetClearValues();
	VkRenderPassBeginInfo renderPassInfo = {};
	renderPassInfo.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassInfo.renderPass            = renderStage.GetRenderPass();
	renderPassInfo.framebuffer =
		renderStage.GetActiveFramebuffer(swapchain.GetCurrentImageIndex());
	renderPassInfo.renderArea      = renderArea;
	renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
	renderPassInfo.pClearValues    = clearValues.data();
	vkCmdBeginRenderPass(currentCmdBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

	return true;
}

void VkRenderer::EndRenderPass(const RenderStage& renderStage)
{
	VkQueue presentQueue           = device.GetPresentQueue();
	CommandBuffer& currentCmdBuffer = GetCurrentCmdBuffer();
	vkCmdEndRenderPass(currentCmdBuffer);

	if (!renderStage.HasSwapchain()) return;

	currentCmdBuffer.End();
	currentCmdBuffer.Submit(availableSemaphores_[currentFrame_],
		finishedSemaphores_[currentFrame_],
		inFlightFences_[currentFrame_]);

	const VkResult res = swapchain.QueuePresent(presentQueue, finishedSemaphores_[currentFrame_]);
	vkCheckError(res, "Failed to presents swapchain image");

	if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR) isFramebufferResized_ = true;
	currentFrame_ = (currentFrame_ + 1) % swapchain.GetImageCount();
}

void VkRenderer::ResetRenderStages()
{
	RecreateSwapChain();

	if (inFlightFences_.size() != swapchain.GetImageCount()) RecreateCommandBuffers();

	renderer_->GetRenderStage().Rebuild(swapchain);

	RecreateAttachments();
}

void VkRenderer::RecreateSwapChain()
{
	vkWindow->MinimizedLoop();

	vkDeviceWaitIdle(device);

	swapchain.Init(swapchain);

	RecreateCommandBuffers();

	if (ImGui::GetCurrentContext()) VkImGui::OnWindowResize();
}

void VkRenderer::RecreateCommandBuffers()
{
	for (size_t i = 0; i < inFlightFences_.size(); i++)
	{
		vkDestroyFence(device, inFlightFences_[i], nullptr);
		vkDestroySemaphore(device, availableSemaphores_[i], nullptr);
		vkDestroySemaphore(device, finishedSemaphores_[i], nullptr);
	}

	const std::size_t imageCount = swapchain.GetImageCount();
	availableSemaphores_.resize(imageCount);
	finishedSemaphores_.resize(imageCount);
	inFlightFences_.resize(imageCount);
	commandBuffers_.clear();
	commandBuffers_.reserve(imageCount);

	VkSemaphoreCreateInfo semaphoreInfo {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	VkFenceCreateInfo fenceInfo {};
	fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	for (size_t i = 0; i < inFlightFences_.size(); i++)
	{
		VkResult res = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &availableSemaphores_[i]);
		vkCheckError(res, "Could not create semaphore!");

		res = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &finishedSemaphores_[i]);
		vkCheckError(res, "Could not create semaphore!");

		res = vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences_[i]);
		vkCheckError(res, "Could not create fence!");

		commandBuffers_.emplace_back(false);
	}
}

void VkRenderer::RecreatePass(RenderStage& renderStage)
{
	VkQueue graphicQueue           = device.GetGraphicsQueue();
	const Vec2u& size              = BasicEngine::GetInstance()->GetConfig().windowSize;
	const VkExtent2D displayExtent = {static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)};

	vkQueueWaitIdle(graphicQueue);

	if (renderStage.HasSwapchain() &&
		(isFramebufferResized_ || !swapchain.CompareExtent(displayExtent)))
	{
		RecreateSwapChain();
	}

	renderStage.Rebuild(swapchain);
	RecreateAttachments();
}

void VkRenderer::RecreateAttachments()
{
	attachments_.clear();

	const auto& descriptors = renderer_->GetRenderStage().GetDescriptors();
	attachments_.insert(descriptors.begin(), descriptors.end());
}

void VkRenderer::CreatePipelineCache()
{
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo {};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
	vkCreatePipelineCache(device, &pipelineCacheCreateInfo, nullptr, &pipelineCache);
}

void VkRenderer::RenderAll()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("RenderAllCPU");
#endif
	BeforeRenderLoop();
	for (auto* renderCommand : currentCommandBuffer_) renderCommand->Render();
	AfterRenderLoop();
}

void VkRenderer::SetWindow(std::unique_ptr<sdl::VulkanWindow> window)
{
	vkWindow = std::move(window);
}

void VkRenderer::SetRenderer(std::unique_ptr<IRenderer>&& newRenderer)
{
	renderer_ = std::move(newRenderer);
	renderer_->Init();
}

void VkRenderer::Destroy()
{
	Renderer::Destroy();
	DestroyResources();
}
}    // namespace neko::vk
