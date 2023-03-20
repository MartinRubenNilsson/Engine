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

		Transform& transform = aRegistry.get<Transform>(entity);
		const float indent = GetStyle().IndentSpacing * transform.GetDepth(aRegistry);

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
		if (!transform.HasChildren())
			flags |= ImGuiTreeNodeFlags_Leaf;
		if (IsSelected({ aRegistry, entity }))
			flags |= ImGuiTreeNodeFlags_Selected;

		std::string label{ ICON_FA_CUBE" " };
		label += transform.GetName();
		label += "##";
		label += std::to_string(std::to_underlying(entity));

		if (indent > 0.f)
			Indent(indent);

		const bool open = TreeNodeEx(label.c_str(), flags);

		if (IsItemClicked() && !IsItemToggledOpen())
			Select({ aRegistry, entity });

		if (BeginPopupContextItem(label.c_str()))
		{
			if (Selectable("Duplicate"))
			{
				Transform& copy = transform.Duplicate(aRegistry, transform.GetParent());
				Select({ aRegistry, copy.GetEntity() });
				dirty = true;
			}
			if (Selectable("Delete"))
			{
				transform.Destroy(aRegistry);
				dirty = true;
			}
			Separator();
			if (Selectable("Create Child"))
			{
				Transform& child = transform.CreateChild(aRegistry);
				Select({ aRegistry, child.GetEntity() });
				dirty = true;
			}
			EndPopup();
		}

		if (BeginDragDropSource())
		{
			SetDragDropPayload("entity", &entity, sizeof(entt::entity));
			Text(transform.GetName().data());
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

	if (IsKeyDown(ImGuiKey_Delete))
	{
		if (auto transform = aRegistry.try_get<Transform>(GetSelectedFront(aRegistry)))
			transform->Destroy(aRegistry);
	}
}
