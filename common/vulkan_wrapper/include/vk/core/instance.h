#pragma once
#include "SDL_video.h"

#include "utilities/service_locator.h"
#include "vk/vulkan_include.h"
#include "vk/vulkan_window.h"

namespace neko::vk
{
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
    explicit Instance();

    void Init(const sdl::VulkanWindow& window);
    void Destroy();

    explicit operator const VkInstance &() const override { return instance_; }
    [[nodiscard]] const VkInstance& GetInstance() const override { return instance_; }

    VkResult CreateDebugUtilsMessengerExt(
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator);
    void SetupDebugMessenger();
    void DestroyDebugUtilsMessengerExt(const VkAllocationCallbacks* pAllocator) const;

private:
    VkInstance instance_{};
    VkDebugUtilsMessengerEXT debugMessenger_{};
};

using InstanceLocator = Locator<IInstance, NullInstance>;
}