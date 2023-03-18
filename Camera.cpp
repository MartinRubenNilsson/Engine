#include "pch.h"
#include "Camera.h"

Matrix GetDefaultViewMatrix()
{
	return XMMatrixLookToLH(Vector3::Zero, Vector3::UnitZ, Vector3::UnitY);
}

/*
* class Camera
*/

Camera::Camera(const aiCamera& aCamera)
	: myAspect{ aCamera.mAspect }
	, myNearZ{ aCamera.mClipPlaneNear }
	, myFarZ{ aCamera.mClipPlaneFar }
{
	if (aCamera.mOrthographicWidth == 0.f)
	{
		myType = CameraType::Perspective;
		myFovOrHeight = 2.f * atan(tan(aCamera.mHorizontalFOV) / aCamera.mAspect);
	}
	else
	{
		myType = CameraType::Orthographic;
		myFovOrHeight = 2.f * aCamera.mOrthographicWidth / aCamera.mAspect;
	}
}

Matrix Camera::GetProjMatrix(bool aReverseZ) const
{
	float nearZ = myNearZ;
	float farZ = myFarZ;

	if (aReverseZ)
		std::swap(nearZ, farZ);

	return myType == CameraType::Perspective ?
		XMMatrixPerspectiveFovLH(myFovOrHeight, myAspect, nearZ, farZ) :
		XMMatrixOrthographicLH(myFovOrHeight, myAspect * myFovOrHeight, nearZ, farZ);
}

void Camera::SetAspect(float anAspect)
{
	myAspect = std::max(0.001f, anAspect);
}

void Camera::SetNearZ(float z)
{
	myNearZ = std::clamp(z, MIN_CLIP_Z, MAX_CLIP_Z);
	myFarZ = std::max(myFarZ, myNearZ + 0.1f);
}

void Camera::SetFarZ(float z)
{
	myFarZ = std::clamp(z, MIN_CLIP_Z, MAX_CLIP_Z);
	myNearZ = std::min(myNearZ, myFarZ - 0.1f);
}

void from_json(const json& j, Camera& c)
{
	j.at("type").get_to(c.myType);
	j.at("fovOrHeight").get_to(c.myFovOrHeight);
	j.at("aspect").get_to(c.myAspect);
	j.at("nearZ").get_to(c.myNearZ);
	j.at("farZ").get_to(c.myFarZ);
}

void to_json(json& j, const Camera& c)
{
	j["type"] = c.myType;
	j["fovOrHeight"] = c.myFovOrHeight;
	j["aspect"] = c.myAspect;
	j["nearZ"] = c.myNearZ;
	j["farZ"] = c.myFarZ;
}

/*
* namespace ImGui
*/

void ImGui::Inspect(Camera& aCamera)
{
	CameraType type = aCamera.GetType();
	if (Combo("Type", reinterpret_cast<int*>(&type), "Perspective\0Orthographic\0\0"))
		aCamera.SetType(type);

	// todo: fov or height
	Checkbox("Use Screen Aspect", &aCamera.useScreenAspect);

	if (!aCamera.useScreenAspect)
	{
		float aspect = aCamera.GetAspect();
		DragFloat("Aspect", &aspect, 0.1f, 0.001f, FLT_MAX);
		aCamera.SetAspect(aspect);
	}

	{
		float z = aCamera.GetNearZ();
		DragFloat("Near Z", &z, 1.f, MIN_CLIP_Z, MAX_CLIP_Z);
		aCamera.SetNearZ(z);
	}

	{
		float z = aCamera.GetFarZ();
		DragFloat("Far Z", &z, 1.f, MIN_CLIP_Z, MAX_CLIP_Z);
		aCamera.SetFarZ(z);
	}
}

void ImGui::DrawCubes(const Camera& aCamera, const Matrix& aCameraTransform, std::span<const Matrix> someCubeTransforms)
{
	if (someCubeTransforms.empty())
		return;

	Matrix view = aCameraTransform.Invert() * GetDefaultViewMatrix();
	Matrix proj = aCamera.GetProjMatrix();
	ImGuizmo::SetOrthographic(aCamera.GetType() == CameraType::Orthographic);
	ImGuizmo::DrawCubes(&view._11, &proj._11, &someCubeTransforms.front()._11, static_cast<int>(someCubeTransforms.size()));
}

void ImGui::DrawGrid(const Camera& aCamera, const Matrix& aCameraTransform, const Matrix& aGridTransform, float aGridSize)
{
	Matrix view = aCameraTransform.Invert() * GetDefaultViewMatrix();
	Matrix proj = aCamera.GetProjMatrix();
	ImGuizmo::SetOrthographic(aCamera.GetType() == CameraType::Orthographic);
	ImGuizmo::DrawGrid(&view._11, &proj._11, &aGridTransform._11, aGridSize);
}

void ImGui::ViewManipulate(const Camera& aCamera, Matrix& aCameraTransform, float aLength, ImVec2 aPosition, ImVec2 aSize, ImU32 aBackgroundColor)
{
	// https://github.com/CedricGuillemet/ImGuizmo/issues/107
	constexpr static Matrix inversionMatrix{ 1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, -1.f, 0, 0, 0, 0, 1.f };
	Matrix view = inversionMatrix * aCameraTransform.Invert() * GetDefaultViewMatrix() * inversionMatrix;
	ImGuizmo::SetOrthographic(aCamera.GetType() == CameraType::Orthographic);
	ImGuizmo::ViewManipulate(&view._11, aLength, aPosition, aSize, aBackgroundColor);
	aCameraTransform = inversionMatrix * GetDefaultViewMatrix() * view.Invert() * inversionMatrix;
}
