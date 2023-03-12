#include "pch.h"
#include "Tags.h"
#include "Hierarchy.h"
#include "Transform.h"

void ImGui::Hierarchy(entt::registry& aRegistry)
{
	entt::entity selection{ aRegistry.view<Tag::Selected>().front() };
	Transform::Ptr selectionTransform{};

	if (auto transform = aRegistry.try_get<Transform::Ptr>(selection))
		selectionTransform = *transform;

	// For each root transform, edit its full tree
	for (auto [entity, transform] : aRegistry.view<Transform::Ptr>().each())
	{
		// What happens if we modify the tree while looping over view????
		if (!transform->HasParent())
			Hierarchy(transform, selectionTransform);
	}

	for (auto [entity, transform] : aRegistry.view<Transform::Ptr>().each())
	{
		if (entity != selection && transform == selectionTransform)
		{
			aRegistry.remove<Tag::Selected>(selection);
			aRegistry.emplace<Tag::Selected>(entity);
		}
	}

	if (IsKeyPressed(ImGuiKey_Delete) && aRegistry.valid(selection))
		aRegistry.destroy(selection);
}
