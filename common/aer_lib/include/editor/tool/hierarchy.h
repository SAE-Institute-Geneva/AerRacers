#pragma once
#include <editor/editor_tool_interface.h>
#include <string>
#include <engine/entity.h>

namespace neko::aer
{
	class Hierarchy final : public EditorToolInterface
	{
	public:
		Hierarchy(ToolType type, int id, std::string name);
		void Init() override;
		void Update(seconds dt) override;
		void DrawImGui() override;
		void Destroy() override;
		void OnEvent(const SDL_Event& event) override;

	};
}
