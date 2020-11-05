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

#include "mathematics/hash.h"

#include "vk/models/quad.h"
#include "vk/renderers/renderer_editor.h"
#include "vk/subrenderers/subrenderer_opaque.h"

#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

namespace neko::vk
{
VkRenderer::VkRenderer(sdl::VulkanWindow* window) : Renderer(), IVkObjects(window)
{
    VkObjectsLocator::provide(this);

    instance.Init(window->GetWindow());
    surface.Init(window->GetWindow());
    gpu.Init();
    surface.SetFormat();
    device.Init();

    testShader_ = Shader("../../data/shaders/aer_racer/01_triangle/quad",
                         VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
    testShader_.AddAttribute(HashString("inPosition"),
            Attribute(63, 0, sizeof(Vec3f), Attribute::AttributeType::VEC3F));
    testShader_.AddAttribute(HashString("inNormal"),
            Attribute(63, 1, sizeof(Vec3f), Attribute::AttributeType::VEC3F));
    testShader_.AddAttribute(HashString("inTexCoords"),
            Attribute(63, 2, sizeof(Vec2f), Attribute::AttributeType::VEC2F));
    /*testShader_.AddAttribute(HashString("inModelMatrix"),
            Attribute(63, 5, sizeof(Mat4f), Attribute::AttributeType::VEC4F));*/

    UniformBlock ubo(0, sizeof(Mat4f) * 2, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
    ubo.AddUniform(HashString("view"),
            Uniform(0, 0, sizeof(Mat4f), VK_FORMAT_R32G32B32A32_SFLOAT, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT));
    ubo.AddUniform(HashString("proj"),
            Uniform(0, sizeof(Mat4f), sizeof(Mat4f), VK_FORMAT_R32G32B32A32_SFLOAT, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT));

    testShader_.AddUniformBlock(HashString("UboScene"), ubo);

    commandPools = std::make_unique<CommandPool>();
    commandPools->Init();

    CreatePipelineCache();

    Renderer::AddPreRenderJob(&initJob_);
}

VkRenderer::~VkRenderer()
{
    vkDeviceWaitIdle(VkDevice(device));

    //ImGui_ImplVulkan_Shutdown();
    //ImGui_ImplSDL2_Shutdown();
    //ImGui::DestroyContext();

    const auto& graphicsQueue = device.GetGraphicsQueue();
    vkQueueWaitIdle(graphicsQueue);
    vkDestroyPipelineCache(VkDevice(device), pipelineCache, nullptr);

    for (size_t i = 0; i < inFlightFences_.size(); i++)
    {
        vkDestroyFence(VkDevice(device), inFlightFences_[i], nullptr);
        vkDestroySemaphore(VkDevice(device), imageAvailableSemaphores_[i], nullptr);
        vkDestroySemaphore(VkDevice(device), renderFinishedSemaphores_[i], nullptr);
    }

    renderer->Destroy();

    for (auto& commandBuffer : commandBuffers)
    {
        commandBuffer->Destroy();
    }
    commandPools->Destroy();

    graphicsPipeline.Destroy();

    swapchain->Destroy();

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
}

void VkRenderer::AfterRenderLoop()
{
    Renderer::AfterRenderLoop();

    const uint32_t windowFlags = SDL_GetWindowFlags(&vkWindow->GetWindow());
    if (renderer == nullptr || windowFlags & SDL_WINDOW_MINIMIZED) return;

    if (!renderer->HasStarted())
    {
        ResetRenderStages();
        renderer->Start();

        graphicsPipeline.Init(
                Pipeline::Stage(0, 0),
                testShader_,
                {Vertex::GetVertexInput()},
                GraphicsPipeline::Mode::POLYGON,
                GraphicsPipeline::Depth::READ_WRITE,
                VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
                VK_POLYGON_MODE_FILL,
                VK_CULL_MODE_BACK_BIT,
                VK_FRONT_FACE_CLOCKWISE,
                false);

        renderer->GetRendererContainer().Get<SubrendererOpaque>().SetUniformBlock(testShader_.GetUniformBlock(HashString("UboScene")));
    }

    const auto acquireResult = swapchain->AcquireNextImage(
            imageAvailableSemaphores_[currentFrame_],
            inFlightFences_[currentFrame_]);

    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapChain();
        return;
    }

    if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) return;

    Pipeline::Stage stage;
    for (auto& renderStage : renderer->GetRenderStages())
    {
        renderStage->Update();
        if (!StartRenderPass(*renderStage)) return;

        auto &commandBuffer = commandBuffers[swapchain->GetCurrentImageIndex()];
        for (const auto &subpass : renderStage->GetSubpasses())
        {
            stage.subPassId = subpass.binding;

            // Renders subpass subrender pipelines.
            renderer->GetRendererContainer().RenderStage(stage, *commandBuffer);

            if (subpass.binding != renderStage->GetSubpasses().back().binding)
                vkCmdNextSubpass(VkCommandBuffer(*commandBuffer), VK_SUBPASS_CONTENTS_INLINE);
        }

        EndRenderPass(*renderStage);
        stage.renderPassId++;
    }
}

bool VkRenderer::StartRenderPass(RenderStage& renderStage)
{
    if (renderStage.IsOutOfDate())
    {
        RecreatePass(renderStage);
        return false;
    }

    if (!commandBuffers[swapchain->GetCurrentImageIndex()]->IsRunning())
    {
        vkWaitForFences(VkDevice(device), 1, &inFlightFences_[currentFrame_],
                        VK_TRUE, std::numeric_limits<uint64_t>::max());
        commandBuffers[swapchain->GetCurrentImageIndex()]->
                Begin(VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT);
    }

    VkRect2D renderArea;
    renderArea.offset = {0, 0};
    renderArea.extent = {
            static_cast<uint32_t>(renderStage.GetSize().x),
            static_cast<uint32_t>(renderStage.GetSize().y)
    };

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(renderArea.extent.width);
    viewport.height = static_cast<float>(renderArea.extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(VkCommandBuffer(*commandBuffers[swapchain->GetCurrentImageIndex()]),
            0, 1, &viewport);

    VkRect2D scissor;
    scissor.offset = {0, 0};
    scissor.extent = renderArea.extent;
    vkCmdSetScissor(VkCommandBuffer(*commandBuffers[swapchain->GetCurrentImageIndex()]),
            0, 1, &scissor);

    auto clearValues = renderStage.GetClearValues();

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderPass = VkRenderPass(*renderStage.GetRenderPass());
    renderPassBeginInfo.framebuffer = renderStage.
            GetActiveFramebuffer(swapchain->GetCurrentImageIndex());
    renderPassBeginInfo.renderArea = renderArea;
    renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassBeginInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(VkCommandBuffer(*commandBuffers[swapchain->GetCurrentImageIndex()]),
            &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    return true;
}

void VkRenderer::EndRenderPass(const RenderStage& renderStage)
{
    const auto presentQueue = device.GetPresentQueue();

    vkCmdEndRenderPass(VkCommandBuffer(*commandBuffers[swapchain->GetCurrentImageIndex()]));

    if (!renderStage.HasSwapchain()) return;

    commandBuffers[swapchain->GetCurrentImageIndex()]->End();
    commandBuffers[swapchain->GetCurrentImageIndex()]->Submit(
            imageAvailableSemaphores_[currentFrame_],
            renderFinishedSemaphores_[currentFrame_],
            inFlightFences_[currentFrame_]);

    const auto res = swapchain->QueuePresent(presentQueue,
            renderFinishedSemaphores_[currentFrame_]);

    if (res == VK_ERROR_OUT_OF_DATE_KHR || res == VK_SUBOPTIMAL_KHR)
        isFramebufferResized_ = true;
    else if(res != VK_SUCCESS)
        neko_assert(false, "Failed to presents swapchain image")

    currentFrame_ = (currentFrame_ + 1) % swapchain->GetImageCount();
}

void VkRenderer::ResetRenderStages()
{
    RecreateSwapChain();

    if (inFlightFences_.size() != swapchain->GetImageCount())
        RecreateCommandBuffers();

    for (auto& renderStage : renderer->GetRenderStages())
        renderStage->Rebuild(*swapchain);

    RecreateAttachments();
}

void VkRenderer::RecreateSwapChain()
{
    vkWindow->MinimizedLoop();

    vkDeviceWaitIdle(VkDevice(device));

    if (!swapchain) swapchain = std::make_unique<Swapchain>();
    swapchain->Init(*swapchain);

    RecreateCommandBuffers();
}

void VkRenderer::RecreateCommandBuffers()
{
    for (size_t i = 0; i < inFlightFences_.size(); i++)
    {
        vkDestroyFence(VkDevice(device), inFlightFences_[i], nullptr);
        vkDestroySemaphore(VkDevice(device), imageAvailableSemaphores_[i], nullptr);
        vkDestroySemaphore(VkDevice(device), renderFinishedSemaphores_[i], nullptr);
    }

    imageAvailableSemaphores_.resize(swapchain->GetImageCount());
    renderFinishedSemaphores_.resize(swapchain->GetImageCount());
    inFlightFences_.resize(swapchain->GetImageCount());
    commandBuffers.clear();
    commandBuffers.resize(swapchain->GetImageCount());

    VkSemaphoreCreateInfo semaphoreCreateInfo{};
    semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < inFlightFences_.size(); i++)
    {
        VkResult res = vkCreateSemaphore(VkDevice(device), &semaphoreCreateInfo,
                                         nullptr, &imageAvailableSemaphores_[i]);
        neko_assert(res == VK_SUCCESS, "Could not create semaphore!")

        res = vkCreateSemaphore(VkDevice(device), &semaphoreCreateInfo,
                        nullptr, &renderFinishedSemaphores_[i]);
        neko_assert(res == VK_SUCCESS, "Could not create semaphore!")

        res = vkCreateFence(VkDevice(device), &fenceCreateInfo,
                        nullptr, &inFlightFences_[i]);
        neko_assert(res == VK_SUCCESS, "Could not create fence!")

        if (commandBuffers[i]) commandBuffers[i]->Destroy();
        else commandBuffers[i] = std::make_unique<CommandBuffer>();
        commandBuffers[i]->Init(false);
    }
}

void VkRenderer::RecreatePass(RenderStage& renderStage)
{
    const auto& graphicQueue = device.GetGraphicsQueue();

    const Vec2u& size = BasicEngine::GetInstance()->config.windowSize;
    const VkExtent2D displayExtent = {
            static_cast<uint32_t>(size.x),
            static_cast<uint32_t>(size.y)
    };

    vkQueueWaitIdle(graphicQueue);

    if (renderStage.HasSwapchain() && (isFramebufferResized_ || !swapchain->CompareExtent(displayExtent)))
    {
        RecreateSwapChain();
    }

    renderStage.Rebuild(*swapchain);
    RecreateAttachments();
}

void VkRenderer::RecreateAttachments()
{
    attachments_.clear();

    for (const auto& renderStage : renderer->GetRenderStages())
    {
        const auto& descriptors = renderStage->GetDescriptors();
        attachments_.insert(descriptors.begin(), descriptors.end());
    }
}

void VkRenderer::CreatePipelineCache()
{
    VkPipelineCacheCreateInfo pipelineCacheCreateInfo{};
    pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    vkCreatePipelineCache(VkDevice(device), &pipelineCacheCreateInfo, nullptr, &pipelineCache);
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

void VkRenderer::SetRenderer(std::unique_ptr<vk::Renderer>&& renderer)
{
    this->renderer = std::move(renderer);
    this->renderer->Init();
}

RenderStage& VkRenderer::GetRenderStage(uint32_t index) const
{
    return renderer->GetRenderStage(index);
}
}