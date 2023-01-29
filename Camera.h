#pragma once

class Camera
{
public:
	Camera();
	Camera(const Camera&);
	Camera& operator=(const Camera&);
	~Camera();

	void WriteCamera(const Matrix& aTransform) const;
	void SetPerspectiveFov(float aFovAngleY = XM_PI / 3.f, float anAspectRatio = 16.f / 9.f, float aNearZ = 0.3f, float aFarZ = 1000.f);

	bool IsOrthographic() const { return myOrthographic; }

private:
	Matrix myProjection;
	bool myOrthographic;
};
