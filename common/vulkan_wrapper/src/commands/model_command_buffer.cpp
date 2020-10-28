#include "vk/commands/model_command_buffer.h"

namespace neko::vk
{
void ModelCommandBuffer::Draw(const ForwardDrawCmd& drawCommand)
{
    forwardDrawingCmd_.emplace_back(drawCommand);
}

void ModelCommandBuffer::PrepareData()
{
    /*for (size_t i = 0; i < modelInstances_.size(); i++)
    {
        modelInstances_[i]->Update(instancesMatrix_[i]);

        instancesMatrix_[i].clear();
    }*/
}

void ModelCommandBuffer::Clear()
{
    forwardDrawingCmd_.clear();

    //modelInstances_.clear();
}

void ModelCommandBuffer::OnUnloadScene()
{
    forwardDrawingCmd_.clear();
    forwardDrawingCmd_.shrink_to_fit();

    //modelInstances_.clear();
}
}