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
class VkRenderer final : public neko::Renderer, public VkResources
{
public:
    explicit VkRenderer(sdl::VulkanWindow* window);

    void ClearScreen() override;

    void BeforeRenderLoop() override;
    void AfterRenderLoop() override;

    void SetWindow(sdl::VulkanWindow* window);
    void SetRenderer(std::unique_ptr<IRenderer>&& newRenderer);

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
};
}
