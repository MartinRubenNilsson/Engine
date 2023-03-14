#pragma once

enum class CameraType
{
	Perspective,
	Orthographic
};

class Camera
{
public:
	Camera() = default;
	Camera(const aiCamera&);

	CameraType GetType() const { return myType; }

	Matrix GetViewMatrix() const;
	Matrix GetProjectionMatrix(bool aReverseZ = false) const;

	void SetAspect(float anAspect) { myAspect = anAspect; }

	float GetNearZ() const { return myNearZ; }
	float GetFarZ() const { return myFarZ; }

private:
	friend void from_json(const json&, Camera&);
	friend void to_json(json&, const Camera&);

	CameraType myType{};
	float myFovOrHeight{ 1.f }, myAspect{ 1.f };
	float myNearZ{ 0.3f }, myFarZ{ 1000.f };
};

namespace ImGui
{
	void Inspect(Camera& aCamera);

	void DrawCubes(const Camera& aCamera, const Matrix& aCameraTransform, std::span<const Matrix> someCubeTransforms);
	void DrawGrid(const Camera& aCamera, const Matrix& aCameraTransform, const Matrix& aGridTransform, float aGridSize);
	bool Manipulate(const Camera& aCamera, const Matrix& aCameraTransform, ImGuizmo::OPERATION anOperation, ImGuizmo::MODE aMode, Matrix& aTransform);
	void ViewManipulate(const Camera& aCamera, Matrix& aCameraTransform, float aLength, ImVec2 position, ImVec2 size, ImU32 backgroundColor);
}

