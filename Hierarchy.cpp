#include "pch.h"
#include "Hierarchy.h"
#include "Transform.h"

void ImGui::Hierarchy(entt::registry& aRegistry, entt::entity& aSelection)
{
	Transform::Ptr selectionTransform;

	if (auto transform = aRegistry.try_get<Transform::Ptr>(aSelection))
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
		if (transform == selectionTransform)
			aSelection = entity;
	}

	if (IsKeyPressed(ImGuiKey_Delete) && aRegistry.valid(aSelection))
	{
		aRegistry.destroy(aSelection);
		aSelection = entt::null;
	}
}
