#include "pch.h"
#include "Inspector.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"

void ImGui::Inspector(entt::handle aHandle)
{
	if (!aHandle)
		return;

	if (auto transform = aHandle.try_get<Transform::Ptr>())
		if (CollapsingHeader(ICON_FA_UP_DOWN_LEFT_RIGHT" Transform", ImGuiTreeNodeFlags_DefaultOpen))
			InspectTransform(*transform);

	if (auto mesh = aHandle.try_get<Mesh::Ptr>())
		if (CollapsingHeader(ICON_FA_CIRCLE_NODES" Mesh", ImGuiTreeNodeFlags_DefaultOpen))
			InspectMesh(**mesh);

	if (auto material = aHandle.try_get<Material>())
		if (CollapsingHeader(ICON_FA_PALETTE" Material", ImGuiTreeNodeFlags_DefaultOpen))
			InspectMaterial(*material);

	if (auto camera = aHandle.try_get<Camera>())
	{
		bool visible = true;
		if (CollapsingHeader(ICON_FA_VIDEO" Camera", &visible, ImGuiTreeNodeFlags_DefaultOpen))
			InspectCamera(*camera);
		if (!visible)
			aHandle.remove<Camera>();
	}

	if (auto light = aHandle.try_get<Light>())
	{
		bool visible = true;
		if (CollapsingHeader(ICON_FA_SUN" Light", &visible, ImGuiTreeNodeFlags_DefaultOpen))
			InspectLight(*light);
		if (!visible)
			aHandle.remove<Light>();
	}

	Separator();

	static constexpr std::array names
	{
		"Camera",
		"Light",
	};

	static const std::array<std::function<void(entt::handle)>, names.size()> emplacers
	{
		&entt::handle::get_or_emplace<Camera>,
		&entt::handle::get_or_emplace<Light>,
	};

	if (Button("Add Component"))
		OpenPopup("AddComponentPopup");

	if (BeginPopup("AddComponentPopup"))
	{
		for (size_t i = 0; i < names.size(); ++i)
			if (Selectable(names.at(i)))
				emplacers.at(i)(aHandle);

		EndPopup();
	}
}
