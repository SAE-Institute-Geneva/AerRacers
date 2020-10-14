#pragma once
#include "graphics/shader.h"

namespace neko::vk
{
class Shader final : public neko::Shader
{
public:
	void LoadFromFile(std::string_view vertexShaderPath, std::string_view fragmentShaderPath) override;

	void InitUbo(const VkDeviceSize& uboSize);
	void Recreate();
    void Destroy() override;

    template<class UboType>
    void UpdateUniformBuffer(uint32_t currentImage, const UboType& bufferData) const
    {
        uniformBuffers_[currentImage].Update(bufferData);
    }

    [[nodiscard]] const DescriptorSets& GetDescriptorSets() const { return descriptorSets_; }
    [[nodiscard]] const GraphicsPipeline& GetPipeline() const { return pipeline_; }
    [[nodiscard]] const std::vector<UniformBuffer>& GetUniformBuffers() const
    { return uniformBuffers_; }

private:
    DescriptorSets descriptorSets_;
	GraphicsPipeline pipeline_;
    std::vector<UniformBuffer> uniformBuffers_{};

    std::string_view vertShaderPath_;
    std::string_view fragShaderPath_;

    VkDeviceSize uboSize_;
};
}
