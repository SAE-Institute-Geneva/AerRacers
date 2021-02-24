#pragma once
/* ----------------------------------------------------
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author: Canas Simon
 Date:
---------------------------------------------------------- */
#include "vk/models/vertex_input.h"
#include "vk/pipelines/pipeline.h"

namespace neko::vk
{
constexpr std::array<VkDynamicState, 3> kDynamicStates = {
	VK_DYNAMIC_STATE_VIEWPORT,
	VK_DYNAMIC_STATE_SCISSOR,
	VK_DYNAMIC_STATE_LINE_WIDTH,
};

struct GraphicsPipelineCreateInfo;
class GraphicsPipeline final : public Pipeline
{
public:
	enum Mode : std::uint8_t
	{
		POLYGON = 0,
		MRT
	};

	enum Depth : std::uint8_t
	{
		NONE       = 0,
		READ       = 1,
		WRITE      = 2,
		READ_WRITE = READ | WRITE
	};

	GraphicsPipeline(Stage stage, const GraphicsPipelineCreateInfo& createInfo);
	GraphicsPipeline(Stage stage,
		std::string_view shaderPath,
		std::vector<VertexInput> vertexInputs,
		Mode mode,
		Depth depthMode,
		VkPrimitiveTopology topology,
		VkPolygonMode polygonMode,
		VkCullModeFlags cullMode,
		VkFrontFace frontFace,
		bool pushDescriptors);

	operator const VkPipeline&() const { return pipeline_; }

	void Destroy() const;

	[[nodiscard]] const VkPipeline& GetPipeline() const override { return pipeline_; }
	[[nodiscard]] const VkPipelineLayout& GetLayout() const { return layout_; }

	[[nodiscard]] const Stage& GetStage() const { return stage_; }
	[[nodiscard]] const Shader& GetShader() const override { return shader_; }
	[[nodiscard]] const VkDescriptorSetLayout& GetDescriptorSetLayout() const override
	{
		return descriptorSetLayout_;
	}

	[[nodiscard]] const VkDescriptorPool& GetDescriptorPool() const override
	{
		return descriptorPool_;
	}

	[[nodiscard]] const VkPipelineLayout& GetPipelineLayout() const override { return layout_; }
	[[nodiscard]] VkPipelineBindPoint GetPipelineBindPoint() const override
	{
		return pipelineBindPoint_;
	}

	[[nodiscard]] bool IsPushDescriptor() const override { return pushDescriptors_; }

private:
	void CreateShaderProgram(const json& jsonShader);
	void CreateDescriptorLayout();
	void CreateDescriptorPool();
	void CreatePipelineLayout();
	void CreateAttributes();
	void CreatePipeline();
	void CreatePipelineMrt();

	VkDescriptorSetLayout descriptorSetLayout_ {};
	VkDescriptorPool descriptorPool_ {};

	VkPipeline pipeline_ {};
	VkPipelineLayout layout_ {};
	VkPipelineBindPoint pipelineBindPoint_ = VK_PIPELINE_BIND_POINT_GRAPHICS;

	Stage stage_ {};
	std::vector<VertexInput> vertexInputs_ {};
	Mode mode_ {};
	Depth depthMode_ {};
	VkPrimitiveTopology topology_ {};
	VkPolygonMode polygonMode_ {};
	VkCullModeFlags cullMode_ {};
	VkFrontFace frontFace_ {};
	bool pushDescriptors_ {};

	Shader shader_;
	std::vector<VkShaderModule> modules_;
	std::vector<VkPipelineShaderStageCreateInfo> stages_;

	VkPipelineVertexInputStateCreateInfo vertexInputInfo_ {};
	VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo_ {};
	VkPipelineViewportStateCreateInfo viewportInfo_ {};
	VkPipelineRasterizationStateCreateInfo rasterizationInfo_ {};
	VkPipelineMultisampleStateCreateInfo multisampleInfo_ {};
	VkPipelineColorBlendAttachmentState colorBlend_ {};
	VkPipelineColorBlendStateCreateInfo colorBlendInfo_ {};
	VkPipelineDepthStencilStateCreateInfo depthStencilInfo_ {};
	VkPipelineDynamicStateCreateInfo dynamicStateInfo_ {};
	VkPipelineTessellationStateCreateInfo tessellationInfo_ {};
};

struct GraphicsPipelineCreateInfo
{
	explicit GraphicsPipelineCreateInfo(std::string_view shaderPath,
		std::vector<VertexInput> vertexInputs = {},
		GraphicsPipeline::Mode mode           = GraphicsPipeline::POLYGON,
		GraphicsPipeline::Depth depth         = GraphicsPipeline::READ_WRITE,
		VkPrimitiveTopology topology          = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		VkPolygonMode polygonMode             = VK_POLYGON_MODE_FILL,
		VkCullModeFlags cullMode              = VK_CULL_MODE_BACK_BIT,
		VkFrontFace frontFace                 = VK_FRONT_FACE_COUNTER_CLOCKWISE,
		bool pushDescriptors                  = false);

	bool operator==(const GraphicsPipelineCreateInfo& other) const
	{
		return mode == other.mode && depth == other.depth && topology == other.topology &&
		       polygonMode == other.polygonMode && cullMode == other.cullMode &&
		       frontFace == other.frontFace && isPushDescriptor == other.isPushDescriptor;
	}

	bool operator!=(const GraphicsPipelineCreateInfo& other) const { return !(*this == other); }

	std::vector<std::string> shaderStages;
	std::vector<VertexInput> vertexInputs;

	bool isPushDescriptor;

	GraphicsPipeline::Mode mode;
	GraphicsPipeline::Depth depth;
	VkPrimitiveTopology topology;
	VkPolygonMode polygonMode;
	VkCullModeFlags cullMode;
	VkFrontFace frontFace;

	ordered_json shaderJson;
};
}    // namespace neko::vk