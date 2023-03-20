#include "pch.h"
#include "Inspector.h"
#include "EnttCommon.h"
#include "Transform.h"
#include "Mesh.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"

namespace ImGui
{
	template <class Component>
	void InspectComponent(entt::handle aHandle, const char* aLabel)
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

	InspectComponent<Transform>(selection, ICON_FA_UP_DOWN_LEFT_RIGHT" Transform");
	InspectComponent<Mesh>(selection, ICON_FA_CIRCLE_NODES" Mesh");
	InspectComponent<Material>(selection, ICON_FA_PALETTE" Material");
	InspectComponent<Camera>(selection, ICON_FA_VIDEO" Camera");
	InspectComponent<Light>(selection, ICON_FA_SUN" Light");

	Separator();

	if (Button("Add Component"))
		OpenPopup("Add Component");
}
