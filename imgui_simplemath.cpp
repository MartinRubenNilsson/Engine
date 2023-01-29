#include "pch.h"
#include "imgui_simplemath.h"

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

IMGUI_API bool ImGui::DragEulerDegrees(const char* label, Quaternion* q, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	Vector3 euler{ q->ToEuler() * 180.f / XM_PI };
	const bool changed = DragVector3(label, &euler, v_speed, v_min, v_max, format, flags);
	if (changed)
		*q = Quaternion::CreateFromYawPitchRoll(euler * XM_PI / 180.f);
	return changed;
}

IMGUI_API bool ImGui::DragTransform(Matrix* m, float v_speed, float v_min, float v_max, const char* format, ImGuiSliderFlags flags)
{
	Vector3 scale{};
	Quaternion rotation{};
	Vector3 translation{};
	m->Decompose(scale, rotation, translation);
	const bool translated = DragVector3("Translation", &translation, v_speed, v_min, v_max, format, flags);
	const bool rotated = DragEulerDegrees("Rotation", &rotation, v_speed, v_min, v_max, format, flags);
	const bool scaled = DragVector3("Scale", &scale, v_speed, v_min, v_max, format, flags);
	const bool changed = translated || rotated || scaled;
	if (changed)
		*m = Matrix::CreateScale(scale) * Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(translation);
	return changed;
}
