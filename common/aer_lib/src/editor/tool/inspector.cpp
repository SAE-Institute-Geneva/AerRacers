#include "editor/tool/inspector.h"

namespace neko::aer
{
	Inspector::Inspector(AerEngine& engine, ToolType type, int id, std::string name)
		: EditorToolInterface(engine, type, id, name)
	{
	}

	void Inspector::Init() {}
	void Inspector::Update(seconds dt) {}
	void Inspector::Destroy() {}

	void Inspector::DrawImGui()
	{
		//If is True Display Window
		if (isVisible) {
			if (!ImGui::Begin((GetName() + "##" + std::to_string(GetId())).c_str(),
				&isVisible)) {






			}
			else {


				//static char buf[128] = "Cube";
				//ImGui::InputText("", buf, IM_ARRAYSIZE(buf));
				ImGui::SetNextTreeNodeOpen(true);
				if (ImGui::TreeNode("Components"))
				{

					if (ImGui::TreeNode("Transform"))
					{
						// static float vPosition[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
					   //  ImGui::InputFloat3("Position", vPosition);

						// static float vScale[4] = { 0.10f, 0.20f, 0.30f, 0.44f };
						// ImGui::InputFloat3("Scale", vScale);

						if (ImGui::Button("Remove Component"))
						{

						}

						ImGui::TreePop();
					}
					if (ImGui::TreeNode("RigidBody"))
					{
						ImGui::TreePop();
					}


					if (ImGui::Button("Add Component"))
					{

					}
					ImGui::TreePop();
				}

				ImGui::End();
			}
		}
	}

	void Inspector::OnEvent(const SDL_Event& event) {}

}