#pragma once
#include "graphics/shader.h"
#include "vk/shaders/uniform.h"

namespace neko::vk
{
//Represents an input attribute (e.g.: vertex positions, uv, normals...)
struct Attribute
{
    enum class AttributeType
    {
        UNDEFINED = 0,
        FLOAT, VEC2F, VEC3F, VEC4F,
        INT, VEC2I, VEC3I, VEC4I,
        UINT, VEC2U, VEC3U, VEC4U,
    };

    explicit Attribute(
            uint32_t set = INVALID_INDEX,
            uint32_t location = INVALID_INDEX,
            uint32_t size = INVALID_INDEX,
            AttributeType type = AttributeType::UNDEFINED);

    [[nodiscard]] uint32_t GetSet() const { return set_; }
    [[nodiscard]] uint32_t GetLocation() const { return location_; }
    [[nodiscard]] uint32_t GetSize() const { return size_; }
    [[nodiscard]] AttributeType GetType() const { return type_; }

    [[nodiscard]] VkFormat GetVkFormat() const;

private:
    uint32_t set_ = INVALID_INDEX;
    uint32_t location_ = INVALID_INDEX;
    uint32_t size_ = INVALID_INDEX;
    AttributeType type_ = AttributeType::UNDEFINED;
};

class Shader final
{
public:
    struct ShaderProgram
    {
        std::vector<VkShaderModule> modules;
        std::vector<VkPipelineShaderStageCreateInfo> pipelineStages;
    };

    explicit Shader() = default;
    explicit Shader(std::string filename, VkShaderStageFlags stages);

	void Init();

	[[nodiscard]] ShaderProgram CreateShaderProgram() const;

    [[nodiscard]] const std::string& GetShaderPath() const { return shaderPath_; }
    [[nodiscard]] const std::vector<VkDescriptorSetLayoutBinding>&
            GetDescriptorSetLayoutBindings() const { return descriptorSetLayoutBindings_; }
    [[nodiscard]] const std::vector<VkDescriptorPoolSize>&
            GetDescriptorPoolSizes() const { return descriptorPoolSizes_; }
    [[nodiscard]] std::vector<VkPushConstantRange> GetPushConstantRanges() const;

    [[nodiscard]] uint32_t GetDescriptorLocation(const std::string_view& name) const;
    [[nodiscard]] uint32_t GetDescriptorLocation(const XXH64_hash_t& descriptorHash) const;
    [[nodiscard]] VkDescriptorType GetDescriptorType(uint32_t location) const;

    [[nodiscard]] const UniformBlock& GetUniformBlock(const std::string_view& name) const;
    [[nodiscard]] const UniformBlock& GetUniformBlock(XXH64_hash_t descriptorHash) const;

    void AddUniformBlock(XXH64_hash_t nameHash, const UniformBlock& uniformBlock);
    void AddUniform(XXH64_hash_t nameHash, const Uniform& uniform);
    void AddAttribute(XXH64_hash_t nameHash, const Attribute& attribute);

private:
    static void IncrementDescriptorPool(
            std::map<VkDescriptorType, uint32_t>& descriptorPoolCounts,
            VkDescriptorType type);

    std::string shaderPath_{};
    VkShaderStageFlags stages_{};

    std::map<XXH64_hash_t, UniformBlock> uniformBlocks_{};
    std::map<XXH64_hash_t, Uniform> uniforms_{};
    std::map<XXH64_hash_t, Attribute> attributes_{};

    std::map<XXH64_hash_t, uint32_t> descriptorLocations_{};
    std::map<XXH64_hash_t, uint32_t> descriptorSizes_{};

    uint32_t lastDescriptorBinding_ = 0;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions_{};
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings_{};
    std::vector<VkDescriptorPoolSize> descriptorPoolSizes_{};
    std::map<uint32_t, VkDescriptorType> descriptorTypes_{};
};
}
