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

 Author : Simon Canas
 Co-Author :
 Date : 09.03.2021
---------------------------------------------------------- */
#include <imgui.h>

#ifdef NEKO_GLES3
#include "graphics/color.h"
#include "graphics/texture.h"
#endif

namespace ImGui
{
/// Used to determine the side at which to draw the label <br>
/// Also used to differentiate our methods from ImGui's version
enum class LabelPos
{
	RIGHT,
	LEFT
};

/// Begin an ImGui window using strings
bool Begin(std::string_view name, bool* isOpen = nullptr, ImGuiWindowFlags flags = 0);

/// A DragFloat with more flexible and better handling of labels
bool DragFloat(std::string_view label,
	float* v,
	LabelPos labelPos      = LabelPos::RIGHT,
	float vSpeed           = 1.0f,
	float vMin             = 0.0f,
	float vMax             = 0.0f,
	const char* format     = "%.3f",
	ImGuiSliderFlags flags = 0);

/// A DragFloat2 with more flexible and better handling of labels
bool DragFloat2(std::string_view label,
	float v[2],
	LabelPos labelPos      = LabelPos::RIGHT,
	float vSpeed           = 1.0f,
	float vMin             = 0.0f,
	float vMax             = 0.0f,
	const char* format     = "%.3f",
	ImGuiSliderFlags flags = 0);

/// A DragFloat3 with more flexible and better handling of labels
bool DragFloat3(std::string_view label,
	float v[3],
	LabelPos labelPos      = LabelPos::RIGHT,
	float vSpeed           = 1.0f,
	float vMin             = 0.0f,
	float vMax             = 0.0f,
	const char* format     = "%.3f",
	ImGuiSliderFlags flags = 0);

/// A DragFloat4 with more flexible and better handling of labels
bool DragFloat4(std::string_view label,
	float v[4],
	LabelPos labelPos      = LabelPos::RIGHT,
	float vSpeed           = 1.0f,
	float vMin             = 0.0f,
	float vMax             = 0.0f,
	const char* format     = "%.3f",
	ImGuiSliderFlags flags = 0);

/// Draws a button that's centered independently of the current indentation
bool ButtonCentered(std::string_view label, const ImVec2& size = ImVec2(0.0f, 0.0f));

/// Draws text using strings <br>
/// Using invisible characters such as '#' can allow for extra padding
void Text(std::string_view text, ...);

/// Draws colored text using strings <br>
/// Using invisible characters such as '#' can allow for extra padding
void TextColored(const ImVec4& col, std::string_view text, ...);

/// Draws text that's centered independently of the current indentation <br>
/// Using invisible characters such as '#' can allow for extra padding
void TextCentered(std::string_view text, ...);

/// Draws button with a certain size using strings
bool Button(std::string_view label, const ImVec2& size = ImVec2(0, 0));

/// An InputText that takes a string as argument <br>
/// Allows for dynamic input size
bool InputText(
	std::string_view label, std::string* str, ImGuiInputTextFlags flags = ImGuiInputTextFlags_None);

/// A combo box that takes a vector of strings as argument <br>
/// Allows for more flexible combo size and labels
bool Combo(std::string_view label,
	int* currentItem,
	const std::vector<std::string>* items,
	int heightInItems = -1);

#ifdef NEKO_GLES3
/// A button that contains an image <br>
/// Since the current ImageButton from ImGui doesn't work yet, we need to create it ourselves
bool ImageButton(std::string_view itemId,
                 neko::TextureName userTextureId,
                 const ImVec2& size,
                 const ImVec2& uv0     = ImVec2(neko::Vec2f::zero),
                 const ImVec2& uv1     = ImVec2(neko::Vec2f::one),
                 int framePadding      = -1,
                 const ImVec4& bgCol   = ImVec4(neko::Color::clear),
                 const ImVec4& tintCol = ImVec4(neko::Color::white));
#endif

/// Draws a menu item using strings
bool MenuItem(std::string_view label,
	std::string_view shortcut = "",
	bool selected             = false,
	bool enabled              = true);

/// Draws a menu item using strings
bool MenuItem(
	std::string_view label, std::string_view shortcut, bool* isSelected, bool enabled = true);

/// Draws a popup context item using strings
bool BeginPopupContextItem(std::string_view strId = "", ImGuiPopupFlags popupFlags = 1);

/// A structure that replaces the default ImGui one
struct ImGuiTextFilter
{
	ImGuiTextFilter(std::string_view defaultFilter = "");
	bool Draw(std::string_view label = "Filter (inc,-exc)", float width = 0.0f);
	void Build();
	void Clear();

	[[nodiscard]] bool PassFilter(std::string_view text, std::string_view textEnd = "") const;

	[[nodiscard]] bool IsActive() const { return !filters.empty(); }

	// [Internal]
	struct ImGuiTextRange
	{
		const char* b;
		const char* e;

		ImGuiTextRange() = default;
		ImGuiTextRange(const char* b, const char* e) : b(b), e(e) {}
		ImGuiTextRange(std::string_view text) : b(&text.front()), e(&text.back() + 1) {}
		[[nodiscard]] bool Empty() const { return b == e; }
		void Split(char separator, ImVector<ImGuiTextRange>* out) const;
	};

	std::string inputBuf {};
	ImVector<ImGuiTextRange> filters;
	int countGrep {};
};
}    // namespace ImGui
