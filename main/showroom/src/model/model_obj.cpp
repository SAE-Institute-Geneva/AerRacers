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
#include "showroom/model/model_obj.h"

#include <tiny_obj_loader.h>

#include "engine/engine.h"

namespace neko::sr
{
ModelObj::ModelObj()
   : processModelJob_([this] { ProcessModel(); }),
	 initModelJob_(
		 [this]
		 {
			 for (auto& mesh : meshes_) mesh.Init();
		 })
{}

void ModelObj::Draw(const gl::Shader& shader, const Mat4f& modelMat)
{
	for (auto& mesh : meshes_) mesh.Draw(shader, modelMat);
}

void ModelObj::Destroy()
{
	for (auto& mesh : meshes_) mesh.Destroy();
	meshes_.clear();
	processModelJob_.Reset();
}

void ModelObj::LoadModel(std::string_view path)
{
	path_ = std::string(path);
#ifdef _WIN32
	directory_ = path.substr(0, path.find_last_of("\\"));
	directory_ += '\\';
	name_ = path.substr(path.find_last_of('\\') + 1, path.size());
#elif linux
	directory_ = path.substr(0, path.find_last_of('/'));
	directory_ += '/';
	name_ = path.substr(path.find_last_of('/') + 1, path.size());
#endif
	logDebug(fmt::format("ASSIMP: Loading model: {}", path_));
	BasicEngine::GetInstance()->ScheduleJob(&processModelJob_, JobThreadType::RESOURCE_THREAD);
}

bool ModelObj::IsLoaded() const
{
	if (!processModelJob_.IsDone()) return false;
	if (std::all_of(
			meshes_.cbegin(), meshes_.cend(), [](const MeshObj& mesh) { return mesh.IsLoaded(); }))
		return true;

	return false;
}

void ModelObj::ProcessModel()
{
	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string war, err;
	tinyobj::LoadObj(&attrib, &shapes, &materials, &war, &err, path_.c_str(), directory_.c_str());
	for (const auto& shape : shapes)
	{
		auto& mesh = meshes_.emplace_back();
		const std::string meshName = shape.name;
		const auto it              = std::find_if(meshes_.cbegin(),
            meshes_.cend(),
            [meshName](const MeshObj& mesh) { return mesh.name_ == meshName; });
		if (it != meshes_.end()) mesh.name_ = it->name_ + "_0";
		else mesh.name_ = meshName;

		if (!shape.mesh.material_ids.empty())
		{
			const tinyobj::material_t mat = materials[shape.mesh.material_ids[0]];
			mesh.diffuse_ = {mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]};
			mesh.specular_ = {mat.specular[0], mat.specular[1], mat.specular[2]};
			mesh.shininess_ = mat.shininess;

			if (!mat.diffuse_texname.empty())
				LoadMaterialTextures(mesh, Texture::TextureType::DIFFUSE, mat.diffuse_texname);
			if (!mat.specular_texname.empty())
				LoadMaterialTextures(mesh, Texture::TextureType::SPECULAR, mat.specular_texname);
			if (!mat.bump_texname.empty())
				LoadMaterialTextures(mesh, Texture::TextureType::NORMAL, mat.bump_texname);
			if (!mat.emissive_texname.empty())
				LoadMaterialTextures(mesh, Texture::TextureType::EMISSIVE, mat.emissive_texname);
		}

		mesh.modelMat_             = Mat4f::Identity;
		auto minExtents = Vec3f(std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max(),
			std::numeric_limits<float>::max());
		auto maxExtents = Vec3f(std::numeric_limits<float>::min(),
			std::numeric_limits<float>::min(),
			std::numeric_limits<float>::min());
		std::unordered_map<VertexObj, std::size_t> uniqueVertices;
		for (const auto& index : shape.mesh.indices)
		{
			VertexObj vertex{};
			vertex.position = {
				attrib.vertices[3 * index.vertex_index + 0],
				attrib.vertices[3 * index.vertex_index + 1],
				attrib.vertices[3 * index.vertex_index + 2]
			};
			vertex.normal = {
				attrib.normals[3 * index.normal_index + 0],
				attrib.normals[3 * index.normal_index + 1],
				attrib.normals[3 * index.normal_index + 2]
			};
			vertex.texCoords = {
				attrib.texcoords[2 * index.texcoord_index + 0],
				attrib.texcoords[2 * index.texcoord_index + 1]
			};

			minExtents = Vec3f(std::min(minExtents.x, vertex.position.x),
				std::min(minExtents.y, vertex.position.y),
				std::min(minExtents.z, vertex.position.z));
			maxExtents = Vec3f(std::max(maxExtents.x, vertex.position.x),
				std::max(maxExtents.y, vertex.position.y),
				std::max(maxExtents.z, vertex.position.z));

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = mesh.vertices_.size();
				mesh.vertices_.emplace_back(vertex);
			}

			mesh.indices_.emplace_back(static_cast<unsigned>(uniqueVertices[vertex]));
		}

