#include "pch.h"
#include "Camera.h"

Camera::Camera()
{
	CreatePerspectiveFov();
}

void Camera::UseForDrawing(const Matrix& aTransform) const
{
	CameraBuffer buffer{};
	buffer.cameraMatrix = aTransform;
	buffer.worldToClipMatrix = aTransform.Invert() * myProjection;

	DX11_WRITE_CBUFFER(Camera, &buffer);
}

void Camera::CreatePerspectiveFov(float aFovAngleY, float anAspectRatio, float aNearZ, float aFarZ)
{
	myProjection = XMMatrixPerspectiveFovLH(aFovAngleY, anAspectRatio, aNearZ, aFarZ);
	myFovAngleY = aFovAngleY;
	myAspectRatio = anAspectRatio;
	myNearZ = aNearZ;
	myFarZ = aFarZ;
	myOrthographic = false;
}
