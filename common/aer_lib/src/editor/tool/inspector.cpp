#include "editor/tool/inspector.h"
#include "aer_engine.h"

namespace neko::aer {
Inspector::Inspector(AerEngine& engine, ToolType type, int id, std::string name)
    : EditorToolInterface(engine, type, id, name),
      editorToolManager_(engine.GetEditorToolManager()),
      entityManager_(engine.GetEntityManager()),
      transform3dManager_(engine.GetTransform3dManager()) { }

void Inspector::Init() {}
void Inspector::Update(seconds dt) {}
void Inspector::Destroy() {}

void Inspector::DrawImGui()
{
    //If is True Display Window
    if (isVisible) {
        if (!ImGui::Begin((GetName() + "##" + std::to_string(GetId())).c_str(),
            &isVisible)) { } else {
            Entity selectedEntity = editorToolManager_.GetSelectedEntity();
            if (selectedEntity == INVALID_ENTITY)
            {
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No Entity selected");
                ImGui::End();
                return;
            }
            std::string name = "Entity " + std::to_string(selectedEntity);
            ImGui::Text(name.c_str());
            ImGui::SetNextTreeNodeOpen(true);
            if (entityManager_.HasComponent(selectedEntity, EntityMask(ComponentType::TRANSFORM3D)))
            {
                if (ImGui::TreeNode("Transform"))
                {
                    transform3dManager_.DrawImGui(selectedEntity);
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
