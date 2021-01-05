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
}

void MaterialDescription::Destroy()
{
}

void MaterialDescription::DrawImGui()
{
	const std::string filepath = "../../data/aer_racers/materials/";
	std::string materialName = "name";
	std::vector<std::string> materialsPaths;
	std::vector<json> materialsJson;
	
	for(const auto& entry : std::filesystem::directory_iterator(filepath))
	{
		materialsPaths.push_back(entry.path().string());
	}
	for(const auto& materialPath : materialsPaths)
	{
		auto& materialJson = LoadJson(materialPath);
	
		materialsJson.push_back(materialJson);
	}
	
	if (isVisible && ImGui::Begin((GetName() + "##" + std::to_string(GetId())).c_str(), &isVisible))
	{
		const char* currentItem = NULL;
		if(ImGui::BeginCombo("##Materials", currentItem))
		{
			for(int i = 0; i < materialsJson.size(); i++)
			{
				bool isSelected = (currentItem == materialsJson[i][materialName].get<std::string>().c_str());

				if (ImGui::Selectable(materialsJson[i][materialName].get<std::string>().c_str(), isSelected))
				{
					currentItem = materialsJson[i][materialName].get<std::string>().c_str();
				}
				if(isSelected)
				{
					ImGui::SetItemDefaultFocus();
				}
			}

			ImGui::EndCombo();
		}
		ImGui::End();
	}
}

void MaterialDescription::OnEvent(const SDL_Event& event)
{
}
}
