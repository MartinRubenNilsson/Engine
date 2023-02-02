#include "pch.h"
#include "Camera.h"

namespace
{
	DirectX::XMVECTOR AiToXm(const aiVector3D& aiVector)
	{
		DirectX::XMFLOAT3 xmVector{ aiVector.x, aiVector.y, aiVector.z };
		return DirectX::XMLoadFloat3(&xmVector);
	}

	DirectX::XMMATRIX LookTo(const aiVector3D& position, const aiVector3D& direction, const aiVector3D& up)
	{
		return DirectX::XMMatrixLookToLH(AiToXm(position), AiToXm(direction), AiToXm(up));
	}
}

Camera::Camera(const aiCamera& aCamera)
	: myLocalViewMatrix{ LookTo(aCamera.mPosition, aCamera.mLookAt, aCamera.mUp) }
	, myVerticalFov{ 2.f * atan(tan(aCamera.mHorizontalFOV) / aCamera.mAspect) }
	, myAspectRatio{ aCamera.mAspect }
	, myNearClipPlane{ aCamera.mClipPlaneNear }
	, myFarClipPlane{ aCamera.mClipPlaneFar }
	, myOrthographicWidth{ 2.f * aCamera.mOrthographicWidth }
{
}

void Camera::SetCamera(const Matrix& aTransform) const
{
	CameraBuffer buffer{};
	//buffer.cameraMatrix = aTransform;
	buffer.worldToClipMatrix = aTransform.Invert() * myLocalViewMatrix * GetProjectionMatrix();

	DX11_WRITE_CONSTANT_BUFFER(buffer);
}

Matrix Camera::GetProjectionMatrix() const
{
	return IsPerspective() ? GetPerspectiveMatrix() : GetOrthographicMatrix();
}

Matrix Camera::GetPerspectiveMatrix() const
{
	return DirectX::XMMatrixPerspectiveFovLH(myVerticalFov, myAspectRatio, myNearClipPlane, myFarClipPlane);
}

Matrix Camera::GetOrthographicMatrix() const
{
	return DirectX::XMMatrixOrthographicLH(myOrthographicWidth, myOrthographicWidth / myAspectRatio, myNearClipPlane, myFarClipPlane);
}
