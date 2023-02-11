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

Camera::Camera(const PerspectiveCamera& aCamera)
	: myCamera{ aCamera }
{
}

Camera::Camera(const OrthographicCamera& aCamera)
	: myCamera{ aCamera }
{
}

Camera::Camera(const aiCamera& aCamera)
{
	if (aCamera.mOrthographicWidth == 0.f)
	{
		PerspectiveCamera camera{};
		camera.fovY = 2.f * atan(tan(aCamera.mHorizontalFOV) / aCamera.mAspect);
		camera.aspect = aCamera.mAspect;
		camera.nearZ = aCamera.mClipPlaneNear;
		camera.farZ = aCamera.mClipPlaneFar;

		*this = Camera(camera);
	}
	else
	{
		OrthographicCamera camera{};
		camera.width = 2.f * aCamera.mOrthographicWidth;
		camera.height = camera.width / aCamera.mAspect;;
		camera.nearZ = aCamera.mClipPlaneNear;
		camera.farZ = aCamera.mClipPlaneFar;

		*this = Camera(camera);
	}

	myLocalViewMatrix = XMMatrixLookToLH(
		{ aCamera.mPosition.x, aCamera.mPosition.y, aCamera.mPosition.z },
		XMVector3Normalize({ aCamera.mLookAt.x, aCamera.mLookAt.y, aCamera.mLookAt.z }),
		XMVector3Normalize({ aCamera.mUp.x, aCamera.mUp.y, aCamera.mUp.z })
	);
}

void Camera::SetCamera(const Matrix& aTransform) const
{
	CameraBuffer buffer{};
	//buffer.cameraMatrix = aTransform;
	buffer.worldToClipMatrix = aTransform.Invert() * myLocalViewMatrix * GetProjectionMatrix();

	DX11_WRITE_CONSTANT_BUFFER(buffer);
}

const Matrix& Camera::GetLocalViewMatrix() const
{
	return myLocalViewMatrix;
}

Matrix Camera::GetProjectionMatrix() const
{
	return IsPerspective() ?
		std::get<PerspectiveCamera>(myCamera).GetProjectionMatrix() :
		std::get<OrthographicCamera>(myCamera).GetProjectionMatrix();
}

void Camera::SetPerspective(const PerspectiveCamera& aCamera)
{
	myCamera = aCamera;
}

PerspectiveCamera Camera::GetPerspective() const
{
	return IsPerspective() ? std::get<PerspectiveCamera>(myCamera) : PerspectiveCamera{};
}

void Camera::SetOrthographic(const OrthographicCamera& aCamera)
{
	myCamera = aCamera;
}

OrthographicCamera Camera::GetOrthographic() const
{
	return IsOrthographic() ? std::get<OrthographicCamera>(myCamera) : OrthographicCamera{};
}

bool Camera::IsPerspective() const
{
	return std::holds_alternative<PerspectiveCamera>(myCamera);
}

bool Camera::IsOrthographic() const
{
	return std::holds_alternative<OrthographicCamera>(myCamera);
}

/*
* namespace ImGui
*/

void ImGui::CameraEdit(class Camera& aCamera)
{
	{
		int orthographic = aCamera.IsOrthographic();
		if (Combo("Projection", &orthographic, "Perspective\0Orthographic\0\0"))
			orthographic ? aCamera.SetOrthographic({}) : aCamera.SetPerspective({});
	}

	if (aCamera.IsPerspective())
	{
		auto camera = aCamera.GetPerspective();
		DragFloat("FoV", &camera.fovY, 0.01f);
		DragFloat("Aspect", &camera.aspect, 0.01f);
		DragFloat("Near Z", &camera.nearZ, 0.1f);
		DragFloat("Far Z", &camera.farZ);

		aCamera.SetPerspective(camera);
	}

	if (aCamera.IsOrthographic())
	{
		auto camera = aCamera.GetOrthographic();
		DragFloat("Width", &camera.width, 0.01f);
		DragFloat("Height", &camera.height, 0.01f);
		DragFloat("Near Z", &camera.nearZ, 0.1f);
		DragFloat("Far Z", &camera.farZ);

		aCamera.SetOrthographic(camera);
	}
}

void ImGui::DrawCubes(const Camera& aCamera, const Matrix& aCameraTransform, std::span<const Matrix> someCubeTransforms)
{
	if (someCubeTransforms.empty())
		return;

	Matrix view = aCameraTransform.Invert() * aCamera.GetLocalViewMatrix();
	Matrix proj = aCamera.GetProjectionMatrix();
	ImGuizmo::SetOrthographic(aCamera.IsOrthographic());
	ImGuizmo::DrawCubes(&view._11, &proj._11, &someCubeTransforms.front()._11, static_cast<int>(someCubeTransforms.size()));
}

void ImGui::DrawGrid(const Camera& aCamera, const Matrix& aCameraTransform, const Matrix& aGridTransform, float aGridSize)
{
	Matrix view = aCameraTransform.Invert() * aCamera.GetLocalViewMatrix();
	Matrix proj = aCamera.GetProjectionMatrix();
	ImGuizmo::SetOrthographic(aCamera.IsOrthographic());
	ImGuizmo::DrawGrid(&view._11, &proj._11, &aGridTransform._11, aGridSize);
}

bool ImGui::Manipulate(const Camera& aCamera, const Matrix& aCameraTransform, ImGuizmo::OPERATION anOperation, ImGuizmo::MODE aMode, Matrix& aTransform)
{
	Matrix view = aCameraTransform.Invert() * aCamera.GetLocalViewMatrix();
	Matrix proj = aCamera.GetProjectionMatrix();
	ImGuizmo::SetOrthographic(aCamera.IsOrthographic());
	return ImGuizmo::Manipulate(&view._11, &proj._11, anOperation, aMode, &aTransform._11);
}

void ImGui::ViewManipulate(const Camera& aCamera, Matrix& aCameraTransform, float aLength, ImVec2 aPosition, ImVec2 aSize, ImU32 aBackgroundColor)
{
	// https://github.com/CedricGuillemet/ImGuizmo/issues/107
	constexpr static Matrix inversionMatrix{ 1.f, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, -1.f, 0, 0, 0, 0, 1.f };
	Matrix view = inversionMatrix * aCameraTransform.Invert() * aCamera.GetLocalViewMatrix() * inversionMatrix;
	ImGuizmo::SetOrthographic(aCamera.IsOrthographic());
	ImGuizmo::ViewManipulate(&view._11, aLength, aPosition, aSize, aBackgroundColor);
	aCameraTransform = inversionMatrix * aCamera.GetLocalViewMatrix() * view.Invert() * inversionMatrix;
}
