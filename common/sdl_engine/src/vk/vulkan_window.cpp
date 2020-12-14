/*
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
 */
#ifdef NEKO_VULKAN

#include "vk/vulkan_window.h"

#include <sstream>
#include <SDL_vulkan.h>

#include "engine/engine.h"
#include "engine/log.h"
#include "vk/graphics.h"

#ifdef EASY_PROFILE_USE
#include <easy/profiler.h>
#endif

namespace neko::sdl
{
void VkOnResizeRenderCommand::Render()
{
	std::ostringstream oss;
	oss << "Resize window with new size: " << newWindowSize_;
	logDebug(oss.str());
	//glViewport(0, 0, newWindowSize_.x, newWindowSize_.y);
}

void VulkanWindow::Init()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("VulkanWindowInit");
#endif
	//const auto& config = BasicEngine::GetInstance()->config;

	SdlWindow::Init();
	
#ifndef __EMSCRIPTEN__
	const std::string videoDriver = SDL_GetCurrentVideoDriver();
	logDebug(videoDriver);
#endif
	
	//TODO VSync
#ifndef __EMSCRIPTEN__
	/*SDL_GL_SetSwapInterval(config.vSync);

	if (!gladLoadGLES2Loader(static_cast<GLADloadproc>(SDL_GL_GetProcAddress)))
	{
		logDebug("Failed to initialize OpenGL context\n");
		assert(false);
	}*/
#else
	//SDL_GL_SetSwapInterval(false);
#endif
	
    //vkCheckError();

	//TODO ImGui implementation
	//InitImGui();
    //vkCheckError();
}

void VulkanWindow::InitImGui()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ImGuiInit");
#endif
	SdlWindow::InitImGui();
	ImGui_ImplSDL2_InitForVulkan(window_);

    //const auto& config = BasicEngine::GetInstance()->config;
    const auto& vkObj = vk::VkObjectsLocator::get();
    const auto& graphicsQueue = vkObj.device.GetGraphicsQueue();
    const auto& queueFamilyIndices = vkObj.gpu.GetQueueFamilyIndices();

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
#ifdef NN_NINTENDO_SDK
    io.IniFilename = NULL;
#endif

    ImGui::StyleColorsDark();

    VkDescriptorPoolSize poolSizes[] =
    {
        { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
        { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
        { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
    };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1000 * IM_ARRAYSIZE(poolSizes);
    poolInfo.poolSizeCount =  static_cast<uint32_t>(IM_ARRAYSIZE(poolSizes));
    poolInfo.pPoolSizes = poolSizes;

    VkResult res = vkCreateDescriptorPool(VkDevice(vkObj.device), &poolInfo, nullptr, &imguiDescriptorPool_);
    neko_assert(res == VK_SUCCESS, "Could not create ImGui's descriptor pool!")

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.Instance = VkInstance(vkObj.instance);
    initInfo.PhysicalDevice = VkPhysicalDevice(vkObj.gpu);
    initInfo.Device = VkDevice(vkObj.device);
    initInfo.QueueFamily = queueFamilyIndices.graphicsFamily;
    initInfo.Queue = VkQueue(graphicsQueue);
    initInfo.PipelineCache = {};
    initInfo.DescriptorPool = imguiDescriptorPool_;
    initInfo.Allocator = nullptr;
    initInfo.MinImageCount = 2;
    initInfo.ImageCount = vkObj.swapchain->GetImageCount();

	//ImGui_ImplVulkan_Init(&initInfo, VkRenderPass(vkObj.renderPass));

    // Create font texture
    unsigned char* fontData;
    int texWidth, texHeight;
    io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);

    // Upload Fonts
    {
        //ImGui_ImplVulkan_CreateFontsTexture(VkCommandBuffer(vkObj.modelCommandBuffer_[0]));

        vkDeviceWaitIdle(VkDevice(vkObj.device));
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }
}

void VulkanWindow::GenerateUiFrame()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ImGuiGenerate");
#endif
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL2_NewFrame(window_);
    ImGui::NewFrame();
}


void VulkanWindow::SwapBuffer()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("SwapBuffer");
#endif
	//SDL_GL_SwapWindow(window_);
}

void VulkanWindow::Destroy()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("DestroyWindow");
#endif
    /*const auto& vkObj = vk::VkResourcesLocator::get();
    vkDestroyDescriptorPool(VkDevice(vkObj.device), imguiDescriptorPool_, nullptr);

    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();*/

	SdlWindow::Destroy();
}

void VulkanWindow::RenderUi()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("ImGuiRender");
#endif
	/*ImGui::Render();
    ImGui::GetDrawData()->Clear();*/
}

void VulkanWindow::OnResize(const Vec2u newWindowSize)
{
	onResizeCommand_.SetWindowSize(newWindowSize);
	RendererLocator::get().Render(&onResizeCommand_);
}

void VulkanWindow::MinimizedLoop() const
{
    while (SDL_GetWindowFlags(window_) & SDL_WINDOW_MINIMIZED)
        SDL_PumpEvents();
}
}

#endif
