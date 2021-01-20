#pragma once

#include <editor/editor_tool_interface.h>
#include <string>
#include <engine/entity.h>

namespace neko::aer
{
	class Hierarchy final : public EditorToolInterface
	{
	public:
		Hierarchy(AerEngine& engine, ToolType type, int id, std::string name);
		void Init() override;
		void Update(seconds dt) override;
		void DrawImGui() override;
		void Destroy() override;
		void OnEvent(const SDL_Event& event) override;

        /**
         * \brief Display an entity and their children
         * \param entityIndex entity to display
         */
		void DisplayEntity(Entity entityParent);

	private:
        EditorToolManager& editorToolManager_;
        EntityManager& entityManager_;

		const ImGuiTreeNodeFlags kNodeTreeNotSelectedFlags_ = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		const ImGuiTreeNodeFlags kNodeTreeSelectedFlags_ = kNodeTreeNotSelectedFlags_ | ImGuiTreeNodeFlags_Selected;  // Add flag to node flags to show it's selected

	};
}
