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

#include "graphics/graphics.h"

namespace neko::sr
{
MeshObj::MeshObj() : loadMesh_([this] { SetupMesh(); }) {}

void MeshObj::Init()
{
	loadMesh_ = Job([this] { SetupMesh(); });
	RendererLocator::get().AddPreRenderJob(&loadMesh_);
	auto& textureManager = TextureManagerLocator::get();
	for (auto& texture : textures_)
	{
		//Waiting for texture to be loaded
		while (!textureManager.IsTextureLoaded(texture.textureId)) {}
		if (textureManager.GetTexture(texture.textureId))
			texture.name = textureManager.GetTexture(texture.textureId)->name;
	}
}

void MeshObj::Draw(const gl::Shader& shader, const Mat4f& modelMat) const
{
	BindTextures(shader);
	shader.SetMat4("model", modelMat_ * modelMat);

	// draw mesh
	glBindVertexArray(vao_);
	glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}

void MeshObj::Destroy()
{
	glDeleteVertexArrays(1, &vao_);
	glDeleteBuffers(1, &vbo_);
	glDeleteBuffers(1, &ebo_);

	for (auto& texture : textures_) gl::DestroyTexture(texture.name);

	textures_.clear();
	vertices_.clear();
	indices_.clear();

	loadMesh_.Reset();
}

void MeshObj::UpdateTextures()
{
	const auto& textureManager = TextureManagerLocator::get();
	for (auto it = textures_.cbegin(); it != textures_.cend();)
	{
		const size_t index = it - textures_.cbegin();
		if (textures_[index].type == Texture::TextureType::NONE)
		{
			it = textures_.erase(textures_.cbegin() + index);
			continue;
		}

		if (textures_[index].name == INVALID_TEXTURE_NAME)
		{
			const neko::Texture* texture = textureManager.GetTexture(textures_[index].textureId);
			if (texture) textures_[index].name = texture->name;
		}

		++it;
	}
}

bool MeshObj::IsLoaded() const
{
	if (!loadMesh_.IsDone()) return false;
	auto lambda = [](const Texture& texture)
	{
		const auto& textureManager = TextureManagerLocator::get();
		return textureManager.IsTextureLoaded(texture.textureId);
	};

	if (std::all_of(textures_.cbegin(), textures_.cend(), lambda)) return true;

	return false;
}

void MeshObj::SetupMesh()
{
	glCheckError();
	glGenVertexArrays(1, &vao_);
	glGenBuffers(1, &vbo_);
	glCheckError();
	glGenBuffers(1, &ebo_);
	glCheckError();
	glBindVertexArray(vao_);
	glBindBuffer(GL_ARRAY_BUFFER, vbo_);
	glCheckError();
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(VertexObj), &vertices_[0], GL_STATIC_DRAW);
	glCheckError();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices_.size() * sizeof(unsigned),
		&indices_[0],
		GL_STATIC_DRAW);
	glCheckError();

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexObj), (void*) offsetof(VertexObj, position));
	glCheckError();

	//TexCoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexObj), (void*) offsetof(VertexObj, texCoords));
	glCheckError();

	//Normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexObj), (void*) offsetof(VertexObj, normal));
	glCheckError();

	//Tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3, 3, GL_FLOAT, GL_FALSE, sizeof(VertexObj), (void*) offsetof(VertexObj, tangent));
	glCheckError();

	//Bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(
		4, 3, GL_FLOAT, GL_FALSE, sizeof(VertexObj), (void*) offsetof(VertexObj, bitangent));
	glCheckError();
	glBindVertexArray(0);
	glCheckError();
}

size_t MeshObj::GetTexture(sr::Texture::TextureType type)
{
	for (size_t i = 0; i < textures_.size(); ++i)
		if (textures_[i].type == type)
			return i;

	return INVALID_INDEX;
}

void MeshObj::BindTextures(const gl::Shader& shader) const
{
    glCheckError();

	for (int i = 0; i < 4; ++i)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, INVALID_TEXTURE_NAME);
	}

	uint8_t usedMaps = 0;
    for (const auto & texture : textures_)
    {
        uint8_t number = 0;
        std::string name;
        switch(texture.type)
        {
            case Texture::TextureType::DIFFUSE:
                name = "diffuse";
                number = 0;
                break;
            case Texture::TextureType::SPECULAR:
                name = "specular";
                number = 1;
                break;
            case Texture::TextureType::NORMAL:
                name = "normal";
                number = 2;
                break;
            case Texture::TextureType::EMISSIVE:
                name = "emissive";
                number = 3;
                break;
            default: ;
        }
	    glActiveTexture(GL_TEXTURE0 + number);
		usedMaps |= 1u << number;
        shader.SetInt(std::string("material.").append(name), number);
        glBindTexture(GL_TEXTURE_2D, texture.name);
    }
    shader.SetFloat("material.shininess", shininess_);
	shader.SetVec3("material.diffuseColor", diffuse_);
	shader.SetVec3("material.specularColor", specular_);

	shader.SetUInt("usedMaps", usedMaps);
    glActiveTexture(GL_TEXTURE0);
    glCheckError();
}

bool MeshObj::operator==(const MeshObj& other) const
{
	return name_ == other.name_;
}
}
#endif