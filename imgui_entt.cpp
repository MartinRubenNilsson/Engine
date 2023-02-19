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
