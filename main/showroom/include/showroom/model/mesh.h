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
    #include <vector>

    #include "assimp/material.h"
    #include "gl/shader.h"
    #include "mathematics/aabb.h"
    #include "mathematics/circle.h"
    #include "mathematics/hash.h"
    #include "mathematics/vector.h"
    #include "showroom/texture.h"

struct aiMesh;
struct aiScene;

namespace neko::sr
{
struct Vertex
{
	Vec3f position;
	Vec3f normal;
	Vec2f texCoords;
    Vec3f tangent;
    Vec3f bitangent;
};

class Mesh
{
public:
	Mesh();
	bool operator==(const Mesh& other) const;

	void Init();
	void Draw(const gl::Shader& shader, const Mat4f& modelMat = Mat4f::Identity) const;
    void BindTextures(const gl::Shader& shader) const;
	void Destroy();

	void UpdateTextures();

	void ProcessMesh(const aiMesh* mesh, const aiScene* scene, std::string_view directory, std::string_view path);
	bool IsLoaded() const;

	[[nodiscard]] XXH64_hash_t GetParentName() const { return parentName_; }
	[[nodiscard]] std::string_view GetName() const { return name_; }
	[[nodiscard]] const Mat4f& GetModelMatrix() const { return modelMat_; }
	[[nodiscard]] size_t GetIndicesCount() const { return indices_.size(); }
	[[nodiscard]] size_t GetVerticesCount() const { return vertices_.size(); }
	[[nodiscard]] const std::vector<Vertex>& GetVertices() const { return vertices_; }
	std::vector<Texture>& GetTextures() { return textures_; }
	Texture& GetTexture(size_t index) { return textures_[index]; }
	size_t GetTexture(sr::Texture::TextureType type);
	float GetShininess() const { return specularExponent_; }
	[[nodiscard]] const Aabb3d& GetAabb() const { return aabb_; }

	void SetParentName(std::string_view parentName) { parentName_ = HashString(parentName); }
	void SetName(std::string_view name) { name_ = name; }
	void SetModelMatrix(const Mat4f& mat) { modelMat_ = mat; }
	void SetShininess(const float shininess) { specularExponent_ = shininess; }

	[[nodiscard]] uint32_t GetVAO() const { return VAO; }

protected:
	void LoadMaterialTextures(aiMaterial* material, aiTextureType aiTexture, Texture::TextureType texture, std::string_view directory);
	void SetupMesh();

	friend class Model;
	std::string name_;
	XXH64_hash_t parentName_ = 0;
	Mat4f modelMat_;
	std::vector<Vertex> vertices_;
	std::vector<unsigned int> indices_;
	std::vector<Texture> textures_;
	Aabb3d aabb_;

	float specularExponent_ = 64.0f;
	Color3 diffuse_ = Color3(0.8f);
	Color3 specular_ = Color3::one;

	uint32_t VAO = 0, VBO = 0, EBO = 0;

	Job loadMeshToGpu;
};
}
#endif