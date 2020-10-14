#include "vk/shader.h"

#include "vk/graphics.h"

namespace neko::vk
{
void Shader::LoadFromFile(std::string_view vertexShaderPath, std::string_view fragmentShaderPath)
{
    const auto& vkObj = VkResourcesLocator::get();

    vertShaderPath_ = vertexShaderPath;
    fragShaderPath_ = fragmentShaderPath;

    descriptorSets_.InitLayout();

    const auto vertShader = LoadBinaries(vertexShaderPath.data());
    const auto fragShader = LoadBinaries(fragmentShaderPath.data());

    const VkShaderModule& vertShaderModule = CreateShaderModule(VkDevice(vkObj.device), vertShader);
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    const VkShaderModule& fragShaderModule = CreateShaderModule(VkDevice(vkObj.device), fragShader);
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] =
            { vertShaderStageInfo, fragShaderStageInfo };

    pipeline_.Init(vkObj.swapchain, vkObj.renderPass, descriptorSets_, shaderStages);

    vkDestroyShaderModule(VkDevice(vkObj.device), fragShaderModule, nullptr);
    vkDestroyShaderModule(VkDevice(vkObj.device), vertShaderModule, nullptr);
}

void Shader::InitUbo(const VkDeviceSize& uboSize)
{
    const auto& vkObj = VkResourcesLocator::get();

    const size_t swapchainImagesCount = vkObj.swapchain.GetImageCount();
    for (size_t i = 0; i < swapchainImagesCount; ++i)
    {
        uniformBuffers_.emplace_back();
        uniformBuffers_[i].Init(vkObj.gpu, uboSize);
    }

    uboSize_ = uboSize;
    descriptorSets_.Init(vkObj.swapchain, uniformBuffers_, vkObj.descriptorPool, uboSize);
}

void Shader::Recreate()
{
    const auto& vkObj = VkResourcesLocator::get();

    descriptorSets_.InitLayout();

    const auto vertShader = LoadBinaries(vertShaderPath_.data());
    const auto fragShader = LoadBinaries(fragShaderPath_.data());

    const VkShaderModule& vertShaderModule = CreateShaderModule(VkDevice(vkObj.device), vertShader);
    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    const VkShaderModule& fragShaderModule = CreateShaderModule(VkDevice(vkObj.device), fragShader);
    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    pipeline_.Init(vkObj.swapchain, vkObj.renderPass, descriptorSets_, shaderStages);

    vkDestroyShaderModule(VkDevice(vkObj.device), fragShaderModule, nullptr);
    vkDestroyShaderModule(VkDevice(vkObj.device), vertShaderModule, nullptr);

    descriptorSets_.Init(vkObj.swapchain, uniformBuffers_, vkObj.descriptorPool, uboSize_);
}

void Shader::Destroy()
{
    pipeline_.Destroy();
    descriptorSets_.Destroy();
}
}
