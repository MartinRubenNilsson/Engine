#include "pch.h"
#include "Hierarchy.h"
#include "Transform.h"
#include "EnttCommon.h"
#include "Tags.h"

void ImGui::Hierarchy(entt::registry& aRegistry)
{
	// Hierarchy is traversed depth-first using a stack.
	// The stack is initialized to contain all root entities.

	std::vector<entt::entity> stack{};

	for (auto [entity, transform] : aRegistry.view<Transform>().each())
	{
		if (!aRegistry.valid(transform.GetParent()))
			stack.push_back(entity);
	}

	while (!stack.empty())
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

		/*if (BeginDragDropSource())
		{
			SetDragDropPayload("transform", &aTransform, sizeof(Transform::Ptr));
			EndDragDropSource();
		}

		if (BeginDragDropTarget())
		{
			if (auto payload = AcceptDragDropPayload("transform"))
			{
				auto child = *reinterpret_cast<Transform::Ptr*>(payload->Data);
				child->SetParent(aTransform);
				invalidated = true;
			}
			EndDragDropTarget();
		}*/

		if (open)
		{
			for (entt::entity child : transform.GetChildren())
				stack.push_back(child);

			TreePop();
		}

		if (depth)
			Unindent(GetStyle().IndentSpacing * depth);
	}

	entt::entity selection = GetSelectedFront(aRegistry);

	if (IsKeyPressed(ImGuiKey_Delete) && aRegistry.all_of<Transform>(selection))
		aRegistry.get<Transform>(selection).Destroy(aRegistry);
}
