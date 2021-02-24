#include "vk/pipelines/graphics_pipeline.h"

#include <utility>

#include "engine/engine.h"
#include "utils/json_utility.h"
#include "vk/vk_resources.h"

namespace neko::vk
{
GraphicsPipeline::GraphicsPipeline(
	Pipeline::Stage stage, const GraphicsPipelineCreateInfo& createInfo)
   : stage_(stage),
	 vertexInputs_(createInfo.vertexInputs),
	 mode_(createInfo.mode),
	 depthMode_(createInfo.depth),
	 topology_(createInfo.topology),
	 polygonMode_(createInfo.polygonMode),
	 cullMode_(createInfo.cullMode),
	 frontFace_(createInfo.frontFace),
	 pushDescriptors_(createInfo.isPushDescriptor)
{
	//Sort the vertex inputs by binding
	std::sort(vertexInputs_.begin(), vertexInputs_.end());
	CreateShaderProgram(createInfo.shaderJson);
	CreateDescriptorLayout();
	CreateDescriptorPool();
	CreatePipelineLayout();
	CreateAttributes();

	switch (mode_)
	{
		case Mode::POLYGON: CreatePipeline(); break;
		case Mode::MRT: CreatePipelineMrt(); break;
		default: neko_assert(false, "Invalid pipeline mode")
	}
}

GraphicsPipeline::GraphicsPipeline(Stage stage,
	std::string_view shaderPath,
	std::vector<VertexInput> vertexInputs,
	Mode mode,
	Depth depthMode,
	VkPrimitiveTopology topology,
	VkPolygonMode polygonMode,
	VkCullModeFlags cullMode,
	VkFrontFace frontFace,
	bool pushDescriptors)
   : stage_(stage),
	 vertexInputs_(std::move(vertexInputs)),
	 mode_(mode),
	 depthMode_(depthMode),
	 topology_(topology),
	 polygonMode_(polygonMode),
	 cullMode_(cullMode),
	 frontFace_(frontFace),
	 pushDescriptors_(pushDescriptors)
{
	const auto& shaderJson = LoadJson(shaderPath);

	//Sort the vertex inputs by binding
	std::sort(vertexInputs_.begin(), vertexInputs_.end());
	CreateShaderProgram(shaderJson);
	CreateDescriptorLayout();
	CreateDescriptorPool();
	CreatePipelineLayout();
	CreateAttributes();

	switch (mode_)
	{
		case Mode::POLYGON: CreatePipeline(); break;
		case Mode::MRT: CreatePipelineMrt(); break;
		default: neko_assert(false, "Invalid pipeline mode")
	}
}

void GraphicsPipeline::Destroy() const
{
	const auto& device = VkDevice(VkResources::Inst->device);

	vkDestroyDescriptorSetLayout(device, descriptorSetLayout_, nullptr);
	vkDestroyDescriptorPool(device, descriptorPool_, nullptr);
	for (auto& module : modules_) { vkDestroyShaderModule(device, module, nullptr); }
	vkDestroyPipeline(device, pipeline_, nullptr);
	vkDestroyPipelineLayout(device, layout_, nullptr);
}

void GraphicsPipeline::CreateShaderProgram(const json& jsonShader)
{
	modules_           = shader_.LoadFromJson(jsonShader);
	const auto& stages = shader_.GetStagePaths();
	for (std::size_t i = 0; i < modules_.size(); ++i)
	{
		const auto stageFlag = Shader::GetShaderStage(stages[i]);

		VkPipelineShaderStageCreateInfo pipelineShaderStageCreateInfo = {};
		pipelineShaderStageCreateInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		pipelineShaderStageCreateInfo.stage  = stageFlag;
		pipelineShaderStageCreateInfo.module = modules_[i];
		pipelineShaderStageCreateInfo.pName  = "main";
		stages_.emplace_back(pipelineShaderStageCreateInfo);
	}
	shader_.Init();
}

void GraphicsPipeline::CreateDescriptorLayout()
{
	const VkResources* vkObj = VkResources::Inst;

	auto& descriptorSetLayouts = shader_.GetDescriptorSetLayoutBindings();

	VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo {};
	descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	descriptorSetLayoutCreateInfo.flags =
		pushDescriptors_ ? VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR : 0;
	descriptorSetLayoutCreateInfo.bindingCount =
		static_cast<std::uint32_t>(descriptorSetLayouts.size());
	descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayouts.data();
	const VkResult res                      = vkCreateDescriptorSetLayout(
        VkDevice(vkObj->device), &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout_);
	neko_assert(res == VK_SUCCESS, "Could not create descriptor set layout!")
}

void GraphicsPipeline::CreateDescriptorPool()
{
	const VkResources* vkObj = VkResources::Inst;
	auto& descriptorPools = shader_.GetDescriptorPoolSizes();

	VkDescriptorPoolCreateInfo createInfo {};
	createInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	createInfo.flags         = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
	createInfo.maxSets       = 8192;    // 16384;
	createInfo.poolSizeCount = static_cast<std::uint32_t>(descriptorPools.size());
	createInfo.pPoolSizes    = descriptorPools.data();
	const VkResult res =
		vkCreateDescriptorPool(vkObj->device, &createInfo, nullptr, &descriptorPool_);
	neko_assert(res == VK_SUCCESS, "Could not create descriptor pool!")
}

void GraphicsPipeline::CreatePipelineLayout()
{
	const VkResources* vkObj = VkResources::Inst;
	auto pushConstantRanges  = shader_.GetPushConstantRanges();

	VkPipelineLayoutCreateInfo createInfo {};
	createInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	createInfo.setLayoutCount         = 1;
	createInfo.pSetLayouts            = &descriptorSetLayout_;
	createInfo.pushConstantRangeCount = static_cast<std::uint32_t>(pushConstantRanges.size());
	createInfo.pPushConstantRanges    = pushConstantRanges.data();
	const VkResult res = vkCreatePipelineLayout(vkObj->device, &createInfo, nullptr, &layout_);
	neko_assert(res == VK_SUCCESS, "Could not create pipeline layout!")
}

void GraphicsPipeline::CreateAttributes()
{
	inputAssemblyInfo_.sType    = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssemblyInfo_.topology = topology_;
	inputAssemblyInfo_.primitiveRestartEnable = VK_FALSE;

	viewportInfo_.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportInfo_.viewportCount = 1;
	viewportInfo_.scissorCount  = 1;

	rasterizationInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizationInfo_.depthClampEnable        = VK_FALSE;
	rasterizationInfo_.rasterizerDiscardEnable = VK_FALSE;
	rasterizationInfo_.polygonMode             = polygonMode_;
	rasterizationInfo_.lineWidth               = 1.0f;
	rasterizationInfo_.cullMode                = cullMode_;
	rasterizationInfo_.frontFace               = frontFace_;
	rasterizationInfo_.depthBiasEnable         = VK_FALSE;

	multisampleInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampleInfo_.sampleShadingEnable  = VK_FALSE;
	multisampleInfo_.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	colorBlend_.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
	                             VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlend_.blendEnable         = VK_TRUE;
	colorBlend_.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlend_.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlend_.colorBlendOp        = VK_BLEND_OP_ADD;
	colorBlend_.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlend_.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
	colorBlend_.alphaBlendOp        = VK_BLEND_OP_MAX;

	colorBlendInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlendInfo_.logicOpEnable     = VK_FALSE;
	colorBlendInfo_.attachmentCount   = 1;
	colorBlendInfo_.pAttachments      = &colorBlend_;
	colorBlendInfo_.blendConstants[0] = 0.0f;
	colorBlendInfo_.blendConstants[1] = 0.0f;
	colorBlendInfo_.blendConstants[2] = 0.0f;
	colorBlendInfo_.blendConstants[3] = 0.0f;

	depthStencilInfo_.sType          = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilInfo_.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencilInfo_.depthBoundsTestEnable = VK_FALSE;
	depthStencilInfo_.stencilTestEnable     = VK_FALSE;
	depthStencilInfo_.front                 = depthStencilInfo_.back;
	depthStencilInfo_.back.compareOp        = VK_COMPARE_OP_ALWAYS;

	switch (depthMode_)
	{
		case Depth::NONE:
			depthStencilInfo_.depthTestEnable  = VK_FALSE;
			depthStencilInfo_.depthWriteEnable = VK_FALSE;
			break;
		case Depth::READ:
			depthStencilInfo_.depthTestEnable  = VK_TRUE;
			depthStencilInfo_.depthWriteEnable = VK_FALSE;
			break;
		case Depth::WRITE:
			depthStencilInfo_.depthTestEnable  = VK_FALSE;
			depthStencilInfo_.depthWriteEnable = VK_TRUE;
			break;
		case Depth::READ_WRITE:
			depthStencilInfo_.depthTestEnable  = VK_TRUE;
			depthStencilInfo_.depthWriteEnable = VK_TRUE;
			break;
		default:;
	}

	dynamicStateInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo_.dynamicStateCount = static_cast<std::uint32_t>(kDynamicStates.size());
	dynamicStateInfo_.pDynamicStates    = kDynamicStates.data();

	tessellationInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
	tessellationInfo_.patchControlPoints = 3;
}

void GraphicsPipeline::CreatePipeline()
{
	const VkResources* vkObj = VkResources::Inst;
	RenderStage* renderStage  = vkObj->GetRenderStage();

	std::vector<VkVertexInputBindingDescription> bindingDescriptions;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
	std::uint32_t lastAttribute = 0;
	for (const auto& vertexInput : vertexInputs_)
	{
		bindingDescriptions.emplace_back(vertexInput.GetBindingDescription());

		for (const auto& attribute : vertexInput.GetAttributeDescriptions())
		{
			attributeDescriptions.push_back(attribute);
			attributeDescriptions[attributeDescriptions.size() - 1].location += lastAttribute;
		}

		if (!vertexInput.GetAttributeDescriptions().empty())
			lastAttribute = attributeDescriptions.back().location + 1;
	}

	vertexInputInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo_.vertexBindingDescriptionCount =
		static_cast<std::uint32_t>(bindingDescriptions.size());
	vertexInputInfo_.pVertexBindingDescriptions = bindingDescriptions.data();
	vertexInputInfo_.vertexAttributeDescriptionCount =
		static_cast<std::uint32_t>(attributeDescriptions.size());
	vertexInputInfo_.pVertexAttributeDescriptions = attributeDescriptions.data();

	VkGraphicsPipelineCreateInfo createInfo = {};
	createInfo.sType      = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	createInfo.stageCount = static_cast<std::uint32_t>(stages_.size());
	createInfo.pStages    = stages_.data();

	createInfo.pVertexInputState   = &vertexInputInfo_;
	createInfo.pInputAssemblyState = &inputAssemblyInfo_;
	createInfo.pTessellationState  = &tessellationInfo_;
	createInfo.pViewportState      = &viewportInfo_;
	createInfo.pRasterizationState = &rasterizationInfo_;
	createInfo.pMultisampleState   = &multisampleInfo_;
	createInfo.pDepthStencilState  = &depthStencilInfo_;
	createInfo.pColorBlendState    = &colorBlendInfo_;
	createInfo.pDynamicState       = &dynamicStateInfo_;

	createInfo.layout             = layout_;
	createInfo.renderPass         = *renderStage->GetRenderPass();
	createInfo.subpass            = stage_.subPassId;
	createInfo.basePipelineHandle = {};
	createInfo.basePipelineIndex  = -1;

	const VkResult res = vkCreateGraphicsPipelines(
		vkObj->device, vkObj->pipelineCache, 1, &createInfo, nullptr, &pipeline_);
	neko_assert(res == VK_SUCCESS, "Could not create graphic pipeline!")
}

void GraphicsPipeline::CreatePipelineMrt()
{
	const auto& renderStage    = VkResources::Inst->GetRenderStage();
	const auto attachmentCount = renderStage->GetAttachmentCount(stage_.subPassId);

	std::vector<VkPipelineColorBlendAttachmentState> blendAttachmentStates;
	blendAttachmentStates.reserve(attachmentCount);
	for (std::uint32_t i = 0; i < attachmentCount; i++)
	{
		VkPipelineColorBlendAttachmentState blend = {};
		blend.blendEnable                         = VK_TRUE;
		blend.srcColorBlendFactor                 = VK_BLEND_FACTOR_SRC_ALPHA;
		blend.dstColorBlendFactor                 = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blend.colorBlendOp                        = VK_BLEND_OP_ADD;
		blend.srcAlphaBlendFactor                 = VK_BLEND_FACTOR_ONE;
		blend.dstAlphaBlendFactor                 = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		blend.alphaBlendOp                        = VK_BLEND_OP_ADD;
		blend.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
		                       VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		blendAttachmentStates.emplace_back(blend);
	}

	//colorBlendStateCreateInfo_.attachmentCount = static_cast<std::uint32_t>(blendAttachmentStates.size());
	//colorBlendStateCreateInfo_.pAttachments = blendAttachmentStates.data();

	CreatePipeline();
}

GraphicsPipelineCreateInfo::GraphicsPipelineCreateInfo(std::string_view shaderPath,
	std::vector<VertexInput> vertexInputs,
	GraphicsPipeline::Mode mode,
	GraphicsPipeline::Depth depth,
	VkPrimitiveTopology topology,
	VkPolygonMode polygonMode,
	VkCullModeFlags cullMode,
	VkFrontFace frontFace,
	bool pushDescriptors)
   : vertexInputs(std::move(vertexInputs)),
	 isPushDescriptor(pushDescriptors),
	 mode(mode),
	 depth(depth),
	 topology(topology),
	 polygonMode(polygonMode),
	 cullMode(cullMode),
	 frontFace(frontFace)
{
	shaderJson = LoadJson(shaderPath);

	if (CheckJsonExists(shaderJson, "vert"))
		shaderStages.emplace_back(shaderJson["vert"].get<std::string>());

	if (CheckJsonExists(shaderJson, "frag"))
		shaderStages.emplace_back(shaderJson["frag"].get<std::string>());
}
}    // namespace neko::vk
