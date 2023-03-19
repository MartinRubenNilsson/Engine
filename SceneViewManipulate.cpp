#include "pch.h"
#include "SceneViewManipulate.h"

void ImGui::SceneViewManipulate(Matrix& aTransform, const Keyboard::State& kState, const Mouse::State& mState)
{
	if (mState.positionMode != Mouse::MODE_RELATIVE)
		return;

	static constexpr float baseMoveSpeed = 10.f;
	static float moveSpeedMultiplier = 1.f;

	moveSpeedMultiplier += mState.scrollWheelValue * 0.06f / 120.f;
	moveSpeedMultiplier = std::clamp(moveSpeedMultiplier, 0.001f, 2.f);

	const float rotSpeed = 0.25f;
	const float moveSpeed = baseMoveSpeed * moveSpeedMultiplier * (kState.LeftShift ? 4.f : 1.f);
	const float deltaTime = GetIO().DeltaTime;

	Vector3 scale{}, translation{};
	Quaternion rotation{};
	aTransform.Decompose(scale, rotation, translation);

	rotation = Quaternion::CreateFromAxisAngle(Vector3::UnitX, mState.y * rotSpeed * deltaTime) * rotation;
	rotation = rotation * Quaternion::CreateFromAxisAngle(Vector3::UnitY, mState.x * rotSpeed * deltaTime);

	aTransform = Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(translation);

	Vector3 moveDir{};

	if (kState.D) moveDir += Vector3::UnitX;
	if (kState.A) moveDir -= Vector3::UnitX;
	if (kState.E) moveDir += Vector3::UnitY;
	if (kState.Q) moveDir -= Vector3::UnitY;
	if (kState.W) moveDir += Vector3::UnitZ;
	if (kState.S) moveDir -= Vector3::UnitZ;

	if (moveDir != Vector3::Zero)
		aTransform = Matrix::CreateTranslation(moveDir * moveSpeed * deltaTime) * aTransform;
}
