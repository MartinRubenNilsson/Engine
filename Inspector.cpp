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
#include "CharacterController.h"
#include "Rigidbody.h"

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
					{
						aHandle.get<Transform>().position = {};
						aHandle.get<Transform>().rotation = {};
						aHandle.get<Transform>().scale = Vector3::One;
					}
				}
				else
				{
					if (MenuItem("Reset"))
						aHandle.replace<Component>();
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
				Inspect(aHandle.get<Component>()); // Inspect() must have an overload for Component
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

	Value("Entity", std::to_underlying(selection.entity()));

	if (auto name = selection.try_get<std::string>())
		InputText("Name", name);

	Separator();

	InspectComponent<Transform>(ICON_FA_UP_DOWN_LEFT_RIGHT" Transform", selection);
	InspectComponent<Mesh>(ICON_FA_CIRCLE_NODES" Mesh", selection);
	InspectComponent<Material>(ICON_FA_PALETTE" Material", selection);
	InspectComponent<Camera>(ICON_FA_VIDEO" Camera", selection);
	InspectComponent<Light>(ICON_FA_SUN" Light", selection);
	InspectComponent<PhysicMaterial>(ICON_FA_HILL_ROCKSLIDE" Physic Material", selection);
	InspectComponent<CharacterController>(ICON_FA_CAPSULES" Character Controller", selection);
	InspectComponent<Rigidbody>(ICON_FA_GAUGE_HIGH" Rigidbody", selection);
	InspectComponent<json>(ICON_FA_CHALKBOARD_USER" JSON", selection);

	Separator();

	if (Button("Add Component"))
		OpenPopup("Add Component");
}
