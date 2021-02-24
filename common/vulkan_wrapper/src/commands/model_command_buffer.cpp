#include "vk/commands/model_command_buffer.h"

#include "vk/models/model_manager.h"

namespace neko::vk
{
ModelCommandBuffer::ModelCommandBuffer()
   : meshInstances_(std::vector<std::unique_ptr<MeshInstance>>())
{}

void ModelCommandBuffer::Destroy()
{
	for (auto& forwardDrawCmd : forwardDrawingCmd_) forwardDrawCmd.uniformHandle.Destroy();

	Clear();
}

ModelInstanceIndex ModelCommandBuffer::AddModelInstanceIndex(
	const Material& material, const Mesh& mesh, const std::vector<Mat4f>& matrices)
{
	for (size_t i = 0; i < meshInstances_.size(); i++)
		if (&meshInstances_[i]->GetMaterial() == &material)
			if (&meshInstances_[i]->GetMesh() == &mesh) return i;

	meshInstances_.emplace_back();
	meshInstances_.back() = std::make_unique<MeshInstance>(mesh, material);
	instanceMatrices_.emplace_back(matrices);
	return meshInstances_.size() - 1;
}

void ModelCommandBuffer::FreeForwardIndex(ModelForwardIndex index)
{
	forwardDrawingCmd_.erase(forwardDrawingCmd_.cbegin() + index);
}

void ModelCommandBuffer::Draw(const ForwardDrawCmd& drawCommand)
{
	forwardDrawingCmd_.push_back(drawCommand);
}

void ModelCommandBuffer::Draw(const Mat4f& worldMatrix, ModelInstanceIndex instanceIndex)
{
	if (instanceMatrices_[instanceIndex].size() == instanceMatrices_.capacity())
		instanceMatrices_[instanceIndex].resize(instanceMatrices_.size() * 2 + 1);

	instanceMatrices_[instanceIndex].emplace_back(worldMatrix);
}

void ModelCommandBuffer::Draw(
	const Mat4f& worldMatrix, const sole::uuid modelId, ModelForwardIndex forwardIndex)
{
	forwardDrawingCmd_[forwardIndex].worldMatrix = worldMatrix;
	forwardDrawingCmd_[forwardIndex].modelId     = modelId;
}

void ModelCommandBuffer::PrepareData()
{
	for (size_t i = 0; i < meshInstances_.size(); i++)
	{
		meshInstances_[i]->Update(instanceMatrices_[i]);
		//instanceMatrices_[i].clear();
	}
}

void ModelCommandBuffer::Clear()
{
	forwardDrawingCmd_.clear();
	meshInstances_.clear();
}

void ModelCommandBuffer::OnUnloadScene()
{
	Clear();
	forwardDrawingCmd_.shrink_to_fit();
}
}    // namespace neko::vk