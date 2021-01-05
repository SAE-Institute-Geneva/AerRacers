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
#include "showroom/texture.h"
#include "mathematics/circle.h"
#include "mathematics/hash.h"
#include "mathematics/vector.h"

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
} __attribute__((aligned(64)));

struct Texture
{
	Texture() = default;
	TextureId textureId = INVALID_TEXTURE_ID;
	TextureName textureName = INVALID_TEXTURE_NAME;
	enum class TextureType : std::uint8_t
	{
		DIFFUSE,
		SPECULAR,
		NORMAL,
		EMISSIVE
	};
	TextureType type = TextureType::DIFFUSE;
} __attribute__((aligned(32))) __attribute__((packed));

class Mesh
{
public:
	Mesh();
	void Init();
	void Draw(const gl::Shader& shader) const;
    void BindTextures(const gl::Shader& shader) const;
	void Destroy();

	void ProcessMesh(const aiMesh* mesh, const aiScene* scene, std::string_view directory, std::string_view path);
	bool IsLoaded() const;

	void SetParentName(std::string_view parentName) { parentName_ = HashString(parentName); }
	[[nodiscard]] XXH64_hash_t GetParentName() const { return parentName_; }
	void SetName(std::string_view name) { name_ = name; }
	[[nodiscard]] std::string_view GetName() const { return name_; }
	[[nodiscard]] size_t GetIndicesCount() const { return indices_.size(); }
	[[nodiscard]] size_t GetVerticesCount() const { return vertices_.size(); }
	[[nodiscard]] const std::vector<Texture>& GetTextures() const { return textures_; }

	[[nodiscard]] uint32_t GetVAO() const { return VAO; }

protected:
	void LoadMaterialTextures(aiMaterial* material, aiTextureType aiTexture, Texture::TextureType texture, std::string_view directory);
	void SetupMesh();

	friend class Model;
	std::string name_;
	XXH64_hash_t parentName_ = 0;
	std::vector<Vertex> vertices_;
	std::vector<unsigned int> indices_;
	std::vector<Texture> textures_;
	float specularExponent_ = 0.0f;
	Vec3f min_, max_;

	uint32_t VAO = 0, VBO = 0, EBO = 0;

	Job loadMeshToGpu;
};
}
#endif