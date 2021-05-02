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
#include "gl/mesh.h"

#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

namespace neko::gl
{
Vertex::Vertex(const Vec3f& pos, const Vec3f& norm, const Vec2f& uv)
   : position(pos), normal(norm), texCoords(uv)
{}

bool Vertex::operator==(const Vertex& other) const
{
	return position == other.position && normal == other.normal && texCoords == other.texCoords &&
	       tangent == other.tangent && bitangent == other.bitangent;
}

void Mesh::Init()
{
	glGenVertexArrays(1, &vao_);
	glCheckError();
	glGenBuffers(1, &vbo_);
	glCheckError();
	glGenBuffers(1, &ebo_);
	glCheckError();

#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Copy Buffers");
#endif
	glBindVertexArray(vao_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glCheckError();
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);
	glCheckError();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(Index), &indices_[0], GL_STATIC_DRAW);
	glCheckError();

#ifdef EASY_PROFILE_USE
	EASY_END_BLOCK;
	EASY_BLOCK("Vertex Attrib");
#endif
	// vertex positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
	glCheckError();

	// vertex texture coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texCoords));
	glCheckError();

	// vertex normals
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
	glCheckError();

	// vertex tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, tangent));
	glCheckError();

	// vertex bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(
		4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, bitangent));
	glCheckError();

	glBindVertexArray(0);
	glCheckError();
#ifdef EASY_PROFILE_USE
	EASY_END_BLOCK;
#endif
}

void Mesh::InitInstanced() const
{
	// Set attribute pointers for matrix (4 times vec4)
	glBindVertexArray(vao_);
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4f), (void*) nullptr);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4f), (void*) (sizeof(Vec4f)));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4f), (void*) (2 * sizeof(Vec4f)));
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Mat4f), (void*) (3 * sizeof(Vec4f)));

	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);
	glBindVertexArray(0);
}

void Mesh::Draw(const Shader& shader) const
{
	BindTextures(shader);

	glBindVertexArray(vao_);
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void Mesh::DrawInstanced(const Shader& shader, const int instanceNum) const
{
	BindTextures(shader);

	glBindVertexArray(vao_);
	glDrawElementsInstanced(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr, instanceNum);
	glBindVertexArray(0);
}

void Mesh::DrawFromTexture(const Shader& shader, const TextureName& texture)
{
	std::size_t diffuse = GetTexture(Texture::Type::DIFFUSE);
	TextureName baseTexture = textures_[diffuse].textureName;
	textures_[diffuse].textureName = texture;
	BindTextures(shader);

	glBindVertexArray(vao_);
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
	textures_[diffuse].textureName = baseTexture;
}

void Mesh::Destroy()
{
	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(1, &vbo_);
	glDeleteBuffers(1, &ebo_);

	vertices_.clear();
	indices_.clear();
}

std::size_t Mesh::GetTexture(gl::Mesh::Texture::Type type)
{
	for (size_t i = 0; i < textures_.size(); ++i)
		if (textures_[i].type == type)
			return i;

	return INVALID_INDEX;
}

void Mesh::BindTextures(const Shader& shader) const
{
	glCheckError();
	std::uint8_t usedMaps = 0;
	for (const auto& texture : textures_)
	{
		// Activate proper texture unit before binding
		std::string name;
		std::uint8_t binding = 0u;
		Texture::Type type   = Texture::NONE;
		switch (texture.type)
		{
			case Texture::DIFFUSE:
				name    = "diffuse";
				type    = Texture::DIFFUSE;
				binding = 0u;
				break;
			case Texture::SPECULAR:
				name    = "specular";
				type    = Texture::SPECULAR;
				binding = 1u;
				break;
			case Texture::NORMAL:
				name    = "normal";
				type    = Texture::NORMAL;
				binding = 2u;
				break;
			case Texture::EMISSIVE:
				name    = "emissive";
				type    = Texture::EMISSIVE;
				binding = 3u;
				break;
			default: break;
		}

		usedMaps |= type;
		shader.SetTexture(std::string("material.").append(name), texture.textureName, binding);
	}

	shader.SetFloat("material.shininess", shininess_);
	shader.SetVec3("material.color", color_);
	shader.SetUInt("usedMaps", usedMaps);
	glActiveTexture(GL_TEXTURE0);
	glCheckError();
}
}    // namespace neko::gl
#endif