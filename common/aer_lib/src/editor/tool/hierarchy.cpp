#include "aer/editor/tool/hierarchy.h"

#include "aer/aer_engine.h"

namespace neko::aer
{
Hierarchy::Hierarchy(AerEngine& engine, ToolType type, int id, std::string_view name)
   : EditorToolInterface(engine, type, id, name),
	 editorToolManager_(engine.GetEditorToolManager()),
	 entityManager_(engine.GetComponentManagerContainer().entityManager)
{}

void Hierarchy::Init() {}

void Hierarchy::Update(seconds) {}

void Hierarchy::Destroy() {}

void Hierarchy::DrawImGui()
{
	if (isVisible)
	{
		//Open window
		const std::string name = std::string(GetName()) + "##" + std::to_string(GetId());
		if (ImGui::Begin(name.c_str(), &isVisible))
		{
			if (ImGui::BeginPopupContextWindow())
			{
				if (ImGui::MenuItem("Add Entity"))
				{
					const Entity newEntity = entityManager_.CreateEntity();
					entityManager_.AddComponentType(
						newEntity, EntityMask(ComponentType::TRANSFORM3D));
				}

				ImGui::EndPopup();
			}

			for (Entity entityIndex = 0; entityIndex < entityManager_.GetEntitiesSize();
				 entityIndex++)
			{
				//Display each entity without parent
				if (entityManager_.GetEntityParent(entityIndex) == INVALID_ENTITY &&
					entityManager_.EntityExists(entityIndex))
				{
					DisplayEntity(entityIndex);
				}
			}
		}
		ImGui::End();
	}
}

void Hierarchy::DisplayEntity(Entity entity)
{
	ImGuiTreeNodeFlags nodeFlags;
	const std::string text = "Entity " + std::to_string(entity);
	if (editorToolManager_.GetSelectedEntity() == entity) nodeFlags = kNodeTreeSelectedFlags_;
	else nodeFlags = kNodeTreeNotSelectedFlags_;

	//Hide arrow if no child
	bool hasChild = false;
	for (Entity entityIndex = 0; entityIndex < entityManager_.GetEntitiesSize(); entityIndex++)
	{
		if (entity == entityManager_.GetEntityParent(entityIndex))
		{
			hasChild = true;
			break;
		}
	}

	if (!hasChild) nodeFlags |= ImGuiTreeNodeFlags_Leaf;

	//Display entity
	const bool nodeOpen = ImGui::TreeNodeEx(text.c_str(), nodeFlags);
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		// Registers entity index in payload
		ImGui::SetDragDropPayload("Entity", &entity, sizeof(Entity));
		ImGui::Text("%s", text.c_str());
		ImGui::EndDragDropSource();
	}

	if (ImGui::BeginDragDropTarget())
	{
		const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Entity");
		if (payload)    // Get payload
		{
			IM_ASSERT(payload->DataSize == sizeof(Entity));
			const Entity child =
				*static_cast<const Entity*>(payload->Data);    // Get entityIndex in payload
			if (entityManager_.GetEntityParent(child) == entity)
				entityManager_.SetEntityParent(child, INVALID_ENTITY);
			else
				entityManager_.SetEntityParent(child, entity);
		}
		ImGui::EndDragDropTarget();
	}

	// Select entity on click
	if (ImGui::IsItemClicked()) editorToolManager_.SetSelectedEntity(entity);

	// Display context menu on right click on entity
	if (ImGui::BeginPopupContextItem("Click Entity##" + std::to_string(entity)))
	{
		if (ImGui::MenuItem("Add Child"))
		{
			const Entity child = entityManager_.CreateEntity();
			entityManager_.AddComponentType(child, EntityMask(ComponentType::TRANSFORM3D));
			entityManager_.SetEntityParent(child, entity);
		}

		if (ImGui::MenuItem("Delete Entity")) entityManager_.DestroyEntity(entity, true);
		ImGui::EndPopup();
	}

	//If node open display their children
	if (nodeOpen)
	{
		for (Entity child = 0; child < entityManager_.GetEntitiesSize(); child++)
		{
			//Display entity if it's a child of this entity
			if (entityManager_.GetEntityParent(child) == entity &&
				entityManager_.EntityExists(child))
			{
				DisplayEntity(child);
			}
		}

		ImGui::TreePop();
	}
}

void Hierarchy::OnEvent(const SDL_Event&) {}
}
