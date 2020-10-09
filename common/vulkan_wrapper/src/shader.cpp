#include "vk/shader.h"

namespace neko::vk
{
void Shader::LoadFromFile(
	const std::string_view vertexShaderPath,
	const std::string_view fragmentShaderPath)
{
    const auto& device = LogicalDeviceLocator::get();
	
    const auto vertShader = LoadBinaries(vertexShaderPath.data());
    const auto fragShader = LoadBinaries(fragmentShaderPath.data());

    vertShaderModule_ = CreateShaderModule(VkDevice(device), vertShader);
    fragShaderModule_ = CreateShaderModule(VkDevice(device), fragShader);
}

VkPipelineShaderStageCreateInfo Shader::GetVertShaderStageInfo() const
{
	VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule_;
    vertShaderStageInfo.pName = "main";

	return vertShaderStageInfo;
}

VkPipelineShaderStageCreateInfo Shader::GetFragShaderStageInfo() const
{
	VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule_;
    fragShaderStageInfo.pName = "main";

	return fragShaderStageInfo;
}

void Shader::Destroy(const LogicalDevice& device) const
{
    vkDestroyShaderModule(VkDevice(device), fragShaderModule_, nullptr);
    vkDestroyShaderModule(VkDevice(device), vertShaderModule_, nullptr);
}
}
