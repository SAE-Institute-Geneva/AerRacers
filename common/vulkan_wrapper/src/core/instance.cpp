#include <iostream>
#include <sstream>

#include <SDL_vulkan.h>

#include "vk/core/instance.h"
#include "vk/graphics.h"

namespace neko::vk
{
VkResult Instance::CreateDebugUtilsMessengerExt(
        const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
        const VkAllocationCallbacks* pAllocator)
{
    const auto func = PFN_vkCreateDebugUtilsMessengerEXT(
            vkGetInstanceProcAddr(instance_, "vkCreateDebugUtilsMessengerEXT"));
    if (func != nullptr)
    {
        return func(instance_, pCreateInfo, pAllocator, &debugMessenger_);
    }

    return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void Instance::Init(SDL_Window& window)
{
#ifdef VALIDATION_LAYERS
    if (!CheckValidationLayerSupport())
        neko_assert(false, "Validation layers requested, but not available!")
#endif
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan Application";
    appInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    appInfo.pEngineName = "Neko Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_0;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;

    // Get the instance extensions and check if they are supported
    std::vector<const char*> extensions = GetRequiredInstanceExtensions(window);
    neko_assert(CheckInstanceExtensionsSupport(extensions), "Required instance extensions not available!")
    createInfo.enabledExtensionCount = static_cast<std::uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef VALIDATION_LAYERS
    createInfo.enabledLayerCount = static_cast<std::uint32_t>(kValidationLayers.size());
    createInfo.ppEnabledLayerNames = kValidationLayers.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
    PopulateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = static_cast<VkDebugUtilsMessengerCreateInfoEXT*>(&debugCreateInfo);
#else
    createInfo.enabledLayerCount = 0;
    createInfo.pNext = nullptr;
#endif

    //Create Vulkan Instance
    logDebug("Initializing Vulkan instance");
    const VkResult res = vkCreateInstance(&createInfo, nullptr, &instance_);
    switch (res)
    {
        case VK_SUCCESS:
            logDebug("Successfully created Vulkan instance\n");
            break;
        case VK_ERROR_INCOMPATIBLE_DRIVER:
            neko_assert(false, "Unable to create vulkan instance, cannot find a compatible Vulkan ICD")
        default:
            neko_assert(false, "Unable to create Vulkan instance: unknown error")
    }

    SetupDebugMessenger();
}

void Instance::Destroy() const
{
#ifdef VALIDATION_LAYERS
    DestroyDebugUtilsMessengerExt(nullptr);
#endif

    vkDestroyInstance(instance_, nullptr);
}

void Instance::SetupDebugMessenger()
{
#ifndef VALIDATION_LAYERS
    return;
#endif

    VkDebugUtilsMessengerCreateInfoEXT createInfo{};
    PopulateDebugMessengerCreateInfo(createInfo);

    neko_assert(CreateDebugUtilsMessengerExt(&createInfo, nullptr) == VK_SUCCESS,
                "Failed to set up debug messenger!")
}

void Instance::DestroyDebugUtilsMessengerExt(const VkAllocationCallbacks* pAllocator) const
{
    const auto func = PFN_vkDestroyDebugUtilsMessengerEXT(
            vkGetInstanceProcAddr(instance_, "vkDestroyDebugUtilsMessengerEXT"));
    if (func != nullptr)
    {
        func(instance_, debugMessenger_, pAllocator);
    }
}

std::vector<const char*> Instance::GetRequiredInstanceExtensions(SDL_Window& window)
{
    std::uint32_t sdlExtCount = 0;
    neko_assert(SDL_Vulkan_GetInstanceExtensions(&window, &sdlExtCount, nullptr),
                "Unable to query the number of Vulkan instance extensions!")

    // Use the amount of extensions queried before to retrieve the names of the extensions
    std::vector<const char*> sdlExtensions(sdlExtCount);
    neko_assert(SDL_Vulkan_GetInstanceExtensions(&window, &sdlExtCount, sdlExtensions.data()),
                "Unable to query the number of Vulkan instance extension names!")

#ifdef VALIDATION_LAYERS
    sdlExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return sdlExtensions;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	const VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity, 
    VkDebugUtilsMessageTypeFlagsEXT msgType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData) 
{
	if (msgSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
	{
		std::ostringstream oss;
		oss << "Validation layer: " << pCallbackData->pMessage << '\n';
		logDebug(oss.str());
	}
	return VK_FALSE;
}

void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
	createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT|
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugCallback;
}

bool CheckInstanceExtensionsSupport(const std::vector<const char*>& extensions)
{
    std::uint32_t instExtCount = 0;
    VkResult res = vkEnumerateInstanceExtensionProperties(nullptr, &instExtCount, nullptr);
    neko_assert(res == VK_SUCCESS, "Unable to query vulkan instance extension count")

    std::vector<VkExtensionProperties> instExtNames(instExtCount);
    res = vkEnumerateInstanceExtensionProperties(nullptr, &instExtCount, instExtNames.data());
    neko_assert(res == VK_SUCCESS, "Unable to retrieve vulkan instance extension names")

#ifdef VALIDATION_LAYERS
    // Display layer names and find the ones we specified above
    std::cout << "Found " << instExtCount << " instance extensions:\n";
    std::uint32_t count(0);
    for (const auto& instExtName : instExtNames)
    {
        std::cout << count << ": " << instExtName.extensionName << '\n';
        count++;
    }
#endif

    for (const auto& extName : extensions)
    {
        bool extFound = false;
        for (const auto& extProperties : instExtNames)
        {
            if (strcmp(extName, extProperties.extensionName) == 0)
            {
                extFound = true;
                break;
            }
        }

        if (!extFound)
        {
            return false;
        }
    }

#ifdef VALIDATION_LAYERS
    // Print the ones we're enabling
    for (const auto& ext : extensions)
        std::cout << "Applying extension: " << ext << "\n";
    std::cout << "\n";
#endif

    return true;
}

bool CheckValidationLayerSupport()
{
	std::uint32_t instLayerCount = 0;
	VkResult res = vkEnumerateInstanceLayerProperties(&instLayerCount, nullptr);
	neko_assert(res == VK_SUCCESS,
	            "Unable to query vulkan instance layer property count")

	std::vector<VkLayerProperties> instLayerNames(instLayerCount);
	res = vkEnumerateInstanceLayerProperties(&instLayerCount,
	                                         instLayerNames.data());
	neko_assert(res == VK_SUCCESS,
	            "Unable to retrieve vulkan instance layer names")

#ifdef VALIDATION_LAYERS
	// Display layer names and find the ones we specified above
	std::cout << "Found " << instLayerCount << " instance layers:\n";
	std::uint32_t count(0);
	for (const auto& instLayerName : instLayerNames)
	{
		std::cout << count << ": " << instLayerName.layerName << ": " <<
			instLayerName.description << '\n';
		count++;
	}
#endif

	for (const auto& layerName : kValidationLayers)
	{
		bool layerFound = false;
		for (const auto& layerProperties : instLayerNames)
		{
			if (strcmp(layerName, layerProperties.layerName) == 0)
			{
				layerFound = true;
				break;
			}
		}

		if (!layerFound)
		{
			return false;
		}
	}

#ifdef VALIDATION_LAYERS
	// Print the ones we're enabling
	for (const auto& layer : kValidationLayers)
		std::cout << "Applying layer: " << layer << "\n";
	std::cout << "\n";
#endif

	return true;
}

void Instance::CmdPushDescriptorSetKhr(
        const VkDevice device,
        const VkCommandBuffer commandBuffer,
        const VkPipelineBindPoint pipelineBindPoint,
        const VkPipelineLayout layout,
        const std::uint32_t set,
        const std::uint32_t descriptorWriteCount,
        const VkWriteDescriptorSet*
        pDescriptorSet)
{
    const auto func = reinterpret_cast<PFN_vkCmdPushDescriptorSetKHR>(
            vkGetDeviceProcAddr(device, "vkCmdPushDescriptorSetKHR"));

    if (func != nullptr)
    {
        func(commandBuffer,
             pipelineBindPoint,
             layout,
             set,
             descriptorWriteCount,
             pDescriptorSet);
    }
}
}
