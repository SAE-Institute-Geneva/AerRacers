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
#include "assimp/scene.h"
#include "assimp/material.h"

#include "gl/gles3_include.h"
#include "graphics/graphics.h"
#include "graphics/texture.h"

namespace neko::sr
{
Mesh::Mesh() : loadMeshToGpu([this]
{
    SetupMesh();
})
{}

void Mesh::Init()
{
#ifdef NEKO_SAMETHREAD
    loadMeshToGpu.Execute();
    const TextureManagerInterface& textureManager = TextureManagerLocator::get();
    for (auto& texture : textures_)
    {
        texture.textureName = textureManager.GetTexture(texture.textureId).name;
    }
#else
    RendererLocator::get().AddPreRenderJob(&loadMeshToGpu);
    auto& textureManager = TextureManagerLocator::get();
    for (auto& texture : textures_)
    {
    	//Waiting for texture to be loaded
    	while(!textureManager.IsTextureLoaded(texture.textureId)) {}
        texture.textureName = textureManager.GetTexture(texture.textureId).name;
    }
#endif
}


void Mesh::Draw(const gl::Shader& shader) const
{
    BindTextures(shader);
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
        gl::DestroyTexture(texture.textureName);
	}

    textures_.clear();
    vertices_.clear();
    indices_.clear();

    loadMeshToGpu.Reset();
}


void Mesh::ProcessMesh(
    const aiMesh* mesh, 
    const aiScene* scene,
    const std::string_view directory,
    std::string_view path)
{
    min_ = Vec3f(mesh->mAABB.mMin);
    max_ = Vec3f(mesh->mAABB.mMax);
	
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        // process vertex positions, normals and texture coordinates
	    vertex.position = Vec3f(mesh->mVertices[i]);
	    if (GetFilenameExtension(path) == ".fbx")
	    {
		    vertex.position = Quaternion::AngleAxis(degree_t(90.0f), Vec3f::left) * vertex.position;
	    }

	    vertex.normal = Vec3f(mesh->mNormals[i]);
    	//TODO: why is tangent sometimes null even with CalcTangent
        if (mesh->mTangents != nullptr)
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

        textures_.reserve(
        material->GetTextureCount(aiTextureType_SPECULAR) +
	        material->GetTextureCount(aiTextureType_DIFFUSE) + 
            material->GetTextureCount(aiTextureType_HEIGHT) +
            material->GetTextureCount(aiTextureType_EMISSIVE));
    	
        material->Get(AI_MATKEY_SHININESS, specularExponent_);
        LoadMaterialTextures(material,
            aiTextureType_DIFFUSE, Texture::TextureType::DIFFUSE, directory);
        LoadMaterialTextures(material,
            aiTextureType_SPECULAR, Texture::TextureType::SPECULAR, directory);
        LoadMaterialTextures(material,
            aiTextureType_HEIGHT, Texture::TextureType::NORMAL, directory);
        LoadMaterialTextures(material,
            aiTextureType_EMISSIVE, Texture::TextureType::EMISSIVE, directory);
    }
}

bool Mesh::IsLoaded() const
{
    if (!loadMeshToGpu.IsDone()) return false;

    const auto& textureManager = TextureManagerLocator::get();
	for (const auto& texture : textures_)
	{
        if (!textureManager.IsTextureLoaded(texture.textureId))
        {
            return false;
        }
	}

    return true;
}

void Mesh::SetupMesh()
{
    glCheckError();
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO); glCheckError();
    glGenBuffers(1, &EBO); glCheckError();
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO); glCheckError();
    glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(Vertex), &vertices_[0], GL_STATIC_DRAW); glCheckError();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(unsigned int), &indices_[0], GL_STATIC_DRAW); glCheckError();

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)nullptr); glCheckError();

    // vertex texture coords
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoords)); glCheckError();

    // vertex normals
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal)); glCheckError();

    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, tangent)); glCheckError();
    glBindVertexArray(0); glCheckError();
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
        std::string path = directory.data();
        path += '/';
    	path += str.C_Str();
    	
        assTexture.textureId = textureManager.LoadTexture(path);
    }
}

void Mesh::BindTextures(const gl::Shader& shader) const
{
    glCheckError();
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int normalNr = 1;
    unsigned int emissiveNr = 1;
    for (size_t i = 0; i < textures_.size(); i++)
    {
        // activate proper texture unit before binding
        glActiveTexture(GL_TEXTURE0 + i);
        // retrieve texture number (the N in diffuse_textureN)
        std::string number;
        std::string name;
        switch(textures_[i].type)
        {
            case Texture::TextureType::DIFFUSE:
                name = "texture_diffuse";
                number = std::to_string(diffuseNr++);
                break;
            case Texture::TextureType::SPECULAR:
                name = "texture_specular";
                number = std::to_string(specularNr++);
                break;
            case Texture::TextureType::NORMAL:
                name = "texture_normal";
                number = std::to_string(normalNr++);
                break;
            case Texture::TextureType::EMISSIVE:
                name = "texture_emissive";
                number = std::to_string(emissiveNr++);
                break;
            default: ;
        }
        shader.SetInt(std::string("material.").append(name).append(number), i);
        glBindTexture(GL_TEXTURE_2D, textures_[i].textureName);
    }
    shader.SetFloat("material.shininess", specularExponent_);
    shader.SetBool("enableNormalMap", normalNr > 1);
    glActiveTexture(GL_TEXTURE0);
    glCheckError();
}
}
#endif