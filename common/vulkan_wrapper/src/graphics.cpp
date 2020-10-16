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

#include "graphics/texture.h"
#include "vk/shapes/quad.h"

#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

namespace neko::vk
{
VkRenderer::VkRenderer() : Renderer()
{
    initJob_ = Job([this] {
        instance.Init();
        surface.Init();
        gpu.Init();
        device.Init();
        swapchain.Init();
        renderPass.Init();
        commandPool.Init();
        framebuffers.Init();
        vertexBuffer_.Init(RenderQuad::kVertices, IM_ARRAYSIZE(RenderQuad::kVertices));
        indexBuffer_.Init(RenderQuad::kIndices, IM_ARRAYSIZE(RenderQuad::kIndices));

        descriptorPool.Init();
        shader_.LoadFromFile("../../data/shaders/aer_racer/01_triangle/quad.vert.spv",
                             "../../data/shaders/aer_racer/01_triangle/quad.frag.spv");
        shader_.InitUbo(sizeof(UniformBufferObject));

        Shader shaders[] = { shader_ };
        commandBuffers.Init(vertexBuffer_, indexBuffer_, shaders);

        CreateSyncObjects();

        camera_.Init();
    });

    Renderer::AddPreRenderJob(&initJob_);
    VkResourcesLocator::provide(this);
}

VkRenderer::~VkRenderer()
{
    vkDeviceWaitIdle(VkDevice(device));

    //ImGui_ImplVulkan_Shutdown();
    //ImGui_ImplSDL2_Shutdown();
    //ImGui::DestroyContext();

    DestroySwapChain();

    /*vkDestroySampler(device_, image_.sampler, nullptr);
    vkDestroyImageView(device_, image_.view, nullptr);

    vkDestroyImage(device_, image_.image, nullptr);
    vkFreeMemory(device_, image_.memory, nullptr);*/

    indexBuffer_.Destroy();
    vertexBuffer_.Destroy();

    for (size_t i = 0; i < kMaxFramesInFlight; i++)
    {
        vkDestroySemaphore(VkDevice(device), renderFinishedSemaphores_[i], nullptr);
        vkDestroySemaphore(VkDevice(device), imageAvailableSemaphores_[i], nullptr);
        vkDestroyFence(VkDevice(device), inFlightFences_[i], nullptr);
    }

    commandPool.Destroy();
    device.Destroy();

    surface.Destroy();
    instance.Destroy();
}

void VkRenderer::ClearScreen()
{
#ifdef EASY_PROFILE_USE
    EASY_BLOCK("Clear Screen");
#endif
    /*glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);*/
}

void VkRenderer::BeforeRenderLoop()
{
    Renderer::BeforeRenderLoop();

    const auto& config = BasicEngine::GetInstance()->config;
    camera_.Update(seconds (1.0f / 60.0f));
    camera_.SetAspect(config.windowSize.x, config.windowSize.y);

    vkWaitForFences(VkDevice(device), 1, &inFlightFences_[currentFrame_], VK_TRUE, UINT64_MAX);

    const VkResult res = vkAcquireNextImageKHR(VkDevice(device), VkSwapchainKHR(swapchain), UINT64_MAX,
                                         imageAvailableSemaphores_[currentFrame_], nullptr, &imageIndex_);
    if (res == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return;
    }

    if (res != VK_SUCCESS && res != VK_SUBOPTIMAL_KHR)
        neko_assert(false, "Failed to acquire swap chain image!")

    // Check if a previous frame is using this image (i.e. there is its fence to wait on)
    if (imagesInFlight_[imageIndex_] != nullptr)
        vkWaitForFences(VkDevice(device), 1, &imagesInFlight_[imageIndex_], VK_TRUE, UINT64_MAX);

    // Mark the image as now being in use by this frame
    imagesInFlight_[imageIndex_] = inFlightFences_[currentFrame_];

    {
        static auto startTime = std::chrono::high_resolution_clock::now();
        const auto currentTime = std::chrono::high_resolution_clock::now();
        const float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

        UniformBufferObject ubo;
        ubo.model = Mat4f::Identity;
        //ubo.model = Transform3d::Rotate(Mat4f::Identity, time * degree_t(90.0f), Vec3f::forward);
        ubo.view = camera_.GenerateViewMatrix();
        ubo.proj = camera_.GenerateProjectionMatrix();
        ubo.proj[1][1] *= -1;
        shader_.UpdateUniformBuffer(imageIndex_, ubo);
    }
}

void VkRenderer::AfterRenderLoop()
{
    Renderer::AfterRenderLoop();

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = {imageAvailableSemaphores_[currentFrame_]};
    VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffers[imageIndex_];

    VkSemaphore signalSemaphores[] = {renderFinishedSemaphores_[currentFrame_]};
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(VkDevice(device), 1, &inFlightFences_[currentFrame_]);
    VkResult res = vkQueueSubmit(device.GetGraphicsQueue(), 1, &submitInfo, inFlightFences_[currentFrame_]);
    neko_assert(res == VK_SUCCESS, "Failed to submit draw command buffer!")

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapChains[] = {VkSwapchainKHR(swapchain)};
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex_;
    presentInfo.pResults = nullptr; // Optional

    res = vkQueuePresentKHR(device.GetPresentQueue(), &presentInfo);
    if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR || isFramebufferResized_)
        RecreateSwapChain();
    else if (res != VK_SUCCESS)
        neko_assert(false, "Failed to present swap chain image!")

