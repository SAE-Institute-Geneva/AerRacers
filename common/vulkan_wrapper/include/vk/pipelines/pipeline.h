#pragma once
#include <cstdint>

#include "vk/commands/command_buffer.h"
#include "vk/shaders/shader.h"

namespace neko::vk
{
enum class SubrendererIndex : uint8_t
{
    DEFERRED = 0,
    GIZMO,
    IMGUI,
    OPAQUE_S,
    PARTICLES,
    TRANSPARENT_S,
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

class Shader;
class Pipeline
{
public:
    struct Stage
    {
        uint32_t renderPassId = 0;
        uint32_t subPassId = 0;

        Stage() = default;
        Stage(const uint32_t renderPassId, const uint32_t subPassId)
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
}