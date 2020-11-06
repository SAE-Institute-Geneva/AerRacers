#include "vk/shaders/shader.h"

#include <utility>

#include "mathematics/hash.h"
#include "utilities/file_utility.h"
#include "vk/graphics.h"

namespace neko::vk
{
Attribute::Attribute(
        const std::uint32_t set,
        const std::uint32_t location,
        const std::uint32_t size,
        const AttributeType type)
        : set_(set),
        location_(location),
        size_(size),
        type_(type)
{}

VkFormat Attribute::GetVkFormat() const
{
    switch (type_)
    {
        case AttributeType::FLOAT:
            return VK_FORMAT_R32_SFLOAT;
        case AttributeType::VEC2F:
            return VK_FORMAT_R32G32_SFLOAT;
        case AttributeType::VEC3F:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case AttributeType::VEC4F:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case AttributeType::INT:
            return VK_FORMAT_R32_SINT;
        case AttributeType::VEC2I:
            return VK_FORMAT_R32G32_SINT;
        case AttributeType::VEC3I:
            return VK_FORMAT_R32G32B32_SINT;
        case AttributeType::VEC4I:
            return VK_FORMAT_R32G32B32A32_SINT;
        case AttributeType::UINT:
            return VK_FORMAT_R32_SINT;
        case AttributeType::VEC2U:
            return VK_FORMAT_R32G32_SINT;
        case AttributeType::VEC3U:
            return VK_FORMAT_R32G32B32_SINT;
        case AttributeType::VEC4U:
            return VK_FORMAT_R32G32B32A32_SINT;
        case AttributeType::UNDEFINED:
        default:
            return VK_FORMAT_UNDEFINED;
    }
}

Shader::Shader(std::string filename, VkShaderStageFlags stages)
    : shaderPath_(std::move(filename)), stages_(stages)
{}

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
                /*case UniformBlock::Type::STORAGE:
                    descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    descriptorSetLayout_.emplace_back(
                            StorageBuffer::
                            GetDescriptorSetLayout(
                                    static_cast<std::uint32_t>(uniformBlock.second.GetBinding()),
                                    descriptorType,
                                    uniformBlock.second.GetStageFlags()));
                    break;*/
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
    /*for (const auto& uniform : uniform_)
    {
        auto descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
        switch (uniform.second.GetGlType())
        {
            case 0x8B5E: // GL_SAMPLER_2D
            case 0x904D: // GL_IMAGE_2D
            case 0x9108: // GL_SAMPLER_2D_MULTISAMPLE
            case 0x9055: // GL_IMAGE_2D_MULTISAMPLE
                descriptorType = uniform.second.IsWriteOnly() ?
                                 VK_DESCRIPTOR_TYPE_STORAGE_IMAGE :
                                 VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorSetLayout_.emplace_back(
                        Image2d::GetDescriptorSetLayout(
                                static_cast<std::uint32_t>(uniform.second.GetBinding()),
                                descriptorType,
                                uniform.second.GetStageFlags()));
                break;
            case 0x8B60: // GL_SAMPLER_CUBE
            case 0x9050: // GL_IMAGE_CUBE
                descriptorType = uniform.second.IsWriteOnly() ?
                                 VK_DESCRIPTOR_TYPE_STORAGE_IMAGE :
                                 VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                descriptorSetLayout_.emplace_back(
                        ImageCube::GetDescriptorSetLayout(
                                static_cast<std::uint32_t>(uniform.second.GetBinding()),
                                descriptorType,
                                uniform.second.GetStageFlags()));
                break;
            default:
                break;
        }

        IncrementDescriptorPool(descriptorPoolCounts, descriptorType);
        descriptorLocations_.emplace(uniform.first, uniform.second.GetBinding());
        descriptorSizes_.emplace(uniform.first, uniform.second.GetSize());
    }*/

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

void Shader::IncrementDescriptorPool(
        std::map<VkDescriptorType, std::uint32_t>& descriptorPoolCounts,
        VkDescriptorType type)
{
    if (type == VK_DESCRIPTOR_TYPE_MAX_ENUM) { return; }

    const auto it = descriptorPoolCounts.find(type);

    if (it != descriptorPoolCounts.end()) it->second++;
    else descriptorPoolCounts.emplace(type, 1);
}

Shader::ShaderProgram Shader::CreateShaderProgram() const
{
    neko_assert(stages_ != 0, "No stages for this shader!")

    std::vector<std::string> extensions;
    if (stages_ & VK_SHADER_STAGE_VERTEX_BIT)
        extensions.emplace_back(".vert.spv");
    if (stages_ & VK_SHADER_STAGE_FRAGMENT_BIT)
        extensions.emplace_back(".frag.spv");

    ShaderProgram shaderProgram;
    shaderProgram.modules.resize(extensions.size());
    shaderProgram.pipelineStages.resize(extensions.size());
    for (size_t i = 0; i < extensions.size(); ++i)
    {
        const auto file = LoadBinaries(shaderPath_ + extensions[i]);

        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = file.size();
        createInfo.pCode = reinterpret_cast<const std::uint32_t*>(file.data());

        const auto& vkObj = VkObjectsLocator::get();

        const VkResult res = vkCreateShaderModule(VkDevice(vkObj.device), &createInfo,
                                                  nullptr, &shaderProgram.modules[i]);
        neko_assert(res == VK_SUCCESS, "Could not create shader module!")

        VkShaderStageFlagBits stage;
        if (extensions[i] == ".vert.spv")
            stage = VK_SHADER_STAGE_VERTEX_BIT;
        if (extensions[i] == ".frag.spv")
            stage = VK_SHADER_STAGE_FRAGMENT_BIT;

        shaderProgram.pipelineStages[i].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderProgram.pipelineStages[i].stage = stage;
        shaderProgram.pipelineStages[i].module = shaderProgram.modules[i];
        shaderProgram.pipelineStages[i].pName = "main";
    }

    return shaderProgram;
}

void Shader::AddUniformBlock(XXH64_hash_t nameHash, const UniformBlock& uniformBlock)
{
    uniformBlocks_.emplace(nameHash, uniformBlock);
}

void Shader::AddUniform(XXH64_hash_t nameHash, const Uniform& uniform)
{
    uniforms_.emplace(nameHash, uniform);
}

void Shader::AddAttribute(XXH64_hash_t nameHash, const Attribute& attribute)
{
    attributes_.emplace(nameHash, attribute);
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

    if(it == descriptorLocations_.end())
        std::cout << "Shader::GetDescriptorLocation\n";

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

const UniformBlock& Shader::GetUniformBlock(const std::string_view& name) const
{
    const auto hash = HashString(name);
    const auto it = uniformBlocks_.find(hash);
    neko_assert(it != uniformBlocks_.end(), "Uniform block doesn't exist");

    return uniformBlocks_.at(hash);
}

const UniformBlock& Shader::GetUniformBlock(XXH64_hash_t descriptorHash) const
{
    const auto it = uniformBlocks_.find(descriptorHash);
    neko_assert(it != uniformBlocks_.end(), "Uniform block doesn't exist");

    return uniformBlocks_.at(descriptorHash);
}
}
