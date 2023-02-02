#include "pch.h"
#include "Camera.h"

using namespace DirectX;

Camera::Camera(const aiCamera& aCamera)
	: myVerticalFov{ XM_PIDIV2 }
	, myAspectRatio{ 1.f }
	, myNearClipPlane{ aCamera.mClipPlaneNear }
	, myFarClipPlane{ aCamera.mClipPlaneFar }
{
	if (aCamera.mAspect > 0.f)
	{
		myVerticalFov = 2.f * atan(tan(aCamera.mHorizontalFOV) / aCamera.mAspect);
		myAspectRatio = aCamera.mAspect;
	}
}

void Camera::UseForDrawing(const Matrix& aTransform) const
{
	CameraBuffer buffer{};
	buffer.cameraMatrix = aTransform;
	buffer.worldToClipMatrix = aTransform.Invert() * GetProjectionMatrix();

	DX11_WRITE_CONSTANT_BUFFER(buffer);
}

Matrix Camera::GetProjectionMatrix() const
{
	return XMMatrixPerspectiveFovLH(myVerticalFov, myAspectRatio, myNearClipPlane, myFarClipPlane);
}
