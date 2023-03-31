#include "pch.h"
#include "Inspector.h"
#include "EnttCommon.h"

#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"

#include "PhysicMaterial.h"

namespace ImGui
{
	void Inspect(json& j)
	{
		/*for (auto& [key, value] : j.items())
		{
			using value_t = nlohmann::json::value_t;

			switch (j.type())
			{
			case value_t::null:
				Text(key.c_str());
				break;
			case value_t::boolean:
				Checkbox(key.c_str(), value.get_ptr<bool*>());
				break;
			}
		}*/

		if (Button("Add"))
			j.emplace_back("Hello");
	}

	template <class Component>
	void InspectComponent(const char* aLabel, entt::handle aHandle)
	{
		constexpr bool isTransform = std::is_same_v<Component, Transform>;

		if (aHandle.all_of<Component>())
		{
			bool open = CollapsingHeader(aLabel, ImGuiTreeNodeFlags_DefaultOpen);

			if (BeginPopupContextItem())
			{
				if constexpr (isTransform)
				{
					if (MenuItem("Reset"))
						aHandle.get<Transform>().SetLocalMatrix({});
				}
				else
				{
					if (MenuItem("Reset"))
						aHandle.get<Component>() = {};
					if (MenuItem("Remove"))
					{
						aHandle.erase<Component>();
						open = false;
					}
				}
				EndPopup();
			}

			if (open)
			{
				Inspect(aHandle.get<Component>());
				Spacing();
			}
		}
		else
		{
			if (BeginPopup("Add Component"))
			{
				if constexpr (!isTransform)
				{
					if (MenuItem(aLabel))
						aHandle.emplace<Component>();
				}
				EndPopup();
			}
		}
	}
}

void ImGui::Inspector(entt::registry& aRegistry)
{
	entt::handle selection{ aRegistry, GetSelected(aRegistry) };
	if (!selection)
		return;

	InspectComponent<Transform>(ICON_FA_UP_DOWN_LEFT_RIGHT" Transform", selection);
	InspectComponent<Mesh>(ICON_FA_CIRCLE_NODES" Mesh", selection);
	InspectComponent<Material>(ICON_FA_PALETTE" Material", selection);
	InspectComponent<Camera>(ICON_FA_VIDEO" Camera", selection);
	InspectComponent<Light>(ICON_FA_SUN" Light", selection);
	InspectComponent<PhysicMaterial>(ICON_FA_HILL_ROCKSLIDE" Physic Material", selection);
	InspectComponent<json>(ICON_FA_CHALKBOARD_USER" JSON", selection);

	Separator();

	if (Button("Add Component"))
		OpenPopup("Add Component");
}
