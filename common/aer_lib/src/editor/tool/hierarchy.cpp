#include <editor/tool/hierarchy.h>
#include <string>
#include "aer_engine.h"

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
			if (!ImGui::Begin((GetName() + "##" +
				std::to_string(GetId())).c_str(),
				&isVisible))
			{
				ImGui::End();
			}
			else
			{
				/*std::string nickiminaj = { "nicki minaj" };
				ImGui::Text(nickiminaj.c_str());*/
				std::string text = "Entity : ";
				text += std::to_string(engine_.GetEntityManager().GetEntitiesNmb(EntityMask(ComponentType::EMPTY)));
				ImGui::Text(text.c_str());
				ImGui::End();
			}
		}
	}

	void Hierarchy::OnEvent(const SDL_Event& event){}
}