#include "pch.h"
#include "Camera.h"

using namespace DirectX;

Matrix PerspectiveCamera::GetProjectionMatrix() const
{
	return XMMatrixPerspectiveFovLH(fovY, aspect, nearZ, farZ);
}

Matrix OrthographicCamera::GetProjectionMatrix() const
{
	return XMMatrixOrthographicLH(width, height, nearZ, farZ);
}

/*
* class Camera
*/

Camera::Camera(const aiCamera& aCamera)
{
	myViewMatrix = XMMatrixLookToLH(
		{ aCamera.mPosition.x, aCamera.mPosition.y, aCamera.mPosition.z },
		XMVector3Normalize({ aCamera.mLookAt.x, aCamera.mLookAt.y, aCamera.mLookAt.z }),
		XMVector3Normalize({ aCamera.mUp.x, aCamera.mUp.y, aCamera.mUp.z })
	);

	if (aCamera.mOrthographicWidth == 0.f)
	{
		PerspectiveCamera camera{};
		camera.fovY = 2.f * atan(tan(aCamera.mHorizontalFOV) / aCamera.mAspect);
		camera.aspect = aCamera.mAspect;
		camera.nearZ = aCamera.mClipPlaneNear;
		camera.farZ = aCamera.mClipPlaneFar;

		SetPerspective(camera);
	}
	else
	{
		OrthographicCamera camera{};
		camera.width = 2.f * aCamera.mOrthographicWidth;
		camera.height = camera.width / aCamera.mAspect;;
		camera.nearZ = aCamera.mClipPlaneNear;
		camera.farZ = aCamera.mClipPlaneFar;

		SetOrthographic(camera);
	}
}

CameraType Camera::GetType() const
{
	return static_cast<CameraType>(myCamera.index());
}

const Matrix& Camera::GetViewMatrix() const
{
	return myViewMatrix;
}

Matrix Camera::GetProjectionMatrix() const
{
	return GetType() == CameraType::Perspective ?
		std::get<PerspectiveCamera>(myCamera).GetProjectionMatrix() :
		std::get<OrthographicCamera>(myCamera).GetProjectionMatrix();
}

void Camera::SetPerspective(const PerspectiveCamera& aCamera)
{
	// todo: validae values
	myCamera = aCamera;
}

void Camera::SetOrthographic(const OrthographicCamera& aCamera)
{
	// todo: validae values
	myCamera = aCamera;
}

/*
* namespace ImGui
*/

void ImGui::InspectCamera(class Camera& aCamera)
{
	int type{ static_cast<int>(aCamera.GetType()) };
	if (Combo("Type", &type, "Perspective\0Orthographic\0\0"))
		type ? aCamera.SetOrthographic({}) : aCamera.SetPerspective({});

	switch (aCamera.GetType())
	{
	case CameraType::Perspective:
	{
		auto& camera{ aCamera.Get<PerspectiveCamera>() };
		DragFloat("FoV", &camera.fovY, 0.01f);
		DragFloat("Aspect", &camera.aspect, 0.01f);
		DragFloat("Near Z", &camera.nearZ, 0.1f);
		DragFloat("Far Z", &camera.farZ);
		break;
	}
	case CameraType::Orthographic:
	{
		auto& camera{ aCamera.Get<OrthographicCamera>() };
		DragFloat("Width", &camera.width, 0.01f);
		DragFloat("Height", &camera.height, 0.01f);
		DragFloat("Near Z", &camera.nearZ, 0.1f);
		DragFloat("Far Z", &camera.farZ);
		break;
	}
	}
}

void ImGui::DrawCubes(const Camera& aCamera, const Matrix& aCameraTransform, std::span<const Matrix> someCubeTransforms)
{
	if (someCubeTransforms.empty())
		return;

	Matrix view = aCameraTransform.Invert() * aCamera.GetViewMatrix();
	Matrix proj = aCamera.GetProjectionMatrix();
	ImGuizmo::SetOrthographic(aCamera.GetType() == CameraType::Orthographic);
	ImGuizmo::DrawCubes(&view._11, &proj._11, &someCubeTransforms.front()._11, static_cast<int>(someCubeTransforms.size()));
}

void ImGui::DrawGrid(const Camera& aCamera, const Matrix& aCameraTransform, const Matrix& aGridTransform, float aGridSize)
{
	Matrix view = aCameraTransform.Invert() * aCamera.GetViewMatrix();
	Matrix proj = aCamera.GetProjectionMatrix();
	ImGuizmo::SetOrthographic(aCamera.GetType() == CameraType::Orthographic);
	ImGuizmo::DrawGrid(&view._11, &proj._11, &aGridTransform._11, aGridSize);
}

bool ImGui::Manipulate(const Camera& aCamera, const Matrix& aCameraTransform, ImGuizmo::OPERATION anOperation, ImGuizmo::MODE aMode, Matrix& aTransform)
{
	Matrix view = aCameraTransform.Invert() * aCamera.GetViewMatrix();
	Matrix proj = aCamera.GetProjectionMatrix();
	ImGuizmo::SetOrthographic(aCamera.GetType() == CameraType::Orthographic);
	return ImGuizmo::Manipulate(&view._11, &proj._11, anOperation, aMode, &aTransform._11);
}

void ImGui::ViewManipulate(const Camera& aCamera, Matrix& aCameraTransform, float aLength, ImVec2 aPosition, ImVec2 aSize, ImU32 aBackgroundColor)
{
	// https://github.com/CedricGuillemet/ImGuizmo/issues/107
	constexpr static Matrix inversionMatrix{ 1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, -1.f, 0, 0, 0, 0, 1.f };
	Matrix view = inversionMatrix * aCameraTransform.Invert() * aCamera.GetViewMatrix() * inversionMatrix;
	ImGuizmo::SetOrthographic(aCamera.GetType() == CameraType::Orthographic);
	ImGuizmo::ViewManipulate(&view._11, aLength, aPosition, aSize, aBackgroundColor);
	aCameraTransform = inversionMatrix * aCamera.GetViewMatrix() * view.Invert() * inversionMatrix;
}
