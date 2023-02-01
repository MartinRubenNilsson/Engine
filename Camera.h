#pragma once

class Camera
{
public:
	using Ptr = std::shared_ptr<Camera>;

	static Ptr Create(const aiCamera&);

	void UseForDrawing(const Matrix& aTransform) const;

	Matrix GetProjection() const;

private:
	Camera(const aiCamera&);

	float myVerticalFov;
	float myAspectRatio;
	float myNearClipPlane;
	float myFarClipPlane;
};

