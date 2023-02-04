#include "pch.h"
#include "imgui_simplemath.h"

using namespace DirectX;

IMGUI_API bool ImGui::DragVector2(const char* label, Vector2* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	return DragFloat2(label, reinterpret_cast<float*>(v), v_speed, v_min, v_max, format, flags);
}

IMGUI_API bool ImGui::DragVector3(const char* label, Vector3* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	return DragFloat3(label, reinterpret_cast<float*>(v), v_speed, v_min, v_max, format, flags);
}

IMGUI_API bool ImGui::DragVector4(const char* label, Vector4* v, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	return DragFloat4(label, reinterpret_cast<float*>(v), v_speed, v_min, v_max, format, flags);
}
