#include "pch.h"
#include "SceneViewManipulate.h"

void ImGui::SceneViewManipulate(Matrix& aTransform)
{
	Mouse::State mouseState{ Mouse::Get().GetState() };
	Mouse::Get().SetMode(mouseState.rightButton ? Mouse::MODE_RELATIVE : Mouse::MODE_ABSOLUTE);
	if (mouseState.positionMode != Mouse::MODE_RELATIVE)
		return;

	Keyboard::State keyboardState{ Keyboard::Get().GetState() };

	const float rotSpeed = 0.25f;
	const float moveSpeed = keyboardState.LeftShift ? 15.f : 5.f;
	const float deltaTime = GetIO().DeltaTime;

	Vector3 scale{}, translation{};
	Quaternion rotation{};
	aTransform.Decompose(scale, rotation, translation);

	rotation = Quaternion::CreateFromAxisAngle(Vector3::UnitX, mouseState.y * rotSpeed * deltaTime) * rotation;
	rotation = rotation * Quaternion::CreateFromAxisAngle(Vector3::UnitY, mouseState.x * rotSpeed * deltaTime);

	aTransform = Matrix::CreateFromQuaternion(rotation) * Matrix::CreateTranslation(translation);

	Vector3 moveDir{};

	if (keyboardState.D) moveDir += Vector3::UnitX;
	if (keyboardState.A) moveDir -= Vector3::UnitX;
	if (keyboardState.E) moveDir += Vector3::UnitY;
	if (keyboardState.Q) moveDir -= Vector3::UnitY;
	if (keyboardState.W) moveDir += Vector3::UnitZ;
	if (keyboardState.S) moveDir -= Vector3::UnitZ;

	if (moveDir != Vector3::Zero)
		aTransform = Matrix::CreateTranslation(moveDir * moveSpeed * deltaTime) * aTransform;
}