    currentFrame_ = (currentFrame_ + 1) % kMaxFramesInFlight;
}

void VkRenderer::RecreateSwapChain()
{
    vkWindow->MinimizedLoop();

    vkDeviceWaitIdle(VkDevice(device));

    isFramebufferResized_ = false;

    DestroySwapChain();

    swapchain.Init();
    renderPass.Init();
    //CreateDepthResources();
    framebuffers.Init();

    descriptorPool.Init();
    shader_.Recreate();

    Shader shaders[] = { shader_ };
    commandBuffers.Init(vertexBuffer_, indexBuffer_, shaders);
}

void VkRenderer::DestroySwapChain()
{
    /*vkDestroyImageView(device_, depthImage_.view, nullptr);
    vkDestroyImage(device_, depthImage_.image, nullptr);
    vkFreeMemory(device_, depthImage_.memory, nullptr);*/

    framebuffers.Destroy();
    commandBuffers.Destroy();

    renderPass.Destroy();

    swapchain.Destroy();

    descriptorPool.Destroy();
    shader_.Destroy();
}

void VkRenderer::CreateSyncObjects()
{
    imageAvailableSemaphores_.resize(kMaxFramesInFlight);
    renderFinishedSemaphores_.resize(kMaxFramesInFlight);
    inFlightFences_.resize(kMaxFramesInFlight);
    imagesInFlight_.resize(swapchain.GetImageCount(), nullptr);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < kMaxFramesInFlight; i++)
    {
        VkResult res = vkCreateSemaphore(VkDevice(device), &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]);
        neko_assert(res == VK_SUCCESS, "Failed to create image semaphore for a frame!")
        res = vkCreateSemaphore(VkDevice(device), &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]);
        neko_assert(res == VK_SUCCESS, "Failed to create render semaphore for a frame!")
        res = vkCreateFence(VkDevice(device), &fenceInfo, nullptr, &inFlightFences_[i]);
        neko_assert(res == VK_SUCCESS, "Failed to create in flight fence for a frame!")
    }
}

void VkRenderer::RenderAll()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("RenderAllCPU");
#endif

	BeforeRenderLoop();
	for (auto* renderCommand : currentCommandBuffer_)
	{
		renderCommand->Render();
	}
	AfterRenderLoop();
}

void VkRenderer::SetWindow(sdl::VulkanWindow* window)
{
    vkWindow = window;
}
}