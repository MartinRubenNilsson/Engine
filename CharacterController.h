#pragma once

enum class CollisionFlags : uint8_t
{
	Sides = 1 << 0,
	Above = 1 << 1,
	Below = 1 << 2,
	_entt_enum_as_bitmask
};

class CharacterController
{
public:
	CharacterController();

	CollisionFlags Move(const Vector3& aDeltaPos, float aDeltaTime);

	void SetRadius(float);
	float GetRadius() const;
	void SetHeight(float);
	float GetHeight() const;

	explicit operator bool() const;

private:
	PxPtr<PxCapsuleController> myImpl{};
	float myMinMoveDistance = 0.001f;
};

namespace ImGui
{
	void Inspect(CharacterController&);
}

