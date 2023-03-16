#include "pch.h"
#include "EnttCommon.h"
#include "Tags.h"

entt::entity GetSelectedFront(entt::registry& aRegistry)
{
	return aRegistry.view<Tag::Selected>().front();
}

void ClearSelection(entt::registry& aRegistry)
{
	auto selection = aRegistry.view<Tag::Selected>();
	aRegistry.erase<Tag::Selected>(selection.begin(), selection.end());
}

void Select(entt::handle aHandle, bool aMultiSelect)
{
	if (!aMultiSelect)
		ClearSelection(*aHandle.registry());
	if (aHandle)
		aHandle.emplace_or_replace<Tag::Selected>();
}

bool IsSelected(entt::handle aHandle)
{
	return aHandle.all_of<Tag::Selected>();
}
