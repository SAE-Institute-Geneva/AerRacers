#include "vk/shaders/shader.h"

#include <utility>

#include "mathematics/hash.h"
#include "utils/file_utility.h"
#include "vk/graphics.h"

namespace neko::vk
{
Attribute::Attribute(
		std::string name,
        const std::uint32_t location,
        const std::uint32_t size,
        const Type type)
        : name_(std::move(name)),
        location_(location),
        size_(size),
        type_(type)
{}

Attribute::Attribute(const json& attributeJson)
{
	FromJson(attributeJson);
}

VkFormat Attribute::GetVkFormat() const
{
    switch (type_)
    {
        case Type::FLOAT:
            return VK_FORMAT_R32_SFLOAT;
        case Type::VEC2F:
            return VK_FORMAT_R32G32_SFLOAT;
        case Type::VEC3F:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case Type::VEC4F:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case Type::INT:
            return VK_FORMAT_R32_SINT;
        case Type::VEC2I:
            return VK_FORMAT_R32G32_SINT;
        case Type::VEC3I:
            return VK_FORMAT_R32G32B32_SINT;
        case Type::VEC4I:
            return VK_FORMAT_R32G32B32A32_SINT;
        case Type::UINT:
            return VK_FORMAT_R32_SINT;
        case Type::VEC2U:
            return VK_FORMAT_R32G32_SINT;
        case Type::VEC3U:
            return VK_FORMAT_R32G32B32_SINT;
        case Type::VEC4U:
            return VK_FORMAT_R32G32B32A32_SINT;
        case Type::MAT2:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case Type::MAT3:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case Type::MAT4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case Type::UNDEFINED:
        default:
            return VK_FORMAT_UNDEFINED;
    }
}

void Attribute::FromJson(const json& attributeJson)
{
	name_ = attributeJson["name"].get<std::string>();
	location_ = attributeJson["location"].get<std::uint32_t>();
	size_ = attributeJson["size"].get<std::uint32_t>();
	type_ = attributeJson["type"].get<Type>();
}

ordered_json Attribute::ToJson() const
{
	ordered_json attributeJson;
	attributeJson["name"] = name_;
	attributeJson["location"] = location_;
	attributeJson["size"] = size_;
	attributeJson["type"] = type_;

	return attributeJson;
}

void Shader::Init()
{
    std::map<VkDescriptorType, std::uint32_t> descriptorPoolCounts;

    // Process to descriptors.
    for (const auto& uniformBlock : uniformBlocks_)
    {
        auto descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
        switch (uniformBlock.second.GetType())
        {
            case UniformBlock::Type::UNIFORM:
                descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                descriptorSetLayoutBindings_.emplace_back(
                        UniformBuffer::GetDescriptorSetLayout(
                                static_cast<std::uint32_t>(uniformBlock.second.GetBinding()),
                                descriptorType,
                                uniformBlock.second.GetStageFlags()));
                break;
            case UniformBlock::Type::STORAGE:
                descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		        descriptorSetLayoutBindings_.emplace_back(
                        StorageBuffer::
                        GetDescriptorSetLayout(
                                static_cast<std::uint32_t>(uniformBlock.second.GetBinding()),
                                descriptorType,
                                uniformBlock.second.GetStageFlags()));
                break;
            default:
                break;
        }

        IncrementDescriptorPool(descriptorPoolCounts, descriptorType);
        descriptorLocations_.emplace(
                uniformBlock.first,
                uniformBlock.second.GetBinding());
        descriptorSizes_.emplace(
                uniformBlock.first,
                uniformBlock.second.GetSize());
    }

    //Uniform for textures
    for (const auto& uniform : uniforms_)
    {
        auto descriptorType = uniform.second.IsWriteOnly() ?
        		VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        switch (uniform.second.GetType())
        {
            case Uniform::Type::SAMPLER_2D:
                descriptorSetLayoutBindings_.emplace_back(
                        Image2d::GetDescriptorSetLayout(
                                static_cast<std::uint32_t>(uniform.second.GetBinding()),
                                descriptorType,
                                uniform.second.GetStageFlags()));
                break;
            /*case Uniform::UniformType::SAMPLER_3D:
		        descriptorSetLayoutBindings_.emplace_back(
                        ImageCube::GetDescriptorSetLayout(
                                static_cast<std::uint32_t>(uniform.second.GetBinding()),
                                descriptorType,
                                uniform.second.GetStageFlags()));
                break;*/
            default:
                break;
        }

        IncrementDescriptorPool(descriptorPoolCounts, descriptorType);
        descriptorLocations_.emplace(uniform.first, uniform.second.GetBinding());
        descriptorSizes_.emplace(uniform.first, uniform.second.GetSize());
    }

    for (const auto& descriptor : descriptorPoolCounts)
    {
        VkDescriptorPoolSize descriptorPoolSize = {};
        descriptorPoolSize.type = descriptor.first;
        descriptorPoolSize.descriptorCount = descriptor.second;
        descriptorPoolSizes_.emplace_back(descriptorPoolSize);
    }

    // FIXME: This is a AMD workaround that works on Nvidia too
    descriptorPoolSizes_ = std::vector<VkDescriptorPoolSize>(6);
    descriptorPoolSizes_[0].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    descriptorPoolSizes_[0].descriptorCount = 4096;
    descriptorPoolSizes_[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    descriptorPoolSizes_[1].descriptorCount = 2048;
    descriptorPoolSizes_[2].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    descriptorPoolSizes_[2].descriptorCount = 2048;
    descriptorPoolSizes_[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
    descriptorPoolSizes_[3].descriptorCount = 2048;
    descriptorPoolSizes_[4].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
    descriptorPoolSizes_[4].descriptorCount = 2048;
    descriptorPoolSizes_[5].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorPoolSizes_[5].descriptorCount = 2048;

    // Sort descriptors by binding.
    std::sort(descriptorSetLayoutBindings_.begin(), descriptorSetLayoutBindings_.end(),
              [](const VkDescriptorSetLayoutBinding& l,
                 const VkDescriptorSetLayoutBinding& r){return l.binding < r.binding;});

    // Gets the last descriptors binding.
    if (!descriptorSetLayoutBindings_.empty())
        lastDescriptorBinding_ = descriptorSetLayoutBindings_.back().binding;

    // Gets the descriptor type for each descriptor.
    for (const auto& descriptor : descriptorSetLayoutBindings_)
        descriptorTypes_.emplace(descriptor.binding, descriptor.descriptorType);

    // Process attribute descriptions.
    std::uint32_t currentOffset = 4;
    for (const auto& attribute : attributes_)
    {
        VkVertexInputAttributeDescription attributeDescription = {};
        attributeDescription.location = static_cast<std::uint32_t>(attribute.second.GetLocation());
        attributeDescription.binding = 0;
        attributeDescription.format = attribute.second.GetVkFormat();
        attributeDescription.offset = currentOffset;
        attributeDescriptions_.emplace_back(attributeDescription);
        currentOffset += attribute.second.GetSize();
    }
}

std::vector<VkShaderModule> Shader::LoadFromJson(const json& shaderJson)
{
	FromJson(shaderJson);

	std::vector<VkShaderModule> shaderModules(stagePaths_.size());
	const auto& config = BasicEngine::GetInstance()->config;
	for (std::size_t i = 0; i < stagePaths_.size(); ++i)
	{
		const auto& file = LoadFile(config.dataRootPath + stagePaths_[i]);

		VkShaderModuleCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = file.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(file.data());

		const VkResult res =
				vkCreateShaderModule(VkDevice(VkObjectsLocator::get().device), &createInfo, nullptr, &shaderModules[i]);
		neko_assert(res == VK_SUCCESS, "Could not create shader module!")
	}

    return shaderModules;
}

VkShaderStageFlagBits Shader::GetShaderStage(const std::string_view& filename)
{
	const size_t& size = filename.size() - std::string(".spv").size();
    const auto fileExt = GetFilenameExtension(static_cast<std::string>(filename.substr(0, size)));
    if (fileExt == ".vert") return VK_SHADER_STAGE_VERTEX_BIT;
    if (fileExt == ".frag") return VK_SHADER_STAGE_FRAGMENT_BIT;
    if (fileExt == ".geom") return VK_SHADER_STAGE_GEOMETRY_BIT;
    if (fileExt == ".comp") return VK_SHADER_STAGE_COMPUTE_BIT;
    if (fileExt == ".tese") return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    if (fileExt == ".tesc") return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;

    logDebug(fileExt + " is not a valid shader extension!");
    return VK_SHADER_STAGE_ALL;
}

void Shader::IncrementDescriptorPool(
        std::map<VkDescriptorType, std::uint32_t>& descriptorPoolCounts,
        VkDescriptorType type)
{
    if (type == VK_DESCRIPTOR_TYPE_MAX_ENUM) { return; }

    const auto it = descriptorPoolCounts.find(type);

    if (it != descriptorPoolCounts.end()) it->second++;
    else descriptorPoolCounts.emplace(type, 1);
}

void Shader::AddAttribute(const Attribute& attribute)
{
	attributes_.emplace(HashString(attribute.GetName()), attribute);
}

void Shader::AddUniform(const Uniform& uniform)
{
	uniforms_.emplace(HashString(uniform.GetName()), uniform);
}

void Shader::AddUniformBlock(const UniformBlock& uniformBlock)
{
    uniformBlocks_.emplace(HashString(uniformBlock.GetName()), uniformBlock);
}

std::vector<VkPushConstantRange> Shader::GetPushConstantRanges() const
{
    std::vector<VkPushConstantRange> pushConstantRanges;
    std::uint32_t currentOffset = 0;

    for (const auto& uniformBlock : uniformBlocks_)
    {
        if (uniformBlock.second.GetType() != UniformBlock::Type::PUSH)
            continue;

        VkPushConstantRange pushConstantRange = {};
        pushConstantRange.stageFlags = uniformBlock.second.GetStageFlags();
        pushConstantRange.offset = currentOffset;
        pushConstantRange.size = static_cast<std::uint32_t>(uniformBlock.second.GetSize());
        pushConstantRanges.emplace_back(pushConstantRange);
        currentOffset += pushConstantRange.size;
    }

    return pushConstantRanges;
}

std::uint32_t Shader::GetDescriptorLocation(const std::string_view& name) const
{
    const auto hash = HashString(name);
    const auto it = descriptorLocations_.find(hash);

    neko_assert(it != descriptorLocations_.end(), "Descriptor doesn't exist")

    return descriptorLocations_.at(hash);
}

std::uint32_t Shader::GetDescriptorLocation(const XXH64_hash_t& descriptorHash) const
{
    const auto it = descriptorLocations_.find(descriptorHash);

    //TODO This a fix made for forcing an update of a material when a descriptor(texture) is added
    // to a material that previously did not have this descriptor.
    if (it == descriptorLocations_.end()) return INVALID_INDEX;
    neko_assert(it != descriptorLocations_.end(), "Descriptor doesn't exist")

    return descriptorLocations_.at(descriptorHash);
}

VkDescriptorType Shader::GetDescriptorType(const std::uint32_t location) const
{
    const auto it = descriptorTypes_.find(location);
    neko_assert(it != descriptorTypes_.end(), "Descriptor doesn't exist")

    return descriptorTypes_.at(location);
}

const Attribute& Shader::GetAttribute(const std::string_view& name) const
{
	const auto hash = HashString(name);
	const auto it = attributes_.find(hash);
	neko_assert(it != attributes_.end(), "Attribute doesn't exist")

	return attributes_.at(hash);
}

const Attribute& Shader::GetAttribute(XXH64_hash_t descriptorHash) const
{
	const auto it = attributes_.find(descriptorHash);
	neko_assert(it != attributes_.end(), "Attribute doesn't exist")

	return attributes_.at(descriptorHash);
}

const Uniform& Shader::GetUniform(const std::string_view& name) const
{
	const auto hash = HashString(name);
	const auto it = uniforms_.find(hash);
	neko_assert(it != uniforms_.end(), "Uniform doesn't exist")

	return uniforms_.at(hash);
}

const Uniform& Shader::GetUniform(XXH64_hash_t descriptorHash) const
{
	const auto it = uniforms_.find(descriptorHash);
	neko_assert(it != uniforms_.end(), "Uniform doesn't exist")

	return uniforms_.at(descriptorHash);
}

const UniformBlock& Shader::GetUniformBlock(const std::string_view& name) const
{
    const auto hash = HashString(name);
    const auto it = uniformBlocks_.find(hash);
    neko_assert(it != uniformBlocks_.end(), "Uniform block doesn't exist")

    return uniformBlocks_.at(hash);
}

const UniformBlock& Shader::GetUniformBlock(XXH64_hash_t descriptorHash) const
{
    const auto it = uniformBlocks_.find(descriptorHash);
    neko_assert(it != uniformBlocks_.end(), "Uniform block doesn't exist")

    return uniformBlocks_.at(descriptorHash);
}

void Shader::LoadAttribute(const json& attributeJson)
{
	const auto name = attributeJson["name"].get<std::string>();

	Attribute attribute(attributeJson);
	attributes_.emplace(HashString(name), attribute);
}

void Shader::LoadUniform(const json& uniformJson)
{
	const auto name = uniformJson["name"].get<std::string>();

	Uniform uniform(uniformJson);
	uniforms_.emplace(HashString(name), uniform);
}

void Shader::LoadUniformBlock(const json& uniformBlockJson)
{
	const auto name = uniformBlockJson["name"].get<std::string>();

	UniformBlock uniformBlock(uniformBlockJson);
	uniformBlocks_.emplace(HashString(name), uniformBlock);
}

void Shader::FromJson(const json& shaderJson)
{
	if (CheckJsonExists(shaderJson, "vert"))
		stagePaths_.emplace_back(shaderJson["vert"].get<std::string>());

	if (CheckJsonExists(shaderJson, "frag"))
		stagePaths_.emplace_back(shaderJson["frag"].get<std::string>());

	if (CheckJsonExists(shaderJson, "geom"))
		stagePaths_.emplace_back(shaderJson["geom"].get<std::string>());

	if (CheckJsonExists(shaderJson, "comp"))
		stagePaths_.emplace_back(shaderJson["comp"].get<std::string>());

	if (CheckJsonExists(shaderJson, "tesc"))
		stagePaths_.emplace_back(shaderJson["tesc"].get<std::string>());

	if (CheckJsonExists(shaderJson, "tese"))
		stagePaths_.emplace_back(shaderJson["tese"].get<std::string>());

	if (CheckJsonExists(shaderJson, "attributes"))
		for (const auto& attributeJson : shaderJson["attributes"])
			LoadAttribute(attributeJson);

	if (CheckJsonExists(shaderJson, "uniforms"))
		for (const auto& uniformJson : shaderJson["uniforms"])
			LoadUniform(uniformJson);

	if (CheckJsonExists(shaderJson, "uniformBlocks"))
		for (const auto& uniformBlockJson : shaderJson["uniformBlocks"])
			LoadUniformBlock(uniformBlockJson);
}

ordered_json Shader::ToJson() const
{
	ordered_json shaderJson;
	for (const auto& stagePath : stagePaths_)
	{
		const auto& stage = GetShaderStage(stagePath);
		switch (stage)
		{
			case VK_SHADER_STAGE_VERTEX_BIT: shaderJson["vert"] = stagePath; break;
			case VK_SHADER_STAGE_FRAGMENT_BIT: shaderJson["frag"] = stagePath; break;
			case VK_SHADER_STAGE_GEOMETRY_BIT: shaderJson["geom"] = stagePath; break;
			case VK_SHADER_STAGE_COMPUTE_BIT: shaderJson["comp"] = stagePath; break;
			case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT: shaderJson["tesc"] = stagePath; break;
			case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT: shaderJson["tese"] = stagePath; break;
			default: logDebug(stagePath + " isn't a valid shader stage!");
		}
	}

	for (const auto& attribute : attributes_)
	{
		shaderJson["attributes"].emplace_back(attribute.second.ToJson());
	}

	for (const auto& uniform : uniforms_)
	{
		shaderJson["uniforms"].emplace_back(uniform.second.ToJson());
	}

	for (const auto& uniformBlocks : uniformBlocks_)
	{
		shaderJson["uniformBlocks"].emplace_back(uniformBlocks.second.ToJson());
	}

	return shaderJson;
}
}
