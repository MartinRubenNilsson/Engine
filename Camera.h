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

class Camera;

namespace ImGui
{
	void InspectCamera(Camera& aCamera);
}

class Camera
{
public:
	Camera() = default;
	Camera(const aiCamera&);

	CameraType GetType() const;

	Matrix GetViewMatrix() const;
	Matrix GetProjectionMatrix(bool aSwapClipPlanes = false) const;

	void SetPerspective(const PerspectiveCamera&);
	void SetOrthographic(const OrthographicCamera&);

	float GetNearZ() const { return myNearZ; }
	float GetFarZ() const { return myFarZ; }

protected:
	friend void ImGui::InspectCamera(Camera&);

	Vector3 myPosition{ Vector3::Zero }, myDirection{ Vector3::UnitZ }, myUp{ Vector3::UnitY };
	std::variant<PerspectiveCamera, OrthographicCamera> myCamera{};
	float myNearZ{ 0.3f }, myFarZ{ 1000.f };
};

namespace ImGui
{
	void DrawCubes(const Camera& aCamera, const Matrix& aCameraTransform, std::span<const Matrix> someCubeTransforms);
	void DrawGrid(const Camera& aCamera, const Matrix& aCameraTransform, const Matrix& aGridTransform, float aGridSize);
	bool Manipulate(const Camera& aCamera, const Matrix& aCameraTransform, ImGuizmo::OPERATION anOperation, ImGuizmo::MODE aMode, Matrix& aTransform);
	void ViewManipulate(const Camera& aCamera, Matrix& aCameraTransform, float aLength, ImVec2 position, ImVec2 size, ImU32 backgroundColor);
}

