#pragma once
#include <string>
#include <utility>
#include <vector>

//#include "graphics/shader.h
#include "vk/shaders/uniform_block.h"

namespace neko::vk
{
//Represents a shader attribute (e.g.: vertex positions, uv, normals...)
struct Attribute
{
    enum class Type
    {
        UNDEFINED = 0,
        FLOAT, VEC2F, VEC3F, VEC4F,
        INT, VEC2I, VEC3I, VEC4I,
        UINT, VEC2U, VEC3U, VEC4U,
        MAT2, MAT3, MAT4
    };

    explicit Attribute(
		    std::string name = "",
		    std::uint32_t location = INVALID_INDEX,
		    std::uint32_t size = INVALID_INDEX,
		    Type type = Type::UNDEFINED);

	explicit Attribute(const json& attributeJson);

	[[nodiscard]] const std::string& GetName() const { return name_; }
    [[nodiscard]] std::uint32_t GetLocation() const { return location_; }
    [[nodiscard]] std::uint32_t GetSize() const { return size_; }
    [[nodiscard]] Type GetType() const { return type_; }

    [[nodiscard]] VkFormat GetVkFormat() const;

	void FromJson(const json& attributeJson);
	[[nodiscard]] ordered_json ToJson() const;

private:
	std::string name_;
    std::uint32_t location_ = INVALID_INDEX;
    std::uint32_t size_ = INVALID_INDEX;
    Type type_ = Type::UNDEFINED;
};

class Shader final
{
public:
	explicit Shader() = default;

	void Init();
	[[nodiscard]] std::vector<VkShaderModule> LoadFromJson(const json& shaderJson);

    [[nodiscard]] const std::vector<VkDescriptorSetLayoutBinding>&
            GetDescriptorSetLayoutBindings() const { return descriptorSetLayoutBindings_; }
    [[nodiscard]] const std::vector<VkDescriptorPoolSize>&
            GetDescriptorPoolSizes() const { return descriptorPoolSizes_; }
    [[nodiscard]] std::vector<VkPushConstantRange> GetPushConstantRanges() const;

    [[nodiscard]] std::uint32_t GetDescriptorLocation(const std::string_view& name) const;
    [[nodiscard]] std::uint32_t GetDescriptorLocation(const XXH64_hash_t& descriptorHash) const;
    [[nodiscard]] VkDescriptorType GetDescriptorType(std::uint32_t location) const;

	void SetStagePaths(const std::vector<std::string>& stagePaths) { stagePaths_ = stagePaths; }
	[[nodiscard]] const std::vector<std::string>& GetStagePaths() const { return stagePaths_; }
	[[nodiscard]] const std::map<XXH64_hash_t, Attribute>& GetAttributes() const { return attributes_; }
	[[nodiscard]] const std::map<XXH64_hash_t, Uniform>& GetUniforms() const { return uniforms_; }
	[[nodiscard]] const std::map<XXH64_hash_t, UniformBlock>& GetUniformBlocks() const { return uniformBlocks_; }

    [[nodiscard]] const Attribute& GetAttribute(const std::string_view& name) const;
    [[nodiscard]] const Attribute& GetAttribute(XXH64_hash_t descriptorHash) const;
    [[nodiscard]] const Uniform& GetUniform(const std::string_view& name) const;
    [[nodiscard]] const Uniform& GetUniform(XXH64_hash_t descriptorHash) const;
    [[nodiscard]] const UniformBlock& GetUniformBlock(const std::string_view& name) const;
    [[nodiscard]] const UniformBlock& GetUniformBlock(XXH64_hash_t descriptorHash) const;

    void AddUniformBlock(const UniformBlock& uniformBlock);
    void AddUniform(const Uniform& uniform);
    void AddAttribute(const Attribute& attribute);

	static VkShaderStageFlagBits GetShaderStage(const std::string_view& filename);

	void FromJson(const json& shaderJson);
	[[nodiscard]] ordered_json ToJson() const;

private:
    static void IncrementDescriptorPool(
            std::map<VkDescriptorType, std::uint32_t>& descriptorPoolCounts,
            VkDescriptorType type);

	void LoadAttribute(const json& attributeJson);
	void LoadUniform(const json& uniformJson);
	void LoadUniformBlock(const json& uniformBlockJson);

	std::vector<std::string> stagePaths_;
	std::map<XXH64_hash_t, Attribute> attributes_{};
	std::map<XXH64_hash_t, Uniform> uniforms_{};
    std::map<XXH64_hash_t, UniformBlock> uniformBlocks_{};

    std::map<XXH64_hash_t, std::uint32_t> descriptorLocations_{};
    std::map<XXH64_hash_t, std::uint32_t> descriptorSizes_{};

    std::uint32_t lastDescriptorBinding_ = 0;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions_{};
    std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings_{};
    std::vector<VkDescriptorPoolSize> descriptorPoolSizes_{};
    std::map<std::uint32_t, VkDescriptorType> descriptorTypes_{};
};
}
