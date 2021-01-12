#pragma once
#include <xxhash.h>

#include "mathematics/matrix.h"
#include "vk/buffers/uniform_handle.h"
#include "vk/descriptors/descriptor_handle.h"
#include "vk/models/mesh_instance.h"
#include "vk/models/model.h"

namespace neko::vk
{
struct ForwardDrawCmd
{
	ForwardDrawCmd() = default;

    Mat4f worldMatrix = Mat4f::Identity;

    sole::uuid modelID = sole::uuid();

    DescriptorHandle descriptorHandle;
    UniformHandle uniformHandle;
} __attribute__((aligned(128))) __attribute__((packed));

using ModelInstanceIndex = size_t;
using ModelForwardIndex = int;

class ModelCommandBuffer
{
public:
	explicit ModelCommandBuffer();

    //void InitData();
	void Destroy();

    std::vector<std::unique_ptr<MeshInstance>>& GetMeshInstances() { return meshInstances_; }
	std::vector<ForwardDrawCmd>& GetForwardModels() { return forwardDrawingCmd_; }

    ModelInstanceIndex GetModelInstanceIndex(
    		const Material& material,
    		const Mesh& mesh,
    		const std::vector<Mat4f>& matrices);

    void FreeForwardIndex(ModelForwardIndex index);

	void Draw(const ForwardDrawCmd& drawCommand);
    void Draw(const Mat4f& worldMatrix, ModelInstanceIndex instanceIndex);
    void Draw(const Mat4f& worldMatrix, sole::uuid model, ModelForwardIndex forwardIndex);
    void PrepareData();

    void Clear();

private:
    void OnUnloadScene();

	//Data for forward rendering
	std::vector<ForwardDrawCmd> forwardDrawingCmd_{};

	//Data for gpu instancing
	std::vector<std::vector<Mat4f>> instanceMatrices_{};
	std::vector<std::unique_ptr<MeshInstance>> meshInstances_{};
};
}