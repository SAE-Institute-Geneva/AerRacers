#include "aer/editor/tool/inspector.h"

#include "aer/aer_engine.h"

namespace neko::aer
{
Inspector::Inspector(AerEngine& engine, ToolType type, int id, std::string_view name)
   : EditorToolInterface(engine, type, id, name),
	 editorToolManager_(engine.GetEditorToolManager()),
	 entityManager_(engine.GetComponentManagerContainer().entityManager),
	 transform3dManager_(engine.GetComponentManagerContainer().transform3dManager),
	 renderManager_(engine.GetComponentManagerContainer().renderManager),
	 lightManager_(engine.GetComponentManagerContainer().lightManager),
	 rigidDynamicManager_(engine.GetComponentManagerContainer().rigidDynamicManager),
	 rigidStaticManager_(engine.GetComponentManagerContainer().rigidStaticManager),
	 transform3dViewer_(engine.GetComponentManagerContainer().transform3dViewer),
	 rendererViewer_(engine.GetComponentManagerContainer().rendererViewer),
	 lightViewer_(engine.GetComponentManagerContainer().lightViewer),
	 rigidDynamicViewer_(engine.GetComponentManagerContainer().rigidDynamicViewer),
	 rigidStaticViewer_(engine.GetComponentManagerContainer().rigidStaticViewer)
{}

void Inspector::Init() {}

void Inspector::Update(seconds) {}

void Inspector::Destroy() {}

void Inspector::DrawImGui()
{
	if (isVisible)
	{
		// Display window
		const std::string winName = std::string(GetName()) + "##" + std::to_string(GetId());
		if (ImGui::Begin(winName, &isVisible))
		{
			//Get selected entity
			Entity entity = editorToolManager_.GetSelectedEntity();
			if (entity == INVALID_ENTITY)
			{
				//Display message if no entity selected
				ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "No Entity selected");
				ImGui::End();
				return;
			}

			// Display entity name
			std::string name = "Entity " + std::to_string(entity);
			ImGui::Text(name);

			ImGui::Separator();
			DisplayLayersAndTags(entity);
			ImGui::Separator();

			// Display Component Viewers
			DisplayComponentViewer(transform3dViewer_, entity, ComponentType::TRANSFORM3D);
			DisplayComponentViewer(rendererViewer_, entity, ComponentType::MODEL);
			DisplayComponentViewer(lightViewer_, entity, ComponentType::LIGHT);
			DisplayComponentViewer(rigidStaticViewer_, entity, ComponentType::RIGID_STATIC);
			DisplayComponentViewer(rigidDynamicViewer_, entity, ComponentType::RIGID_DYNAMIC);

			ImGui::Separator();
			DisplayNewComponentButtons(entity);
		}

		ImGui::End();
	}
}

void Inspector::OnEvent(const SDL_Event&) {}

void Inspector::DisplayComponentViewer(ComponentViewer& viewer, Entity entity, ComponentType type)
{
	if (!entityManager_.HasComponent(entity, EntityMask(type))) return;

	viewer.DrawImGui(entity);
	const std::string componentName = ComponentTypeToString(type);
	if (ImGui::BeginPopupContextItem(componentName))
	{
		if (ImGui::Button("Delete " + componentName))
		{
			entityManager_.RemoveComponentType(entity, EntityMask(type));
			ImGui::CloseCurrentPopup();
		}

		ImGui::EndPopup();
	}
}

void Inspector::DisplayLayersAndTags(Entity selectedEntity)
{
	//Tags
	auto& tagManager = TagLocator::get();
	currentTag_ = tagManager.GetEntityTagIndex(selectedEntity);
	std::vector<std::string> tags = engine_.GetComponentManagerContainer().sceneManager.GetTags();
	if (ImGui::Combo("Tag", &currentTag_, &tags))
	{
		tagManager.SetEntityTag(selectedEntity, tags[currentTag_]);
	}

	// Context Menu
	if (ImGui::BeginPopupContextItem("Create New Tag"))
	{
		ImGui::Text("Add New Tag"); ImGui::Separator();
		ImGui::SetNextItemWidth(ImGui::CalcTextSize("Add New Tag").x * 2.0f);
		ImGui::InputText("##newTag", &newStr_); ImGui::SameLine();
		if (ImGui::Button("Add Tag"))
		{
			if (!newStr_.empty())
				engine_.GetComponentManagerContainer().sceneManager.AddTag(newStr_);

			newStr_ = "";
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}

	//Layers
	currentLayer_ = tagManager.GetEntityLayerIndex(selectedEntity);
	std::vector<std::string> layers =
		engine_.GetComponentManagerContainer().sceneManager.GetLayers();
	if (ImGui::Combo("Layer", &currentLayer_, &layers))
	{
		tagManager.SetEntityLayer(selectedEntity, layers[currentLayer_]);
	}

	// Context Menu
	if (ImGui::BeginPopupContextItem("Create New Layer"))
	{
		ImGui::Text("Add New Layer"); ImGui::Separator();
		ImGui::SetNextItemWidth(ImGui::CalcTextSize("Add New Layer").x * 2.0f);
		ImGui::InputText("##newLayer", &newStr_); ImGui::SameLine();
		if (ImGui::Button("Add Layer"))
		{
			if (!newStr_.empty())
				engine_.GetComponentManagerContainer().sceneManager.AddLayer(newStr_);

			newStr_ = "";
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void Inspector::DisplayNewComponentButtons(Entity selectedEntity)
{
	if (ImGui::ButtonCentered("Add Component...##button"))
		ImGui::OpenPopup("Add Component##popup");

	if (ImGui::BeginPopup("Add Component##popup"))
	{
		searchFilter_.Draw("##searchFilter");
		ImGui::Separator();

		// Miscellaneous
		for (const auto& component : componentTypes_)
		{
			if (entityManager_.HasComponent(selectedEntity, (EntityMask) component)) continue;

			const std::string componentName = ComponentTypeToString(component) + "##component";
			if (!searchFilter_.PassFilter(componentName)) continue;

			if (ImGui::MenuItem(componentName))
			{
				if (component == ComponentType::RIGID_DYNAMIC)
				{
					rigidDynamicManager_.AddRigidDynamic(selectedEntity, {});
				}
				else if (component == ComponentType::RIGID_STATIC)
				{
					rigidStaticManager_.AddRigidStatic(selectedEntity, {});
				}
				else
				{
					entityManager_.AddComponentType(selectedEntity, (EntityMask) component);

					auto& tagManager = TagLocator::get();
					tagManager.SetEntityTag(selectedEntity, 0);
					tagManager.SetEntityLayer(selectedEntity, 0);
				}
			}
		}

		ImGui::EndPopup();
	}
}
}    // namespace neko::aer
