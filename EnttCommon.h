#pragma once

template <class Component>
void TryCopy(entt::handle dst, entt::const_handle src)
{
    if (auto component = src.try_get<Component>())
        dst.emplace<Component>(*component);
};

float GetDeltaTime(const entt::registry&);

void Select(entt::registry&, entt::entity); // Pass null handle to clear selection
entt::entity GetSelected(entt::registry&);

entt::entity CreateEmpty(entt::registry&);
entt::entity CreatePlane(entt::registry&);
entt::entity CreateCube(entt::registry&);
entt::entity CreateSphere(entt::registry&);
entt::entity CreateCylinder(entt::registry&);
entt::entity CreateCone(entt::registry&);
entt::entity CreateTorus(entt::registry&);
entt::entity CreateSuzanne(entt::registry&);

// Sets world position of transform, character controller, etc.
void SetWorldPosition(entt::registry&, entt::entity, const Vector3& aPos);

// Moves character controller, then sets transform's world position to controller's new position
void Move(entt::registry&, entt::entity, const Vector3& aDeltaPos);
