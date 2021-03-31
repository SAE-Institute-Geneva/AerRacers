#include "gl/model_loader.h"

#include "engine/engine.h"
#include "graphics/graphics.h"

#ifdef EASY_PROFILE_USE
#include "easy/profiler.h"
#endif

namespace neko::gl
{
ModelLoader::ModelLoader(std::string_view path, ModelId modelId)
	: path_(path),
	  modelId_(modelId),
	  loadModelJob_([this]() { LoadModel(); }),
	  processModelJob_([this]() { ProcessModel(); }),
	  uploadJob_([this]() { UploadMeshesToGl(); })
{}

ModelLoader::ModelLoader(ModelLoader&& modelLoader) noexcept
	: path_(std::move(modelLoader.path_)),
	  modelId_(modelLoader.modelId_),
	  loadModelJob_([this]() { LoadModel(); }),
	  processModelJob_([this]() { ProcessModel(); }),
	  uploadJob_([this]() { UploadMeshesToGl(); })
{}

void ModelLoader::Start()
{
	directory_ = path_.substr(0, path_.find_last_of('/'));
	BasicEngine::GetInstance()->ScheduleJob(&loadModelJob_, JobThreadType::RESOURCE_THREAD);
}

void ModelLoader::Update()
{
	if (flags_ & ERROR_LOADING) return;
	if (!(flags_ & LOADED) && processModelJob_.IsDone())
	{
		const auto& textureManager = TextureManagerLocator::get();

		// load textures if possible
		for (auto& mesh : model_.meshes_)
		{
			for (auto& texture : mesh.textures_)
			{
				if (texture.textureId != INVALID_TEXTURE_ID &&
				    texture.textureName == INVALID_TEXTURE_NAME)
				{
					const auto* texturePtr = textureManager.GetTexture(texture.textureId);
					if (texturePtr) 
						texture.textureName = texturePtr->name;
					else return;
				}
			}
		}

		if (uploadJob_.IsDone()) flags_ = flags_ | LOADED;
	}
}

void ModelLoader::LoadModel()
{
	std::string war, err;
	tinyobj::LoadObj(&attrib_, &shapes_, &materials_, &war, &err, path_.c_str(), directory_.c_str());
	if (!war.empty()) logDebug(war);
	if (!err.empty()) logDebug(err);

	BasicEngine::GetInstance()->ScheduleJob(&processModelJob_, JobThreadType::OTHER_THREAD);
}

void ModelLoader::ProcessModel()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Process Shapes");
#endif
	model_.meshes_.reserve(shapes_.size());
	for (const auto& shape : shapes_) ProcessShape(shape);
#ifdef EASY_PROFILE_USE
	EASY_END_BLOCK;
#endif

	RendererLocator::get().AddPreRenderJob(&uploadJob_);
}

