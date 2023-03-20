#pragma once

void Select(entt::registry&, entt::entity); // Pass null handle to clear selection
entt::entity GetSelected(entt::registry&);

template <class Component>
void TryCopy(entt::handle dst, entt::const_handle src)
{
    if (auto component = src.try_get<Component>())
        dst.emplace<Component>(*component);
};

