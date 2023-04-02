#include "pch.h"
#include "PhysicsSystems.h"
#include "CharacterController.h"
#include "EnttCommon.h"

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
