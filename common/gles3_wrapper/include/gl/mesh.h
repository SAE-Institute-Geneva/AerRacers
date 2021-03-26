#pragma once
#include "graphics/color.h"
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
#include "engine/globals.h"
#include "mathematics/aabb.h"

#include "gl/mesh.h"
#include "gl/shader.h"

namespace neko::gl
{
struct Vertex
{
	Vertex() = default;
	Vertex(const Vec3f& pos, const Vec3f& norm, const Vec2f& uv);

	bool operator==(const Vertex& other) const;

	Vec3f position  = Vec3f::zero;
	Vec3f normal    = Vec3f::zero;
	Vec2f texCoords = Vec2f::zero;
	Vec3f tangent   = Vec3f::zero;
	Vec3f bitangent = Vec3f::zero;
};

class Mesh
{
public:
	struct Texture
	{
		enum Type : std::uint8_t
		{
			NONE     = 0u,
			DIFFUSE  = 1u << 0u,
			SPECULAR = 1u << 1u,
			NORMAL   = 1u << 2u,
			EMISSIVE = 1u << 3u,
		};

		TextureId textureId     = INVALID_TEXTURE_ID;
		TextureName textureName = INVALID_TEXTURE_NAME;

		Type type = NONE;
	};

	void Init();
	void Draw(const Shader& shader) const;
	void Destroy();

	[[nodiscard]] const std::vector<Vertex>& GetVertices() const { return vertices_; }
	[[nodiscard]] const std::vector<Index>& GetIndices() const { return indices_; }
	[[nodiscard]] const Aabb3d& GetAabb() const { return aabb_; }

private:
	friend class Model;
	friend class ModelLoader;
	void BindTextures(const Shader& shader) const;

	std::vector<Vertex> vertices_;
	std::vector<Index> indices_;
	std::vector<Texture> textures_;

	float shininess_ = 64.0f;
	Color3 color_    = Vec3f::one * 0.8f;
	Aabb3d aabb_ {};

	GLuint vao_ = 0, vbo_ = 0, ebo_ = 0;
};
}    // namespace neko::gl
#endif