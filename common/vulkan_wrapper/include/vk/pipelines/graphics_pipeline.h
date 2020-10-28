#pragma once
#include "vk/descriptors/descriptor_set.h"
#include "vk/framebuffers/swapchain.h"
#include "vk/framebuffers/renderpass.h"
#include "vk/models/mesh.h"
#include "vk/pipelines/pipeline.h"

#include <utility>
#include "vk/shaders/shader.h"

namespace neko::vk
{
const std::array<VkDynamicState, 3> kDynamicStates =
{
    VK_DYNAMIC_STATE_VIEWPORT,
    VK_DYNAMIC_STATE_SCISSOR,
    VK_DYNAMIC_STATE_LINE_WIDTH
};

class GraphicsPipeline : public Pipeline
{
public:
    enum class Mode : uint8_t
    {
        POLYGON = 0,
        MRT
    };

    enum class Depth : uint8_t
    {
        NONE = 0,
        READ = 1,
        WRITE = 2,
        READ_WRITE = READ | WRITE
    };

    void Init(Stage pipelineStage,
            const Shader& shader,
            const std::vector<VertexInput>& vertexInputs,
            Mode mode, Depth depthMode,
            VkPrimitiveTopology topology,
            VkPolygonMode polygonMode,
            VkCullModeFlags cullMode,
            VkFrontFace frontFace,
            bool pushDescriptors);
    void Destroy() const;

    explicit operator const VkPipeline &() const { return pipeline_; }
    [[nodiscard]] const VkPipeline& GetGraphicsPipeline() const { return pipeline_; }
    [[nodiscard]] const VkPipelineLayout& GetLayout() const { return layout_; }

    [[nodiscard]] const Pipeline::Stage& GetStage() const { return stage_; }
    [[nodiscard]] bool IsPushDescriptor() const override { return pushDescriptors_; }
    [[nodiscard]] const Shader& GetShader() const override { return shader_; }
    [[nodiscard]] const VkDescriptorSetLayout& GetDescriptorSetLayout() const override { return descriptorSetLayout_; }
    [[nodiscard]] const VkDescriptorPool& GetDescriptorPool() const override { return descriptorPool_; }
    [[nodiscard]] const VkPipeline& GetPipeline() const override { return pipeline_; }
    [[nodiscard]] const VkPipelineLayout& GetPipelineLayout() const override { return layout_; }
    [[nodiscard]] VkPipelineBindPoint GetPipelineBindPoint() const override { return pipelineBindPoint_; }

private:
    void CreateShaderProgram();
    void CreateDescriptorLayout();
    void CreateDescriptorPool();
    void CreatePipelineLayout();
    void CreateAttributes();
    void CreatePipeline();
    void CreatePipelineMrt();

    VkDescriptorSetLayout descriptorSetLayout_{};
    VkDescriptorPool descriptorPool_{};

    VkPipeline pipeline_{};
    VkPipelineLayout layout_{};
    VkPipelineBindPoint pipelineBindPoint_{};

    Stage stage_{};
    std::vector<VertexInput> vertexInputs_{};
    Mode mode_{};
    Depth depthMode_{};
    VkPrimitiveTopology topology_{};
    VkPolygonMode polygonMode_{};
    VkCullModeFlags cullMode_{};
    VkFrontFace frontFace_{};
    bool pushDescriptors_{};

    Shader shader_{};
    Shader::ShaderProgram shaderProgram_{};

    VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo_{};
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo_{};
    VkPipelineViewportStateCreateInfo viewportStateCreateInfo_{};
    VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo_{};
    VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo_{};
    VkPipelineColorBlendAttachmentState colorBlendAttachmentState_{};
    VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo_{};
    VkPipelineDepthStencilStateCreateInfo depthStencilStateCreateInfo_{};
    VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo_{};
    VkPipelineTessellationStateCreateInfo tessellationStateCreateInfo_{};
};

struct GraphicsPipelineCreateInfo
{
    explicit GraphicsPipelineCreateInfo(
            Shader shader,
            std::vector<VertexInput> vertexInputs = {},
            const GraphicsPipeline::Mode mode = GraphicsPipeline::Mode::POLYGON,
            const GraphicsPipeline::Depth depth = GraphicsPipeline::Depth::READ_WRITE,
            const VkPrimitiveTopology topology =
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
            const VkPolygonMode polygonMode = VK_POLYGON_MODE_FILL,
            const VkCullModeFlags cullMode = VK_CULL_MODE_BACK_BIT,
            const VkFrontFace frontFace = VK_FRONT_FACE_CLOCKWISE,
            const bool pushDescriptors = false)
            : shader(std::move(shader)),
              vertexInputs(std::move(vertexInputs)),
              mode(mode),
              depth(depth),
              topology(topology),
              polygonMode(polygonMode),
              cullMode(cullMode),
              frontFace(frontFace),
              isPushDescriptor(pushDescriptors)
    {
    }

    bool operator==(const GraphicsPipelineCreateInfo& other) const
    {
        return mode == other.mode &&
               depth == other.depth &&
               topology == other.topology &&
               polygonMode == other.polygonMode &&
               cullMode == other.cullMode &&
               frontFace == other.frontFace &&
               isPushDescriptor == other.isPushDescriptor;
    }

    bool operator!=(const GraphicsPipelineCreateInfo& other) const
    {
        return !(*this == other);
    }

    Shader shader;
    std::vector<VertexInput> vertexInputs;

    GraphicsPipeline::Mode mode;
    GraphicsPipeline::Depth depth;
    VkPrimitiveTopology topology;
    VkPolygonMode polygonMode;
    VkCullModeFlags cullMode;
    VkFrontFace frontFace;
    bool isPushDescriptor;
};
}