#pragma once

enum class CameraType
{
	Perspective,
	Orthographic
};

struct PerspectiveCamera
{
	float fovY = XM_PIDIV2;
	float aspect = 1.f;
	float nearZ = 0.01f;
	float farZ = 1000.f;
};

struct OrthographicCamera
{
	float width = 10.f;
	float height = 10.f;
	float nearZ = 0.01f;
	float farZ = 1000.f;
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
	Matrix GetProjectionMatrix() const;

	void SetPerspective(const PerspectiveCamera&);
	void SetOrthographic(const OrthographicCamera&);

private:
	friend void ImGui::InspectCamera(Camera&);

	Vector3 myPosition{ Vector3::Zero }, myDirection{ -Vector3::Forward }, myUp{ Vector3::Up };
	std::variant<PerspectiveCamera, OrthographicCamera> myCamera{};
};

namespace ImGui
{
	void DrawCubes(const Camera& aCamera, const Matrix& aCameraTransform, std::span<const Matrix> someCubeTransforms);
	void DrawGrid(const Camera& aCamera, const Matrix& aCameraTransform, const Matrix& aGridTransform, float aGridSize);
	bool Manipulate(const Camera& aCamera, const Matrix& aCameraTransform, ImGuizmo::OPERATION anOperation, ImGuizmo::MODE aMode, Matrix& aTransform);
	void ViewManipulate(const Camera& aCamera, Matrix& aCameraTransform, float aLength, ImVec2 position, ImVec2 size, ImU32 backgroundColor);
}

