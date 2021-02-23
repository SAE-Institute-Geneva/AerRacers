#include "aer/editor/tool/inspector.h"
#include "aer/aer_engine.h"

namespace neko::aer
{
Inspector::Inspector(AerEngine& engine, ToolType type, int id, std::string name)
    : EditorToolInterface(engine, type, id, name),
      editorToolManager_(engine.GetEditorToolManager()),
      entityManager_(engine.GetComponentManagerContainer().entityManager),
      transform3dManager_(engine.GetComponentManagerContainer().transform3dManager),
      renderManager_(engine.GetComponentManagerContainer().renderManager),
      rigidDynamicManager_(engine.GetComponentManagerContainer().rigidDynamicManager),
     rigidStaticManager_(engine.GetComponentManagerContainer().rigidStaticManager),
    shipControllerManager_(engine.GetComponentManagerContainer().shipControllerManager),
      rendererViewer_(engine.GetComponentManagerContainer().rendererViewer),
     rigidDynamicViewer_(engine.GetComponentManagerContainer().rigidDynamicViewer),
     rigidStaticViewer_(engine.GetComponentManagerContainer().rigidStaticViewer),
      transform3dViewer_(engine.GetComponentManagerContainer().transform3dViewer),
    shipControllerViewer_(engine.GetComponentManagerContainer().shipControllerViewer){ }

void Inspector::Init() {}
void Inspector::Update(seconds dt) {}
void Inspector::Destroy() {}

void Inspector::DrawImGui()
{
    //If is True Display Window
    if (isVisible)
    {
        //Display window
        if (!ImGui::Begin((GetName() + "##" + std::to_string(GetId())).c_str(), &isVisible))
        { ImGui::End(); }
        else
        {
            //Get selected entity
            Entity selectedEntity = editorToolManager_.GetSelectedEntity();
            if (selectedEntity == INVALID_ENTITY)
            {
                //Display message if no entity selected
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No Entity selected");
                ImGui::End();
                return;
            }
            //Dispay entity name
            std::string name = "Entity " + std::to_string(selectedEntity);
            ImGui::Text(name.c_str());

            DisplayLayersAndTags(selectedEntity);

            //Display Component
            //Ask component how to display their information
            transform3dViewer_.DrawImGui(selectedEntity);
            rendererViewer_.DrawImGui(selectedEntity);
            rigidStaticViewer_.DrawImGui(selectedEntity);
            rigidDynamicViewer_.DrawImGui(selectedEntity);
            shipControllerViewer_.DrawImGui(selectedEntity);
            engine_.GetComponentManagerContainer().cameraControllerViewer.DrawImGui(selectedEntity);
            DisplayNewComponentButtons(selectedEntity);

            ImGui::End();
        }
    }
}

void Inspector::OnEvent(const SDL_Event& event) {}
void Inspector::DisplayLayersAndTags(Entity selectedEntity)
{
    const char* tags[]   = {"Untagged", "", "", "", "", "", "", "", "", ""};
    const char* layers[] = {"Default", "", "", "", "", "", "", "", "", ""};

    std::vector<std::string> strings =
        engine_.GetComponentManagerContainer().sceneManager.GetTags();
    std::vector<char*> cstrings;
    cstrings.reserve(strings.size());

    for (size_t i = 0; i < strings.size(); ++i)
    {
        cstrings.push_back(const_cast<char*>(strings[i].c_str()));
        tags[i] = cstrings[i];
    }

    std::vector<std::string> stringsBis =
        engine_.GetComponentManagerContainer().sceneManager.GetLayers();
    std::vector<char*> cstringsBis;

    for (size_t i = 0; i < stringsBis.size(); ++i)
    {
        cstringsBis.push_back(const_cast<char*>(stringsBis[i].c_str()));
        layers[i] = cstringsBis[i];
    }

    //Tags
    tag_ = TagLocator::get().GetEntityTag(selectedEntity);
    if (ImGui::BeginCombo("Tag", tag_.c_str()))
    {
        for (int n = 0; n < IM_ARRAYSIZE(tags); n++)
        {
            const bool isSelected = currentItem_ == tags[n];
            if (ImGui::Selectable(tags[n], isSelected))
            {
                currentItem_ = tags[n];
                TagLocator::get().SetEntityTag(selectedEntity, currentItem_);
            }

            if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    layer_ = TagLocator::get().GetEntityLayer(selectedEntity);
    if (ImGui::BeginCombo("Layer", layer_.c_str()))
    {
        for (int n = 0; n < IM_ARRAYSIZE(layers); n++)
        {
            const bool isSelected = currentItem_ == layers[n];
            if (ImGui::Selectable(layers[n], isSelected))
            {
                currentItem_ = layers[n];
                TagLocator::get().SetEntityLayer(selectedEntity, currentItem_);
            }

            if (isSelected) ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    if (ImGui::TreeNode("New Tag & Layer"))
    {
        ImGui::InputText("New tag", newTag_, IM_ARRAYSIZE(newTag_));
        ImGui::SameLine();
        if (ImGui::Button("Add Tag"))
        {
            if (newTag_ != "") engine_.GetComponentManagerContainer().sceneManager.AddTag(newTag_);
        }

        //Layers

        ImGui::InputText("New layer", newLayer_, IM_ARRAYSIZE(newLayer_));
        ImGui::SameLine();
        if (ImGui::Button("Add Layer"))
        {
            if (newLayer_ != "")
                engine_.GetComponentManagerContainer().sceneManager.AddLayer(newLayer_);
        }
        ImGui::TreePop();
    }
}

void Inspector::DisplayNewComponentButtons(Entity selectedEntity)
{
    if (ImGui::TreeNode("Components"))
    {
        //Add Buttons

        if (entityManager_.HasComponent(selectedEntity, EntityMask(ComponentType::TRANSFORM3D)))
        {
            if (ImGui::Button("Delete Transform3D"))
            {
                transform3dManager_.DestroyComponent(selectedEntity);
            }
        } else {
            if (ImGui::Button("Add Transform3D"))
            {
                transform3dManager_.AddComponent(selectedEntity);
            }
        }
        if (entityManager_.HasComponent(selectedEntity, EntityMask(ComponentType::MODEL)))
        {
            if (ImGui::Button("Delete Model")) {
                renderManager_.DestroyComponent(selectedEntity);
            }
        }
        else
        {
            if (ImGui::Button("Add Model")) {
                renderManager_.AddComponent(selectedEntity);
            }
        }

        if (entityManager_.HasComponent(selectedEntity, EntityMask(ComponentType::RIGID_DYNAMIC)))
        {
            if (ImGui::Button("Delete RigidDynamic")) {
                rigidDynamicManager_.DestroyComponent(selectedEntity);
            }
        }
        else
        {
            if (ImGui::Button("Add RigidDynamic")) {
                rigidDynamicManager_.AddRigidDynamic(selectedEntity, physics::RigidDynamicData());
            }
        }

        if (entityManager_.HasComponent(selectedEntity, EntityMask(ComponentType::RIGID_STATIC)))
        {
            if (ImGui::Button("Delete RigidStatic")) {
                rigidStaticManager_.DestroyComponent(selectedEntity);
            }
        }
        else
        {
            if (ImGui::Button("Add RigidStatic")) {
                rigidStaticManager_.AddRigidStatic(selectedEntity, physics::RigidStaticData());
            }
        }

        if (entityManager_.HasComponent(selectedEntity, EntityMask(ComponentType::SHIP_CONTROLLER)))
        {
            if (ImGui::Button("Delete ShipController")) {
                shipControllerManager_.DestroyComponent(selectedEntity);
            }
        }
        else
        {
            if (ImGui::Button("Add ShipController")) {
                shipControllerManager_.AddComponent(selectedEntity);
            }
        }

        ImGui::TreePop();
    }
}

}    // namespace neko::aer