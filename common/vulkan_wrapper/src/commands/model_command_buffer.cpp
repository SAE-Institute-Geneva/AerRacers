#include "vk/commands/model_command_buffer.h"

#include "vk/models/model_manager.h"

namespace neko::vk
{
ModelCommandBuffer::ModelCommandBuffer()
   : modelInstances_(std::vector<std::unique_ptr<ModelInstance>>())
{}

void ModelCommandBuffer::Destroy()
{
	for (auto& forwardDrawCmd : forwardDrawingCmd_) forwardDrawCmd.uniformHandle.Destroy();

	Clear();
}

ModelInstanceIndex ModelCommandBuffer::AddModelInstanceIndex(const ModelId& modelId)
{
	for (size_t i = 0; i < modelInstances_.size(); i++)
		if (modelInstances_[i]->GetModelId() == modelId) return i;

	modelInstances_.emplace_back();
	modelInstances_.back() = std::make_unique<ModelInstance>(modelId);
	instanceMatrices_.emplace_back();
	return modelInstances_.size() - 1;
}

ModelInstanceIndex ModelCommandBuffer::AddModelInstanceIndex(
	const ModelId& modelId, const Mat4f& matrix)
{
	for (size_t i = 0; i < modelInstances_.size(); i++)
	{
		if (modelInstances_[i]->GetModelId() == modelId)
		{
			instanceMatrices_[i].push_back(matrix);
			return i;
		}
	}

	modelInstances_.emplace_back();
	modelInstances_.back() = std::make_unique<ModelInstance>(modelId);
	instanceMatrices_.emplace_back().push_back(matrix);
	return modelInstances_.size() - 1;
}

ModelInstanceIndex ModelCommandBuffer::AddModelInstanceIndex(
	const ModelId& modelId, const std::vector<Mat4f>& matrices)
{
	for (size_t i = 0; i < modelInstances_.size(); i++)
	{
		if (modelInstances_[i]->GetModelId() == modelId)
		{
			instanceMatrices_[i].insert(
				instanceMatrices_[i].end(), matrices.begin(), matrices.end());
			return i;
		}
	}

	modelInstances_.emplace_back();
	modelInstances_.back() = std::make_unique<ModelInstance>(modelId);
	instanceMatrices_.emplace_back(matrices);
	return modelInstances_.size() - 1;
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
	for (size_t i = 0; i < modelInstances_.size(); i++)
	{
		modelInstances_[i]->Update(instanceMatrices_[i]);
		instanceMatrices_[i].clear();
	}
}

void ModelCommandBuffer::Clear()
{
	forwardDrawingCmd_.clear();
	modelInstances_.clear();
}

void ModelCommandBuffer::AddMatrix(ModelInstanceIndex index, const Mat4f& matrix)
{
	instanceMatrices_[index].push_back(matrix);
	modelInstances_[index]->AddInstance();
}

void ModelCommandBuffer::SetMatrices(ModelInstanceIndex index, const std::vector<Mat4f>& matrices)
{
	instanceMatrices_[index] = matrices;
	modelInstances_[index]->AddInstance(matrices.size());
}

void ModelCommandBuffer::SetModelId(ModelInstanceIndex index, const ModelId& modelId)
{
	modelInstances_[index]->SetModelId(modelId);
}
}    // namespace neko::vk