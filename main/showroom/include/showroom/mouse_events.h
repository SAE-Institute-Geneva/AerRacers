#pragma once
#include "imgui.h"
#include "imgui_internal.h"

namespace neko::sr
{
static constexpr int MouseButtons         = IM_ARRAYSIZE(ImGuiContext::IO.MouseDown);
static constexpr float MouseDragThreshold = 2;

struct MouseEvents
{
    bool mouseDown[MouseButtons];
    bool mouseClicked[MouseButtons];
    bool mouseReleased[MouseButtons];
    bool mouseDragging[MouseButtons];
    bool mousePotentialClickRelease[MouseButtons];

    ImVec2 mouseDragDelta[MouseButtons];

    void MouseFrame()
    {
        using namespace ImGui;
        for (int i = 0; i < MouseButtons; i++)
        {
            mouseDown[i]      = IsMouseDown(i);
            mouseClicked[i]   = IsMouseClicked(i);
            mouseReleased[i]  = IsMouseReleased(i);
            mouseDragging[i]  = ImGui::IsMouseDragging(i, 0);
            mouseDragDelta[i] = ImGui::GetMouseDragDelta(i, 0);

            if (mouseDragging[i])
            {
                if (mouseClicked[i] || mousePotentialClickRelease[i])
                {
                    if (std::abs(mouseDragDelta[i].x) < MouseDragThreshold &&
                        std::abs(mouseDragDelta[i].y) < MouseDragThreshold)
                    {
                        mouseDragging[i] = false;
                    }
                    else
                    {
                        mousePotentialClickRelease[i] = false;
                    }
                }
            }
            else if (!mouseDown[i] && !mouseReleased[i])
            {
                mousePotentialClickRelease[i] = false;
            }
        }
    }

    bool IsMouseDown(ImGuiMouseButton button) { return mouseDown[button]; }

    bool IsMouseClicked(ImGuiMouseButton button) { return mouseClicked[button]; }

    bool IsMouseDragging(ImGuiMouseButton button) { return mouseDragging[button]; }

    ImVec2 GetMouseDragDelta(ImGuiMouseButton button) { return mouseDragDelta[button]; }

    void ConsumeMouseEvents(ImGuiMouseButton button)
    {
        mouseDown[button]     = false;
        mouseClicked[button]  = false;
        mouseDragging[button] = false;
    }

    bool IsMouseClickReleased(ImGuiMouseButton button)
    {
        if (mouseReleased[button] && mousePotentialClickRelease[button]) return true;
        if (mouseClicked[button]) mousePotentialClickRelease[button] = true;
        return false;
    }
};
}    // namespace neko::sr
