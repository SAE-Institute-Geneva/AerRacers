#include "vk/pipelines/graphics_pipeline.h"

#include "utilities/file_utility.h"
#include "vk/graphics.h"

namespace neko::vk
{
void GraphicsPipeline::Init(
        const Stage pipelineStage,
        const Shader& shader,
        const std::vector<VertexInput>& vertexInputs,
        const Mode mode,
        const Depth depthMode,
        const VkPrimitiveTopology topology,
        const VkPolygonMode polygonMode,
        const VkCullModeFlags cullMode,
        const VkFrontFace frontFace,
        const bool pushDescriptors)
{
    stage_ = pipelineStage;
    shader_ = shader;
    vertexInputs_ = vertexInputs;
    mode_ = mode;
    depthMode_ = depthMode;
    topology_ = topology;
    polygonMode_ = polygonMode;
    cullMode_ = cullMode;
    frontFace_ = frontFace;
    pushDescriptors_ = pushDescriptors;

    //Sort the vertex inputs by binding
    std::sort(vertexInputs_.begin(), vertexInputs_.end());
    CreateShaderProgram();
    CreateDescriptorLayout();
    CreateDescriptorPool();
    CreatePipelineLayout();
    CreateAttributes();

    switch (mode_)
    {
        case Mode::POLYGON:
            CreatePipeline();
            break;
        case Mode::MRT:
            CreatePipelineMrt();
            break;
        default:
            neko_assert(false, "Invalid pipeline mode");
    }
}

void GraphicsPipeline::Destroy() const
{
    const auto& vkObj = VkObjectsLocator::get();

    vkDestroyPipeline(VkDevice(vkObj.device), pipeline_, nullptr);
    vkDestroyPipelineLayout(VkDevice(vkObj.device), layout_, nullptr);
}

void GraphicsPipeline::CreateShaderProgram()
{
    shaderProgram_ = shader_.CreateShaderProgram();
    shader_.Init();
}

void GraphicsPipeline::CreateDescriptorLayout()
{
    const auto& vkObj = VkObjectsLocator::get();

    auto& descriptorSetLayouts = shader_.GetDescriptorSetLayoutBindings();

    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
    descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.flags = pushDescriptors_ ?
                                          VK_DESCRIPTOR_SET_LAYOUT_CREATE_PUSH_DESCRIPTOR_BIT_KHR : 0;
    descriptorSetLayoutCreateInfo.bindingCount = static_cast<uint32_t>(descriptorSetLayouts.size());
    descriptorSetLayoutCreateInfo.pBindings = descriptorSetLayouts.data();
    const VkResult res = vkCreateDescriptorSetLayout(
            VkDevice(vkObj.device), &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout_);
    neko_assert(res == VK_SUCCESS, "Could not create descriptor set layout!")
}

void GraphicsPipeline::CreateDescriptorPool()
{
    const auto& vkObj = VkObjectsLocator::get();

    auto descriptorPools = shader_.GetDescriptorPoolSizes();

    VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{};
    descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolCreateInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolCreateInfo.maxSets = 8192; // 16384;
    descriptorPoolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPools.size());
    descriptorPoolCreateInfo.pPoolSizes = descriptorPools.data();
    const VkResult res = vkCreateDescriptorPool(
            VkDevice(vkObj.device), &descriptorPoolCreateInfo, nullptr, &descriptorPool_);
    neko_assert(res == VK_SUCCESS, "Could not create descriptor pool!")
}

void GraphicsPipeline::CreatePipelineLayout()
{
    const auto& vkObj = VkObjectsLocator::get();

    auto pushConstantRanges = shader_.GetPushConstantRanges();

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = 1;
    pipelineLayoutCreateInfo.pSetLayouts = &descriptorSetLayout_;
    pipelineLayoutCreateInfo.pushConstantRangeCount = static_cast<uint32_t>(pushConstantRanges.size());
    pipelineLayoutCreateInfo.pPushConstantRanges = pushConstantRanges.data();
    const VkResult res = vkCreatePipelineLayout(
            VkDevice(vkObj.device), &pipelineLayoutCreateInfo, nullptr, &layout_);
    neko_assert(res == VK_SUCCESS, "Could not create pipeline layout!")
}

