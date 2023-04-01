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
	CharacterController(const CharacterController&);
	CharacterController& operator=(const CharacterController&);

	CollisionFlags Move(const Vector3& aDeltaPos, entt::registry&);

	void SetPosition(const Vector3&); // Set world position of center of capsule
	Vector3 GetPosition() const; // Get world position of center of capsule

	void SetRadius(float);
	float GetRadius() const;
	void SetHeight(float);
	float GetHeight() const;

	explicit operator bool() const;

private:
	friend void from_json(const json&, CharacterController&);
	friend void to_json(json&, const CharacterController&);

	PxPtr<PxCapsuleController> myImpl{};

public:
	float minMoveDistance = 0.001f;
};

namespace ImGui
{
	void Inspect(CharacterController&);
}

