#include <shellapi.h>
#include <utilities/file_utility.h>

#include "editor/tool/project.h"

#include <imgui_internal.h>
#include <stb_image.h>



#include "glad/glad.h"

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

	DataFile::DataFile(const std::string name, const std::filesystem::path path,
	                   const Type type) : DataResource(name, path), type_(type)
	{
	}

	void DataFile::OpenFile()
	{
		std::string action;
		switch (type_)
		{
		case Type::Text:
			action = "../../data/" + path_.string();
			ShellExecute(0, 0, action.c_str(), 0, 0, SW_SHOW);
			//system(action.c_str());
			break;
		case Type::Data:
			action = "notepad.exe ./../../data/" + path_.string();
			ShellExecute(0, 0, action.c_str(), 0, 0, SW_SHOW);
		default:
			break;
		}
	}


	void DataFile::Display()
	{
		ImGui::Indent();
		if (ImGui::Selectable(name_.c_str()));

		if(ImGui::IsItemHovered())
		{
			if (ImGui::IsMouseClicked(1))
			{
				logDebug(name_ + " 1");
				isSelect_ = true;
				
			}
			if(ImGui::IsMouseClicked(0))
			{	
				logDebug(name_ + " 0");
				isSelect_ = true;
			}
			
		}

	
		//isSelect_ = !isSelect_;
		//switch (type_)
		//{
		//case Type::Text:
		//	OpenFile();
		//	break;
		//}


		//if (isSelect_) {
		//	switch (type_)
		//	{
		//	case Type::Texture:

		//		break;
		//	}
		//}
		ImGui::Unindent();
	}


	DataFolder::DataFolder(const std::string name, const std::filesystem::path path) : DataResource(name, path)
	{
		for (const auto& file : std::filesystem::directory_iterator(path))
		{
			fileNumber_++;
			if (!file.path().has_extension())
			{
				// Todo Create DataFolder
				resources_.push_back(new DataFolder( GetFilename(file.path().string()), file.path().string()));
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
				if (GetFilenameExtension(file.path().string()) == ".ktx")
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

	void DataFolder::Display()
	{
		if (ImGui::TreeNode((name_).c_str()))
		{
			if (ImGui::IsItemHovered())
			{
				if (ImGui::IsMouseClicked(1))
				{
					logDebug(name_ + " Right");
				}
				if (ImGui::IsMouseClicked(0))
				{
					logDebug(name_ + " Left");
				}
			}
			
			for (auto element : this->resources_)
			{
				element->Display();
			}
			
			ImGui::Text("");
			ImGui::TreePop();		
		}else
		{
			if (ImGui::IsItemHovered())
			{
				if (ImGui::IsMouseClicked(1))
				{
					logDebug(name_ + " Right");
				}
				if (ImGui::IsMouseClicked(0))
				{
					logDebug(name_ + " Left");
				}

			}
		}
		

		if(ImGui::IsItemToggledOpen())
		{
		}
		if(ImGui::IsItemToggledSelection())
		{

		}

		if(ImGui::IsItemActivated())
		{
		}
		
		if (ImGui::IsItemClicked(0))
		{
		}

		if (ImGui::IsItemClicked(1))
		{
		}

		//if(ImGui::IsItemHovered() )
		//{
		//	isHover_ = true;
		//	logDebug("Hover");
		//}else
		//{
		//	isHover_ = false;
		//}

	}

	std::vector<DataResource*> DataFolder::GetDataResources() const
	{
		return resources_;
	}

	int DataFolder::GetFileNumber() const
	{
		return fileNumber_;
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



	void Project::Init()
	{
		LoadData(path_);
	}

	void Project::Update(seconds dt)
	{
		if (!contextMenuIsOpen_) {
			DataResource* resourceHovered = nullptr;

				if(resourceHovered != nullptr )
				{
					resourceHovered->isHover_ = false;
				}
				resourceHovered = resources_->GetResourceIsSelected();
				if (resourceHovered != nullptr) {
					resourceHovered->isHover_ = true;
					logDebug(resourceHovered->GetName());
				}


			if (mouseButtonRight_ && resourceHovered != nullptr)
			{
				
				resourceHovered_ = resourceHovered;
				contextMenuIsOpen_ = true;

			}

			if (mouseButtonLeft_ && resourceHovered != nullptr)
			{

				resourceHovered_ = resourceHovered;
				contextMenuIsOpen_ = false;

			}

				
		}else
		{
			if (ImGui::IsMouseClicked(0))
			{
				contextMenuIsOpen_ = false;
			}
		}
		
		
		
		
	}

	void Project::Destroy()
	{
	}

	void Project::DrawImGui()
	{
		if(ImGui::IsMouseClicked(1))
		{
			mouseButtonRight_ = true;
			mouseButtonLeft_ = false;
		}
		if (ImGui::IsMouseClicked(0))
		{
			mouseButtonRight_ = false;
			mouseButtonLeft_ = true;
		}
		
		if (isVisible)
		{
			if (!ImGui::Begin((GetName() + "##" + std::to_string(GetId())).c_str(),
			                  &isVisible))
			{
				ImGui::End();
			}
			else
			{
#pragma region Header
				// nbrResource contains the quantity of Resource
				const int nbrResourceDisplayMax =
					ImGui::GetWindowHeight() / ImGui::GetTextLineHeightWithSpacing();

				//Todo Search
				ImGui::Separator();
#pragma endregion

#pragma region Body
				ImGui::BeginChild("Resources");

				posY_ = ImGui::GetScrollY() /
					ImGui::GetTextLineHeightWithSpacing();
				//ImGui::SetCursorPos({0, posY_ * ImGui::GetTextLineHeightWithSpacing()});


				ImGui::Text(path_.c_str());
				ImGui::Separator();
				ImGui::Columns(2);
				auto folder = static_cast<DataFolder*>(resources_);

				

				//Displays Resource
				folder->Display();
				ImGui::NextColumn();

				/*ImGui::SetCursorPos(
					{0, folder_.size() * ImGui::GetTextLineHeightWithSpacing()});
				ImGui::Text("");*/
				
				if (resourceHovered_ != nullptr)
				{
					ImGui::Text(resourceHovered_->GetName().c_str());
					auto file = static_cast<DataFile*>(resourceHovered_);
					if (file->GetType() == DataFile::Type::Texture)
					{

						TextureName texture = neko::gl::stbCreateTexture(file->GetPath().string());

						ImGui::Image((void*)(intptr_t)texture, ImVec2(206, 206));
					}

				}
				
				ImGui::EndChild();

				

				if (contextMenuIsOpen_) {
					ImGui::OpenPopup("context");
					//ImGui::SetNextWindowPos(ImGui::GetIO().MousePos);
					if (ImGui::BeginPopupContextWindow("context"))
					{
						ImGui::Text(("   " + resourceHovered_->GetName() + "   ").c_str());
						if (ImGui::Selectable("Create Folder"))
						{
							// Todo create folder
						}
						if (ImGui::Selectable("Open Folder"))
						{
							// Todo Open folder with explorer.exe
						}
						if (ImGui::Selectable("Rename"))
						{
							//Todo Rename Folder
						}
						ImGui::EndPopup();
					}
				}
				
				ImGui::End();

		
#pragma endregion
			}
		}
	}


	void Project::OnEvent(const SDL_Event& event)
	{
	
	}
}
