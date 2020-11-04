#include "vk/subrenderers/subrenderer_opaque.h"
#include "vk/graphics.h"

namespace neko::vk
{
SubrendererOpaque::SubrendererOpaque(Pipeline::Stage stage)
    : RenderPipeline(stage),
    uniformScene_(true),
    modelCmdBuffer_(VkObjectsLocator::get().modelCommandBuffer)
{
    quad_.Init();
    ForwardDrawCmd drawCmd;
    drawCmd.worldMatrix = Mat4f::Identity;
    drawCmd.uniformHandle = uniformScene_;
    modelCmdBuffer_.Draw(drawCmd);
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
        //if(modelDrawCommand.materialID != 0 && modelDrawCommand.materialID != 1)
            //if (MaterialsManagerLocator::Get().GetMaterial(modelDrawCommand.materialID).GetRenderMode() == Material::RenderMode::OPAQUE)
                CmdRender(commandBuffer, modelDrawCommand);

    //GPU Instancing
    /*for (auto& modelInstance : modelCmdBuffer_.GetModelInstances())
        if (modelInstance->GetMaterial().GetRenderMode() == Material::RenderMode::OPAQUE)
            modelInstance->CmdRender(commandBuffer, uniformScene_);*/
}

bool SubrendererOpaque::CmdRender(const CommandBuffer& commandBuffer, ForwardDrawCmd& modelDrawCommand)
{
    //modelDrawCommand.uniformHandle.Push(kTransformHash, modelDrawCommand.worldMatrix);

    /*auto& mat = MaterialsManagerLocator::Get().GetMaterial(modelDrawCommand.materialID);
    modelDrawCommand.uniformHandle.PushUniformData(mat.ExportUniformData());*/

    // Check if we are in the correct pipeline stage.
    //const auto& materialPipeline = mat.GetPipelineMaterial();
    const auto& materialPipeline = VkObjectsLocator::get().graphicsPipeline;
    if (materialPipeline.GetStage() != GetStage())
        return false;

    // Binds the material pipeline.
    /*if (!materialPipeline.BindPipeline(commandBuffer))
        return false;*/
    materialPipeline.BindPipeline(commandBuffer);

    //const auto& pipeline = materialPipeline.GetPipeline();

    // Updates descriptors.
    modelDrawCommand.descriptorHandle.Push(kUboSceneHash, uniformScene_);
    //modelDrawCommand.descriptorHandle.Push(kUboObjectHash, modelDrawCommand.uniformHandle);

    //modelDrawCommand.descriptorHandle.PushDescriptorData(mat.ExportDescriptorData());

    //if (!modelDrawCommand.descriptorHandle.Update(pipeline))
    if (!modelDrawCommand.descriptorHandle.Update(materialPipeline))
        return false;

    // Draws the object.
    //modelDrawCommand.descriptorHandle.BindDescriptor(commandBuffer, pipeline);
    modelDrawCommand.descriptorHandle.BindDescriptor(commandBuffer, materialPipeline);

    //return MeshManagerLocator::Get().GetMesh(modelDrawCommand.meshID).CmdRender(commandBuffer);
    return quad_.CmdRender(commandBuffer);
}

void SubrendererOpaque::SetUniformBlock(const UniformBlock& uniformBlock)
{
    uniformScene_.Update(uniformBlock);
}
}