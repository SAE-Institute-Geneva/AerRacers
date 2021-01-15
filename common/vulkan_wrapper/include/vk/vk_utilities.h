#pragma once
#include <array>

#include "engine/globals.h"
#include "vk/vulkan_include.h"

namespace neko::vk
{
static const VkFormat kFormat                   = VK_FORMAT_B8G8R8A8_UNORM; //TODO(@Simon) Change to sRGB latter
static const VkColorSpaceKHR kColorSpace        = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
static const VkPresentModeKHR kPresentationMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;

static const std::array<const char*, 1> kValidationLayers   = { "VK_LAYER_KHRONOS_validation" };
static const std::array<const char*, 1> kDeviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

struct QueueFamilyIndices
{
    std::uint32_t graphicsFamily = INVALID_INDEX;
    std::uint32_t presentFamily = INVALID_INDEX;

    [[nodiscard]] bool IsComplete() const { return graphicsFamily != INVALID_INDEX && presentFamily != INVALID_INDEX; }
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};
}
