#include <shellapi.h>
#include <utilities/file_utility.h>
#include <stdio.h>

#include "editor/tool/project.h"
#include "editor/editor_tool_manager.h"

using std::filesystem::directory_iterator;


namespace neko::aer
{
DataResource::DataResource(const std::string name, const std::filesystem::path path) : path_(path), name_(name)
{
}

DataResource::~DataResource()
{
	path_ = "";
	name_ = "";
}

std::string DataResource::GetName() const
{
	return name_;
}

bool DataResource::GetIsSelect()
{
	return isSelect_;
}

std::filesystem::path DataResource::GetPath()
{
	return path_;
}

DataFile::DataFile(
	const std::string name, const std::filesystem::path path,
	const Type type) : DataResource(name, path), type_(type)
{
}

DataResource* DataFile::GetResourceIsSelected()
{
	if (isSelect_)
	{
		isSelect_ = false;
		return this;
	}
}

DataFile::Type DataFile::GetType()
{
	return type_;
}

//void DataFile::OpenFile()
//{
//	
//}


void DataFile::Display(Project& projectManager)
{
	ImGui::Indent();
	if (ImGui::Selectable(name_.c_str()));

	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseClicked(0))
		{
			projectManager.resourceHovered_ = this;
			projectManager.Open();
		}

		if (ImGui::IsMouseClicked(1))
		{
			projectManager.resourceHovered_ = this;
			projectManager.contextMenuIsOpen_ = true;
		}
	}
	ImGui::Unindent();
}


DataFolder::DataFolder(const std::string name, const std::filesystem::path path) : DataResource(name, path)
{
	for (const auto& file : std::filesystem::directory_iterator(path))
	{
		if (!file.path().has_extension())
		{
			// Todo Create DataFolder
			resources_.push_back(new DataFolder(GetFilename(file.path().string()), file.path().string()));
		}
		else
		{
			// Todo DataResource
			DataFile::Type type = DataFile::Type::None;
			// Text
			if (GetFilenameExtension(file.path().string()) == ".txt")
			{
				type = DataFile::Type::Text;
			}

			// Font
			if (GetFilenameExtension(file.path().string()) == ".ttf")
			{
				type = DataFile::Type::Font;
			}

			// Model
			if (GetFilenameExtension(file.path().string()) == ".obj")
			{
				type = DataFile::Type::Model;
			}


			// Prefab
			if (GetFilenameExtension(file.path().string()) == ".prefab")
			{
				type = DataFile::Type::Prefab;
			}

			// Scene
			if (GetFilenameExtension(file.path().string()) == ".scene")
			{
				type = DataFile::Type::Scene;
			}

			//Image
			if (GetFilenameExtension(file.path().string()) == ".jpg")
			{
				type = DataFile::Type::Texture;
			}
			if (GetFilenameExtension(file.path().string()) == ".JPG")
			{
				type = DataFile::Type::Texture;
			}
			if (GetFilenameExtension(file.path().string()) == ".png")
			{
				type = DataFile::Type::Texture;
			}
			if (GetFilenameExtension(file.path().string()) == ".ktx")
			{
				type = DataFile::Type::Texture;
			}
			// Sound
			if (GetFilenameExtension(file.path().string()) == ".ogg")
			{
				type = DataFile::Type::Sound;
			}
			if (GetFilenameExtension(file.path().string()) == ".wav")
			{
				type = DataFile::Type::Sound;
			}

			// Data
			if (GetFilenameExtension(file.path().string()) == ".json")
			{
				type = DataFile::Type::Data;
			}
			if (GetFilenameExtension(file.path().string()) == ".meta")
			{
				type = DataFile::Type::Data;
			}


			// Shader
			if (GetFilenameExtension(file.path().string()) == ".vert")
			{
				type = DataFile::Type::Shader;
			}
			if (GetFilenameExtension(file.path().string()) == ".frag")
			{
				type = DataFile::Type::Shader;
			}

			resources_.push_back(new DataFile(file.path().filename().string(), file.path().string(),
			                                  type));
		}
	}
}

