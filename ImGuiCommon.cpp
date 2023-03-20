#include "pch.h"
#include "ImGuiCommon.h"
#include <imgui/imgui_internal.h>

// https://github.com/ocornut/imgui/issues/5539
bool ImGui::BeginDrapDropTargetWindow(const char* payload_type)
{
    ImRect inner_rect = GetCurrentWindow()->InnerRect;
    if (BeginDragDropTargetCustom(inner_rect, GetID("##WindowBgArea")))
    {
        if (const ImGuiPayload* payload = AcceptDragDropPayload(payload_type, ImGuiDragDropFlags_AcceptBeforeDelivery | ImGuiDragDropFlags_AcceptNoDrawDefaultRect))
        {
            if (payload->IsPreview())
            {
                ImDrawList* draw_list = GetForegroundDrawList();
                draw_list->AddRectFilled(inner_rect.Min, inner_rect.Max, GetColorU32(ImGuiCol_DragDropTarget, 0.05f));
                draw_list->AddRect(inner_rect.Min, inner_rect.Max, GetColorU32(ImGuiCol_DragDropTarget), 0.0f, 0, 2.0f);
            }
            if (payload->IsDelivery())
                return true;
            EndDragDropTarget();
        }
    }
    return false;
}
