#include "pch.h"
#include "Camera.h"
#include "ConstantBuffer.h"
#include "ShaderCommon.h"

namespace
{
	struct CameraData
	{
		Matrix cameraMatrix{};
		Matrix worldToClipMatrix{};
	} ourCameraData{};

	std::unique_ptr<ConstantBuffer> ourCameraBuffer{};
}

Camera::Camera()
{
	CreatePerspectiveFov();
}

void Camera::UseForDrawing(const Matrix& aTransform) const
{
	if (!ourCameraBuffer)
	{
		ourCameraBuffer = std::make_unique<ConstantBuffer>(sizeof(CameraData));
		assert(*ourCameraBuffer);
		ourCameraBuffer->VSSetConstantBuffer(CBUFFER_SLOT_CAMERA);
		ourCameraBuffer->PSSetConstantBuffer(CBUFFER_SLOT_CAMERA);
	}

	ourCameraData.cameraMatrix = aTransform;
	ourCameraData.worldToClipMatrix = aTransform.Invert() * myProjection;

	ourCameraBuffer->WriteConstantBuffer(&ourCameraData);
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
