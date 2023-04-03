#pragma once

class Rigidbody
{
public:
	Rigidbody();
	Rigidbody(const Rigidbody&);
	Rigidbody& operator=(const Rigidbody&);

	void SetTransform(const Vector3& p, const Quaternion& q);
	void GetTransform(Vector3& p, Quaternion& q) const;

	Vector3 GetVelocity() const;

	void SetMass(float);
	float GetMass() const;
	void SetDrag(float);
	float GetDrag() const;

	bool IsSleeping() const;

	explicit operator bool() const;

private:
	PxPtr<PxRigidDynamic> myImpl{};
};

void from_json(const json&, Rigidbody&);
void to_json(json&, const Rigidbody&);

namespace ImGui
{
	void Inspect(Rigidbody&);
}

