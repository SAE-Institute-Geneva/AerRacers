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

#include <thread>

#include "graphics/graphics.h"
#include "graphics/camera.h"
#include "vk/buffers/index_buffer.h"
#include "vk/buffers/uniform_buffer.h"
#include "vk/buffers/vertex_buffer.h"
#include "vk/commands/command_buffers.h"
#include "vk/commands/command_pool.h"
#include "vk/core/instance.h"
#include "vk/core/logical_device.h"
#include "vk/core/physical_device.h"
#include "vk/core/surface.h"
#include "vk/descriptors/descriptor_set.h"
#include "vk/descriptors/descriptor_pool.h"
#include "vk/framebuffers/framebuffers.h"
#include "vk/framebuffers/renderpass.h"
#include "vk/framebuffers/swapchain.h"
#include "vk/pipelines/graphics_pipeline.h"

namespace neko::vk
{
const int kMaxFramesInFlight = 2;

class VkRenderer : public Renderer
{
public:
    VkRenderer();
    ~VkRenderer() override;

    void ClearScreen() override;



    void BeforeRenderLoop() override;
    void AfterRenderLoop() override;

    void SetWindow(sdl::VulkanWindow* window);

private:
    void RecreateSwapChain();
    void DestroySwapChain();

    void UpdateUniformBuffer(uint32_t currentImage);

    void CreateSyncObjects();

    Job initJob_;

    sdl::VulkanWindow* vkWindow_ = nullptr;

    Instance instance_;
    Surface surface_;
    PhysicalDevice gpu_;
    LogicalDevice device_;

    Swapchain swapchain_;

    RenderPass renderPass_;
    DescriptorSets descriptorSets_;
    GraphicsPipeline graphicsPipeline_;

    CommandPool commandPool_;
    Framebuffers framebuffers_;

    VertexBuffer vertexBuffer_;
    IndexBuffer indexBuffer_;
    std::vector<UniformBuffer> uniformBuffers_{};

    DescriptorPool descriptorPool_;
    CommandBuffers commandBuffers_;

    MovableCamera3D camera_;

    size_t currentFrame_ = 0;
    std::vector<VkFence> inFlightFences_{};
    std::vector<VkFence> imagesInFlight_{};
    std::vector<VkSemaphore> imageAvailableSemaphores_{};
    std::vector<VkSemaphore> renderFinishedSemaphores_{};

    bool isFramebufferResized_ = false;
};
}