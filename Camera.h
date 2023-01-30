#pragma once

class Camera
{
public:
	Camera();

	void UseForDrawing(const Matrix& aTransform) const;

	void CreatePerspectiveFov(float aFovAngleY = XM_PI / 3.f, float anAspectRatio = 16.f / 9.f, float aNearZ = 0.3f, float aFarZ = 1000.f);
	
	bool IsOrthographic() const { return myOrthographic; }

private:
	Matrix myProjection;
	float myFovAngleY;
	float myAspectRatio;
	float myNearZ;
	float myFarZ;
	bool myOrthographic;
};

