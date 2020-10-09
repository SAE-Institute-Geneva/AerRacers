#pragma once
#include "vk/vulkan_include.h"
#include "vk/vulkan_window.h"

namespace neko::vk
{
class Instance
{
public:
    explicit Instance() = default;

    void Init(const sdl::VulkanWindow* window);
    void Destroy() const;

    explicit operator const VkInstance &() const { return instance_; }
    [[nodiscard]] const VkInstance& GetInstance() const { return instance_; }

    VkResult CreateDebugUtilsMessengerExt(
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator);
    void SetupDebugMessenger();
    void DestroyDebugUtilsMessengerExt(const VkAllocationCallbacks* pAllocator) const;

private:
    VkInstance instance_{};
    VkDebugUtilsMessengerEXT debugMessenger_{};
};

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT msgSeverity,
	VkDebugUtilsMessageTypeFlagsEXT msgType,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData);

static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

static bool CheckInstanceExtensionsSupport(const std::vector<const char*>& extensions);
static bool CheckValidationLayerSupport();
}