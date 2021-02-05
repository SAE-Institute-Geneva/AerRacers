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
	#include "showroom/model/mesh.h"

	#include "assimp/material.h"
	#include "assimp/scene.h"

	#include "graphics/graphics.h"
	#include "graphics/texture.h"
	#include "io_system.h"

namespace neko::sr
{
Mesh::Mesh() : loadMeshToGpu([this]
{
    SetupMesh();
})
{}

void Mesh::Init()
{
	RendererLocator::get().AddPreRenderJob(&loadMeshToGpu);
	auto& textureManager = TextureManagerLocator::get();
	for (auto& texture : textures_)
	{
		//Waiting for texture to be loaded
		while (!textureManager.IsTextureLoaded(texture.textureId)) {}
		if (textureManager.GetTexture(texture.textureId))
			texture.name = textureManager.GetTexture(texture.textureId)->name;
	}
}


void Mesh::Draw(const gl::Shader& shader) const
{
    BindTextures(shader);
	shader.SetMat4("model", modelMat_);

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices_.size(), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Mesh::Destroy()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

	for(auto& texture : textures_)
	{
        gl::DestroyTexture(texture.name);
	}

    textures_.clear();
    vertices_.clear();
    indices_.clear();

    loadMeshToGpu.Reset();
}

void Mesh::UpdateTextures()
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

void Mesh::ProcessMesh(
    const aiMesh* mesh, 
    const aiScene* scene,
    const std::string_view directory,
    std::string_view path)
{
	aabb_.lowerLeftBound  = Vec3f(mesh->mAABB.mMin);
	aabb_.upperRightBound = Vec3f(mesh->mAABB.mMax);
    modelMat_ = Mat4f::Identity;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // process vertex positions, normals and texture coordinates
	    vertex.position = Vec3f(mesh->mVertices[i]);

	    vertex.normal = Vec3f(mesh->mNormals[i]);
    	//TODO: why is tangent sometimes null even with CalcTangent
        if (mesh->mTangents)
        {
	        vertex.tangent = Vec3f(mesh->mTangents[i]);
	        vertex.bitangent = Vec3f(mesh->mBitangents[i]);
        }

	    if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		    vertex.texCoords = Vec2f(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
	    else
		    vertex.texCoords = Vec2f(0.0f, 0.0f);

        vertices_.push_back(vertex);
    }

    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
	    const aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices_.push_back(face.mIndices[j]);
    }

    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

        const bool hasHeight = material->GetTextureCount(aiTextureType_HEIGHT) != 0;
        textures_.reserve(material->GetTextureCount(aiTextureType_SPECULAR) +
            material->GetTextureCount(aiTextureType_DIFFUSE) +
            material->GetTextureCount(aiTextureType_EMISSIVE) +
            (hasHeight ?
                 material->GetTextureCount(aiTextureType_HEIGHT) :
                 material->GetTextureCount(aiTextureType_NORMALS)));

		material->Get(AI_MATKEY_SHININESS, specularExponent_);

		aiColor4D diffuse;
	    aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &diffuse);
		diffuse_ = Color3(diffuse.r, diffuse.g, diffuse.b);

		aiColor4D specular;
	    aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &specular);
	    specular_ = Color3(specular.r, specular.g, specular.b);

        LoadMaterialTextures(material,
            aiTextureType_DIFFUSE,
            Texture::TextureType::DIFFUSE,
            directory);
        LoadMaterialTextures(material,
            aiTextureType_SPECULAR,
            Texture::TextureType::SPECULAR,
            directory);

        if (hasHeight)
            LoadMaterialTextures(material,
                aiTextureType_HEIGHT,
                Texture::TextureType::NORMAL,
                directory);
        else
            LoadMaterialTextures(material,
                aiTextureType_NORMALS,
                Texture::TextureType::NORMAL,
                directory);

        LoadMaterialTextures(material,
            aiTextureType_EMISSIVE,
            Texture::TextureType::EMISSIVE,
            directory);
    }
}

bool Mesh::IsLoaded() const
{
    if (!loadMeshToGpu.IsDone()) return false;

    auto lambda = [](const Texture& texture)
    {
	    const auto& textureManager = TextureManagerLocator::get();
    	return textureManager.IsTextureLoaded(texture.textureId);
    };

	if (std::all_of(textures_.cbegin(), textures_.cend(), lambda))
		return true;

	return false;
}

void Mesh::SetupMesh()
{
	glCheckError();
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glCheckError();
	glGenBuffers(1, &EBO);
	glCheckError();
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glCheckError();
	glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW);
	glCheckError();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		indices_.size() * sizeof(unsigned int),
		&indices_[0],
		GL_STATIC_DRAW);
	glCheckError();

	//Position
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(
		0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, position));
	glCheckError();

	//TexCoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(
		1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, texCoords));
	glCheckError();

	//Normal
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(
		2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, normal));
	glCheckError();

	//Tangent
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(
		3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, tangent));
	glCheckError();

	//Bitangent
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(
		4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) offsetof(Vertex, bitangent));
	glCheckError();
	glBindVertexArray(0);
	glCheckError();
}

size_t Mesh::GetTexture(sr::Texture::TextureType type)
{
	for (size_t i = 0; i < textures_.size(); ++i)
		if (textures_[i].type == type)
			return i;

	return INVALID_INDEX;
}

void Mesh::LoadMaterialTextures(
    aiMaterial* material,
    aiTextureType aiTexture,
    Texture::TextureType texture,
    const std::string_view directory)
{
    auto& textureManager = TextureManagerLocator::get();
    for (unsigned int i = 0; i < material->GetTextureCount(aiTexture); i++)
    {
        aiString str;
        material->GetTexture(aiTexture, i, &str);

        textures_.emplace_back();
        auto& assTexture = textures_.back();
        assTexture.type = texture;
		assTexture.sName = str.C_Str();
        std::string path = directory.data();
        path += '/';
    	path += str.C_Str();

		assTexture.textureId = textureManager.LoadTexture(path);
	}
}

void Mesh::BindTextures(const gl::Shader& shader) const
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
    shader.SetFloat("material.shininess", specularExponent_);
	shader.SetVec3("material.diffuseColor", diffuse_);
	shader.SetVec3("material.specularColor", specular_);

	shader.SetUInt("usedMaps", usedMaps);
    glActiveTexture(GL_TEXTURE0);
    glCheckError();
}

bool Mesh::operator==(const Mesh& other) const
{
	return name_ == other.name_;
}
}
#endif