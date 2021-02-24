#include "vk/models/mesh_instance.h"

namespace neko::vk
{
VertexInput MeshInstance::Instance::GetVertexInput(uint32_t baseBinding)
{
	VkVertexInputBindingDescription bindingDescription {};
	bindingDescription.binding   = baseBinding;
	bindingDescription.stride    = sizeof(Instance);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

	const std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
		{0, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Instance, modelMatrix)},
		{1, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Instance, modelMatrix) + 16},
		{2, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Instance, modelMatrix) + 32},
		{3, baseBinding, VK_FORMAT_R32G32B32A32_SFLOAT, offsetof(Instance, modelMatrix) + 48},
	};

	return VertexInput(0, bindingDescription, attributeDescriptions);
}

MeshInstance::MeshInstance(const Mesh& mesh, const Material& material)
   : kMesh_(mesh),
	 kMaterial_(material),
	 instanceBuffer_(sizeof(Instance) * kMaxInstances),
	 uniformObject_(false)
{}

std::unique_ptr<MeshInstance> MeshInstance::Create(const Mesh& mesh, const Material& material)
{
	return std::make_unique<MeshInstance>(mesh, material);
}

void MeshInstance::Update(std::vector<Mat4f>& modelMatrices)
{
	maxInstances_ = kMaxInstances;
	instances_    = 0;

	if (modelMatrices.empty()) return;

	Instance* instances;
	instanceBuffer_.MapMemory(reinterpret_cast<char**>(&instances));

	for (auto& modelMatrix : modelMatrices)
	{
		if (instances_ >= maxInstances_) break;

		auto instance         = &instances[instances_];
		instance->modelMatrix = modelMatrix;
		instances_++;
	}

	instanceBuffer_.UnmapMemory();
}

bool MeshInstance::CmdRender(const CommandBuffer& commandBuffer, UniformHandle& uniformScene)
{
	if (instances_ == 0) return false;    //No instances

	const auto& materialPipeline = kMaterial_.GetPipelineMaterial();

	if (!kMaterial_.BindPipeline(commandBuffer)) return false;

	const auto& pipeline = materialPipeline.GetPipeline();

	//Push uniforms to shader
	uniformObject_.PushUniformData(kMaterial_.ExportUniformData());

	//Push texture to shader
	descriptorSet_.Push(kUboObjectHash, uniformObject_);
	descriptorSet_.Push(kUboSceneHash, uniformScene);

	descriptorSet_.PushDescriptorData(kMaterial_.ExportDescriptorData());

	if (!descriptorSet_.Update(pipeline)) return false;

	descriptorSet_.BindDescriptor(commandBuffer, pipeline);

	VkBuffer vertexBuffers[] = {kMesh_.GetVertexBuffer(), instanceBuffer_};
	VkDeviceSize offset[]    = {0, 0};

	//Bind buffers
	vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offset);
	vkCmdBindIndexBuffer(commandBuffer, kMesh_.GetIndexBuffer(), 0, Mesh::GetIndexType());

	//Draw the instances
	vkCmdDrawIndexed(commandBuffer, kMesh_.GetIndexCount(), instances_, 0, 0, 0);

	return true;
}
}    // namespace neko::vk