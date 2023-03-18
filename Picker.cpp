#include "pch.h"
#include "Picker.h"
#include "Renderer.h"
#include "EnttCommon.h"

void ImGui::Picker(entt::registry& aRegistry)
{
    if (GetIO().WantCaptureMouse)
        return;
    if (!IsMouseClicked(ImGuiMouseButton_Left))
        return;

    entt::entity selection{ entt::null };

    Renderer::Get().GetTexture(t_GBufferEntity).GetTexel(
        std::as_writable_bytes(std::span{ &selection, 1 }),
        static_cast<unsigned>(GetMousePos().x),
        static_cast<unsigned>(GetMousePos().y)
    );

    Select({ aRegistry, selection }); // todo: if holding ctrl, then select multiple
}
