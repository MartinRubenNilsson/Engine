#include "pch.h"
#include "Camera.h"

namespace
{
	struct ProjectionGetter
	{
		float nearZ{}, farZ{};

		Matrix operator()(const PerspectiveCamera& aCamera)
		{
			return XMMatrixPerspectiveFovLH(aCamera.fovY, aCamera.aspect, nearZ, farZ);
		}

		Matrix operator()(const OrthographicCamera& aCamera)
		{
			return XMMatrixOrthographicLH(aCamera.width, aCamera.height, nearZ, farZ);
		}
	};
}

/*
* class Camera
*/

Camera::Camera(const aiCamera& aCamera)
	: myNearZ{ aCamera.mClipPlaneNear }
	, myFarZ{ aCamera.mClipPlaneFar }
{
	std::memcpy(&myPosition, &aCamera.mPosition, sizeof(Vector3));
	std::memcpy(&myDirection, &aCamera.mLookAt, sizeof(Vector3));
	std::memcpy(&myUp, &aCamera.mUp, sizeof(Vector3));

	myDirection.Normalize();
	myUp.Normalize();

	if (aCamera.mOrthographicWidth == 0.f)
	{
		PerspectiveCamera camera{};
		camera.fovY = 2.f * atan(tan(aCamera.mHorizontalFOV) / aCamera.mAspect);
		camera.aspect = aCamera.mAspect;

		SetPerspective(camera);
	}
	else
	{
		OrthographicCamera camera{};
		camera.width = 2.f * aCamera.mOrthographicWidth;
		camera.height = camera.width / aCamera.mAspect;

		SetOrthographic(camera);
	}
}

CameraType Camera::GetType() const
{
	return static_cast<CameraType>(myCamera.index());
}

Matrix Camera::GetViewMatrix() const
{
	return XMMatrixLookToLH(myPosition, myDirection, myUp);
}

Matrix Camera::GetProjectionMatrix(bool aReverseZ) const
{
	ProjectionGetter getter{ myNearZ, myFarZ };
	if (aReverseZ)
		std::swap(getter.nearZ, getter.farZ);
	return std::visit(getter, myCamera);
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

namespace ImGui
{
	struct CameraInspector
	{
		void operator()(PerspectiveCamera& aCamera)
		{
			DragFloat("FoV", &aCamera.fovY, 0.01f);
			DragFloat("Aspect", &aCamera.aspect, 0.01f);
		}

		void operator()(OrthographicCamera& aCamera)
		{
			DragFloat("Width", &aCamera.width, 0.01f);
			DragFloat("Height", &aCamera.height, 0.01f);
		}
	};
}

void ImGui::InspectCamera(Camera& aCamera)
{
	int type{ static_cast<int>(aCamera.GetType()) };
	if (Combo("Type", &type, "Perspective\0Orthographic\0\0"))
		type ? aCamera.SetOrthographic({}) : aCamera.SetPerspective({});

	//DragFloat("Near Z", &aCamera.nearZ, 0.1f);
	//DragFloat("Far Z", &aCamera.farZ);
	std::visit(CameraInspector{}, aCamera.myCamera);
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
