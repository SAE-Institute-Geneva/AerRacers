#pragma once
/*
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
 */

#ifdef NEKO_GLES3
#include "showroom/model/mesh_obj.h"

namespace neko::sr
{
class ModelObj
{
public:
	ModelObj();
	void LoadModel(std::string_view path);
	bool IsLoaded() const;
	void Draw(const gl::Shader& shader, const Mat4f& modelMat = Mat4f::Identity);
	void Destroy();

	[[nodiscard]] std::string_view GetName() const { return name_; };

	[[nodiscard]] size_t GetMeshCount() const { return meshes_.size(); };
	std::vector<MeshObj>& GetMeshes() { return meshes_; };
	MeshObj& GetMesh(size_t index) { return meshes_[index]; }

	[[nodiscard]] std::size_t GetMeshId(const MeshObj& mesh) const;
	[[nodiscard]] std::string_view GetPath() const { return path_; };

private:
	void ProcessModel();

	void LoadMaterialTextures(
		MeshObj& mesh, Texture::TextureType textureType, std::string_view matName);

	std::string name_;
	std::vector<MeshObj> meshes_;
	std::string directory_;
	std::string path_;

	Job processModelJob_;
	Job initModelJob_;
};
}    // namespace neko::sr
#endif