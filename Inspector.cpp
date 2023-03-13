#include "pch.h"
#include "Inspector.h"
#include "Tags.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"

void ImGui::Inspector(entt::registry& aRegistry)
{
	entt::handle selection{ aRegistry, aRegistry.view<Tag::Selected>().front() };
	if (!selection)
		return;

	if (auto transform = selection.try_get<Transform::Ptr>())
		if (CollapsingHeader(ICON_FA_UP_DOWN_LEFT_RIGHT" Transform", ImGuiTreeNodeFlags_DefaultOpen))
			Inspect(*transform);

	if (auto mesh = selection.try_get<Mesh::Ptr>())
		if (CollapsingHeader(ICON_FA_CIRCLE_NODES" Mesh", ImGuiTreeNodeFlags_DefaultOpen))
			Inspect(**mesh);

	if (auto material = selection.try_get<Material>())
		if (CollapsingHeader(ICON_FA_PALETTE" Material", ImGuiTreeNodeFlags_DefaultOpen))
			Inspect(*material);

	if (auto camera = selection.try_get<Camera>())
	{
		bool visible = true;
		if (CollapsingHeader(ICON_FA_VIDEO" Camera", &visible, ImGuiTreeNodeFlags_DefaultOpen))
			Inspect(*camera);
		if (!visible)
			selection.remove<Camera>();
	}

	if (auto light = selection.try_get<Light>())
	{
		bool visible = true;
		if (CollapsingHeader(ICON_FA_SUN" Light", &visible, ImGuiTreeNodeFlags_DefaultOpen))
			Inspect(*light);
		if (!visible)
			selection.remove<Light>();
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
				emplacers.at(i)(selection);

		EndPopup();
	}
}
