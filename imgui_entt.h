#pragma once

namespace ImGui
{
	void Inspector(entt::handle);
	void Hierarchy(entt::registry&, entt::entity& aSelection);
}