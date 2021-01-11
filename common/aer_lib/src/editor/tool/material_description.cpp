#include <editor/tool/material_description.h>
#include <string>
#include <filesystem>
#include "aer_engine.h"
#include "utilities/json_utility.h"
#include "utilities/file_utility.h"

namespace neko::aer
{
MaterialDescription::MaterialDescription(AerEngine& engine, ToolType type, int id, std::string name) : EditorToolInterface(engine, type, id, name)
{
}

void MaterialDescription::Init()
{
}

void MaterialDescription::Update(seconds dt)
{
}

void MaterialDescription::Destroy()
{
}

void MaterialDescription::DrawImGui()
{	
	if (isVisible && ImGui::Begin((GetName() + "##" + std::to_string(GetId())).c_str(), &isVisible))
	{
		LoadMaterialFiles();
		
		if (ImGui::BeginCombo("Material", selectedMaterialName_.c_str()))
		{
			for(int i = 0; i < materialsJson_.size(); i++)
			{
				const std::string_view materialName = materialsJson_[i]["name"].get<std::string_view>();
				bool isSelected = materialName == selectedMaterialName_;

				if(ImGui::Selectable(materialName.data(), &isSelected))
				{
					selectedMaterialName_ = materialName;

					selectedMaterial_ = materialsJson_[i];

					selectedMaterialIndex_ = i;
				}
			}

			ImGui::EndCombo();
		}

		if(selectedMaterial_ != nullptr)
		{
			//name
			std::string materialName = selectedMaterial_["name"];

			if(ImGui::InputText("Material Name", &materialName, ImGuiInputTextFlags_None))
			{
				selectedMaterial_["name"] = materialName;
			}
			
			//shaderpath
			std::string shaderPath = selectedMaterial_["shaderPath"];

			if(ImGui::InputText("Shader Path", &shaderPath, ImGuiInputTextFlags_None))
			{
				if(std::filesystem::exists(shaderPath))
				{
					selectedMaterial_["shaderPath"] = shaderPath;
				}
				else
				{
					ImGui::Text("Wrong path.");
				}	
			}
			
			//type
			int type = selectedMaterial_["type"];
			if(ImGui::Combo("Type", &type, typeLabels_, IM_ARRAYSIZE(typeLabels_)))
			{
				selectedMaterial_["type"] = type;
			}

			//color
			Color4 materialColors;
			materialColors.r = selectedMaterial_["color"]["r"];
			materialColors.g = selectedMaterial_["color"]["g"];
			materialColors.b = selectedMaterial_["color"]["b"];
			materialColors.a = selectedMaterial_["color"]["a"];

			if(ImGui::ColorEdit4("Material Color", & materialColors[0]))
			{
				selectedMaterial_["color"]["r"] = materialColors.r;
				selectedMaterial_["color"]["g"] = materialColors.g;
				selectedMaterial_["color"]["b"] = materialColors.b;
				selectedMaterial_["color"]["a"] = materialColors.a;
				
			}

			if(ImGui::Button("Save"))
			{
				std::ofstream material(materialsPaths_[selectedMaterialIndex_]);

				material << std::setw(4) << selectedMaterial_;

				//std::cout << std::setw(4) << selectedMaterial_ << std::endl;
				
				LoadMaterialFiles();
			}
		}
		
		ImGui::End();
	}
}

void MaterialDescription::OnEvent(const SDL_Event& event)
{
}

void MaterialDescription::LoadMaterialFiles()
{
	if(!materialsPaths_.empty() && !materialsJson_.empty())
	{
		materialsPaths_.clear();

		materialsJson_.clear();
	}
	
	for (const auto& entry : std::filesystem::directory_iterator(filepath_))
	{
		auto it = std::find(materialsPaths_.begin(), materialsPaths_.end(), entry);
		if (it == materialsPaths_.end())
		{
			materialsPaths_.push_back(entry.path().string());
		}
	}
	for (const auto& materialPath : materialsPaths_)
	{
		auto& materialJson = LoadJson(materialPath);
		auto it = std::find(materialsJson_.begin(), materialsJson_.end(), materialJson);

		if (it == materialsJson_.end())
		{
			materialsJson_.push_back(materialJson);
		}
	}
}
}

int ImGui::InputTextCallback(ImGuiInputTextCallbackData* data)
{
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		auto* str = (std::string*)data->UserData;
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = (char*)str->c_str();
	}

	return 0;
}

bool ImGui::InputText(const char* label, std::string* str, ImGuiInputTextFlags flags)
{
	flags |= ImGuiInputTextFlags_CallbackResize;
	return InputText(label, (char*)str->c_str(), str->capacity() + 1, flags, InputTextCallback, (void*)str);
}
