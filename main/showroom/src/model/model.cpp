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
#include "showroom/model/model.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <engine/log.h>

#include "io_system.h"
#include "engine/engine.h"

#include <fmt/format.h>

namespace neko::sr
{
Model::Model() : processModelJob_([this]
                                  {
	                                  ProcessModel();
                                  })
{}

void Model::Draw(const gl::Shader& shader, const Mat4f& modelMat)
{
	for (auto& mesh : meshes_)
		mesh.Draw(shader, modelMat);
}

void Model::Destroy()
{
	for (auto& mesh : meshes_)
		mesh.Destroy();
	meshes_.clear();
	processModelJob_.Reset();
}

void Model::LoadModel(std::string_view path)
{
	path_ = std::string(path);
#ifdef _WIN32
	directory_ = path.substr(0, path.find_last_of("\\"));
#elif linux
	directory_ = path.substr(0, path.find_last_of('/'));
#endif
	logDebug(fmt::format("ASSIMP: Loading model: {}", path_));
#ifdef NEKO_SAMETHREAD
	processModelJob_.Execute();
#else
	BasicEngine::GetInstance()->ScheduleJob(&processModelJob_, JobThreadType::OTHER_THREAD);
#endif
}

bool Model::IsLoaded() const
{
	if (!processModelJob_.IsDone())
	{
		return false;
	}

	if (std::all_of(meshes_.cbegin(), meshes_.cend(), [](const Mesh& mesh) { return mesh.IsLoaded(); }))
		return true;

	return false;
}

void Model::ProcessModel()
{
	Assimp::Importer import;
	const aiScene* scene = nullptr;
#ifdef NEKO_SAMETHREAD
	//assimp delete automatically the IO System
	NekoIOSystem* ioSystem = new NekoIOSystem();
	import.SetIOHandler(ioSystem);

	scene = import.ReadFile(path_.data(),
							aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals |
							aiProcess_CalcTangentSpace);
#else
	Job loadingModelJob = Job([this, &import, &scene]
	{
		//assimp delete automatically the IO System
		import.SetIOHandler(new NekoIOSystem(BasicEngine::GetInstance()->GetFilesystem()));

		scene = import.ReadFile(path_.data(),
		                        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals |
		                        aiProcess_CalcTangentSpace | aiProcess_GenBoundingBoxes);
	});
	BasicEngine::GetInstance()->ScheduleJob(&loadingModelJob, JobThreadType::RESOURCE_THREAD);
	loadingModelJob.Join();
#endif

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		logDebug(fmt::format("[ERROR] ASSIMP {}", import.GetErrorString()));
		return;
	}

	name_ = scene->mRootNode->mName.C_Str();
	meshes_.reserve(scene->mNumMeshes);
	ProcessNode(scene->mRootNode, scene);
	for (auto& mesh : meshes_)
	{
		mesh.Init();
	}
}

void Model::ProcessNode(aiNode* node, const aiScene* scene)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		meshes_.emplace_back();
		auto& mesh = meshes_.back();

		aiVector3D pos, scale;
		aiQuaternion rot;
		node->mTransformation.Decompose(scale, rot, pos);

		Quaternion newRot = Quaternion::AngleAxis(degree_t(90.0f), Vec3f::right) * Quaternion(rot);
		mesh.modelMat_    = Transform3d::ScalingMatrixFrom(Vec3f(scale) / 100.0f);
		mesh.modelMat_    = Transform3d::Rotate(mesh.modelMat_, newRot);
		mesh.modelMat_    = Transform3d::Translate(mesh.modelMat_, Vec3f(pos));

		aiMesh* assMesh      = scene->mMeshes[node->mMeshes[i]];
		const std::string meshName = node->mName.C_Str();

		const auto it = std::find_if(meshes_.cbegin(),
			meshes_.cend(),
			[meshName](const Mesh& mesh) { return mesh.name_ == meshName; });
		if (it != meshes_.end())
			mesh.name_ = it->name_ + "_0";
		else
			mesh.name_ = meshName;

		node->mName = mesh.name_;
		if (node->mParent->mNumMeshes != 0)
			mesh.parentName_ = HashString(std::string(node->mParent->mName.C_Str()));
		mesh.ProcessMesh(assMesh, scene, directory_, path_);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}

size_t Model::GetMeshId(const Mesh& mesh) const
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