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
		if (isVisible)
		{
			if (ImGui::Begin((GetName() + "##" +
				std::to_string(GetId())).c_str(),
				&isVisible))
			{
				auto& entityManager = engine_.GetEntityManager();
				for (auto entityIndex = 0; entityIndex < entityManager.GetEntitiesNmb(); entityIndex++)
				{
					if (entityManager.GetEntityParent(entityIndex) == INVALID_ENTITY)
					{
						std::string text = "Entity " + std::to_string(entityIndex);
						if (ImGui::TreeNode(text.c_str()))
						{
							for (auto entityChildren = 0; entityChildren < entityManager.GetEntitiesNmb(); entityChildren++)
							{
								if (entityManager.GetEntityParent(entityChildren) == entityIndex)
								{
									std::string textChild = "Child" + std::to_string(entityChildren);
									if (ImGui::TreeNode(textChild.c_str()))
									{
										DisplayChildren(entityChildren);
										ImGui::TreePop();
									}

								}
							}
							ImGui::TreePop();
						}
					}
					

				}

			}
			ImGui::End();
			/*if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("test"))
				{

				}
				ImGui::EndPopup();
			}*/
		}
	}

	void Hierarchy::DisplayChildren(Entity entityIndex)
	{
		auto& entityManager = engine_.GetEntityManager();
		for (auto entityChildren = 0; entityChildren < entityManager.GetEntitiesNmb(); entityChildren++)
		{
			if (entityManager.GetEntityParent(entityChildren) == entityIndex)
			{
				std::string textChild = "Child" + std::to_string(entityChildren);
				if (ImGui::TreeNode(textChild.c_str()))
				{
					DisplayChildren(entityChildren);
					ImGui::TreePop();
				}

			}
		}
	}

	void Hierarchy::OnEvent(const SDL_Event& event){}
}