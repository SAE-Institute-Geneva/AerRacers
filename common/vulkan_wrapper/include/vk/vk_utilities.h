#pragma once
#include <array>

#include "SDL_vulkan.h"

#include "engine/engine.h"
#include "engine/globals.h"
#include "vk/vulkan_include.h"

namespace neko::vk
{
static const VkFormat kFormat                   = VK_FORMAT_B8G8R8A8_SRGB;
static const VkColorSpaceKHR kColorSpace        = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
static const VkPresentModeKHR kPresentationMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;

static const std::array<const char*, 1> kValidationLayers   = { "VK_LAYER_KHRONOS_validation" };
static const std::array<const char*, 1> kDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

struct QueueFamilyIndices
{
    uint32_t graphicsFamily = INVALID_INDEX;
    uint32_t presentFamily = INVALID_INDEX;

    [[nodiscard]] bool IsComplete() const { return graphicsFamily != INVALID_INDEX && presentFamily != INVALID_INDEX; }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

static VkFormat FindSupportedFormat(
	const VkPhysicalDevice& gpu,
	const std::vector<VkFormat>& candidates,
	VkImageTiling tiling,
	VkFormatFeatureFlags features);

[[nodiscard]] static VkFormat FindDepthFormat(const VkPhysicalDevice& gpu);
}
