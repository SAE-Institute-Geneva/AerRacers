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
#include "mathematics/hash.h"

#include "vk/shaders/attribute.h"
#include "vk/shaders/uniform_block.h"

namespace neko::vk
{
class Shader final
{
public:
	Shader() = default;

	void Init();
	[[nodiscard]] std::vector<VkShaderModule> LoadFromJson(const json& shaderJson);

	[[nodiscard]] const std::vector<VkDescriptorSetLayoutBinding>& GetDescriptorSetLayoutBindings()
		const
	{
		return descriptorSetLayoutBindings_;
	}

	[[nodiscard]] const std::vector<VkDescriptorPoolSize>& GetDescriptorPoolSizes() const
	{
		return descriptorPoolSizes_;
	}

	[[nodiscard]] std::vector<VkPushConstantRange> GetPushConstantRanges() const;

	[[nodiscard]] std::uint32_t GetDescriptorLocation(const std::string_view& name) const;
	[[nodiscard]] std::uint32_t GetDescriptorLocation(const StringHash& descriptorHash) const;
	[[nodiscard]] VkDescriptorType GetDescriptorType(std::uint32_t location) const;

	void SetStagePaths(const std::vector<std::string>& stagePaths) { stagePaths_ = stagePaths; }
	[[nodiscard]] const std::vector<std::string>& GetStagePaths() const { return stagePaths_; }

	[[nodiscard]] const std::map<StringHash, Attribute>& GetAttributes() const
	{
		return attributes_;
	}

	[[nodiscard]] const std::map<StringHash, Uniform>& GetUniforms() const { return uniforms_; }
	[[nodiscard]] const std::map<StringHash, UniformBlock>& GetUniformBlocks() const
	{
		return uniformBlocks_;
	}

	[[nodiscard]] const Attribute& GetAttribute(const std::string_view& name) const;
	[[nodiscard]] const Attribute& GetAttribute(StringHash descriptorHash) const;
	[[nodiscard]] const Uniform& GetUniform(const std::string_view& name) const;
	[[nodiscard]] const Uniform& GetUniform(StringHash descriptorHash) const;
	[[nodiscard]] const UniformBlock& GetUniformBlock(const std::string_view& name) const;
	[[nodiscard]] const UniformBlock& GetUniformBlock(StringHash descriptorHash) const;

	void AddUniformBlock(const UniformBlock& uniformBlock);
	void AddUniform(const Uniform& uniform);
	void AddAttribute(const Attribute& attribute);

	[[nodiscard]] static VkShaderStageFlagBits GetShaderStage(const std::string_view& filename);

	void FromJson(const json& shaderJson);
	[[nodiscard]] ordered_json ToJson() const;

private:
	static void IncrementDescriptorPool(
		std::map<VkDescriptorType, std::uint32_t>& descriptorPoolCounts, VkDescriptorType type);

	void LoadAttribute(const json& attributeJson);
	void LoadUniform(const json& uniformJson);
	void LoadUniformBlock(const json& uniformBlockJson);

	std::vector<std::string> stagePaths_;
	std::map<StringHash, Attribute> attributes_ {};
	std::map<StringHash, Uniform> uniforms_ {};
	std::map<StringHash, UniformBlock> uniformBlocks_ {};

	std::map<StringHash, std::uint32_t> descriptorLocations_ {};
	std::map<StringHash, std::uint32_t> descriptorSizes_ {};

	std::uint32_t lastDescriptorBinding_ = 0;
	std::vector<VkVertexInputAttributeDescription> attributeDescriptions_ {};
	std::vector<VkDescriptorSetLayoutBinding> descriptorSetLayoutBindings_ {};
	std::vector<VkDescriptorPoolSize> descriptorPoolSizes_ {};
	std::map<std::uint32_t, VkDescriptorType> descriptorTypes_ {};
};
}    // namespace neko::vk
