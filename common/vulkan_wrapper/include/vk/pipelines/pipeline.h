#pragma once
#include "vk/commands/command_buffer.h"
#include "vk/shaders/shader.h"

namespace neko::vk
{
enum class SubrendererIndex : uint8_t
{
    DEFERRED = 0,
    GIZMO,
    IMGUI,
    OPAQUE,
    PARTICLES,
    TRANSPARENT,
    FILTER_DEFAULT,
    FILTER_BLUR,
    FILTER_FXAA,
    FILTER_RIPPLE
};

template <typename T>
class HasGetSubrendererIndex
{
private:
    typedef char YesType[1];
    typedef char NoType[2];

    template <typename C> static YesType& test(decltype(&C::GetSubrendererIndex));
    template <typename C> static NoType& test(...);

public:
    enum { value = sizeof(test<T>(0)) == sizeof(YesType) };
};

class Pipeline
{
public:
    struct Stage
    {
        uint32_t renderPassId = INVALID_INDEX;
        uint32_t subPassId = INVALID_INDEX;

        Stage() = default;
        Stage(uint32_t renderPassId, uint32_t subPassId)
            : renderPassId(renderPassId), subPassId(subPassId) {}

        bool operator==(const Stage& right) const
        {
            return renderPassId == right.renderPassId && subPassId == right.subPassId;
        }

        bool operator!=(const Stage& right) const { return !(*this == right); }

        bool operator<(const Stage& right) const
        {
            return renderPassId < right.renderPassId ||
                   (right.renderPassId >= renderPassId && subPassId < right.subPassId);
        }

        bool operator>(const Stage& right) const { return right < *this; }
    };

    explicit Pipeline() = default;
    virtual ~Pipeline() = default;

    void BindPipeline(const CommandBuffer& commandBuffer) const
    {
        vkCmdBindPipeline(VkCommandBuffer(commandBuffer), GetPipelineBindPoint(), GetPipeline());
    }

    [[nodiscard]] virtual bool IsPushDescriptor() const = 0;
    [[nodiscard]] virtual const Shader& GetShader() const = 0;
    [[nodiscard]] virtual const VkDescriptorSetLayout& GetDescriptorSetLayout() const = 0;
    [[nodiscard]] virtual const VkDescriptorPool& GetDescriptorPool() const = 0;
    [[nodiscard]] virtual const VkPipeline& GetPipeline() const = 0;
    [[nodiscard]] virtual const VkPipelineLayout& GetPipelineLayout() const = 0;
    [[nodiscard]] virtual VkPipelineBindPoint GetPipelineBindPoint() const = 0;
};

class RenderPipeline
{
public:
    explicit RenderPipeline(const Pipeline::Stage stage) : stage_(stage), enabled_(true) {}
    virtual ~RenderPipeline() = default;

    virtual void OnRender(const CommandBuffer& commandBuffer) = 0;

    [[nodiscard]] const Pipeline::Stage& GetStage() const { return stage_; }

    [[nodiscard]] bool IsEnabled() const { return enabled_; }
    void SetEnabled(const bool& enable) { enabled_ = enable; }

private:
    Pipeline::Stage stage_;
    bool enabled_;
};
}