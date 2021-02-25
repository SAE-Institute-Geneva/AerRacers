#include "imgui_impl_sdl.h"
#include "imgui_impl_vulkan.h"

#include "vk/vk_resources.h"

namespace neko::vk
{
VkImGui::VkImGui()
{
	BasicEngine::GetInstance()->RegisterOnDrawUi(*this);
	static_cast<sdl::SdlEngine*>(BasicEngine::GetInstance())->RegisterOnEvent(*this);

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
		{VK_DESCRIPTOR_TYPE_SAMPLER, 1000},
		{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000},
		{VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000},
		{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000},
		{VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000},
		{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000},
	};

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	poolInfo.maxSets                    = 1000 * IM_ARRAYSIZE(poolSizes);
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
	initInfo.ImageCount                = vkObj->swapchain->GetImageCount();
	initInfo.CheckVkResultFn           = nullptr;
	ImGui_ImplVulkan_Init(&initInfo, vkObj->GetRenderPass());

	// Create font texture
	unsigned char* fontData;
	int texWidth, texHeight;

	ImFontConfig fontConfig;
	fontConfig.OversampleH = 2;
	fontConfig.OversampleV = 2;
	fontConfig.PixelSnapH  = true;
	io.Fonts->GetTexDataAsRGBA32(&fontData, &texWidth, &texHeight);

	// Upload Fonts
	{
		// Use any command queue
		CommandBuffer commandBuffer(true);
		ImGui_ImplVulkan_CreateFontsTexture(VkCommandBuffer(commandBuffer));
		commandBuffer.SubmitIdle();

		vkDeviceWaitIdle(VkDevice(vkObj->device));
		ImGui_ImplVulkan_DestroyFontUploadObjects();
	}

	//Start drawing
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplSDL2_NewFrame(vkObj->vkWindow->GetWindow());
	ImGui::NewFrame();
}

VkImGui::~VkImGui() { Destroy(); }

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
	hasBeenDrawn_ = true;

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

void VkImGui::OnEndOfFrame()
{
	if (!hasBeenDrawn_)
	{
		//End drawing
		ImGui::Render();
		ImGui::GetDrawData()->Clear();

		//Start drawing
		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplSDL2_NewFrame(VkResources::Inst->vkWindow->GetWindow());
		ImGui::NewFrame();
	}

	hasBeenDrawn_ = false;
}

void VkImGui::DrawImGui() {}

void VkImGui::OnEvent(const SDL_Event& event) { ImGui_ImplSDL2_ProcessEvent(&event); }
}    // namespace neko::vk