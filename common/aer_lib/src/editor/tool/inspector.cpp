#include "aer/editor/tool/inspector.h"
#include "aer/aer_engine.h"

namespace neko::aer {
Inspector::Inspector(AerEngine& engine, ToolType type, int id, std::string name)
   : EditorToolInterface(engine, type, id, name),
     editorToolManager_(engine.GetEditorToolManager()),
     entityManager_(engine.GetComponentManagerContainer().entityManager),
     transform3dManager_(engine.GetComponentManagerContainer().transform3dManager),
     renderManager_(engine.GetComponentManagerContainer().renderManager),
     rendererViewer_(engine.GetComponentManagerContainer().rendererViewer),
     transform3dViewer_(engine.GetComponentManagerContainer().transform3dViewer)
{ }

void Inspector::Init() {}
void Inspector::Update(seconds dt) {}
void Inspector::Destroy() {}

void Inspector::DrawImGui()
{
    //If is True Display Window
    if (isVisible) {
        //Display window
        if (!ImGui::Begin((GetName() + "##" + std::to_string(GetId())).c_str(), &isVisible))
        { ImGui::End(); }
        else
        {
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
                //Ask component how to display their information
                transform3dViewer_.DrawImGui(selectedEntity);
                rendererViewer_.DrawImGui(selectedEntity);
            }

            if (ImGui::Button("Add Component")) {

            }
            ImGui::End();
        }
    }
}

void Inspector::OnEvent(const SDL_Event& event) {}

}
