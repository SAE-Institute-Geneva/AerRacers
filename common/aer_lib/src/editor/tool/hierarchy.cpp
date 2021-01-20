#include <aer/editor/tool/hierarchy.h>
#include "vector"
#include <string>
#include "aer/aer_engine.h"
#include "imgui_internal.h"
#include "imgui.h"
#include "engine/entity.h"

namespace neko::aer {
Hierarchy::Hierarchy(
    AerEngine& engine,
    ToolType type,
    int id,
    std::string name)
    : EditorToolInterface(engine, type, id, name),
      editorToolManager_(engine.GetEditorToolManager()),
      entityManager_(engine.GetComponentManagerContainer().entityManager) { }

void Hierarchy::Init() { }
void Hierarchy::Update(seconds dt) {}
void Hierarchy::Destroy() {}

void Hierarchy::DrawImGui()
{
    if (isVisible) {
        //Open window
        if (ImGui::Begin((GetName() + "##" +
                          std::to_string(GetId())).c_str(),
            &isVisible)) {
            for (auto entityIndex = 0; entityIndex < entityManager_.GetEntitiesSize();
                 entityIndex++) {
                //Display each entity without parent
                if (entityManager_.GetEntityParent(entityIndex) == INVALID_ENTITY && entityManager_.
                    EntityExists(entityIndex)) { DisplayEntity(entityIndex); }
            }
        }
        ImGui::End();
    }
}

void Hierarchy::DisplayEntity(Entity entityIndex)
{
    ImGuiTreeNodeFlags nodeFlags;

    std::string text = "Entity " + std::to_string(entityIndex);
    if (editorToolManager_.GetSelectedEntity() == entityIndex) {
        nodeFlags = kNodeTreeSelectedFlags_;
    } else { nodeFlags = kNodeTreeNotSelectedFlags_; }
    //Display entity
    const bool nodeOpen = ImGui::TreeNodeEx(text.c_str(), nodeFlags);
    //Select entity on click
    if (ImGui::IsItemClicked()) { editorToolManager_.SetSelectedEntity(entityIndex); }
    //Display context menu on right click on entity
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Add Entity")) { entityManager_.CreateEntity(); }

        if (ImGui::MenuItem("Delete Entity")) {
            entityManager_.DestroyEntity(entityIndex);
        }
        ImGui::EndPopup();
    }
    //If node open display their children
    if (nodeOpen) {
        for (Entity entityChild = 0; entityChild < entityManager_.GetEntitiesSize();
             entityChild++) {
            //Display entity if it's a child of this entity
            if (entityManager_.GetEntityParent(entityChild) == entityIndex &&
                entityManager_.EntityExists(entityChild)) { DisplayEntity(entityChild); }
        }
        ImGui::TreePop();
    }
}

void Hierarchy::OnEvent(const SDL_Event& event) {}
}
