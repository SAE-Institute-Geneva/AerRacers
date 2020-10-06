#include <iostream>
#include <sstream>

#include <SDL_vulkan.h>

#include "vk/core/instance.h"
#include "vk/vk_utilities.h"

namespace neko::vk
{
Instance::Instance()
{}

void Instance::Init(const sdl::VulkanWindow& window)
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

    const auto extensions = window.GetRequiredInstanceExtensions();
    neko_assert(CheckInstanceExtensionsSupport(extensions), "Required instance extensions not available!")
    createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();

#ifdef VALIDATION_LAYERS
    createInfo.enabledLayerCount = static_cast<uint32_t>(kValidationLayers.size());
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
            break;
        default:
            neko_assert(false, "Unable to create Vulkan instance: unknown error")
            break;
    }

    SetupDebugMessenger();

    InstanceLocator::provide(this);
}

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

void Instance::Destroy()
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
}