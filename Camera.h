#pragma once

#define MIN_CLIP_Z 0.3f
#define MAX_CLIP_Z 1000.f

enum class CameraType
{
	Perspective,
	Orthographic
};

Matrix GetDefaultViewMatrix();

class Camera
{
public:
	Camera() = default;
	Camera(const aiCamera&);

	void SetType(CameraType aType) { myType = aType; }
	CameraType GetType() const { return myType; }

	Matrix GetProjMatrix() const;

	void SetAspect(float);
	float GetAspect() const { return myAspect; }
	void SetNearZ(float);
	float GetNearZ() const { return myNearZ; }
	void SetFarZ(float);
	float GetFarZ() const { return myFarZ; }

	void SwapNearAndFarZ();

private:
	friend void from_json(const json&, Camera&);
	friend void to_json(json&, const Camera&);

	CameraType myType{};
	float myFovOrHeight{ 1.f }, myAspect{ 1.f };
	float myNearZ{ MIN_CLIP_Z }, myFarZ{ MAX_CLIP_Z };

public:
	float depth{ 0.f }; // Cameras with lower depth are rendered before cameras with higher depth.
	bool customAspect{ false }; // If true, use camera aspect instead of screen's aspect when rendering.
};

void SortCamerasByDepth(entt::registry&);
void GetFirstCamera(const entt::registry&, Camera&, Matrix&);

namespace ImGui
{
	void Inspect(Camera& aCamera);

	void DrawCubes(const Camera& aCamera, const Matrix& aCameraTransform, std::span<const Matrix> someCubeTransforms);
	void DrawGrid(const Camera& aCamera, const Matrix& aCameraTransform, const Matrix& aGridTransform, float aGridSize);
	void ViewManipulate(const Camera& aCamera, Matrix& aCameraTransform, float aLength, ImVec2 position, ImVec2 size, ImU32 backgroundColor);
}

