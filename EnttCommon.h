#pragma once

entt::entity GetSelectedFront(entt::registry&);
void ClearSelection(entt::registry&);
void Select(entt::handle, bool aMultiSelect = false); // Passing null handle is ok
bool IsSelected(entt::handle);
