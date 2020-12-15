#pragma once
#include "vk/vulkan_window.h"

namespace neko::vk
{
class Instance
{
public:
    explicit Instance() = default;

    void Init(SDL_Window& window);
    void Destroy() const;

    explicit operator const VkInstance &() const { return instance_; }
    [[nodiscard]] const VkInstance& GetInstance() const { return instance_; }

    VkResult CreateDebugUtilsMessengerExt(
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator);
    void SetupDebugMessenger();
    void DestroyDebugUtilsMessengerExt(const VkAllocationCallbacks* pAllocator) const;

    [[nodiscard]] static std::vector<const char*> GetRequiredInstanceExtensions(SDL_Window& window);

    static void CmdPushDescriptorSetKhr(
            VkDevice device,
            VkCommandBuffer commandBuffer,
            VkPipelineBindPoint pipelineBindPoint,
            VkPipelineLayout layout,
            std::uint32_t set,
            std::uint32_t descriptorWriteCount,
            const VkWriteDescriptorSet*
            pDescriptorSet);

private:
    VkInstance instance_{};
    VkDebugUtilsMessengerEXT debugMessenger_{};
};

static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

static bool CheckInstanceExtensionsSupport(const std::vector<const char*>& extensions);
static bool CheckValidationLayerSupport();
}