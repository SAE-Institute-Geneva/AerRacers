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
		void DisplayChildren(Entity entityParent);
		void Destroy() override;
		void OnEvent(const SDL_Event& event) override;

	private:
		size_t selectedEntity = -1;

		const ImGuiTreeNodeFlags nodeTreeNotSelectedFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
		const ImGuiTreeNodeFlags nodeTreeSelectedFlags = nodeTreeNotSelectedFlags | ImGuiTreeNodeFlags_Selected;  // Add flag to node flags to show it's selected

	};
}
