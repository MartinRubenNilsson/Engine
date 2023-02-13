#include "pch.h"
#include "imgui_entt.h"
#include "Transform.h"
#include "Material.h"

void ImGui::Inspector(entt::handle aHandle)
{
	if (auto transform = aHandle.try_get<Transform::Ptr>())
	{
		if (CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
			InspectTransform(*transform);
	}

	if (auto material = aHandle.try_get<Material::Ptr>())
	{
		if (CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
			InspectMaterial(**material);
	}
}

void ImGui::Hierarchy(entt::registry& aRegistry, entt::entity& aSelection)
{
	Transform::Ptr selection;

	if (auto transform = aRegistry.try_get<Transform::Ptr>(aSelection))
		selection = *transform;

	for (auto [entity, transform] : aRegistry.view<Transform::Ptr>().each())
	{
		if (!transform->HasParent())
			Hierarchy(transform, selection);
	}

	for (auto [entity, transform] : aRegistry.view<Transform::Ptr>().each())
	{
		if (transform == selection)
			aSelection = entity;
	}
}
