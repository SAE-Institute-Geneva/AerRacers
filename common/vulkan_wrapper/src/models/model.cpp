#include "vk/models/model.h"

namespace neko::vk
{
void Model::Destroy() const
{
	for (const auto& mesh : meshes_) mesh.Destroy();
}

bool Model::CmdRender(const CommandBuffer& commandBuffer, std::uint32_t instance) const
{
	if (meshes_.empty()) return true;

	return std::all_of(meshes_.cbegin(),
		meshes_.cend(),
		[commandBuffer, instance](const Mesh& mesh)
		{ return mesh.DrawCmd(commandBuffer, instance); });
}
}    // namespace neko::vk