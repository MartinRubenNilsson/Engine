#pragma once

entt::entity GetSelectedFront(entt::registry&);
void ClearSelection(entt::registry&);
void Select(entt::handle, bool aMultiSelect = false);
bool IsSelected(entt::handle);
