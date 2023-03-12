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
	float width = 10.f;
	float height = 10.f;
};

using CameraVariant = std::variant<PerspectiveCamera, OrthographicCamera>;

struct CameraBuffer;

class Camera
{
public:
	Camera() = default;
	Camera(const aiCamera&);

	CameraType GetType() const;
	CameraBuffer GetBuffer(bool aReverseZ = false) const;

	Matrix GetViewMatrix() const;
	Matrix GetProjectionMatrix(bool aReverseZ = false) const;

	void SetVariant(const CameraVariant&);
	const CameraVariant& GetVariant() const;

protected:
	Vector3 myPosition{ Vector3::Zero }, myDirection{ Vector3::UnitZ }, myUp{ Vector3::UnitY };
	float myNearZ{ 0.3f }, myFarZ{ 1000.f };
	CameraVariant myVariant{};
};

namespace ImGui
{
	void InspectCamera(Camera& aCamera);

	void DrawCubes(const Camera& aCamera, const Matrix& aCameraTransform, std::span<const Matrix> someCubeTransforms);
	void DrawGrid(const Camera& aCamera, const Matrix& aCameraTransform, const Matrix& aGridTransform, float aGridSize);
	bool Manipulate(const Camera& aCamera, const Matrix& aCameraTransform, ImGuizmo::OPERATION anOperation, ImGuizmo::MODE aMode, Matrix& aTransform);
	void ViewManipulate(const Camera& aCamera, Matrix& aCameraTransform, float aLength, ImVec2 position, ImVec2 size, ImU32 backgroundColor);
}