void GraphicsPipeline::CreateAttributes()
{
    const auto& vkObj = VkObjectsLocator::get();

    inputAssemblyStateCreateInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyStateCreateInfo_.topology = topology_;
    inputAssemblyStateCreateInfo_.primitiveRestartEnable = VK_FALSE;

    viewportStateCreateInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateCreateInfo_.viewportCount = 1;
    viewportStateCreateInfo_.scissorCount = 1;

    rasterizationStateCreateInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationStateCreateInfo_.depthClampEnable = VK_FALSE;
    rasterizationStateCreateInfo_.rasterizerDiscardEnable = VK_FALSE;
    rasterizationStateCreateInfo_.polygonMode = polygonMode_;
    rasterizationStateCreateInfo_.lineWidth = 1.0f;
    rasterizationStateCreateInfo_.cullMode = cullMode_;
    rasterizationStateCreateInfo_.frontFace = frontFace_;
    rasterizationStateCreateInfo_.depthBiasEnable = VK_FALSE;

    multisampleStateCreateInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleStateCreateInfo_.sampleShadingEnable = VK_FALSE;
    multisampleStateCreateInfo_.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    colorBlendAttachmentState_.colorWriteMask = VK_COLOR_COMPONENT_R_BIT |
                                                VK_COLOR_COMPONENT_G_BIT |
                                                VK_COLOR_COMPONENT_B_BIT |
                                                VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachmentState_.blendEnable = VK_TRUE;
    colorBlendAttachmentState_.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentState_.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
    colorBlendAttachmentState_.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachmentState_.srcAlphaBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
    colorBlendAttachmentState_.dstAlphaBlendFactor = VK_BLEND_FACTOR_DST_ALPHA;
    colorBlendAttachmentState_.alphaBlendOp = VK_BLEND_OP_MAX;

    colorBlendStateCreateInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateCreateInfo_.logicOpEnable = VK_FALSE;
    colorBlendStateCreateInfo_.attachmentCount = 1;
    colorBlendStateCreateInfo_.pAttachments = &colorBlendAttachmentState_;
    colorBlendStateCreateInfo_.blendConstants[0] = 0.0f;
    colorBlendStateCreateInfo_.blendConstants[1] = 0.0f;
    colorBlendStateCreateInfo_.blendConstants[2] = 0.0f;
    colorBlendStateCreateInfo_.blendConstants[3] = 0.0f;

    depthStencilStateCreateInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilStateCreateInfo_.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depthStencilStateCreateInfo_.depthBoundsTestEnable = VK_FALSE;
    depthStencilStateCreateInfo_.stencilTestEnable = VK_FALSE;
    depthStencilStateCreateInfo_.front = depthStencilStateCreateInfo_.back;
    depthStencilStateCreateInfo_.back.compareOp = VK_COMPARE_OP_ALWAYS;

    switch (depthMode_)
    {
        case Depth::NONE:
            depthStencilStateCreateInfo_.depthTestEnable = VK_FALSE;
            depthStencilStateCreateInfo_.depthWriteEnable = VK_FALSE;
            break;
        case Depth::READ:
            depthStencilStateCreateInfo_.depthTestEnable = VK_TRUE;
            depthStencilStateCreateInfo_.depthWriteEnable = VK_FALSE;
            break;
        case Depth::WRITE:
            depthStencilStateCreateInfo_.depthTestEnable = VK_FALSE;
            depthStencilStateCreateInfo_.depthWriteEnable = VK_TRUE;
            break;
        case Depth::READ_WRITE:
            depthStencilStateCreateInfo_.depthTestEnable = VK_TRUE;
            depthStencilStateCreateInfo_.depthWriteEnable = VK_TRUE;
            break;
        default: ;
    }

    dynamicStateCreateInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateCreateInfo_.dynamicStateCount = static_cast<uint32_t>(kDynamicStates.size());
    dynamicStateCreateInfo_.pDynamicStates = kDynamicStates.data();

    tessellationStateCreateInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tessellationStateCreateInfo_.patchControlPoints = 3;
}

void GraphicsPipeline::CreatePipeline()
{
    const auto& vkObj = VkObjectsLocator::get();
    const auto& renderStage = vkObj.GetRenderStage(stage_.renderPassId);

    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    uint32_t lastAttribute = 0;

    for (const auto& vertexInput : vertexInputs_)
    {
        bindingDescriptions.emplace_back(vertexInput.GetBindingDescription());

        for (const auto& attribute : vertexInput.GetAttributeDescriptions()) {
            attributeDescriptions.push_back(attribute);
            attributeDescriptions[attributeDescriptions.size() - 1].location += lastAttribute;
        }

        if (!vertexInput.GetAttributeDescriptions().empty()) {
            lastAttribute = attributeDescriptions.back().location + 1;
        }
    }

    vertexInputStateCreateInfo_.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputStateCreateInfo_.vertexBindingDescriptionCount =
            static_cast<uint32_t>(bindingDescriptions.size());
    vertexInputStateCreateInfo_.pVertexBindingDescriptions = bindingDescriptions.data();
    vertexInputStateCreateInfo_.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputStateCreateInfo_.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderProgram_.pipelineStages.size());
    pipelineCreateInfo.pStages = shaderProgram_.pipelineStages.data();

    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo_;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo_;
    pipelineCreateInfo.pTessellationState = &tessellationStateCreateInfo_;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo_;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo_;
    pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo_;
    pipelineCreateInfo.pDepthStencilState = &depthStencilStateCreateInfo_;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo_;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo_;

    pipelineCreateInfo.layout = layout_;
    pipelineCreateInfo.renderPass = VkRenderPass(*renderStage.GetRenderPass());
    pipelineCreateInfo.subpass = stage_.subPassId;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineCreateInfo.basePipelineIndex = -1;

    const VkResult res =
            vkCreateGraphicsPipelines(VkDevice(vkObj.device), vkObj.pipelineCache, 1,
                                      &pipelineCreateInfo, nullptr, &pipeline_);
    neko_assert(res == VK_SUCCESS, "Could not create graphic pipeline!")
}

void GraphicsPipeline::CreatePipelineMrt()
{
    /*auto& renderStage = GraphicsEngineLocator::Get().GetRenderStage(stage_.first);
    const auto attachmentCount = renderStage.GetAttachmentCount(stage_.second);

    std::vector<VkPipelineColorBlendAttachmentState> blendAttachmentStates;
    blendAttachmentStates.reserve(attachmentCount);

    for (uint32_t i = 0; i < attachmentCount; i++)
    {
        VkPipelineColorBlendAttachmentState blendAttachmentState = {};
        blendAttachmentState.blendEnable = VK_TRUE;
        blendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        blendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
        blendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        blendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        blendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
        blendAttachmentState.colorWriteMask =
                VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
                VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        blendAttachmentStates.emplace_back(blendAttachmentState);
    }

    colorBlendStateCreateInfo_.attachmentCount = static_cast<uint32_t>(blendAttachmentStates.size());
    colorBlendStateCreateInfo_.pAttachments = blendAttachmentStates.data();*/

    CreatePipeline();
}
}
