#pragma once

enum class CameraType
{
	Perspective,
	Orthographic
};

struct PerspectiveCamera
{
	float fovY = 1.04719755119f; // 60 degrees
	float aspect = 1.f;
};

struct OrthographicCamera
{
	float width = 5.f;
	float height = 5.f;
};

using CameraVariant = std::variant<PerspectiveCamera, OrthographicCamera>;

class Camera
{
public:
	Camera() = default;
	Camera(const aiCamera&);

	CameraType GetType() const;

	Matrix GetViewMatrix() const;
	Matrix GetProjectionMatrix(bool aReverseZ = false) const;

	float GetNearZ() const { return myNearZ; }
	float GetFarZ() const { return myFarZ; }

	void SetVariant(const CameraVariant&);
	const CameraVariant& GetVariant() const;

private:
	friend void to_json(json&, const Camera&);

	float myNearZ{ 0.3f }, myFarZ{ 1000.f };
	CameraVariant myVariant{};
};

namespace ImGui
{
	void Inspect(Camera& aCamera);

	void DrawCubes(const Camera& aCamera, const Matrix& aCameraTransform, std::span<const Matrix> someCubeTransforms);
	void DrawGrid(const Camera& aCamera, const Matrix& aCameraTransform, const Matrix& aGridTransform, float aGridSize);
	bool Manipulate(const Camera& aCamera, const Matrix& aCameraTransform, ImGuizmo::OPERATION anOperation, ImGuizmo::MODE aMode, Matrix& aTransform);
	void ViewManipulate(const Camera& aCamera, Matrix& aCameraTransform, float aLength, ImVec2 position, ImVec2 size, ImU32 backgroundColor);
}

