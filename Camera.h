#pragma once

class Camera
{
public:
	Camera(const aiCamera&);

	void SetCamera(const Matrix& aTransform) const;

	Matrix GetProjectionMatrix() const;

	bool IsPerspective() const { return myOrthographicWidth == 0.f; }

private:
	Matrix GetPerspectiveMatrix() const;
	Matrix GetOrthographicMatrix() const;

	Matrix myLocalViewMatrix;
	float myVerticalFov;
	float myAspectRatio;
	float myNearClipPlane;
	float myFarClipPlane;
	float myOrthographicWidth;
};

