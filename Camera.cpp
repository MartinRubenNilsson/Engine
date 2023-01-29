#include "pch.h"
#include "Camera.h"
#include "ConstantBuffer.h"

#define CBUFFER_SLOT_CAMERA 0

namespace
{
	struct CameraData
	{
		Matrix worldToClipMatrix;
		Vector4 cameraPosition;
	};

	size_t ourCameraCount{ 0 };
	std::unique_ptr<ConstantBuffer> ourCameraBuffer{};

	void CreateCamera()
	{
		if (ourCameraCount++ == 0)
		{
			ourCameraBuffer = std::make_unique<ConstantBuffer>(sizeof(CameraData));
			ourCameraBuffer->VSSetConstantBuffer(CBUFFER_SLOT_CAMERA);
			ourCameraBuffer->PSSetConstantBuffer(CBUFFER_SLOT_CAMERA);
		}
	}

	void DestroyCamera()
	{
		assert(ourCameraCount > 0);
		if (ourCameraCount-- == 0)
			ourCameraBuffer = nullptr;
	}
}

Camera::Camera()
{
	CreateCamera();
	SetPerspectiveFov();
}

Camera::Camera(const Camera& aCamera)
	: myProjection(aCamera.myProjection)
	, myOrthographic(aCamera.myOrthographic)
{
	CreateCamera();
}

Camera& Camera::operator=(Camera aCamera)
{
	swap(*this, aCamera);
	return *this;
}

Camera::~Camera()
{
	DestroyCamera();
}

void Camera::WriteCamera(const Matrix& aTransform) const
{
	const Vector3 pos = aTransform.Translation();

	CameraData data{};
	data.worldToClipMatrix = aTransform.Invert() * myProjection;
	data.cameraPosition = { pos.x, pos.y, pos.z, 1.f };

	ourCameraBuffer->WriteConstantBuffer(&data);
}

void Camera::SetPerspectiveFov(float aFovAngleY, float anAspectRatio, float aNearZ, float aFarZ)
{
	myProjection = XMMatrixPerspectiveFovLH(aFovAngleY, anAspectRatio, aNearZ, aFarZ);
	myOrthographic = false;
}

void swap(Camera& aFirst, Camera& aSecond)
{
	using std::swap;
	swap(aFirst.myProjection, aSecond.myProjection);
	swap(aFirst.myOrthographic, aSecond.myOrthographic);
}
