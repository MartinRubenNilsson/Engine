#include "pch.h"
#include "Picker.h"
#include "Tags.h"
#include "Renderer.h"

void ImGui::Picker(entt::registry& aRegistry)
{
    if (GetIO().WantCaptureMouse)
        return;

    if (!IsMouseClicked(ImGuiMouseButton_Left))
        return;

    // todo: if not holding ctrl, then select multiple
    {
        auto selected{ aRegistry.view<Tag::Selected>() };
        aRegistry.erase<Tag::Selected>(selected.begin(), selected.end());
    }

    entt::entity selection{ entt::null };

    Renderer::Get().GetTexture(t_GBufferEntity).GetTexel(
        std::as_writable_bytes(std::span{ &selection, 1 }),
        static_cast<unsigned>(GetMousePos().x),
        static_cast<unsigned>(GetMousePos().y)
    );

    if (aRegistry.valid(selection))
        aRegistry.emplace_or_replace<Tag::Selected>(selection);
}
