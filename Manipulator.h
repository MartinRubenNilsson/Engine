#pragma once
#include "Camera.h"

namespace ImGui
{
	using namespace ImGuizmo;

	void Shortcut(OPERATION&);
	void Shortcut(MODE&);
	void Manipulator(entt::registry&, const Camera&, const Matrix&, OPERATION&, MODE&);
}

