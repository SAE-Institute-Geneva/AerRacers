#pragma once
#include <array>
#include <cstring>
#include <sstream>

#include "SDL_vulkan.h"

#include "engine/engine.h"
#include "engine/globals.h"
#include "vk/vulkan_include.h"

namespace neko::vk
{
static const VkFormat kFormat                   = VK_FORMAT_B8G8R8A8_SRGB;
static const VkColorSpaceKHR kColorSpace        = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
static const VkPresentModeKHR kPresentationMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;

static const std::vector<const char*> kValidationLayers   = { "VK_LAYER_KHRONOS_validation" };
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
    Vec3f position = Vec3f::zero;
    Vec3f normal = Vec3f::zero;
    Vec2f texCoords = Vec2f::zero;
    Vec3f tangent = Vec3f::zero;
    Vec3f bitangent = Vec3f::zero;
};

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

static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
{
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                                 VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                             VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = DebugCallback;
}

static uint32_t FindMemoryType(
        const VkPhysicalDeviceMemoryProperties& memProperties,
        const VkMemoryRequirements& memRequirements,
        const VkMemoryPropertyFlags& properties)
{
    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        if (memRequirements.memoryTypeBits & (1u << i))
            if ((memProperties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;

    neko_assert(false, "Failed to find suitable memory type!")
}


static std::vector<const char*> GetRequiredInstanceExtensions(SDL_Window* window)
{
    uint32_t sdlExtCount = 0;
    neko_assert(SDL_Vulkan_GetInstanceExtensions(window, &sdlExtCount, nullptr),
                "Unable to query the number of Vulkan instance extensions!")

    // Use the amount of extensions queried before to retrieve the names of the extensions
    std::vector<const char*> sdlExtensions(sdlExtCount);
    neko_assert(SDL_Vulkan_GetInstanceExtensions(window, &sdlExtCount, sdlExtensions.data()),
                "Unable to query the number of Vulkan instance extension names!")

#ifdef VALIDATION_LAYERS
    sdlExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
#endif

    return sdlExtensions;
}

static bool CheckInstanceExtensionsSupport(const std::vector<const char*>& extensions)
{
    uint32_t instExtCount = 0;
    VkResult res = vkEnumerateInstanceExtensionProperties(nullptr, &instExtCount, nullptr);
    neko_assert(res == VK_SUCCESS, "Unable to query vulkan instance extension count")

    std::vector<VkExtensionProperties> instExtNames(instExtCount);
    res = vkEnumerateInstanceExtensionProperties(nullptr, &instExtCount, instExtNames.data());
    neko_assert(res == VK_SUCCESS, "Unable to retrieve vulkan instance extension names")

#ifndef NDEBUG
    // Display layer names and find the ones we specified above
    std::cout << "Found " << instExtCount << " instance extensions:\n";
    uint32_t count(0);
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

#ifndef NDEBUG
    // Print the ones we're enabling
    for (const auto& ext : extensions)
        std::cout << "Applying extension: " << ext << "\n";
    std::cout << "\n";
#endif

    return true;
}

static bool CheckValidationLayerSupport()
{
    uint32_t instLayerCount = 0;
    VkResult res = vkEnumerateInstanceLayerProperties(&instLayerCount, nullptr);
    neko_assert(res == VK_SUCCESS, "Unable to query vulkan instance layer property count")

    std::vector<VkLayerProperties> instLayerNames(instLayerCount);
    res = vkEnumerateInstanceLayerProperties(&instLayerCount, instLayerNames.data());
    neko_assert(res == VK_SUCCESS, "Unable to retrieve vulkan instance layer names")

#ifndef NDEBUG
    // Display layer names and find the ones we specified above
    std::cout << "Found " << instLayerCount << " instance layers:\n";
    uint32_t count(0);
    for (const auto& instLayerName : instLayerNames)
    {
        std::cout << count << ": " << instLayerName.layerName << ": " << instLayerName.description << '\n';
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

#ifndef NDEBUG
    // Print the ones we're enabling
    for (const auto& layer : kValidationLayers)
        std::cout << "Applying layer: " << layer << "\n";
    std::cout << "\n";
#endif

    return true;
}

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

static bool IsDeviceSuitable(
        const VkPhysicalDevice& gpu,
        const VkSurfaceKHR& surface,
        const QueueFamilyIndices& queueFamilyIndices)
{
    VkPhysicalDeviceProperties deviceProperties;
    VkPhysicalDeviceFeatures deviceFeatures;
    vkGetPhysicalDeviceProperties(gpu, &deviceProperties);
    vkGetPhysicalDeviceFeatures(gpu, &deviceFeatures);

    const bool extensionsSupported = CheckDeviceExtensionSupport(gpu);

    bool swapChainAdequate = false;
    if (extensionsSupported)
    {
        const SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(gpu, VkSurfaceKHR(surface));
        swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
    }

    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(gpu, &supportedFeatures);

    return queueFamilyIndices.IsComplete() && extensionsSupported && swapChainAdequate &&
           supportedFeatures.samplerAnisotropy &&
           deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
           deviceFeatures.geometryShader;
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
    std::cout << config.windowSize.x << config.windowSize.y << '\n';

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
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
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

static void CreateBuffer(const VkPhysicalDevice& gpu,
    const VkDevice& device,
    const VkDeviceSize& size,
    const VkBufferUsageFlags usage,
    const VkMemoryPropertyFlags properties,
    VkBuffer& buffer,
    VkDeviceMemory& bufferMemory)
{
    VkBufferCreateInfo bufferInfo{};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkResult res = vkCreateBuffer(VkDevice(device), &bufferInfo, nullptr, &buffer);
    neko_assert(res == VK_SUCCESS, "Failed to create vertex buffer!")

    VkMemoryRequirements memRequirements;
    vkGetBufferMemoryRequirements(VkDevice(device), buffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = FindMemoryType(gpu, memRequirements.memoryTypeBits, properties);

    res = vkAllocateMemory(VkDevice(device), &allocInfo, nullptr, &bufferMemory);
    neko_assert(res == VK_SUCCESS, "Failed to allocate vertex buffer memory!")

    vkBindBufferMemory(VkDevice(device), buffer, bufferMemory, 0);
}

static VkCommandBuffer BeginSingleTimeCommands(const VkDevice& device, const VkCommandPool& commandPool)
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = commandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer{};
    vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

static void EndSingleTimeCommands(
        const VkQueue& graphicsQueue,
        const VkDevice& device,
        const VkCommandPool& commandPool,
        VkCommandBuffer const& commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(graphicsQueue);

    vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
}

static void CopyBuffer(
        const VkQueue& graphicsQueue,
        const VkDevice& device,
        const VkCommandPool& commandPool,
        const VkBuffer& srcBuffer,
        const VkBuffer& dstBuffer,
        const VkDeviceSize size)
{
    VkCommandBuffer commandBuffer = BeginSingleTimeCommands(device, commandPool);

    VkBufferCopy copyRegion{};
    copyRegion.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

    EndSingleTimeCommands(graphicsQueue, device, commandPool, commandBuffer);
}
}