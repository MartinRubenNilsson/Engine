#include "pch.h"
#include "PhysicsSystems.h"
#include "CharacterController.h"

void Systems::MoveCharacterControllersUsingKeyboard(entt::registry& aRegistry)
{
	static constexpr float speed = 5.f;

	const float dt = aRegistry.ctx().get<float>(DELTA_TIME);

	Vector3 dir{};
	dir.x -= ImGui::IsKeyDown(ImGuiKey_LeftArrow);
	dir.x += ImGui::IsKeyDown(ImGuiKey_RightArrow);
	dir.z -= ImGui::IsKeyDown(ImGuiKey_DownArrow);
	dir.z += ImGui::IsKeyDown(ImGuiKey_UpArrow);

	for (auto [entity, controller] : aRegistry.view<CharacterController>().each())
	{
		controller.Move(dir * speed * dt, aRegistry);
	}
}
