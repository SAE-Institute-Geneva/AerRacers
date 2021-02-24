#pragma once
/* ----------------------------------------------------
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author: Canas Simon
 Date:
---------------------------------------------------------- */
#include "vk/models/mesh.h"

namespace neko::vk
{
class Model
{
public:
	Model& operator=(const Model& other) noexcept;

	void Destroy() const;
	[[nodiscard]] bool CmdRender(
		const CommandBuffer& commandBuffer, std::uint32_t instance = 1) const;

	[[nodiscard]] const Mesh& GetMesh(std::size_t meshIndex) const { return meshes_[meshIndex]; }
	[[nodiscard]] const std::vector<Mesh>& GetMeshes() const { return meshes_; }
	[[nodiscard]] std::size_t GetMeshCount() const { return meshes_.size(); }

private:
	friend class ModelLoader;
	std::vector<Mesh> meshes_;
};
}    // namespace neko::vk
