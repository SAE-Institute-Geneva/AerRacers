#pragma once

/* ----------------------------------------------------
 MIT License

 Copyright (c) 2020 SAE Institute Switzerland AG

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.

 Author : Lorna Dupuis
 Co-Author :
 Date : 02.02.2021
---------------------------------------------------------- */
#include "engine/entity.h"

#include "aer/editor/editor_tool_interface.h"

namespace neko::aer
{
/// Hierarchy tool displays entities and their children
class Hierarchy final : public EditorToolInterface
{
public:
	Hierarchy(AerEngine& engine, ToolType type, int id, std::string_view name);

	void Init() override;
	void Update(seconds dt) override;
	void Destroy() override;

	void DrawImGui() override;

	void OnEvent(const SDL_Event& event) override;

private:
	/**
    * \brief Display an entity and their children
    * \param entityIndex entity to display
    */
	void DisplayEntity(Entity entity);

	EditorToolManager& editorToolManager_;
	EntityManager& entityManager_;

	const ImGuiTreeNodeFlags kNodeTreeNotSelectedFlags_ = ImGuiTreeNodeFlags_OpenOnArrow |
	                                                      ImGuiTreeNodeFlags_OpenOnDoubleClick |
	                                                      ImGuiTreeNodeFlags_SpanAvailWidth;
	const ImGuiTreeNodeFlags kNodeTreeSelectedFlags_ =
		kNodeTreeNotSelectedFlags_ |
		ImGuiTreeNodeFlags_Selected;    // Add flag to node flags to show it's selected
};
}    // namespace neko::aer