void DataFolder::Display(Project& projectManager)
{
	if (ImGui::TreeNode((name_).c_str()))
	{
		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(1))
			{
				projectManager.resourceHovered_ = this;
				projectManager.contextMenuIsOpen_ = true;
			}
		}

		for (auto element : this->resources_)
		{
			element->Display(projectManager);
		}

		ImGui::Text("");
		ImGui::TreePop();
	}
	else
	{
		if (ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(1))
			{
				projectManager.resourceHovered_ = this;
				projectManager.contextMenuIsOpen_ = true;
			}
		}
	}
}

std::vector<DataResource*> DataFolder::GetResources() const
{
	return resources_;
}

DataResource* DataFolder::GetResourceIsSelected()
{
	if (isHover_)
	{
		return this;
	}
	else
	{
		for (auto element : resources_)
		{
			DataResource* resource = element->GetResourceIsSelected();
			if (resource != nullptr)
			{
				return resource;
			}
		}
	}

	return nullptr;
}

int DataFolder::GetSize()
{
	return resources_.size();
}


Project::Project(ToolType type, const int id, const std::string name)
	: EditorToolInterface(type, id, name), resources_(nullptr)
{
}

void Project::LoadData(const std::string path)
{
	resources_ = nullptr;
	resources_ = new DataFolder("data", path);
}

bool Project::OpenFile(const std::filesystem::path& path)
{
	//Open Text File
	if (path.extension() == ".txt")
	{
		std::string filename = path.filename().string();
		ShellExecute(0, 0, filename.c_str(), 0, 0, SW_SHOW);
		return true;
	}
	return false;
}

void Project::SetEditorToolManager(EditorToolManager* editorToolManager)
{
	editorToolManager_ = editorToolManager;
}

void Project::Open() const
{
	editorToolManager_->OpenFile(resourceHovered_->GetPath());
}

void Project::Init()
{
	resourceHovered_ = nullptr;
	LoadData(path_);
}

void Project::Update(seconds dt)
{
}

void Project::Destroy()
{
}

void Project::DrawImGui()
{
	if (isVisible)
	{
		if (!ImGui::Begin((GetName() + "##" + std::to_string(GetId())).c_str(),
		                  &isVisible))
		{
			ImGui::End();
		}
		else
		{
			// HEADER
			// nbrResource contains the quantity of Resource
			if (ImGui::Button("Refresh"))
			{
				Init();
			}
			const int nbrResourceDisplayMax =
				ImGui::GetWindowHeight() / ImGui::GetTextLineHeightWithSpacing();

			ImGui::Separator();

			// BODY
			ImGui::BeginChild("Resources");
			ImGui::Text(path_.c_str());
			ImGui::Separator();
			ImGui::Columns(2);
			auto folder = static_cast<DataFolder*>(resources_);


			//Displays Resource
			folder->Display(*this);
			ImGui::NextColumn();
			ImGui::EndChild();

			if (contextMenuIsOpen_)
			{
				ImGui::OpenPopup("context");
				if (ImGui::BeginPopupContextWindow("context"))
				{
					ImGui::Text(("   " + resourceHovered_->GetName() + "   ").c_str());
					if (ImGui::Selectable("Open File"))
					{
						Open();
						contextMenuIsOpen_ = false;
					}
					if (ImGui::Selectable("Remove File"))
					{
						remove(resourceHovered_->GetPath());
						Init();
						contextMenuIsOpen_ = false;
					}
					if (ImGui::Selectable("Back"))
					{
						contextMenuIsOpen_ = false;
					}
					ImGui::EndPopup();
				}
			}
			ImGui::End();
		}
	}
}

void Project::OnEvent(const SDL_Event& event)
{
}
}
