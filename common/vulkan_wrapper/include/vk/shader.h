#pragma once
#include "graphics/shader.h"

namespace neko::vk
{
class Shader final : public neko::Shader
{
public:
	void LoadFromFile(std::string_view vertexShaderPath, std::string_view fragmentShaderPath) override;

	const VkShaderModule& GetVertShaderModule() const
	{ return vertShaderModule_; }
	
	const VkShaderModule& GetFragShaderModule() const
	{ return fragShaderModule_; }
	
	VkPipelineShaderStageCreateInfo GetVertShaderStageInfo() const;
	VkPipelineShaderStageCreateInfo GetFragShaderStageInfo() const;

	void Destroy() override {}
	void Destroy(const LogicalDevice& device) const;

private:
	VkShaderModule vertShaderModule_{};
	VkShaderModule fragShaderModule_{};
};
}
