#include "utils/imgui_utility.h"

#include <imgui_internal.h>

namespace ImGui
{
void DrawLabel(std::string_view label, LabelPos labelPos)
{
	PushItemWidth(-1.0f);
	{
		const std::string labelStr = std::string(label);
		const std::string text     = " " + labelStr.substr(0, labelStr.find_first_of('#'));
		if (labelPos == LabelPos::LEFT)
		{
			Text("%s", text.c_str());
			ImGui::SameLine();
		}
		else
		{
			const float width = CalcItemWidth();
			PushItemWidth(width - CalcTextSize(text.c_str()).x);
		}
	}
}

void EndDrawLabel(LabelPos labelPos)
{
	if (labelPos == LabelPos::RIGHT) PopItemWidth();
	PopItemWidth();
}

bool Begin(std::string_view name, bool* isOpen, ImGuiWindowFlags flags)
{
	return Begin(name.data(), isOpen, flags);
}

bool DragFloat(std::string_view label,
	float* v,
	LabelPos labelPos,
	float vSpeed,
	float vMin,
	float vMax,
	const char* format,
	ImGuiSliderFlags flags)
{
	bool result;
	DrawLabel(label, labelPos);
	result = DragScalar(("##" + std::string(label)).c_str(),
		ImGuiDataType_Float,
		v,
		vSpeed,
		&vMin,
		&vMax,
		format,
		flags);
	EndDrawLabel(labelPos);

	return result;
}

bool DragFloat2(std::string_view label,
	float v[2],
	LabelPos labelPos,
	float vSpeed,
	float vMin,
	float vMax,
	const char* format,
	ImGuiSliderFlags flags)
{
	bool result;
	DrawLabel(label, labelPos);
	result = DragScalarN(("##" + std::string(label)).c_str(),
		ImGuiDataType_Float,
		v,
		2,
		vSpeed,
		&vMin,
		&vMax,
		format,
		flags);
	EndDrawLabel(labelPos);

	return result;
}

bool DragFloat3(std::string_view label,
	float v[3],
	LabelPos labelPos,
	float vSpeed,
	float vMin,
	float vMax,
	const char* format,
	ImGuiSliderFlags flags)
{
	bool result;
	DrawLabel(label, labelPos);
	result = DragScalarN(("##" + std::string(label)).c_str(),
		ImGuiDataType_Float,
		v,
		3,
		vSpeed,
		&vMin,
		&vMax,
		format,
		flags);
	EndDrawLabel(labelPos);

	return result;
}

bool DragFloat4(std::string_view label,
	float v[4],
	LabelPos labelPos,
	float vSpeed,
	float vMin,
	float vMax,
	const char* format,
	ImGuiSliderFlags flags)
{
	bool result;
	DrawLabel(label, labelPos);
	result = DragScalarN(("##" + std::string(label)).c_str(),
		ImGuiDataType_Float,
		v,
		4,
		vSpeed,
		&vMin,
		&vMax,
		format,
		flags);
	EndDrawLabel(labelPos);

	return result;
}

bool SliderFloat(const char* label,
                        float* v,
                        float vMin,
                        float vMax,
                        LabelPos labelPos,
                        const char* format,
                        ImGuiSliderFlags flags)
{
	bool result;
	DrawLabel(label, labelPos);
	result = SliderScalar(
		("##" + std::string(label)).c_str(), ImGuiDataType_Float, v, &vMin, &vMax, format, flags);
	EndDrawLabel(labelPos);

	return result;
}

bool SliderFloat2(const char* label,
	float v[2],
	float vMin,
	float vMax,
	LabelPos labelPos,
	const char* format,
	ImGuiSliderFlags flags)
{
	bool result;
	DrawLabel(label, labelPos);
	result = SliderScalarN(("##" + std::string(label)).c_str(),
		ImGuiDataType_Float,
		v,
		2,
		&vMin,
		&vMax,
		format,
		flags);
	EndDrawLabel(labelPos);

	return result;
}

bool SliderFloat3(const char* label,
	float v[3],
	float vMin,
	float vMax,
	LabelPos labelPos,
	const char* format,
	ImGuiSliderFlags flags)
{
	bool result;
	DrawLabel(label, labelPos);
	result = SliderScalarN(("##" + std::string(label)).c_str(),
		ImGuiDataType_Float,
		v,
		3,
		&vMin,
		&vMax,
		format,
		flags);
	EndDrawLabel(labelPos);

	return result;
}

bool SliderFloat4(const char* label,
	float v[4],
	float vMin,
	float vMax,
	LabelPos labelPos,
	const char* format,
	ImGuiSliderFlags flags)
{
	bool result;
	DrawLabel(label, labelPos);
	result = SliderScalarN(("##" + std::string(label)).c_str(),
		ImGuiDataType_Float,
		v,
		4,
		&vMin,
		&vMax,
		format,
		flags);
	EndDrawLabel(labelPos);

	return result;
}

bool ColorEdit3(std::string_view label, float* col, LabelPos labelPos, ImGuiColorEditFlags flags)
{
	return ColorEdit4(label, col, labelPos, flags | ImGuiColorEditFlags_NoAlpha);
}

bool ColorEdit4(std::string_view label, float* col, LabelPos labelPos, ImGuiColorEditFlags flags)
{
	bool result;
	DrawLabel(label, labelPos);
	result = ColorEdit4(("##" + std::string(label)).c_str(), col, flags);
	EndDrawLabel(labelPos);

	return result;
}

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wvarargs"
void Text(std::string_view text, ...)
{
	va_list args;
	va_start(args, text.data());
	TextV(text.data(), args);
	va_end(args);
}

void TextColored(const ImVec4& col, std::string_view text, ...)
{
	va_list args;
	va_start(args, text.data());
	TextColoredV(col, text.data(), args);
	va_end(args);
}

void TextCentered(std::string_view text, ...)
{
	const float textSize   = CalcTextSize(text.data()).x;
	const float cursorPos  = GetCursorPosX();
	const float availWidth = GetContentRegionAvailWidth() - cursorPos;
	SetCursorPosX(cursorPos + (availWidth / 2.0f) - textSize + (textSize / 2.0f));

	va_list args;
	va_start(args, text.data());
	TextV(text.data(), args);
	va_end(args);
}
#pragma clang diagnostic pop

bool ButtonCentered(std::string_view label, const ImVec2& size)
{
	const std::string labelName = std::string(label).substr(0, label.find_first_of('#'));
	const float width           = CalcTextSize(labelName.data()).x;
	const float cursorPos       = GetCursorPosX();
	const float padding         = GetStyle().FramePadding.x;
	const float availWidth      = GetContentRegionAvailWidth() - cursorPos;
	if (size.x != 0.0f) SetCursorPosX(cursorPos + (availWidth / 2.0f) - size.x + (size.x / 2.0f));
	else SetCursorPosX(cursorPos + (availWidth / 2.0f) - width + (width / 2.0f) - padding);

	return Button(label.data(), size);
}

bool Button(std::string_view label, const ImVec2& size)
{
	return Button(label.data(), size);
}

/// A callback to make InputText (string) work <br>
/// Not to be used by itself
static int InputTextCallback(ImGuiInputTextCallbackData* data)
{
	if (data->EventFlag == ImGuiInputTextFlags_CallbackResize)
	{
		// Resize string callback
		auto* str = static_cast<std::string*>(data->UserData);
		IM_ASSERT(data->Buf == str->c_str());
		str->resize(data->BufTextLen);
		data->Buf = const_cast<char*>(str->c_str());
	}

	return 0;
}

bool InputText(
	std::string_view label, std::string* str, LabelPos labelPos, ImGuiInputTextFlags flags)
{
	flags |= ImGuiInputTextFlags_CallbackResize;

	bool result;
	DrawLabel(label, labelPos);
	result = InputText(("##" + std::string(label)).c_str(),
		const_cast<char*>(str->c_str()),
		str->capacity() + 1,
		flags,
		InputTextCallback,
		static_cast<void*>(str));
	EndDrawLabel(labelPos);
	return result;
}

bool Combo(std::string_view label,
	int* currentItem,
	const std::vector<std::string>* items,
	LabelPos labelPos,
	int heightInItems)
{
	const auto lambda = [](void* data, const int idx, const char** outText)
	{
		*outText = static_cast<const std::vector<std::string>*>(data)->at(idx).c_str();
		return true;
	};

	bool result;
	DrawLabel(label, labelPos);
	result = Combo(("##" + std::string(label)).c_str(),
		currentItem,
		lambda,
		(void*) items,
		items->size(),
		heightInItems);
	EndDrawLabel(labelPos);

	return result;
}

bool Combo(std::string_view label,
	unsigned int* currentItem,
	const std::vector<std::string>* items,
	LabelPos labelPos,
	int heightInItems)
{
	const auto lambda = [](void* data, const int idx, const char** outText)
	{
		*outText = static_cast<const std::vector<std::string>*>(data)->at(idx).c_str();
		return true;
	};

	bool result;
	DrawLabel(label, labelPos);
	result = Combo(("##" + std::string(label)).c_str(),
		reinterpret_cast<int*>(currentItem),
		lambda,
		(void*) items,
		items->size(),
		heightInItems);
	EndDrawLabel(labelPos);

	return result;
}

bool Checkbox(std::string_view label, bool* v, LabelPos labelPos)
{
	bool result;
	DrawLabel(label, labelPos);
	result = Checkbox(("##" + std::string(label)).c_str(), v);
	EndDrawLabel(labelPos);

	return result;
}

#ifdef NEKO_GLES3
bool ImageButton(const std::string_view itemId,
	const neko::TextureName userTextureId,
	const ImVec2& size,
	const ImVec2& uv0,
	const ImVec2& uv1,
	const int framePadding,
	const ImVec4& bgCol,
	const ImVec4& tintCol)
{
	ImGuiContext& g     = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	if (window->SkipItems) return false;

	auto* texId      = reinterpret_cast<ImTextureID>(userTextureId);
	const ImGuiID id = window->GetID(itemId.data());
	const ImVec2 padding =
		framePadding >= 0 ? ImVec2(float(framePadding), float(framePadding)) : g.Style.FramePadding;
	return ImageButtonEx(id, texId, size, uv0, uv1, padding, bgCol, tintCol);
}
#endif

bool MenuItem(std::string_view label, std::string_view shortcut, bool selected, bool enabled)
{
	return MenuItem(label.data(), shortcut.data(), selected, enabled);
}

bool MenuItem(
		std::string_view label, std::string_view shortcut, bool* isSelected, bool enabled)
{
	return MenuItem(label.data(), shortcut.data(), isSelected, enabled);
}

bool BeginPopupContextItem(std::string_view strId, ImGuiPopupFlags popupFlags)
{
	return BeginPopupContextItem(strId.data(), popupFlags);
}

ImGuiTextFilter::ImGuiTextFilter(std::string_view defaultFilter)
{
	ImStrncpy(inputBuf.data(), defaultFilter.data(), inputBuf.size());
	Build();
}

bool ImGuiTextFilter::Draw(std::string_view label, float width)
{
	if (width != 0.0f) ImGui::SetNextItemWidth(width);

	bool valueChanged = ImGui::InputText(label.data(), &inputBuf);
	if (valueChanged) Build();

	return valueChanged;
}

void ImGuiTextFilter::Build()
{
	filters.resize(0);
	ImGuiTextRange inputRange(inputBuf);
	inputRange.Split(',', &filters);

	countGrep = 0;
	for (auto& f : filters)
	{
		while (f.b < f.e && ImCharIsBlankA(f.b[0])) f.b++;
		while (f.e > f.b && ImCharIsBlankA(f.e[-1])) f.e--;
		if (f.Empty()) continue;
		if (f.b[0] != '-') countGrep += 1;
	}
}

void ImGuiTextFilter::Clear()
{
	inputBuf[0] = 0;
	Build();
}

bool ImGuiTextFilter::PassFilter(std::string_view text, std::string_view textEnd) const
{
	if (filters.empty()) return true;
	for (const auto& f : filters)
	{
		if (f.Empty()) continue;

		const char* end = textEnd.empty() ? nullptr : textEnd.data();
		if (f.b[0] == '-')    // Subtract
		{
			if (ImStristr(text.data(), end, f.b + 1, f.e) != nullptr) return false;
		}
		else    // Grep
		{
			if (ImStristr(text.data(), end, f.b, f.e) != nullptr) return true;
		}
	}

	// Implicit * grep
	if (countGrep == 0) return true;

	return false;
}

void ImGuiTextFilter::ImGuiTextRange::Split(char separator, ImVector<ImGuiTextRange>* out) const
{
	out->resize(0);
	const char* wb = b;
	const char* we = wb;
	while (we < e)
	{
		if (*we == separator)
		{
			out->push_back(ImGuiTextRange(wb, we));
			wb = we + 1;
		}

		we++;
	}

	if (wb != we) out->push_back(ImGuiTextRange(wb, we));
}
}
