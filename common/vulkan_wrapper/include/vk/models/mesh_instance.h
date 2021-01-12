#pragma once
#include "mathematics/matrix.h"
#include "vk/commands/command_buffer.h"
#include "vk/buffers/instance_buffer.h"
#include "vk/buffers/uniform_handle.h"
#include "vk/descriptors/descriptor_handle.h"
#include "vk/models/mesh.h"

namespace neko::vk
{
class MeshInstance
{
public:
	struct Instance
	{
		[[nodiscard]] static VertexInput GetVertexInput(std::uint32_t baseBinding = 0);

		Mat4f modelMatrix = Mat4f::Identity;
	};

	explicit MeshInstance(
			const Mesh& mesh,
			const Material& material);

	[[nodiscard]] static std::unique_ptr<MeshInstance> Create(
			const Mesh& mesh,
			const Material& material);

	void Update(std::vector<Mat4f>& modelMatrices);

	bool CmdRender(const CommandBuffer& commandBuffer, UniformHandle& uniformScene);

	[[nodiscard]] const Mesh& GetMesh() const { return kMesh_; }
	[[nodiscard]] const Material& GetMaterial() const { return kMaterial_; }

private:
	const Mesh& kMesh_;
	const Material& kMaterial_;

	std::uint32_t maxInstances_ = 0;
	std::uint32_t instances_ = 0;

	DescriptorHandle descriptorSet_;
	InstanceBuffer instanceBuffer_;
	UniformHandle uniformObject_;

	inline static const StringHash kUniformSceneHash = HashString("UboScene");
	inline static const StringHash kUniformObjectHash = HashString("UboObject");

	static const std::uint32_t kMaxInstances = 32;
};
}
