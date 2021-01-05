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

void Model::Draw(const gl::Shader& shader)
{
	for (auto& mesh : meshes_)
		mesh.Draw(shader);
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
	directory_ = path.substr(0, path.find_last_of('/'));
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

	for (const auto& mesh : meshes_)
	{
		if (!mesh.IsLoaded())
			return false;
	}

	return true;
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
		auto* ioSystem = new NekoIOSystem();
		import.SetIOHandler(ioSystem);

		scene = import.ReadFile(path_.data(),
		                        aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals |
		                        aiProcess_CalcTangentSpace);
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

		aiMesh* assMesh = scene->mMeshes[node->mMeshes[i]];
		mesh.name_ = node->mName.C_Str();
		if (node->mParent->mNumMeshes != 0)
			mesh.parentName_ = HashString(node->mParent->mName.C_Str());
		mesh.ProcessMesh(assMesh, scene, directory_, path_);
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProcessNode(node->mChildren[i], scene);
	}
}
}
#endif