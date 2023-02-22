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

	if (IsKeyPressed(ImGuiKey_Delete) && aRegistry.all_of<Transform::Ptr>(aSelection))
	{
		std::unordered_map<Transform::Ptr, entt::entity> transformToEntity{};

		for (auto [entity, transform] : aRegistry.view<Transform::Ptr>().each())
			transformToEntity.emplace(transform, entity);

		auto destroyRecursive = [&aRegistry, &transformToEntity](this auto aSelf, Transform::Ptr aTransform) -> void
		{
			aRegistry.destroy(transformToEntity.at(aTransform));
			for (const auto& child : aTransform->GetChildren())
				aSelf(child);
		};

		auto transform{ aRegistry.get<Transform::Ptr>(aSelection) };
		destroyRecursive(transform);
		transform->SetParent(nullptr, false);

		aSelection = entt::null;
	}
}
