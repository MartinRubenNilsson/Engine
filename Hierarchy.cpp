#include "pch.h"
#include "Hierarchy.h"
#include "Transform.h"
#include "EnttCommon.h"

void ImGui::Hierarchy(entt::registry& aRegistry)
{
	if (BeginPopupContextWindow())
	{
		if (Selectable("Create Root"))
			Transform::Create(aRegistry);
		EndPopup();
	}

	// Hierarchy is traversed depth-first using a stack.
	// The stack is initialized to contain all root entities.

	std::vector<entt::entity> stack{};

	for (auto [entity, transform] : aRegistry.view<Transform>().each())
	{
		if (!aRegistry.valid(transform.GetParent()))
			stack.push_back(entity);
	}

	bool changed = false;

	while (!changed && !stack.empty())
	{
		entt::entity entity = stack.back();
		stack.pop_back();

		Transform& transform = aRegistry.get<Transform>(entity);
		const size_t depth = transform.GetDepth(aRegistry);

		ImGuiTreeNodeFlags flags{ ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth };
		if (!transform.HasChildren())
			flags |= ImGuiTreeNodeFlags_Leaf;
		if (IsSelected({ aRegistry, entity }))
			flags |= ImGuiTreeNodeFlags_Selected;

		std::string name{ ICON_FA_CUBE" " };
		name += transform.GetName();

		if (depth)
			Indent(GetStyle().IndentSpacing * depth);

		const bool open = TreeNodeEx(name.c_str(), flags);

		if (IsItemClicked() && !IsItemToggledOpen())
			Select({ aRegistry, entity });

		if (BeginPopupContextItem(std::to_string(std::to_underlying(entity)).c_str()))
		{
			if (Selectable("Delete"))
			{
				transform.Destroy(aRegistry);
				changed = true;
			}
			if (Selectable("Create Child"))
			{
				transform.CreateChild(aRegistry);
				changed = true;
			}
			EndPopup();
		}

		if (BeginDragDropSource())
		{
			SetDragDropPayload("entity", &entity, sizeof(entt::entity));
			EndDragDropSource();
		}

		if (BeginDragDropTarget())
		{
			if (auto payload = AcceptDragDropPayload("entity"))
			{
				auto otherEntity = *reinterpret_cast<entt::entity*>(payload->Data);
				if (auto otherTrans = aRegistry.try_get<Transform>(otherEntity))
				{
					otherTrans->SetParent(aRegistry, entity); // Invalidates stack
					changed = true;
				}
			}
			EndDragDropTarget();
		}

		if (open)
		{
			for (entt::entity child : transform.GetChildren())
				stack.push_back(child);

			TreePop();
		}

		if (depth)
			Unindent(GetStyle().IndentSpacing * depth);
	}
}
