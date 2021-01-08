#include "vk/subrenderers/subrenderer_opaque.h"

#include "vk/graphics.h"
#include "vk/material/material_manager.h"

namespace neko::vk
{
SubrendererOpaque::SubrendererOpaque(Pipeline::Stage stage)
    : RenderPipeline(stage),
    uniformScene_(true),
    modelCmdBuffer_(VkObjectsLocator::get().modelCommandBuffer)
{
    quad_.Init();
    ForwardDrawCmd drawCmd;
    drawCmd.worldMatrix = Transform3d::Rotate(Mat4f::Identity, degree_t(45.0f), Vec3f::left);
    drawCmd.materialID = HashString("Test");
    drawCmd.uniformHandle = uniformScene_;
    modelCmdBuffer_.Draw(drawCmd);
}

void SubrendererOpaque::Destroy() const
{
	uniformScene_.Destroy();
	modelCmdBuffer_.Destroy();
	quad_.Destroy();
}

void SubrendererOpaque::OnRender(const CommandBuffer& commandBuffer)
{
    const auto& camera = CameraLocator::get();

    const auto& view = camera.GenerateViewMatrix();
    auto proj = camera.GenerateProjectionMatrix();
    proj[1][1] *= -1;

    uniformScene_.Push(kProjectionHash, proj);
    uniformScene_.Push(kViewHash, view);

    modelCmdBuffer_.PrepareData();

    //Single Draw
    for (auto& modelDrawCommand : modelCmdBuffer_.GetForwardModels())
        if(modelDrawCommand.materialID != 0 && modelDrawCommand.materialID != 1)
        {
        	const auto& material = MaterialManagerLocator::get().GetMaterial(modelDrawCommand.materialID);
	        if (material.GetRenderMode() == Material::RenderMode::VK_OPAQUE)
		        CmdRender(commandBuffer, modelDrawCommand);
        }

    //GPU Instancing
    /*for (auto& modelInstance : modelCmdBuffer_.GetModelInstances())
        if (modelInstance->GetMaterial().GetRenderMode() == Material::RenderMode::VK_OPAQUE)
            modelInstance->CmdRender(commandBuffer, uniformScene_);*/
}

bool SubrendererOpaque::CmdRender(const CommandBuffer& commandBuffer, ForwardDrawCmd& modelDrawCommand)
{
    modelDrawCommand.uniformHandle.Push(kModelHash, modelDrawCommand.worldMatrix);

    auto& mat = MaterialManagerLocator::get().GetMaterial(modelDrawCommand.materialID);
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

    //return MeshManagerLocator::Get().GetMesh(modelDrawCommand.meshID).CmdRender(commandBuffer);
    return quad_.CmdRender(commandBuffer);
}

void SubrendererOpaque::SetUniformBlock(const UniformBlock& uniformBlock)
{
    uniformScene_.Update(uniformBlock);
}
}