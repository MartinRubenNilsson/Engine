#include "pch.h"
#include "EnttCommon.h"
#include "Tags.h"

void Select(entt::registry& aRegistry, entt::entity anEntity)
{
	auto selection = aRegistry.view<Tag::Selected>();
	aRegistry.erase<Tag::Selected>(selection.begin(), selection.end());

	if (aRegistry.valid(anEntity))
		aRegistry.emplace_or_replace<Tag::Selected>(anEntity);
}

entt::entity GetSelected(entt::registry& aRegistry)
{
	return aRegistry.view<Tag::Selected>().front();
}
