#pragma once
#include "vk/models/mesh.h"

namespace neko::vk
{
class Model
{
public:
	Model& operator=(const Model& other) noexcept;

	void Destroy() const;
	[[nodiscard]] bool CmdRender(const CommandBuffer& commandBuffer, std::uint32_t instance = 1) const;

	[[nodiscard]] const Mesh& GetMesh(std::size_t meshIndex) const { return meshes_[meshIndex]; }
	[[nodiscard]] const std::vector<Mesh>& GetMeshes() const { return meshes_; }
	[[nodiscard]] std::size_t GetMeshCount() const { return meshes_.size(); }

private:
	friend class ModelLoader;
	std::vector<Mesh> meshes_;
};
}
