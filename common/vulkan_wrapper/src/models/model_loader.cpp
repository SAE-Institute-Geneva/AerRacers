#include "vk/material/material_manager.h"
#include "vk/vk_resources.h"

namespace neko::vk
{
ModelLoader::ModelLoader(std::string_view path, ModelId modelId)
   : path_(path),
	 modelId_(modelId),
	 loadModelJob_([this]() { LoadModel(); }),
	 processModelJob_([this]() { ProcessModel(); })
{}

ModelLoader::ModelLoader(ModelLoader&& other) noexcept
   : path_(std::move(other.path_)),
	 modelId_(other.modelId_),
	 loadModelJob_([this]() { LoadModel(); }),
	 processModelJob_([this]() { ProcessModel(); })
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
		auto& materialManager = MaterialManagerLocator::get();
		bool isLoaded = false;

		// load textures if possible
		for (auto& mesh : model_.meshes_)
		{
			std::uint8_t loadedTextures = 0;
			DiffuseMaterial& material = materialManager.GetDiffuseMaterial(mesh.materialId_);
			if (diffuseId_ != INVALID_TEXTURE_ID)
			{
				const auto* texturePtr = textureManager.GetTexture(diffuseId_);
				if (texturePtr)
				{
					material.SetDiffuse(*texturePtr);
					loadedTextures |= DiffuseMaterial::DIFFUSE;
				}
			}

			if (specularId_ != INVALID_TEXTURE_ID)
			{
				const auto* texturePtr = textureManager.GetTexture(specularId_);
				if (texturePtr)
				{
					material.SetSpecular(*texturePtr);
					loadedTextures |= DiffuseMaterial::SPECULAR;
				}
			}

			if (normalId_ != INVALID_TEXTURE_ID)
			{
				const auto* texturePtr = textureManager.GetTexture(normalId_);
				if (texturePtr)
				{
					material.SetNormal(*texturePtr);
					loadedTextures |= DiffuseMaterial::NORMAL;
				}
			}

			if (loadedTextures == textureMaps_) isLoaded = true;
		}

		if (isLoaded) flags_ |= LOADED;
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
	EASY_BLOCK("vk::Process Model");
#endif
	model_.meshes_.reserve(shapes_.size());
	for (const auto& shape : shapes_)
	{
		auto& mesh = model_.meshes_.emplace_back();
		if (!shape.mesh.material_ids.empty() && shape.mesh.material_ids[0] != -1)
		{
			const tinyobj::material_t mat = materials_[shape.mesh.material_ids[0]];
			if (!mat.diffuse_texname.empty())
				LoadMaterialTextures(mat, mesh, DiffuseMaterial::DIFFUSE, mat.diffuse_texname);
			if (!mat.specular_texname.empty())
				LoadMaterialTextures(mat, mesh, DiffuseMaterial::SPECULAR, mat.specular_texname);
			if (!mat.bump_texname.empty())
				LoadMaterialTextures(mat, mesh, DiffuseMaterial::NORMAL, mat.bump_texname);
			if (!mat.emissive_texname.empty())
				LoadMaterialTextures(mat, mesh, DiffuseMaterial::EMISSIVE, mat.emissive_texname);
		}

		std::vector<Vertex> vertices;
		vertices.reserve(shape.mesh.indices.size());

		std::vector<std::uint32_t> indices;
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

			// Y coordinates are inverted in Vulkan
			vertex.texCoords = {
				attrib_.texcoords[2 * index.texcoord_index + 0],
				-attrib_.texcoords[2 * index.texcoord_index + 1]
			};

			if (uniqueVertices.count(vertex) == 0)
			{
				uniqueVertices[vertex] = vertices.size();
				vertices.emplace_back(vertex);
			}

			indices.emplace_back(static_cast<std::uint32_t>(uniqueVertices[vertex]));
		}

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

		mesh.InitData(vertices, indices);
	}

#ifdef EASY_PROFILE_USE
	EASY_END_BLOCK
#endif
}

void ModelLoader::LoadMaterialTextures(const tinyobj::material_t& mat,
	Mesh& mesh,
	DiffuseMaterial::TextureMaps textureType,
	std::string_view texName)
{
	auto& textureManager = TextureManagerLocator::get();
	auto& materialManager = MaterialManagerLocator::get();
	{
		if (materialManager.IsMaterialLoaded(mat.name))
		{
			mesh.materialId_ = HashString(mat.name);
			return;
		}

		const Vec3f diffuseCol = {mat.diffuse[0], mat.diffuse[1], mat.diffuse[2]};
		mesh.materialId_       = materialManager.AddNewMaterial(mat.name, MaterialType::DIFFUSE);
		materialManager.GetDiffuseMaterial(mesh.materialId_).SetShininess(mat.shininess);
		materialManager.GetDiffuseMaterial(mesh.materialId_).SetColor(Vec4f(diffuseCol, 1.0f));

		const ResourceHash textureId =
			textureManager.AddTexture(fmt::format("{}/{}.ktx", directory_, texName));
		switch (textureType)
		{
			case DiffuseMaterial::DIFFUSE:
				textureMaps_ |= DiffuseMaterial::DIFFUSE;
				diffuseId_ = textureId;
				break;
			case DiffuseMaterial::SPECULAR:
				textureMaps_ |= DiffuseMaterial::SPECULAR;
				specularId_ = textureId;
				break;
			case DiffuseMaterial::NORMAL:
				textureMaps_ |= DiffuseMaterial::NORMAL;
				normalId_ = textureId;
				break;
			case DiffuseMaterial::EMISSIVE: break;
			default: logDebug("Unsupported texture type"); break;
		}
	}

	/*auto& texture        = mesh.textures_.emplace_back();
	texture.type         = textureType;
	texture.sName        = matName;

	texture.textureId = textureManager.LoadTexture(directory_ + matName.data());*/
}
}    // namespace neko::vk