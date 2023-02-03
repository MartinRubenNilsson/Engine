#pragma once

class Camera
{
public:
	Camera(const aiCamera&);

	void SetCamera(const Matrix& aTransform) const;

	void ToPerspective(float aDepth = 10.f);
	void ToOrthographic(float aDepth = 10.f);

	Matrix GetProjectionMatrix() const;

	void SetVerticalFov(float aFov);
	float GetVerticalFov() const { return myVerticalFov; }
	void SetOrthograpicHeight(float aHeight);
	float GetOrthographicHeight() const { return myOrthographicHeight; }
	void SetAspectRatio(float aRatio);
	float GetAspectRatio() const { return myAspectRatio; }
	void SetClipPlanes(float aNear, float aFar);
	void GetClipPlanes(float& aNear, float& aFar) const;

	bool IsPerspective() const { return myOrthographicHeight == 0.f; }

private:
	Matrix GetPerspectiveMatrix() const;
	Matrix GetOrthographicMatrix() const;

	Matrix myLocalViewMatrix;
	float myVerticalFov;
	float myOrthographicHeight;
	float myAspectRatio;
	float myNearClipPlane;
	float myFarClipPlane;
};

namespace ImGui
{
	void CameraEdit(Camera* aCamera);
}

