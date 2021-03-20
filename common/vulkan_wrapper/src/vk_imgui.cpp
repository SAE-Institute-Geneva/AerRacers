
#include <vk/vk_imgui.h>

#include "imgui_impl_sdl.h"
#include "imgui_impl_vulkan.h"

#include "vk/vk_resources.h"

namespace neko::vk
{
void VkImGui::Init()
{
	const VkResources* vkObj = VkResources::Inst;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Dimensions
	ImGuiIO& io = ImGui::GetIO();
	(void) io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
#ifdef NN_NINTENDO_SDK
	io.IniFilename = NULL;
#endif

	ImGui::StyleColorsDark();

	VkDescriptorPoolSize poolSizes[] = {
		{VK_DESCRIPTOR_TYPE_SAMPLER, 128},
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 128},
		{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 128},
		{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 128},
		{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 128},
		{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 128},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 128},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 128},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 128},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 128},
		{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 128},
	};

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.maxSets                    = 128 * IM_ARRAYSIZE(poolSizes);
	poolInfo.poolSizeCount              = static_cast<uint32_t>(IM_ARRAYSIZE(poolSizes));
	poolInfo.pPoolSizes                 = poolSizes;
	vkCreateDescriptorPool(VkDevice(vkObj->device), &poolInfo, nullptr, &descriptorPool_);

	ImGui_ImplSDL2_InitForVulkan(vkObj->vkWindow->GetWindow());
	QueueFamilyIndices queueFamilyIndices = VkResources::Inst->gpu.GetQueueFamilyIndices();
	const auto surfaceCapabilities        = vkObj->surface.GetCapabilities();

	// Init Imgui
	ImGui_ImplVulkan_InitInfo initInfo = {};
	initInfo.Instance                  = VkInstance(vkObj->instance);
	initInfo.PhysicalDevice            = VkPhysicalDevice(vkObj->gpu);
	initInfo.Device                    = VkDevice(vkObj->device);
	initInfo.QueueFamily               = queueFamilyIndices.graphicsFamily;
	initInfo.Queue                     = vkObj->device.GetPresentQueue();
	initInfo.PipelineCache             = VK_NULL_HANDLE;
	initInfo.DescriptorPool            = descriptorPool_;
	initInfo.Allocator                 = nullptr;
	initInfo.MinImageCount             = surfaceCapabilities.minImageCount;
	initInfo.ImageCount                = vkObj->swapchain.GetImageCount();
	initInfo.CheckVkResultFn           = nullptr;
	ImGui_ImplVulkan_Init(&initInfo, vkObj->GetRenderPass());

	// Create font texture
	ImFontConfig fontConfig;
	fontConfig.OversampleH = 2;
	fontConfig.OversampleV = 2;
	fontConfig.PixelSnapH  = true;

	const float fontSizeInPixels = 16.0f;
	const std::string path =
		BasicEngine::GetInstance()->GetConfig().dataRootPath + "fonts/droid_sans.ttf";
	io.Fonts->AddFontFromFileTTF(path.c_str(), fontSizeInPixels, &fontConfig);

	// Upload Fonts
	{
		// Use any command queue
		CommandBuffer commandBuffer(true);
		ImGui_ImplVulkan_CreateFontsTexture(commandBuffer);
		commandBuffer.SubmitIdle();

		vkDeviceWaitIdle(vkObj->device);
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	//Start drawing
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame(vkObj->vkWindow->GetWindow());
	ImGui::NewFrame();
}

void VkImGui::Destroy() const
{
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();

	const auto& device = VkDevice(VkResources::Inst);
	vkDestroyDescriptorPool(device, descriptorPool_, nullptr);
}

void VkImGui::Render(const CommandBuffer& commandBuffer)
{
	//End drawing
	ImGui::Render();
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
	ImGui::GetDrawData()->Clear();

	//Start drawing
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame(VkResources::Inst->vkWindow->GetWindow());
	ImGui::NewFrame();
}

void VkImGui::OnWindowResize()
{
	//End drawing
	ImGui::Render();
	ImGui::GetDrawData()->Clear();

	//Start drawing
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame(VkResources::Inst->vkWindow->GetWindow());
	ImGui::NewFrame();
}
}    // namespace neko::vk