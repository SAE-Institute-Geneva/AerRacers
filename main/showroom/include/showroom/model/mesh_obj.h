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
#include "gl/shader.h"
#include "mathematics/aabb.h"
#include "mathematics/hash.h"
#include "showroom/texture.h"

namespace neko::sr
{
struct VertexObj
{
	bool operator==(const VertexObj& other) const
	{
		return position == other.position && normal == other.normal &&
		       texCoords == other.texCoords && tangent == other.tangent &&
		       bitangent == other.bitangent;
	}

	Vec3f position  = Vec3f::zero;
	Vec3f normal    = Vec3f::zero;
	Vec2f texCoords = Vec2f::zero;
	Vec3f tangent   = Vec3f::zero;
	Vec3f bitangent = Vec3f::zero;
};

class MeshObj
{
public:
	MeshObj();
	bool operator==(const MeshObj& other) const;

	void Init();
	void Draw(const gl::Shader& shader, const Mat4f& modelMat = Mat4f::Identity) const;
	void BindTextures(const gl::Shader& shader) const;
	void Destroy();

	void UpdateTextures();

	bool IsLoaded() const;

	[[nodiscard]] std::string_view GetName() const { return name_; }
	[[nodiscard]] const Mat4f& GetModelMatrix() const { return modelMat_; }

	[[nodiscard]] size_t GetIndicesCount() const { return indices_.size(); }
	[[nodiscard]] size_t GetVerticesCount() const { return vertices_.size(); }
	[[nodiscard]] const std::vector<VertexObj>& GetVertices() const { return vertices_; }

	std::vector<Texture>& GetTextures() { return textures_; }
	Texture& GetTexture(std::size_t index) { return textures_[index]; }
	std::size_t GetTexture(sr::Texture::TextureType type);

	float GetShininess() const { return shininess_; }
	[[nodiscard]] const Aabb3d& GetAabb() const { return aabb_; }

	void SetName(std::string_view name) { name_ = name; }
	void SetModelMatrix(const Mat4f& mat) { modelMat_ = mat; }
	void SetShininess(const float shininess) { shininess_ = shininess; }

	[[nodiscard]] std::uint32_t GetVao() const { return vao_; }

protected:
	void SetupMesh();

	friend class ModelObj;
	std::string name_;
	Mat4f modelMat_;
	std::vector<VertexObj> vertices_;
	std::vector<unsigned int> indices_;
	std::vector<Texture> textures_;
	Aabb3d aabb_;

	float shininess_ = 64.0f;
	Color3 diffuse_  = Color3(0.8f);
	Color3 specular_ = Color3::one;

	uint32_t vao_ = 0, vbo_ = 0, ebo_ = 0;

	Job loadMesh_;
};
}    // namespace neko::sr

MAKE_HASHABLE(neko::Vec2f, t.x, t.y)
MAKE_HASHABLE(neko::Vec3f, t.x, t.y, t.z)
MAKE_HASHABLE(neko::sr::VertexObj, t.position, t.normal, t.texCoords, t.tangent, t.bitangent)
#endif