void ModelLoader::ProcessShape(const tinyobj::shape_t& shape)
{
	auto& mesh = model_.meshes_.emplace_back();
	if (!shape.mesh.material_ids.empty() && shape.mesh.material_ids[0] != -1)
	{
		const tinyobj::material_t mat = materials_[shape.mesh.material_ids[0]];
		mesh.shininess_ = mat.shininess;
		mesh.color_ = {mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]};

		if (!mat.diffuse_texname.empty())
			LoadMaterialTextures(mat, mesh, Mesh::Texture::DIFFUSE, mat.diffuse_texname);
		if (!mat.specular_texname.empty())
			LoadMaterialTextures(mat, mesh, Mesh::Texture::SPECULAR, mat.specular_texname);
		if (!mat.bump_texname.empty())
			LoadMaterialTextures(mat, mesh, Mesh::Texture::NORMAL, mat.bump_texname);
		if (!mat.emissive_texname.empty())
			LoadMaterialTextures(mat, mesh, Mesh::Texture::EMISSIVE, mat.emissive_texname);
	}

	auto minExtents = Vec3f(MAXFLOAT);
	auto maxExtents = Vec3f(-MAXFLOAT);

	std::vector<Vertex> vertices;
	vertices.reserve(shape.mesh.indices.size());

	std::vector<Index> indices;
	indices.reserve(shape.mesh.indices.size());

	std::unordered_map<Vertex, std::size_t> uniqueVertices;
	for (const auto& index : shape.mesh.indices)
	{
		Vertex vertex{};
		vertex.position = {
			attrib_.vertices[3 * index.vertex_index + 0],
			attrib_.vertices[3 * index.vertex_index + 1],
			attrib_.vertices[3 * index.vertex_index + 2]
		};
		vertex.normal = {
			attrib_.normals[3 * index.normal_index + 0],
			attrib_.normals[3 * index.normal_index + 1],
			attrib_.normals[3 * index.normal_index + 2]
		};

		vertex.texCoords = {
			attrib_.texcoords[2 * index.texcoord_index + 0],
			-attrib_.texcoords[2 * index.texcoord_index + 1]
		};

		minExtents = Vec3f(std::min(minExtents.x, vertex.position.x),
			std::min(minExtents.y, vertex.position.y),
			std::min(minExtents.z, vertex.position.z));
		maxExtents = Vec3f(std::max(maxExtents.x, vertex.position.x),
			std::max(maxExtents.y, vertex.position.y),
			std::max(maxExtents.z, vertex.position.z));

		if (uniqueVertices.count(vertex) == 0)
		{
			uniqueVertices[vertex] = vertices.size();
			vertices.emplace_back(vertex);
		}

		indices.emplace_back(static_cast<Index>(uniqueVertices[vertex]));
	}

	mesh.aabb_.lowerLeftBound  = minExtents;
	mesh.aabb_.upperRightBound = maxExtents;
	for (std::size_t i = 0; i < indices.size(); i += 3)
	{
		if (i + 2 >= indices.size()) break;

		const unsigned index  = indices[i];
		const unsigned index1 = indices[i + 1];
		const unsigned index2 = indices[i + 2];
		const Vec3f edge1     = vertices[index1].position - vertices[index].position;
		const Vec3f edge2     = vertices[index2].position - vertices[index].position;
		const Vec2f deltaUv1  = vertices[index1].texCoords - vertices[index].texCoords;
		const Vec2f deltaUv2  = vertices[index2].texCoords - vertices[index].texCoords;

		const float f             = 1.0f / (deltaUv1.u * deltaUv2.v - deltaUv2.u * deltaUv1.v);
		vertices[index].tangent.x = f * (deltaUv2.v * edge1.x - deltaUv1.v * edge2.x);
		vertices[index].tangent.y = f * (deltaUv2.v * edge1.y - deltaUv1.v * edge2.y);
		vertices[index].tangent.z = f * (deltaUv2.v * edge1.z - deltaUv1.v * edge2.z);
		vertices[index1].tangent  = vertices[index].tangent;
		vertices[index2].tangent  = vertices[index].tangent;

		vertices[index].bitangent.x = f * (-deltaUv2.x * edge1.x + deltaUv1.x * edge2.x);
		vertices[index].bitangent.y = f * (-deltaUv2.x * edge1.y + deltaUv1.x * edge2.y);
		vertices[index].bitangent.z = f * (-deltaUv2.x * edge1.z + deltaUv1.x * edge2.z);
		vertices[index1].bitangent  = vertices[index].bitangent;
		vertices[index2].bitangent  = vertices[index].bitangent;
	}

	mesh.vertices_ = std::move(vertices);
	mesh.indices_  = std::move(indices);
}

void ModelLoader::LoadMaterialTextures(const tinyobj::material_t& mat,
                                       Mesh& mesh,
                                       Mesh::Texture::Type textureType,
                                       std::string_view textureName) const
{
	auto& textureManager = TextureManagerLocator::get();
	const TextureId textureId =
		textureManager.LoadTexture(fmt::format("{}/{}", directory_, textureName));

	mesh.textures_.push_back({textureId, INVALID_TEXTURE_NAME, textureType});
}

void ModelLoader::UploadMeshesToGl()
{
#ifdef EASY_PROFILE_USE
	EASY_BLOCK("Upload Meshes to GPU");
#endif
	for (auto& mesh : model_.meshes_) mesh.Init();
#ifdef EASY_PROFILE_USE
	EASY_END_BLOCK;
#endif
}
}