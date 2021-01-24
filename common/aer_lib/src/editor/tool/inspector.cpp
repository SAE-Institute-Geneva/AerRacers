#include "aer/editor/tool/inspector.h"
#include "aer/aer_engine.h"

namespace neko::aer {
Inspector::Inspector(AerEngine& engine, ToolType type, int id, std::string name)
    : EditorToolInterface(engine, type, id, name),
      editorToolManager_(engine.GetEditorToolManager()),
      entityManager_(engine.GetComponentManagerContainer().entityManager),
      transform3dManager_(engine.GetComponentManagerContainer().transform3dManager),
     renderManager_(engine.GetComponentManagerContainer().renderManager)
{ }

void Inspector::Init() {}
void Inspector::Update(seconds dt) {}
void Inspector::Destroy() {}

void Inspector::DrawImGui()
{
    //If is True Display Window
    if (isVisible) {
        //Display window
        if (!ImGui::Begin((GetName() + "##" + std::to_string(GetId())).c_str(),
            &isVisible)) { } else {
            //Get selected entity
            Entity selectedEntity = editorToolManager_.GetSelectedEntity();
            if (selectedEntity == INVALID_ENTITY) {
                //Display message if no entity selected
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No Entity selected");
                ImGui::End();
                return;
            }
            //Dispay entity name
            std::string name = "Entity " + std::to_string(selectedEntity);
            ImGui::Text(name.c_str());
            if (entityManager_.HasComponent(selectedEntity, EntityMask(ComponentType::TRANSFORM3D)))
            {
                //Display Component
                ImGui::SetNextTreeNodeOpen(true);
                if (ImGui::TreeNode("Transform")) {
                    //Ask component how to display their information
                    transform3dManager_.DrawImGui(selectedEntity);
                    ImGui::TreePop();
                }
                if (ImGui::TreeNode("Renderer"))
                {
                    //Ask component how to display their information
                    renderManager_.DrawImGui(selectedEntity);
                    ImGui::TreePop();
                }
            }

            if (ImGui::Button("Add Component")) {

            }
            ImGui::End();
        }
    }
}

void Inspector::OnEvent(const SDL_Event& event) {}

}
