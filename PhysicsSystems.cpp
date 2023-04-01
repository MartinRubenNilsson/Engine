#include "pch.h"
#include "PhysicsSystems.h"
#include "CharacterController.h"
#include "Transform.h"

void Systems::MoveCharacterControllersUsingKeyboard(entt::registry& aRegistry)
{
	static constexpr float speed = 5.f;

	const float dt = ImGui::GetIO().DeltaTime;

	Vector3 dir{};
	dir.x -= ImGui::IsKeyDown(ImGuiKey_LeftArrow);
	dir.x += ImGui::IsKeyDown(ImGuiKey_RightArrow);
	dir.z -= ImGui::IsKeyDown(ImGuiKey_DownArrow);
	dir.z += ImGui::IsKeyDown(ImGuiKey_UpArrow);

	auto view = aRegistry.view<CharacterController, Transform>();
	for (auto [entity, controller, transform] : view.each())
	{
		controller.Move(dir * speed * dt, dt);
		transform.SetWorldPosition(aRegistry, controller.GetPosition());
	}
}
