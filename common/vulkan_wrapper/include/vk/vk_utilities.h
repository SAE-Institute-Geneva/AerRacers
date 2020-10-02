#pragma once
#include <array>

#include "engine/engine.h"
#include "engine/globals.h"
#include "vk/vulkan_include.h"

namespace neko::vk
{
static const VkFormat kFormat                   = VK_FORMAT_B8G8R8A8_SRGB;
static const VkColorSpaceKHR kColorSpace        = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
static const VkPresentModeKHR kPresentationMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;

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

struct Vertex
{
    Vec3f position;
    Vec3f normal;
    Vec2f texCoords;
    Vec3f tangent;
    Vec3f bitangent;
};

static bool CheckDeviceExtensionSupport(const VkPhysicalDevice& gpu)
{
    uint32_t extensionCount = 0;
    VkResult res = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, nullptr);
    neko_assert(res == VK_SUCCESS, "Unable to query vulkan device extensions count")

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    res = vkEnumerateDeviceExtensionProperties(gpu, nullptr, &extensionCount, availableExtensions.data());
    neko_assert(res == VK_SUCCESS, "Unable to retrieve vulkan device extension names")

#ifdef VALIDATION_LAYERS
    // Display layer names and find the ones we specified above
    std::cout << "Found " << extensionCount << " device extensions:\n";
    uint32_t count(0);
    for (const auto& availableExtension : availableExtensions)
    {
        std::cout << count << ": " << availableExtension.extensionName << '\n';
        count++;
    }
#endif

    for (const auto& extName : kDeviceExtensions)
    {
        bool layerFound = false;
        for (const auto& extProperties : availableExtensions)
        {
            if (strcmp(extName, extProperties.extensionName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound) return false;
    }

#ifdef VALIDATION_LAYERS
    // Print the ones we're enabling
    for (const auto& extension : kDeviceExtensions)
        std::cout << "Applying device extension: " << extension << "\n";
    std::cout << "\n";
#endif

    return true;
}

static QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface)
{
    QueueFamilyIndices indices;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(gpu, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(gpu, i, surface, &presentSupport);
        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
            indices.graphicsFamily = i;

        if (presentSupport)
            indices.presentFamily = i;

        if (indices.IsComplete())
            break;

        i++;
    }

#ifdef VALIDATION_LAYERS
    if (indices.graphicsFamily != INVALID_INDEX)
    {
        std::cout << "Queue family #" << indices.graphicsFamily << " supports graphics\n";

        if (indices.presentFamily != INVALID_INDEX)
            std::cout << "Queue family #" << indices.presentFamily << " supports presentation\n";
        else
            std::cout << "could not find a valid queue family with present support!\n";
    }
    else
        std::cout << "could not find a valid queue family with graphics support!\n";
    std::cout << '\n';
#endif

    return indices;
}

static SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice& gpu, const VkSurfaceKHR& surface)
{
    //Fill swap chain details
    SwapChainSupportDetails details{};
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(gpu, surface, &details.capabilities);

    //Fill swap chain formats data
    uint32_t formatCount = 0;
    vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, nullptr);
    if (formatCount != 0)
    {
        details.formats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(gpu, surface, &formatCount, details.formats.data());
    }

    //Fill swap chain presentation modes data
    uint32_t presentModeCount = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, nullptr);
    if (presentModeCount != 0)
    {
        details.presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(gpu, surface, &presentModeCount, details.presentModes.data());
    }

    return details;
}

static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
{
    for (const auto& availableFormat : availableFormats)
        if (availableFormat.format == kFormat && availableFormat.colorSpace == kColorSpace)
            return availableFormat;

#ifdef VALIDATION_LAYERS
    std::cout << "Warning: no matching color format found, picking first available one\n";
#endif

    return availableFormats[0];
}

static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
{
    for (const auto& availablePresentMode : availablePresentModes)
        if (availablePresentMode == kPresentationMode)
            return availablePresentMode;

#ifdef VALIDATION_LAYERS
    std::cout << "Unable to obtain preferred display mode, fallback to FIFO\n";
#endif

    return VK_PRESENT_MODE_FIFO_KHR;
}

static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
{
    if (capabilities.currentExtent.width != UINT32_MAX)
        return capabilities.currentExtent;

    auto& config = BasicEngine::GetInstance()->config;

    VkExtent2D actualExtent = {config.windowSize.x, config.windowSize.y};

    actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
    actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

    return actualExtent;
}

static uint32_t FindMemoryType(const VkPhysicalDevice& gpu, uint32_t typeFilter, const VkMemoryPropertyFlags& properties)
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(gpu, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        if (typeFilter & 1 << i && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            return i;

    neko_assert(false, "Failed to find suitable memory type!")
}

static VkImageView CreateImageView(
        const VkDevice& device,
        const VkImage& image,
        VkFormat format,
        VkImageAspectFlags aspectFlags)
{
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = format;
    viewInfo.subresourceRange.aspectMask = aspectFlags;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    const VkResult res = vkCreateImageView(device, &viewInfo, nullptr, &imageView);
    neko_assert(res == VK_SUCCESS, "Failed to create texture image view!")

    return imageView;
}

static VkFormat FindSupportedFormat(
        const VkPhysicalDevice& gpu,
        const std::vector<VkFormat>& candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlags features)
{
    for (const VkFormat format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(gpu, format, &props);

        if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
            return format;

        if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
            return format;
    }

    neko_assert(false, "Failed to find supported format!")
    return VK_FORMAT_D32_SFLOAT;
}

static VkFormat FindDepthFormat(const VkPhysicalDevice& gpu)
{
    return FindSupportedFormat(gpu,
                               {
                                       VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT,
                                       VK_FORMAT_D24_UNORM_S8_UINT
                               },
                               VK_IMAGE_TILING_OPTIMAL,
                               VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

static VkShaderModule CreateShaderModule(const VkDevice& device, const std::string_view& data)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = data.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(data.data());

    VkShaderModule shaderModule{};
    const VkResult res = vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);
	neko_assert(res == VK_SUCCESS, "Failed to create shader module!")

    return shaderModule;
}

static VkVertexInputBindingDescription GetBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription;
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

static std::array<VkVertexInputAttributeDescription, 5> GetAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 5> attributeDescriptions{};
    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, normal);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, texCoords);

    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(Vertex, tangent);

    attributeDescriptions[4].binding = 0;
    attributeDescriptions[4].location = 4;
    attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[4].offset = offsetof(Vertex, bitangent);

    return attributeDescriptions;
}
}