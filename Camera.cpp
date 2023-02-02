#include "pch.h"
#include "Camera.h"

using namespace DirectX;

namespace
{
	XMVECTOR AiToXm(const aiVector3D& aiVector)
	{
		XMFLOAT3 xmVector{ aiVector.x, aiVector.y, aiVector.z };
		return XMLoadFloat3(&xmVector);
	}

	XMMATRIX LookTo(const aiVector3D& position, const aiVector3D& direction, const aiVector3D& up)
	{
		return XMMatrixLookToLH(
			AiToXm(position),
			XMVector3Normalize(AiToXm(direction)),
			XMVector3Normalize(AiToXm(up))
		);
	}
}

Camera::Camera(const aiCamera& aCamera)
	: myLocalViewMatrix{ LookTo(aCamera.mPosition, aCamera.mLookAt, aCamera.mUp) }
	, myVerticalFov{ 2.f * atan(tan(aCamera.mHorizontalFOV) / aCamera.mAspect) }
	, myOrthographicHeight{ 2.f * aCamera.mOrthographicWidth / aCamera.mAspect }
	, myAspectRatio{ aCamera.mAspect }
	, myNearClipPlane{ aCamera.mClipPlaneNear }
	, myFarClipPlane{ aCamera.mClipPlaneFar }
{
}

void Camera::SetCamera(const Matrix& aTransform) const
{
	CameraBuffer buffer{};
	//buffer.cameraMatrix = aTransform;
	buffer.worldToClipMatrix = aTransform.Invert() * myLocalViewMatrix * GetProjectionMatrix();

	DX11_WRITE_CONSTANT_BUFFER(buffer);
}

void Camera::ToPerspective(float aDepth)
{
	if (!IsPerspective())
	{
		myVerticalFov = 2.f * atan(myOrthographicHeight / (aDepth * 2.f));
		myOrthographicHeight = 0.f;
	}
}

void Camera::ToOrthographic(float aDepth)
{
	if (IsPerspective())
	{
		myOrthographicHeight = aDepth * 2.f * tan(myVerticalFov / 2.f);
		myVerticalFov = 0.f;
	}
}

Matrix Camera::GetProjectionMatrix() const
{
	return IsPerspective() ? GetPerspectiveMatrix() : GetOrthographicMatrix();
}

void Camera::SetVerticalFov(float aFov)
{
	if (IsPerspective())
		myVerticalFov = std::clamp(aFov, 0.01f, XM_PI - 0.01f);
}

void Camera::SetOrthograpicHeight(float aHeight)
{
	if (!IsPerspective())
		myOrthographicHeight = std::max(aHeight, 0.01f);
}

void Camera::SetAspectRatio(float aRatio)
{
	myAspectRatio = std::max(aRatio, 0.01f);
}

void Camera::SetClipPlanes(float aNear, float aFar)
{
	myNearClipPlane = std::max(aNear, 0.01f);
	myFarClipPlane = std::max(aFar, aNear + 0.01f);
}

void Camera::GetClipPlanes(float& aNear, float& aFar) const
{
	aNear = myNearClipPlane;
	aFar = myFarClipPlane;
}

Matrix Camera::GetPerspectiveMatrix() const
{
	return XMMatrixPerspectiveFovLH(myVerticalFov, myAspectRatio, myNearClipPlane, myFarClipPlane);
}

Matrix Camera::GetOrthographicMatrix() const
{
	return XMMatrixOrthographicLH(myOrthographicHeight, myOrthographicHeight / myAspectRatio, myNearClipPlane, myFarClipPlane);
}

void ImGui::CameraEdit(Camera* aCamera)
{
	if (RadioButton("Perspective", aCamera->IsPerspective()))
		aCamera->ToPerspective();
	SameLine();
	if (RadioButton("Orthographic", !aCamera->IsPerspective()))
		aCamera->ToOrthographic();

	if (aCamera->IsPerspective())
	{
		float fov{ aCamera->GetVerticalFov() };
		DragFloat("Vertical FoV", &fov, 0.01f);
		aCamera->SetVerticalFov(fov);
	}
	else
	{
		float height{ aCamera->GetOrthographicHeight() };
		DragFloat("Ortho height", &height, 0.1f);
		aCamera->SetOrthograpicHeight(height);
	}

	float aspectRatio{ aCamera->GetAspectRatio() };
	DragFloat("Aspect ratio", &aspectRatio, 0.01f);
	aCamera->SetAspectRatio(aspectRatio);

	float nearPlane, farPlane;
	aCamera->GetClipPlanes(nearPlane, farPlane);
	DragFloat("Near plane", &nearPlane, 0.1f);
	DragFloat("Far plane", &farPlane);
	aCamera->SetClipPlanes(nearPlane, farPlane);
}