		mesh.aabb_.lowerLeftBound  = minExtents;
		mesh.aabb_.upperRightBound = maxExtents;
		for(std::size_t i = 0; i < mesh.vertices_.size(); i+=3)
		{
			const Vec3f edge1    = mesh.vertices_[i + 1].position - mesh.vertices_[i].position;
			const Vec3f edge2    = mesh.vertices_[i + 2].position - mesh.vertices_[i].position;
			const Vec2f deltaUv1 = mesh.vertices_[i + 1].texCoords - mesh.vertices_[i].texCoords;
			const Vec2f deltaUv2 = mesh.vertices_[i + 2].texCoords - mesh.vertices_[i].texCoords;

			const float f = 1.0f / (deltaUv1.u * deltaUv2.v - deltaUv2.u * deltaUv1.v);
			mesh.vertices_[i].tangent.x   = f * (deltaUv2.v * edge1.x - deltaUv1.v * edge2.x);
			mesh.vertices_[i].tangent.y   = f * (deltaUv2.v * edge1.y - deltaUv1.v * edge2.y);
			mesh.vertices_[i].tangent.z   = f * (deltaUv2.v * edge1.z - deltaUv1.v * edge2.z);
			mesh.vertices_[i + 1].tangent = mesh.vertices_[i].tangent;
			mesh.vertices_[i + 2].tangent = mesh.vertices_[i].tangent;

			mesh.vertices_[i].bitangent.x   = f * (-deltaUv2.x * edge1.x + deltaUv1.x * edge2.x);
			mesh.vertices_[i].bitangent.y   = f * (-deltaUv2.x * edge1.y + deltaUv1.x * edge2.y);
			mesh.vertices_[i].bitangent.z   = f * (-deltaUv2.x * edge1.z + deltaUv1.x * edge2.z);
			mesh.vertices_[i + 1].bitangent = mesh.vertices_[i].bitangent;
			mesh.vertices_[i + 2].bitangent = mesh.vertices_[i].bitangent;
		}
	}

	BasicEngine::GetInstance()->ScheduleJob(&initModelJob_, JobThreadType::OTHER_THREAD);
}

void ModelObj::LoadMaterialTextures(
	MeshObj& mesh, Texture::TextureType textureType, std::string_view matName)
{
	auto& textureManager = TextureManagerLocator::get();
	auto& texture        = mesh.textures_.emplace_back();
	texture.type         = textureType;
	texture.sName        = matName;

	texture.textureId = textureManager.LoadTexture(directory_ + matName.data());
}

size_t ModelObj::GetMeshId(const MeshObj& mesh) const
{
	auto it = std::find(meshes_.cbegin(), meshes_.cend(), mesh);
	if (it != meshes_.end())
	{
		//Font is already loaded
		return it - meshes_.cbegin();
	}

	return INVALID_INDEX;
}
}
#endif