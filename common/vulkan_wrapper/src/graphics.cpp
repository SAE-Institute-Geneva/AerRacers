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

#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

namespace neko::vk
{
VkRenderer::VkRenderer()
        : Renderer(),
          surface_(instance_),
          swapchain_(device_),
          renderPass_(device_),
          descriptorSets_(device_),
          graphicsPipeline_(device_),
          commandPool_(device_),
          framebuffers_(device_),
          vertexBuffer_(device_),
          indexBuffer_(device_),
          descriptorPool_(device_),
          commandBuffers_(device_, commandPool_)
{
    initJob_ = Job([this] {
        instance_.Init(vkWindow_);
        surface_.Init(vkWindow_);
        gpu_.Init(instance_, surface_);
        device_.Init(gpu_);
        swapchain_.Init(gpu_, surface_);
        renderPass_.Init(gpu_, swapchain_);
        descriptorSets_.InitLayout();
        graphicsPipeline_.Init(swapchain_, renderPass_, descriptorSets_);
        commandPool_.Init(gpu_);
        framebuffers_.Init(swapchain_, renderPass_);
        vertexBuffer_.Init(gpu_, commandPool_);
        indexBuffer_.Init(gpu_, commandPool_);

        const size_t swapchainImagesCount = swapchain_.GetImagesCount();
        for (size_t i = 0; i < swapchainImagesCount; ++i)
        {
            uniformBuffers_.emplace_back(device_);
            uniformBuffers_[i].Init(gpu_);
        }

        descriptorPool_.Init(swapchain_);
        descriptorSets_.Init(swapchain_, uniformBuffers_, descriptorPool_);
        commandBuffers_.Init(swapchain_, renderPass_, graphicsPipeline_,
                             framebuffers_, vertexBuffer_, indexBuffer_, descriptorSets_);
        CreateSyncObjects();

        camera_.Init();
    });

    Renderer::AddPreRenderJob(&initJob_);
}

VkRenderer::~VkRenderer()
{
    vkDeviceWaitIdle(VkDevice(device_));

    //ImGui_ImplVulkan_Shutdown();
    //ImGui_ImplSDL2_Shutdown();
    //ImGui::DestroyContext();

    DestroySwapChain();

    /*vkDestroySampler(device_, image_.sampler, nullptr);
    vkDestroyImageView(device_, image_.view, nullptr);

    vkDestroyImage(device_, image_.image, nullptr);
    vkFreeMemory(device_, image_.memory, nullptr);*/

    descriptorSets_.Destroy();

    indexBuffer_.Destroy();
    vertexBuffer_.Destroy();

    for (size_t i = 0; i < kMaxFramesInFlight; i++)
    {
        vkDestroySemaphore(VkDevice(device_), renderFinishedSemaphores_[i], nullptr);
        vkDestroySemaphore(VkDevice(device_), imageAvailableSemaphores_[i], nullptr);
        vkDestroyFence(VkDevice(device_), inFlightFences_[i], nullptr);
    }

    commandPool_.Destroy();
    device_.Destroy();

    surface_.Destroy();
    instance_.Destroy();

    vkWindow_->Destroy();
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
}

void VkRenderer::AfterRenderLoop()
{
    Renderer::AfterRenderLoop();
}

void VkRenderer::RecreateSwapChain()
{
    vkWindow_->MinimizedLoop();

    vkDeviceWaitIdle(VkDevice(device_));

    isFramebufferResized_ = false;

    DestroySwapChain();

    swapchain_.Init(gpu_, surface_);
    renderPass_.Init(gpu_, swapchain_);
    graphicsPipeline_.Init(swapchain_, renderPass_, descriptorSets_);
    //CreateDepthResources();
    framebuffers_.Init(swapchain_, renderPass_);

    for (size_t i = 0; i < swapchain_.GetImagesCount(); ++i)
    {
        uniformBuffers_.emplace_back(device_);
        uniformBuffers_[i].Init(gpu_);
    }

    descriptorPool_.Init(swapchain_);
    descriptorSets_.Init(swapchain_, uniformBuffers_, descriptorPool_);
    commandBuffers_.Init(swapchain_, renderPass_, graphicsPipeline_,
                         framebuffers_, vertexBuffer_, indexBuffer_, descriptorSets_);
}

void VkRenderer::DestroySwapChain()
{
    /*vkDestroyImageView(device_, depthImage_.view, nullptr);
    vkDestroyImage(device_, depthImage_.image, nullptr);
    vkFreeMemory(device_, depthImage_.memory, nullptr);*/

    framebuffers_.Destroy();
    commandBuffers_.Destroy();

    graphicsPipeline_.Destroy();
    renderPass_.Destroy();

    swapchain_.Destroy();

    for (auto& buffer : uniformBuffers_)
        buffer.Destroy();

    descriptorPool_.Destroy();
}

void VkRenderer::UpdateUniformBuffer(uint32_t currentImage)
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    const auto currentTime = std::chrono::high_resolution_clock::now();
    const float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo;
    //ubo.model = Mat4f::Identity;
    ubo.model = Transform3d::Rotate(Mat4f::Identity, time * degree_t(90.0f), Vec3f::forward);
    ubo.view = camera_.GenerateViewMatrix();
    ubo.proj = camera_.GenerateProjectionMatrix();
    ubo.proj[1][1] *= -1;

    void* data = nullptr;
    vkMapMemory(VkDevice(device_), VkDeviceMemory(uniformBuffers_[currentImage]), 0, sizeof(ubo), 0, &data);
    memcpy(data, &ubo, sizeof(ubo));
    vkUnmapMemory(VkDevice(device_), VkDeviceMemory(uniformBuffers_[currentImage]));
}

void VkRenderer::CreateSyncObjects()
{
    imageAvailableSemaphores_.resize(kMaxFramesInFlight);
    renderFinishedSemaphores_.resize(kMaxFramesInFlight);
    inFlightFences_.resize(kMaxFramesInFlight);
    imagesInFlight_.resize(swapchain_.GetImagesCount(), nullptr);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < kMaxFramesInFlight; i++)
    {
        VkResult res = vkCreateSemaphore(VkDevice(device_), &semaphoreInfo, nullptr, &imageAvailableSemaphores_[i]);
        neko_assert(res == VK_SUCCESS, "Failed to create image semaphore for a frame!")
        res = vkCreateSemaphore(VkDevice(device_), &semaphoreInfo, nullptr, &renderFinishedSemaphores_[i]);
        neko_assert(res == VK_SUCCESS, "Failed to create render semaphore for a frame!")
        res = vkCreateFence(VkDevice(device_), &fenceInfo, nullptr, &inFlightFences_[i]);
        neko_assert(res == VK_SUCCESS, "Failed to create in flight fence for a frame!")
    }
}

void VkRenderer::SetWindow(sdl::VulkanWindow* window)
{
    vkWindow_ = window;
}
}