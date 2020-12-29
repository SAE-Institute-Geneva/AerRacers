#include "vk/subrenderers/subrenderer_opaque.h"

#include "vk/graphics.h"
#include "vk/material/material_manager.h"

namespace neko::vk
{
SubrendererOpaque::SubrendererOpaque(Pipeline::Stage stage)
    : RenderPipeline(stage),
    uniformScene_(true),
    modelCmdBuffer_(VkObjectsLocator::get().modelCommandBuffer) {}

void SubrendererOpaque::Destroy() const
{
	uniformScene_.Destroy();
	modelCmdBuffer_.Destroy();
}

void SubrendererOpaque::OnRender(const CommandBuffer& commandBuffer)
{
    const auto& camera = CameraLocator::get();

    const auto& view = camera.GenerateViewMatrix();
    auto proj = camera.GenerateProjectionMatrix();
    proj[1][1] *= -1;

    uniformScene_.Push(kProjectionHash, proj);
    uniformScene_.Push(kViewHash, view);
    uniformScene_.Push(kViewPosHash, camera.position);

    modelCmdBuffer_.PrepareData();

    //Single Draw
    auto& modelManager = ModelManagerLocator::get();
    auto& materialManager = MaterialManagerLocator::get();
    for (auto& modelDrawCommand : modelCmdBuffer_.GetForwardModels())
    {
    	const auto& model = modelManager.GetModel(modelDrawCommand.modelID);
	    for (std::size_t i = 0; i < model->GetMeshCount(); ++i)
	    {
		    const auto& mesh = model->GetMesh(i);
		    const ResourceHash matId = mesh.GetMaterialId();
		    if (matId != 0 && matId != 1)
		    {
			    const auto& material = materialManager.GetMaterial(matId);
			    if (material.GetRenderMode() == Material::RenderMode::OPAQUE)
				    CmdRender(commandBuffer, modelDrawCommand, mesh, material);
		    }
	    }
    }

    //GPU Instancing
    for (auto& meshInstance : modelCmdBuffer_.GetMeshInstances())
        if (meshInstance->GetMaterial().GetRenderMode() == Material::RenderMode::OPAQUE)
            meshInstance->CmdRender(commandBuffer, uniformScene_);
}

bool SubrendererOpaque::CmdRender(
		const CommandBuffer& commandBuffer,
		ForwardDrawCmd& modelDrawCommand,
		const Mesh& mesh,
		const Material& mat)
{
    modelDrawCommand.uniformHandle.Push(kModelHash, modelDrawCommand.worldMatrix);
    modelDrawCommand.uniformHandle.PushUniformData(mat.ExportUniformData());

    // Check if we are in the correct pipeline stage.
    const auto& materialPipeline = mat.GetPipelineMaterial();
    if (materialPipeline.GetStage() != GetStage())
        return false;

    // Binds the material pipeline.
    if (!mat.BindPipeline(commandBuffer))
        return false;

    const auto& pipeline = materialPipeline.GetPipeline();

    // Updates descriptors.
    modelDrawCommand.descriptorHandle.Push(kUboSceneHash, uniformScene_);
    modelDrawCommand.descriptorHandle.Push(kUboObjectHash, modelDrawCommand.uniformHandle);
    modelDrawCommand.descriptorHandle.PushDescriptorData(mat.ExportDescriptorData());

    if (!modelDrawCommand.descriptorHandle.Update(pipeline))
        return false;

    // Draws the object.
    modelDrawCommand.descriptorHandle.BindDescriptor(commandBuffer, pipeline);

	return mesh.CmdRender(commandBuffer);
}

void SubrendererOpaque::SetUniformBlock(const UniformBlock& uniformBlock)
{
    uniformScene_.Update(uniformBlock);
}
}