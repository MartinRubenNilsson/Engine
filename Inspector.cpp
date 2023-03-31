#include "pch.h"
#include "Inspector.h"
#include "EnttCommon.h"
#include "ImGuiCommon.h"

// Common
#include "Transform.h"

// Graphics
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"

// Physics
#include "PhysicMaterial.h"

// Other
#include "JsonCommon.h"

namespace ImGui
{
	template <class Component>
	void InspectComponent(const char* aLabel, entt::handle aHandle)
	{
		if (aHandle.all_of<Component>())
		{
			bool open = CollapsingHeader(aLabel, ImGuiTreeNodeFlags_DefaultOpen);

			if (BeginPopupContextItem())
			{
				if constexpr (std::is_same_v<Component, Transform>)
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
				if constexpr (!std::is_same_v<Component, Transform>)
				{
					if (MenuItem(aLabel))
						aHandle.emplace<Component>() = {}; // json misbehaves if we just emplace
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
	InspectComponent<json>(ICON_FA_CHALKBOARD" JSON", selection);

	Separator();

	if (Button("Add Component"))
		OpenPopup("Add Component");
}
