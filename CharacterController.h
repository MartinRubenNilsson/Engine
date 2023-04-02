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

	CollisionFlags Move(const Vector3& aDeltaPos, float aDeltaTime);

	void SetPosition(const Vector3&); // Set world position of center of capsule
	Vector3 GetPosition() const; // Get world position of center of capsule

	void SetRadius(float);
	float GetRadius() const;
	void SetHeight(float);
	float GetHeight() const;

	PxCapsuleController* GetImpl();

	explicit operator bool() const;

	float minMoveDistance = 0.001f;

private:
	PxPtr<PxCapsuleController> myImpl{};
};

void from_json(const json&, CharacterController&);
void to_json(json&, const CharacterController&);

namespace ImGui
{
	void Inspect(CharacterController&);
}

