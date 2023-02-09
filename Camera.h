#pragma once

struct PerspectiveCamera
{
	float fovY = 1.570796327f; // pi/2
	float aspect = 1.f;
	float nearZ = 0.01f;
	float farZ = 1000.f;

	Matrix GetProjectionMatrix() const;
};

struct OrthographicCamera
{
	float width = 10.f;
	float height = 10.f;
	float nearZ = 0.01f;
	float farZ = 1000.f;

	Matrix GetProjectionMatrix() const;
};

class Camera
{
public:
	Camera();
	Camera(const PerspectiveCamera&);
	Camera(const OrthographicCamera&);
	Camera(const aiCamera&);

	void SetCamera(const Matrix& aTransform) const;

	const Matrix& GetLocalViewMatrix() const;
	Matrix GetProjectionMatrix() const;

	void SetPerspective(const PerspectiveCamera&);
	PerspectiveCamera GetPerspective() const;
	void SetOrthographic(const OrthographicCamera&);
	OrthographicCamera GetOrthographic() const;

	bool IsPerspective() const;
	bool IsOrthographic() const;

private:
	Matrix myLocalViewMatrix;
	std::variant<PerspectiveCamera, OrthographicCamera> myCamera;
};

namespace ImGui
{
	void CameraEdit(Camera& aCamera);

	void DrawCubes(const Camera& aCamera, const Matrix& aCameraTransform, std::span<const Matrix> someCubeTransforms);
	void DrawGrid(const Camera& aCamera, const Matrix& aCameraTransform, const Matrix& aGridTransform, float aGridSize);

	bool Manipulate(
		const Camera& aCamera,
		const Matrix& aCameraTransform,
		ImGuizmo::OPERATION anOperation, 
		ImGuizmo::MODE aMode,
		Matrix& aTransform
	);
}

