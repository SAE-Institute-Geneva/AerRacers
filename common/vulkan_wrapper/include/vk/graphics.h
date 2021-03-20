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

#include "vk/vk_resources.h"

namespace neko::vk
{
/// Handles all the rendering from Vulkan
class VkRenderer final : public neko::Renderer, public VkResources
{
public:
	/// Inits the Vulkan Objects necessary
    explicit VkRenderer(sdl::VulkanWindow* window);

	/// Currently unused
    void ClearScreen() override;

	/// Executes commands before the render loop starts
    void BeforeRenderLoop() override;

	/// Executes commands after the render loop has ended
    void AfterRenderLoop() override;

	/// Sets the window used by the renderer
    void SetWindow(std::unique_ptr<sdl::VulkanWindow> window);

	/// Sets the renderer to use
    void SetRenderer(std::unique_ptr<IRenderer>&& newRenderer);

private:
	/// Recreates the render pass if needed and starts it
	bool StartRenderPass(RenderStage& renderStage);

	/// Ends the render pass and submits the render data to the queue
    void EndRenderPass(const RenderStage& renderStage);

	/// Rebuilds the swapchain, command buffers (if needed), the render stage and the image attachments
    void ResetRenderStages();

	/// Recreates the swapchain (e.g.: when the screen changes size)
    void RecreateSwapChain();

	/// Recreates the command buffers (e.g.: when the number of images in the swapchain changes)
    void RecreateCommandBuffers();

	/// Recreate the render pass (e.g.: when it is out of date)
    void RecreatePass(RenderStage& renderStage);

	/// Recreate the image attachments
    void RecreateAttachments();

	/// Creates the pipeline cache
    void CreatePipelineCache();

	/// Starts the rendering
	void RenderAll() override;
};
}
