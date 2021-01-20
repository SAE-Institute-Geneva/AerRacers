#include <editor/tool/hierarchy.h>
#include "vector"
#include <string>
#include "aer_engine.h"
#include "imgui_internal.h"
#include "imgui.h"
#include "engine/entity.h"

namespace neko::aer
{
	Hierarchy::Hierarchy(
		AerEngine& engine,
		ToolType type,
		int id, 
		std::string name) : 
		EditorToolInterface(engine, type, id, name)
	{

	}
	void Hierarchy::Init(){}
	void Hierarchy::Update(seconds dt){}
	void Hierarchy::Destroy(){}

	void Hierarchy::DrawImGui()
	{
		auto& entityManager = engine_.GetEntityManager();
		bool showDemoWindow = true;
		ImGui::ShowDemoWindow(&showDemoWindow);

		int nodeClicked = -1;

		if (isVisible)
		{
			if (ImGui::Begin((GetName() + "##" +
				std::to_string(GetId())).c_str(),
				&isVisible))
			{
				for (auto entityIndex = 0; entityIndex < entityManager.GetEntitiesNmb(); entityIndex++)
				{
					ImGuiTreeNodeFlags currentFlag;
					if (selectedEntity == entityIndex) {
						currentFlag = nodeTreeSelectedFlags;
					}
					else {
						currentFlag = nodeTreeNotSelectedFlags;
					}

					if (entityManager.GetEntityParent(entityIndex) == INVALID_ENTITY)
					{
						std::string text = "Entity " + std::to_string(entityIndex);

						if (ImGui::TreeNodeEx(text.c_str(), currentFlag))
						{
							if (ImGui::IsItemClicked())
							{
								selectedEntity = entityIndex;
							}

							DisplayChildren(entityIndex);

							ImGui::TreePop();
						}
						
					}

					
				}
			}
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Add Entity"))
				{
					entityManager.CreateEntity();
				}
				
				if (ImGui::MenuItem("Delete Entity"))
				{
						entityManager.DestroyEntity(selectedEntity);
				}
				ImGui::EndPopup();
			}
			ImGui::End();
			
		}
	}

	void Hierarchy::DisplayChildren(Entity entityParent)
	{
		auto& entityManager = engine_.GetEntityManager();
		for (auto entityChildren = 0; entityChildren < entityManager.GetEntitiesNmb(); entityChildren++)
		{
			ImGuiTreeNodeFlags currentFlag;
			if (selectedEntity == entityChildren) {
				currentFlag = nodeTreeSelectedFlags;
			}
			else {
				currentFlag = nodeTreeNotSelectedFlags;
			}

			if (entityManager.GetEntityParent(entityChildren) == entityParent)
			{
				std::string textChild = "Child" + std::to_string(entityChildren);
				if (ImGui::TreeNodeEx(textChild.c_str(), currentFlag))
				{
					if (ImGui::IsItemClicked())
					{
						selectedEntity = entityChildren;
					}
					DisplayChildren(entityChildren);
					ImGui::TreePop();
				}

			}
		}
		
	}

	void Hierarchy::OnEvent(const SDL_Event& event){}
}