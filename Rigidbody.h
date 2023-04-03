#pragma once

class Rigidbody
{
public:
	Rigidbody();
	Rigidbody(const Rigidbody&);
	Rigidbody& operator=(const Rigidbody&);

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

