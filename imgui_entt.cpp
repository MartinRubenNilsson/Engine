#include "pch.h"
#include "imgui_entt.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"

void ImGui::Inspector(entt::handle aHandle)
{
	if (auto transform = aHandle.try_get<Transform::Ptr>())
	{
		if (CollapsingHeader(ICON_FA_UP_DOWN_LEFT_RIGHT" Transform", ImGuiTreeNodeFlags_DefaultOpen))
			InspectTransform(*transform);
	}

	if (auto mesh = aHandle.try_get<Mesh::Ptr>())
	{
		if (CollapsingHeader(ICON_FA_CIRCLE_NODES" Mesh", ImGuiTreeNodeFlags_DefaultOpen))
			InspectMesh(**mesh);
	}

	if (auto material = aHandle.try_get<Material::Ptr>())
	{
		if (CollapsingHeader(ICON_FA_PALETTE" Material", ImGuiTreeNodeFlags_DefaultOpen))
			InspectMaterial(**material);
	}

	if (auto camera = aHandle.try_get<Camera>())
	{
		if (CollapsingHeader(ICON_FA_VIDEO" Camera", ImGuiTreeNodeFlags_DefaultOpen))
			InspectCamera(*camera);
	}

	if (auto light = aHandle.try_get<Light>())
	{
		if (CollapsingHeader(ICON_FA_SUN" Light", ImGuiTreeNodeFlags_DefaultOpen))
			InspectLight(*light);
	}
}

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
