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

	bool dirty = false;

	while (!dirty && !stack.empty())
	{
		entt::entity entity = stack.back();
		stack.pop_back();

		Transform& transform = aRegistry.get<Transform>(entity);
		const float indent = GetStyle().IndentSpacing * transform.GetDepth(aRegistry);

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (!transform.HasChildren())
			flags |= ImGuiTreeNodeFlags_Leaf;
		if (IsSelected({ aRegistry, entity }))
			flags |= ImGuiTreeNodeFlags_Selected;

		std::string name{ ICON_FA_CUBE" " };
		name += transform.GetName();

		if (indent > 0.f)
			Indent(indent);

		const bool open = TreeNodeEx(name.c_str(), flags);

		if (IsItemClicked() && !IsItemToggledOpen())
			Select({ aRegistry, entity });

		if (BeginPopupContextItem(std::to_string(std::to_underlying(entity)).c_str()))
		{
			if (Selectable("Delete"))
			{
				transform.Destroy(aRegistry);
				dirty = true;
			}
			if (Selectable("Create Child"))
			{
				transform.CreateChild(aRegistry);
				dirty = true;
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
			for (entt::entity child : transform.GetChildren())
				stack.push_back(child);
			TreePop();
		}

		if (indent > 0.f)
			Unindent(indent);
	}
}
