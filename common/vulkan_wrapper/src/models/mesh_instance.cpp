#include "vk/models/mesh_instance.h"

#include "vk/material/material_manager.h"

namespace neko::vk
{
VertexInput ModelInstance::Instance::GetVertexInput(uint32_t baseBinding)
{
	VkVertexInputBindingDescription bindingDescription {};
	bindingDescription.binding   = baseBinding;
	bindingDescription.stride    = sizeof(Instance);
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;

	const VkFormat format = VK_FORMAT_R32G32B32A32_SFLOAT;
	const std::vector<VkVertexInputAttributeDescription> attributeDescriptions = {
		{0, baseBinding, format, offsetof(Instance, modelMatrix)},
		{1, baseBinding, format, offsetof(Instance, modelMatrix) + sizeof(Vec4f)},
		{2, baseBinding, format, offsetof(Instance, modelMatrix) + 2 * sizeof(Vec4f)},
		{3, baseBinding, format, offsetof(Instance, modelMatrix) + 3 * sizeof(Vec4f)},
	};

	return VertexInput(0, bindingDescription, attributeDescriptions);
}

ModelInstance::ModelInstance(const ModelId& modelId)
   : modelId_(modelId),
	 instanceBuffer_(sizeof(Instance) * kMaxInstances),
	 uniformObject_(false)
{}

std::unique_ptr<ModelInstance> ModelInstance::Create(const ModelId& modelId)
{
	return std::make_unique<ModelInstance>(modelId);
}

void ModelInstance::Update(std::vector<Mat4f>& modelMatrices)
{
	maxInstances_ = kMaxInstances;
	instances_    = 0;
	if (modelMatrices.empty()) return;

	Instance* instances;
	instanceBuffer_.MapMemory(reinterpret_cast<char**>(&instances));
	{
		for (auto& modelMatrix : modelMatrices)
		{
			if (instances_ >= maxInstances_) break;

			auto instance         = &instances[instances_];
			instance->modelMatrix = modelMatrix;
			instances_++;
		}
	}
	instanceBuffer_.UnmapMemory();
}

bool ModelInstance::CmdRender(const CommandBuffer& commandBuffer, UniformHandle& uniformScene)
{
	if (instances_ == 0) return false;    //No instances

	auto& modelManager = ModelManagerLocator::get();
	if (!modelManager.IsLoaded(modelId_)) return false;

	const Model* model          = modelManager.GetModel(modelId_);
	const std::size_t meshCount = model->GetMeshCount();
	for (std::size_t i = 0; i < meshCount; ++i)
	{
		const Mesh& mesh         = model->GetMesh(i);
		auto& materialManager    = MaterialManagerLocator::get();
		const Material& material = materialManager.GetMaterial(mesh.GetMaterialId());
		switch (material.GetRenderMode())
		{
			case Material::RenderMode::VK_OPAQUE:
				if (!CmdRenderOpaque(commandBuffer, uniformScene, mesh, material)) return false;
				break;
			case Material::RenderMode::VK_TRANSPARENT: break;
		}
	}

	return true;
}

bool ModelInstance::CmdRenderOpaque(const CommandBuffer& commandBuffer,
	UniformHandle& uniformScene,
	const Mesh& mesh,
	const Material& material)
{
	const MaterialPipeline& materialPipeline = material.GetPipelineMaterial();
	if (!material.BindPipeline(commandBuffer)) return false;

	const GraphicsPipeline& pipeline = materialPipeline.GetPipeline();

	//Push uniforms to shader
	uniformObject_.PushUniformData(material.ExportUniformData());

	//Push texture to shader
	descriptorSet_.Push(kUboObjectHash, uniformObject_);
	descriptorSet_.Push(kUboSceneHash, uniformScene);
	descriptorSet_.PushDescriptorData(material.ExportDescriptorData());
	if (!descriptorSet_.Update(pipeline)) return false;

	descriptorSet_.BindDescriptor(commandBuffer, pipeline);

	VkBuffer vertexBuffers[] = {mesh.GetVertexBuffer(), instanceBuffer_};
	VkDeviceSize offset[]    = {0, 0};

	//Bind buffers
	vkCmdBindVertexBuffers(commandBuffer, 0, 2, vertexBuffers, offset);
	vkCmdBindIndexBuffer(commandBuffer, mesh.GetIndexBuffer(), 0, Mesh::GetIndexType());

	//Draw the instances
	vkCmdDrawIndexed(commandBuffer, mesh.GetIndexCount(), instances_, 0, 0, 0);

	return true;
}
}    // namespace neko::vk