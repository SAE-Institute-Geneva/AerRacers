#include <editor/tool/material_description.h>
#include <string>
#include <filesystem>
#include "aer_engine.h"
#include "utilities/file_utility.h"
#include "utilities/json_utility.h"

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
	for (const auto& entry : std::filesystem::directory_iterator(filepath))
	{
		auto it = std::find(materialsPaths.begin(), materialsPaths.end(), entry);
		if(it == materialsPaths.end())
		{
			materialsPaths.push_back(entry.path().string());
		}
	}
	for (const auto& materialPath : materialsPaths)
	{
		auto& materialJson = LoadJson(materialPath);
		auto it = std::find(materialsJson.begin(), materialsJson.end(), materialJson);

		if(it == materialsJson.end())
		{
			materialsJson.push_back(materialJson);
		}		
	}
}

void MaterialDescription::Destroy()
{
}

void MaterialDescription::DrawImGui()
{	
	if (isVisible && ImGui::Begin((GetName() + "##" + std::to_string(GetId())).c_str(), &isVisible))
	{
		if (ImGui::BeginCombo("Material", selectedMaterialName.c_str()))
		{
			for(const auto& material: materialsJson)
			{
				const std::string_view materialName = material["name"].get<std::string_view>();
				bool isSelected = materialName == selectedMaterialName;

				if(ImGui::Selectable(materialName.data(), &isSelected))
				{
					selectedMaterialName = materialName;

					selectedMaterial = material;
				}
			}

			ImGui::EndCombo();
		}

		if(selectedMaterial != nullptr)
		{
			//name
			std::string materialName = selectedMaterial["name"];

			if(ImGui::InputText("Material Name", &materialName, ImGuiInputTextFlags_None))
			{
				selectedMaterial["name"] = materialName;
			}
			
			//shaderpath
			std::string shaderPath = selectedMaterial["shaderPath"];

			if(ImGui::InputText("Shader Path", &shaderPath, ImGuiInputTextFlags_None))
			{
				selectedMaterial["shaderPath"] = shaderPath;
			}
			
			//type
			int type = selectedMaterial["type"];
			if(ImGui::Combo("Type", &type, typeLabels, IM_ARRAYSIZE(typeLabels)))
			{
				selectedMaterial["type"] = type;
			}

			//color
			Color4 materialColors;
			materialColors.r = selectedMaterial["color"]["r"];
			materialColors.g = selectedMaterial["color"]["g"];
			materialColors.b = selectedMaterial["color"]["b"];
			materialColors.a = selectedMaterial["color"]["a"];

			if(ImGui::ColorEdit4("Material Color", & materialColors[0]))
			{
				selectedMaterial["color"]["r"] = materialColors.r;
				selectedMaterial["color"]["g"] = materialColors.g;
				selectedMaterial["color"]["b"] = materialColors.b;
				selectedMaterial["color"]["a"] = materialColors.a;
				
			}
		}
		
		ImGui::End();
	}
}

void MaterialDescription::OnEvent(const SDL_Event& event)
{
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
