#include <editor/tool/hierarchy.h>
#include <string>

namespace neko::aer
{
	Hierarchy::Hierarchy(
		ToolType type,
		int id, 
		std::string name) : 
		EditorToolInterface(type, id, name)
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
				std::string nickiminaj = { "nicki minaj" };
				ImGui::Text(nickiminaj.c_str());
				ImGui::End();
			}
		}
	}

	void Hierarchy::OnEvent(const SDL_Event& event){}
}