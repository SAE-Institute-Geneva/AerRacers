#pragma once
#include <SDL_video.h>

#include "utilities/service_locator.h"
#include "vk/vulkan_include.h"

namespace neko::vk
{
static const std::vector<const char*> kValidationLayers = { "VK_LAYER_KHRONOS_validation" };

class IInstance
{
public:
    virtual explicit operator const VkInstance &() const = 0;
    [[nodiscard]] virtual const VkInstance& GetInstance() const = 0;
};

class NullInstance : public IInstance
{
public:
    explicit operator const VkInstance &() const override
    {
        neko_assert(false, "Vulkan Instance is null!")
        return nullptr;
    }
    [[nodiscard]] const VkInstance& GetInstance() const override
    {
        neko_assert(false, "Vulkan Instance is null!")
        return nullptr;
    }
};

class Instance : public IInstance
{
public:
    explicit Instance(SDL_Window* window);
    ~Instance();

    explicit operator const VkInstance &() const override { return instance_; }
    [[nodiscard]] const VkInstance& GetInstance() const override { return instance_; }

    VkResult CreateDebugUtilsMessengerExt(
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator);
    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    void SetupDebugMessenger();
    void DestroyDebugUtilsMessengerExt(const VkAllocationCallbacks* pAllocator) const;

    [[nodiscard]] static uint32_t FindMemoryType(
        const VkPhysicalDeviceMemoryProperties& memProperties,
        const VkMemoryRequirements& memRequirements,
        const VkMemoryPropertyFlags& properties);

    [[nodiscard]] static std::vector<const char*> GetRequiredInstanceExtensions(SDL_Window* window);
    static bool CheckInstanceExtensionsSupport(const std::vector<const char*>& extensions);
    static bool CheckValidationLayerSupport();

private:
    VkInstance instance_;
    VkDebugUtilsMessengerEXT debugMessenger_;
};

using InstanceLocator = Locator<IInstance, NullInstance>;
}