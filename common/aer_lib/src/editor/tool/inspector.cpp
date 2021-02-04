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
     rendererViewer_(engine.GetComponentManagerContainer().rendererViewer),
     transform3dViewer_(engine.GetComponentManagerContainer().transform3dViewer)
{}

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
            if (entityManager_.HasComponent(selectedEntity, EntityMask(ComponentType::TRANSFORM3D)))
            {
                //Display Component
                ImGui::SetNextTreeNodeOpen(true);
                //Ask component how to display their information
                transform3dViewer_.DrawImGui(selectedEntity);
                rendererViewer_.DrawImGui(selectedEntity);
            }

            //TODO Replace by actual tag list
            char* tags[] = {"Untagged"};


            std::copy(
                engine_.GetComponentManagerContainer().sceneManager.GetCurrentScene().tags.begin(),
                engine_.GetComponentManagerContainer().sceneManager.GetCurrentScene().tags.end(),
                tags);
           

            
            

            if (ImGui::BeginCombo("Tag",TagLocator::get().GetEntityTag(selectedEntity).c_str()))
            {
                for (int n = 0; n < IM_ARRAYSIZE(tags); n++)
                {
                    bool is_selected = current_item == tags [n];    // You can store your selection however you want, outside or inside your objects
                    if (ImGui::Selectable(tags[n], is_selected))
                    {

                        current_item = tags[n];
                        TagLocator::get().SetEntityTag(selectedEntity, current_item);
                    }
                        

                    if (is_selected)
                         ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
                }
                ImGui::EndCombo();
            }

            

            layer = TagLocator::get().GetEntityLayer(selectedEntity);
            ImGui::Text((layer.c_str()));

            ImGui::InputText("New layer", newLayer, IM_ARRAYSIZE(newLayer));
            engine_.GetComponentManagerContainer().sceneManager.AddLayer(newLayer);

            ImGui::InputText("New tag", newTag, IM_ARRAYSIZE(newTag));
            engine_.GetComponentManagerContainer().sceneManager.AddTag(newTag);

            tag = TagLocator::get().GetEntityTag(selectedEntity);
            ImGui::Text((tag.c_str()));

            /// <summary>
            //
            /// </summary>

            if (ImGui::TreeNode("Components"))
            {
                //Add Buttons

                if (ImGui::Button("Add Body2D"))
                {
                    entityManager_.AddComponentType(
                        selectedEntity, EntityMask(ComponentType::BODY2D));
                }

                if (ImGui::Button("Add Box_Collider2D"))
                {
                    entityManager_.AddComponentType(
                        selectedEntity, EntityMask(ComponentType::BOX_COLLIDER2D));
                }

                if (ImGui::Button("Add Circle_Collider2D"))
                {
                    entityManager_.AddComponentType(
                        selectedEntity, EntityMask(ComponentType::CIRCLE_COLLIDER2D));
                }

                if (ImGui::Button("Add Convex_Shaped2D"))
                {
                    entityManager_.AddComponentType(
                        selectedEntity, EntityMask(ComponentType::CONVEX_SHAPE2D));
                }

                if (ImGui::Button("Add Transform3D"))
                {
                    entityManager_.AddComponentType(
                        selectedEntity, EntityMask(ComponentType::TRANSFORM3D));
                }

                if (ImGui::Button("Add Transform2D"))
                {
                    entityManager_.AddComponentType(
                        selectedEntity, EntityMask(ComponentType::TRANSFORM2D));
                }

                if (ImGui::Button("Add Model"))
                {
                    entityManager_.AddComponentType(
                        selectedEntity, EntityMask(ComponentType::MODEL));
                }

                if (ImGui::Button("Add Collider2D"))
                {
                    entityManager_.AddComponentType(
                        selectedEntity, EntityMask(ComponentType::POLYGON_COLLIDER2D));
                }

                if (ImGui::Button("Add Position2D"))
                {
                    entityManager_.AddComponentType(
                        selectedEntity, EntityMask(ComponentType::POSITION2D));
                }

                if (ImGui::Button("Add Position3D"))
                {
                    entityManager_.AddComponentType(
                        selectedEntity, EntityMask(ComponentType::POSITION3D));
                }

                if (ImGui::Button("Add Sprite2D"))
                {
                    entityManager_.AddComponentType(
                        selectedEntity, EntityMask(ComponentType::SPRITE2D));
                }

                if (ImGui::Button("Add Scale3D"))
                {
                    entityManager_.AddComponentType(
                        selectedEntity, EntityMask(ComponentType::SCALE3D));
                }

                if (ImGui::Button("Add Scale2D"))
                {
                    entityManager_.AddComponentType(
                        selectedEntity, EntityMask(ComponentType::SCALE2D));
                }

                if (ImGui::Button("Add Rotation3D"))
                {
                    entityManager_.AddComponentType(
                        selectedEntity, EntityMask(ComponentType::ROTATION3D));
                }

                if (ImGui::Button("Add Rotation2D"))
                {
                    entityManager_.AddComponentType(
                        selectedEntity, EntityMask(ComponentType::ROTATION2D));
                }

                // Delete Buttons

                if (entityManager_.HasComponent(selectedEntity, EntityMask(ComponentType::BODY2D)))
                {
                    if (ImGui::Button("Delete Body2D"))
                    {
                        entityManager_.RemoveComponentType(
                            selectedEntity, EntityMask(ComponentType::BODY2D));
                    }
                }

                if (entityManager_.HasComponent(
                        selectedEntity, EntityMask(ComponentType::BOX_COLLIDER2D)))
                {
                    if (ImGui::Button("Delete Box_Collider2D"))
                    {
                        entityManager_.RemoveComponentType(
                            selectedEntity, EntityMask(ComponentType::BOX_COLLIDER2D));
                    }
                }

                if (entityManager_.HasComponent(
                        selectedEntity, EntityMask(ComponentType::CIRCLE_COLLIDER2D)))
                {
                    if (ImGui::Button("Delete Circle_Collider2D"))
                    {
                        entityManager_.RemoveComponentType(
                            selectedEntity, EntityMask(ComponentType::CIRCLE_COLLIDER2D));
                    }
                }

                if (entityManager_.HasComponent(
                        selectedEntity, EntityMask(ComponentType::CONVEX_SHAPE2D)))
                {
                    if (ImGui::Button("Delete Convex_Shaped2D"))
                    {
                        entityManager_.RemoveComponentType(
                            selectedEntity, EntityMask(ComponentType::CONVEX_SHAPE2D));
                    }
                }

                if (entityManager_.HasComponent(
                        selectedEntity, EntityMask(ComponentType::TRANSFORM3D)))
                {
                    if (ImGui::Button("Delete Transform3D"))
                    {
                        entityManager_.RemoveComponentType(
                            selectedEntity, EntityMask(ComponentType::TRANSFORM3D));
                    }
                }

                if (entityManager_.HasComponent(
                        selectedEntity, EntityMask(ComponentType::TRANSFORM2D)))
                {
                    if (ImGui::Button("Delete Transform2D"))
                    {
                        entityManager_.RemoveComponentType(
                            selectedEntity, EntityMask(ComponentType::TRANSFORM2D));
                    }
                }

                if (entityManager_.HasComponent(selectedEntity, EntityMask(ComponentType::MODEL)))
                {
                    if (ImGui::Button("Delete Model"))
                    {
                        entityManager_.RemoveComponentType(
                            selectedEntity, EntityMask(ComponentType::MODEL));
                    }
                }

                if (entityManager_.HasComponent(
                        selectedEntity, EntityMask(ComponentType::POLYGON_COLLIDER2D)))
                {
                    if (ImGui::Button("Delete Collider2D"))
                    {
                        entityManager_.RemoveComponentType(
                            selectedEntity, EntityMask(ComponentType::POLYGON_COLLIDER2D));
                    }
                }

                if (entityManager_.HasComponent(
                        selectedEntity, EntityMask(ComponentType::POSITION2D)))
                {
                    if (ImGui::Button("Delete Position2D"))
                    {
                        entityManager_.RemoveComponentType(
                            selectedEntity, EntityMask(ComponentType::POSITION2D));
                    }
                }

                if (entityManager_.HasComponent(
                        selectedEntity, EntityMask(ComponentType::POSITION3D)))
                {
                    if (ImGui::Button("Delete Position3D"))
                    {
                        entityManager_.RemoveComponentType(
                            selectedEntity, EntityMask(ComponentType::POSITION3D));
                    }
                }

                if (entityManager_.HasComponent(
                        selectedEntity, EntityMask(ComponentType::SPRITE2D)))
                {
                    if (ImGui::Button("Delete Sprite2D"))
                    {
                        entityManager_.RemoveComponentType(
                            selectedEntity, EntityMask(ComponentType::SPRITE2D));
                    }
                }

                if (entityManager_.HasComponent(selectedEntity, EntityMask(ComponentType::SCALE3D)))
                {
                    if (ImGui::Button("Delete Scale3D"))
                    {
                        entityManager_.RemoveComponentType(
                            selectedEntity, EntityMask(ComponentType::SCALE3D));
                    }
                }

                if (entityManager_.HasComponent(selectedEntity, EntityMask(ComponentType::SCALE2D)))
                {
                    if (ImGui::Button("Delete Scale2D"))
                    {
                        entityManager_.RemoveComponentType(
                            selectedEntity, EntityMask(ComponentType::SCALE2D));
                    }
                }

                if (entityManager_.HasComponent(
                        selectedEntity, EntityMask(ComponentType::ROTATION3D)))
                {
                    if (ImGui::Button("Delete Rotation3D"))
                    {
                        entityManager_.RemoveComponentType(
                            selectedEntity, EntityMask(ComponentType::ROTATION3D));
                    }
                }

                if (entityManager_.HasComponent(
                        selectedEntity, EntityMask(ComponentType::ROTATION2D)))
                {
                    if (ImGui::Button("Delete Rotation2D"))
                    {
                        entityManager_.RemoveComponentType(
                            selectedEntity, EntityMask(ComponentType::ROTATION2D));
                    }
                }

                ImGui::TreePop();
            }

            ImGui::End();
        }
    }
}

void Inspector::OnEvent(const SDL_Event& event) {}

}    // namespace neko::aer