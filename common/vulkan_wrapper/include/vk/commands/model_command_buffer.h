#pragma once
#include <xxh3.h>

#include "mathematics/matrix.h"
#include "vk/buffers/uniform_handle.h"
#include "vk/descriptors/descriptor_handle.h"

namespace neko::vk
{
struct ForwardDrawCmd
{
    Mat4f worldMatrix = Mat4f::Identity;

    XXH64_hash_t materialID;
    XXH64_hash_t meshID;

    DescriptorHandle descriptorHandle;
    UniformHandle uniformHandle;
};

using ModelInstanceIndex = size_t;
using ModelForwardIndex = int;

class ModelCommandBuffer
{
public:
    //void InitData();

    std::vector<ForwardDrawCmd>& GetForwardModels() { return forwardDrawingCmd_; }

    //std::vector<std::unique_ptr<ModelInstance>>& GetModelInstances();
    //ModelInstanceIndex GetModelInstanceIndex(const Material& material, const Mesh& mesh);
    //ModelInstanceIndex GetForwardIndex();

    //void FreeForwardIndex(ModelForwardIndex index);

    /*void Draw(Mat4f worldMatrix,
            ModelInstanceIndex instanceIndex);*/
    /*void Draw(Mat4f worldMatrix,
            Model model,
            ModelForwardIndex forwardIndex);*/

    void Draw(const ForwardDrawCmd& drawCommand);
    void PrepareData();

    void Clear();

private:
    void OnUnloadScene();

    static const int kSizePerType = 200;

    //Data for gpu instancing
    //std::vector<std::vector<InstancingDrawCmd>> instancesMatrix_;
    //std::vector<std::unique_ptr<ModelInstance>> modelInstances_;

    //Data for forward rendering
    uint64_t nextFreeForwardIndex_ = 0;
    std::vector<ForwardDrawCmd> forwardDrawingCmd_;
};
}