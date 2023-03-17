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
		if (!aHandle.all_of<Component>())
			return;

		PushID(aLabel);

		bool open = CollapsingHeader(aLabel, ImGuiTreeNodeFlags_DefaultOpen);

		if (BeginPopupContextItem())
		{
			if constexpr (!std::is_same_v<Component, Transform>)
			{
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

		PopID();
	}
}

void ImGui::Inspector(entt::registry& aRegistry)
{
	entt::handle selection{ aRegistry, GetSelectedFront(aRegistry) };
	if (!selection)
		return;

	/*if (BeginPopupContextWindow())
	{
		MenuItem("Bruh");
		EndPopup();
	}*/

	InspectComponent<Transform>(selection, ICON_FA_UP_DOWN_LEFT_RIGHT" Transform");
	InspectComponent<Mesh>(selection, ICON_FA_CIRCLE_NODES" Mesh");
	InspectComponent<Material>(selection, ICON_FA_PALETTE" Material");
	InspectComponent<Camera>(selection, ICON_FA_VIDEO" Camera");
	InspectComponent<Light>(selection, ICON_FA_SUN" Light");
}
