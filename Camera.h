#pragma once

class Camera
{
public:
	Camera(const aiCamera&);

	void UseForDrawing(const Matrix& aTransform) const;

	Matrix GetProjection() const;

private:
	float myVerticalFov;
	float myAspectRatio;
	float myNearClipPlane;
	float myFarClipPlane;
};

