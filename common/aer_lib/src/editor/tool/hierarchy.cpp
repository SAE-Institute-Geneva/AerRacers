#include <editor/tool/hierarchy.h>
#include "vector"
#include <string>
#include "aer_engine.h"
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
      entityManager_(engine.GetEntityManager()) { }

void Hierarchy::Init() { }
void Hierarchy::Update(seconds dt) {}
void Hierarchy::Destroy() {}

void Hierarchy::DrawImGui()
{
    bool showDemoWindow = true;
    ImGui::ShowDemoWindow(&showDemoWindow);

    int nodeClicked = -1;

    if (isVisible) {
        if (ImGui::Begin((GetName() + "##" +
                          std::to_string(GetId())).c_str(),
            &isVisible)) {
            for (auto entityIndex = 0; entityIndex < entityManager_.GetEntitiesSize();
                 entityIndex++) {
                if (entityManager_.GetEntityParent(entityIndex) == INVALID_ENTITY && entityManager_.
                    EntityExists(entityIndex)) { DisplayEntity(entityIndex); }
            }
        }
        ImGui::End();
    }
}

void Hierarchy::DisplayEntity(Entity entityIndex)
{
    ImGuiTreeNodeFlags nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow |
                                   ImGuiTreeNodeFlags_OpenOnDoubleClick |
                                   ImGuiTreeNodeFlags_SpanAvailWidth;

    std::string text = "Entity " + std::to_string(entityIndex);
    if (editorToolManager_.GetSelectedEntity() == entityIndex) {
        nodeFlags = nodeTreeSelectedFlags;
    } else { nodeFlags = nodeTreeNotSelectedFlags; }
    const bool nodeOpen = ImGui::TreeNodeEx(text.c_str(), nodeFlags);
    if (ImGui::IsItemClicked()) { editorToolManager_.SetSelectedEntity(entityIndex); }
    if (ImGui::BeginPopupContextItem()) {
        if (ImGui::MenuItem("Add Entity")) { entityManager_.CreateEntity(); }

        if (ImGui::MenuItem("Delete Entity")) { entityManager_.DestroyEntity(entityIndex); }
        ImGui::EndPopup();
    }
    if (nodeOpen) {
        for (Entity entityChild = 0; entityChild < entityManager_.GetEntitiesNmb();
             entityChild++) {
            if (entityManager_.GetEntityParent(entityChild) == entityIndex &&
                entityManager_.EntityExists(entityChild)) { DisplayEntity(entityChild); }
        }
        ImGui::TreePop();
    }
}

void Hierarchy::OnEvent(const SDL_Event& event) {}
}
