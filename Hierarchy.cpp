#include "pch.h"
#include "Hierarchy.h"
#include "Transform.h"
#include "EnttCommon.h"
#include "ImGuiCommon.h"

namespace ImGui
{
	entt::entity EntityCreationContextMenu(entt::registry& aRegistry)
	{
		entt::entity entity{ entt::null };

		if (MenuItem("Create Empty"))
			entity = CreateEmpty(aRegistry);
		if (BeginMenu("Primitive"))
		{
			if (MenuItem("Plane"))
				entity = CreatePlane(aRegistry);
			if (MenuItem("Cube"))
				entity = CreateCube(aRegistry);
			if (MenuItem("Sphere"))
				entity = CreateSphere(aRegistry);
			if (MenuItem("Cylinder"))
				entity = CreateCylinder(aRegistry);
			if (MenuItem("Cone"))
				entity = CreateCone(aRegistry);
			if (MenuItem("Torus"))
				entity = CreateTorus(aRegistry);
			if (MenuItem("Suzanne"))
				entity = CreateSuzanne(aRegistry);
			EndMenu();
		}

		if (aRegistry.valid(entity))
			Select(aRegistry, entity);

		return entity;
	}
}

void ImGui::Hierarchy(entt::registry& aRegistry)
{
	if (BeginPopupContextWindow())
	{
		EntityCreationContextMenu(aRegistry);
		EndPopup();
	}

	/*
	* Hierarchy is traversed depth-first using a stack,
	* which is initialized to contain all root entities.
	*/

	std::vector<entt::entity> stack{};

	for (auto [entity, transform] : aRegistry.view<Transform>().each())
	{
		if (!aRegistry.valid(transform.GetParent()))
			stack.push_back(entity);
	}

	bool dirty = false;

	while (!dirty && !stack.empty())
	{
		entt::entity entity = stack.back();
		stack.pop_back();

		std::string& name = aRegistry.get<std::string>(entity);
		Transform& transform = aRegistry.get<Transform>(entity);
		const float indent = GetStyle().IndentSpacing * transform.GetDepth(aRegistry);

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (!transform.HasChildren())
			flags |= ImGuiTreeNodeFlags_Leaf;
		if (entity == GetSelected(aRegistry))
			flags |= ImGuiTreeNodeFlags_Selected;

		std::string label{ ICON_FA_CUBE" " };
		label += name;
		label += "##";
		label += std::to_string(std::to_underlying(entity));

		if (indent > 0.f)
			Indent(indent);

		const bool open = TreeNodeEx(label.c_str(), flags);

		if (IsItemClicked() && !IsItemToggledOpen())
			Select(aRegistry, entity);

		if (BeginPopupContextItem(label.c_str()))
		{
			if (Selectable("Duplicate"))
			{
				Transform& copy = transform.Duplicate(aRegistry, transform.GetParent());
				Select(aRegistry, copy.GetEntity());
				dirty = true;
			}
			if (Selectable("Delete"))
			{
				transform.Destroy(aRegistry);
				dirty = true;
			}

			Separator();

			entt::entity newEntity = EntityCreationContextMenu(aRegistry);
			if (auto newTransform = aRegistry.try_get<Transform>(newEntity))
			{
				newTransform->SetParent(aRegistry, entity, false);
				dirty = true;
			}

			EndPopup();
		}

		if (BeginDragDropSource())
		{
			SetDragDropPayload("entity", &entity, sizeof(entt::entity));
			Text(name.c_str());
			EndDragDropSource();
		}

		if (BeginDragDropTarget())
		{
			if (auto payload = AcceptDragDropPayload("entity"))
			{
				auto dropEntity = *reinterpret_cast<entt::entity*>(payload->Data);
				if (auto dropTrans = aRegistry.try_get<Transform>(dropEntity))
				{
					dropTrans->SetParent(aRegistry, entity);
					dirty = true;
				}
			}
			EndDragDropTarget();
		}

		if (open)
		{
			stack.append_range(transform.GetChildren());
			TreePop();
		}

		if (indent > 0.f)
			Unindent(indent);
	}

	if (BeginDrapDropTargetWindow("entity"))
	{
		if (auto payload = AcceptDragDropPayload("entity"))
		{
			auto entity = *reinterpret_cast<entt::entity*>(payload->Data);
			if (auto transform = aRegistry.try_get<Transform>(entity))
				transform->SetParent(aRegistry, entt::null);
		}
		EndDragDropTarget();
	}
	
	/*
	* Shortcuts
	*/

	// todo: move to main

	if (IsKeyPressed(ImGuiKey_Delete))
	{
		if (auto transform = aRegistry.try_get<Transform>(GetSelected(aRegistry)))
			transform->Destroy(aRegistry);
	}

	if (IsKeyDown(ImGuiMod_Ctrl) && IsKeyPressed(ImGuiKey_D))
	{
		if (auto transform = aRegistry.try_get<Transform>(GetSelected(aRegistry)))
			transform->Duplicate(aRegistry);
	}
}
