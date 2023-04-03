#include "pch.h"
#include "PhysicsSystems.h"
#include "EnttCommon.h"

// Components
#include "Transform.h"
#include "Rigidbody.h"
#include "CharacterController.h"

void Systems::UpdateTransformsOfRigidbodies(entt::registry& aRegistry)
{
	auto view = aRegistry.view<Transform, const Rigidbody>();
	for (auto [entity, transform, rigidbody] : view.each())
	{
		rigidbody.GetTransform(transform.position, transform.rotation);
	}
}

void Systems::MoveCharacterControllersUsingKeyboard(entt::registry& aRegistry)
{
	// Todo: use state of keyboard instead of imgui

	Vector3 dir{};
	dir.x -= ImGui::IsKeyDown(ImGuiKey_LeftArrow);
	dir.x += ImGui::IsKeyDown(ImGuiKey_RightArrow);
	dir.z -= ImGui::IsKeyDown(ImGuiKey_DownArrow);
	dir.z += ImGui::IsKeyDown(ImGuiKey_UpArrow);

	Vector3 deltaPos = dir * 5.f * GetDeltaTime(aRegistry);

	for (entt::entity entity : aRegistry.view<CharacterController>())
	{
		Move(aRegistry, entity, deltaPos);
	}
}
